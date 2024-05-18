#include "ext_flash.h"
#include "nrf_qspi.h"

#if defined(QSPI_FLASH_USED)

static const nrfx_qspi_config_t flash_qspi_config = {
    .xip_offset = 0,
    .pins = {
        .sck_pin = PIN_FLASH_SCK,
        .csn_pin = PIN_FLASH_CSN,
        .io0_pin = PIN_FLASH_MOSI,
        .io1_pin = PIN_FLASH_MISO,
        .io2_pin = PIN_FLASH_IO2,
        .io3_pin = PIN_FLASH_IO3,
    },
    .prot_if = {
        .readoc = NRF_QSPI_READOC_READ4IO,
        .writeoc = NRF_QSPI_WRITEOC_PP4O,
        .addrmode = NRF_QSPI_ADDRMODE_24BIT,
        .dpmconfig = true,  //Enabled deep power mode
    },
    .phy_if = {
        .sck_delay = 0x01,
        .dpmen     = false, //Exit deep power mode     
        .spi_mode  = NRF_QSPI_MODE_0,    
        .sck_freq  = NRF_QSPI_FREQ_DIV1,
    },
    .irq_priority  = (uint8_t)7
};

NRF_STATIC_INLINE bool eflash_event_check(NRF_QSPI_Type const * p_reg, nrf_qspi_event_t event)
{
    return (bool)*(volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event);
}

static jedec_id_t readExtFlashJEDEC_ID(){
    jedec_id_t ret;
    const nrf_qspi_cinstr_conf_t QPI_enter_ci{
        .opcode = EFLASH_JEDEC_ID,
        .length = NRF_QSPI_CINSTR_LEN_4B,
        .io2_level = 1,
        .io3_level = 1,
        .wipwait = false,
        .wren = false
    };
    nrfx_qspi_cinstr_xfer(&QPI_enter_ci, NULL, &ret);
    return ret;
}

static uint8_t readExtFlashDeviceID(){
    uint8_t ret[5];
    const uint8_t in[5] = {0, 0, 0, 0, 0};
    const nrf_qspi_cinstr_conf_t QPI_enter_ci{
        .opcode = EFLASH_MAN_DEV_ID,
        .length = NRF_QSPI_CINSTR_LEN_6B,
        .io2_level = 1,
        .io3_level = 1,
        .wipwait = false,
        .wren = false
    };
    nrfx_qspi_cinstr_xfer(&QPI_enter_ci, in, ret);
    return ret[4];
}

static uint64_t readExtFlashUniqueID(){
    uint8_t ret[12];
    uint8_t in[12];
    const nrf_qspi_cinstr_conf_t QPI_enter_ci{
        .opcode = EFLASH_UNIQUE_ID,
        .length = NRF_QSPI_CINSTR_LEN_1B,
        .io2_level = 1,
        .io3_level = 1,
        .wipwait = false,
        .wren = false
    };
    nrfx_qspi_lfm_start(&QPI_enter_ci);
    nrfx_qspi_lfm_xfer(in, ret, 12, true);
    return *((const uint64_t*)(&ret[4]));
}

__WEAK void extFlashReadyCallback(nrfx_qspi_evt_t event, void * p_context)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
       the extFlashReadyCallback could be implemented in the user file
  */
}

static flash_info_t ExtFlashInfo;
static bool ExtFlashInitDone_ = false;

void initExtFlash(){
    ExtFlashInitDone_ = nrfx_qspi_init(&flash_qspi_config, extFlashReadyCallback, NULL) == NRFX_SUCCESS;
    if(!ExtFlashInitDone_){
        return;
    }

    NRF_QSPI->DPMDUR = (1UL << 16) | (1UL << 0);    //Set 16us delay after sleep/wake-up instruction

    bool dummy_res;

    //Reading IDs
    ExtFlashInfo.JEDEC_ID = readExtFlashJEDEC_ID();
    EFLASH_WAIT_MAX_1ms_UNTIL_READY(dummy_res);
    ExtFlashInfo.DeviceID = readExtFlashDeviceID();
    EFLASH_WAIT_MAX_1ms_UNTIL_READY(dummy_res);
    ExtFlashInfo.UniqueID = readExtFlashUniqueID();
    EFLASH_WAIT_MAX_1ms_UNTIL_READY(dummy_res);

    //Read status register 2
    static const nrf_qspi_cinstr_conf_t read_stat2_ci{
        .opcode = EFLASH_READ_STATUS2,
        .length = NRF_QSPI_CINSTR_LEN_2B,
        .io2_level = 1,
        .io3_level = 1,
        .wipwait = false,
        .wren = false
    };
    uint8_t status2_reg;
    ExtFlashInitDone_ = nrfx_qspi_cinstr_xfer(&read_stat2_ci, NULL, &status2_reg) == NRFX_SUCCESS;
    if(!ExtFlashInitDone_){
        return;
    }

    if((status2_reg & 2) == 0){
        //QE bit is not set

        //Write to status register only volatile
        const nrf_qspi_cinstr_conf_t write_volatile_ci{
            .opcode = EFLASH_WRITE_ENABLE_VOLATILE,
            .length = NRF_QSPI_CINSTR_LEN_1B,
            .io2_level = 1,
            .io3_level = 1,
            .wipwait = true,
            .wren = false
        };
        ExtFlashInitDone_ = nrfx_qspi_cinstr_xfer(&write_volatile_ci, NULL, NULL) == NRFX_SUCCESS;
        if(!ExtFlashInitDone_){
            return;
        }

        status2_reg |= 2; //Set QE
        //Set status register 2
        const nrf_qspi_cinstr_conf_t write_stat2_ci{
            .opcode = EFLASH_WRITE_STATUS2,
            .length = NRF_QSPI_CINSTR_LEN_2B,
            .io2_level = 1,
            .io3_level = 1,
            .wipwait = true,
            .wren = true
        };
        uint8_t dummy;
        ExtFlashInitDone_ = nrfx_qspi_cinstr_xfer(&write_stat2_ci, &status2_reg, &dummy) == NRFX_SUCCESS;
        if(!ExtFlashInitDone_){
            return;
        }
    }

    EFLASH_WAIT_MAX_1ms_UNTIL_READY(dummy_res);

    //Setting QPI mode
    static const nrf_qspi_cinstr_conf_t QPI_enter_ci{
        .opcode = EFLASH_ENTER_QPI_MODE,
        .length = NRF_QSPI_CINSTR_LEN_1B,
        .io2_level = 1,
        .io3_level = 1,
        .wipwait = true,
        .wren = false
    };
    ExtFlashInitDone_ = nrfx_qspi_cinstr_xfer(&QPI_enter_ci, NULL, NULL) == NRFX_SUCCESS;
    if(!ExtFlashInitDone_){
        return;
    }

    EFLASH_WAIT_MAX_1ms_UNTIL_READY(dummy_res);

    ExtFlashInitDone_ = dummy_res;
}

bool isExtFlashInitDone(){
    return ExtFlashInitDone_;
}

uint8_t getExtFlashManufacturerID(){
    return ExtFlashInfo.JEDEC_ID.ManufacturerID;
}

uint8_t getExtFlashDeviceID(){
    return ExtFlashInfo.DeviceID;
}

uint64_t getExtFlashUniqueID(){
    return ExtFlashInfo.UniqueID;
}

uint8_t getExtFlashType(){
    return ExtFlashInfo.JEDEC_ID.MemoryType;
}

uint32_t getExtFlashSize(){
    return (ExtFlashInfo.JEDEC_ID.Capacity == 0) ? 0 : (1UL << (uint32_t)ExtFlashInfo.JEDEC_ID.Capacity);
}

bool readExtFlash(void* buffer, size_t buffer_size, uint32_t address){
    return nrfx_qspi_read(buffer, buffer_size, address) == NRFX_SUCCESS;
}

bool writeExtFlash(void* buffer, size_t buffer_size, uint32_t address){
    return nrfx_qspi_write(buffer, buffer_size, address) == NRFX_SUCCESS;
}

bool erase4kBSectorExtFlash(uint32_t address){
    return nrfx_qspi_erase(NRF_QSPI_ERASE_LEN_4KB, address) == NRFX_SUCCESS;
}

bool erase64kBSectorExtFlash(uint32_t address){
    return nrfx_qspi_erase(NRF_QSPI_ERASE_LEN_64KB, address) == NRFX_SUCCESS;
}

bool eraseAllExtFlash(uint32_t address){
    return nrfx_qspi_erase(NRF_QSPI_ERASE_LEN_ALL, address) == NRFX_SUCCESS;
}

void sleepExtFlash(){
    uint32_t tmp = NRF_QSPI->IFCONFIG1;
    tmp |= 1 << QSPI_IFCONFIG1_DPMEN_Pos;
}

void wakeUpExtFlash(){
    uint32_t tmp = NRF_QSPI->IFCONFIG1;
    tmp = tmp & (~(1 << QSPI_IFCONFIG1_DPMEN_Pos));
}

#endif
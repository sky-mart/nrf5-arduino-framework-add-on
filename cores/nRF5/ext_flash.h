/**
 * @file ext_flash.h
 * @brief This file contains functions for controlling external flash.
 *
 * # Credits
 * @author Matej Fitoš
 * @date Mar 23, 2023
 *
 */

/*
  Copyright (c) 2023 Matej Fitoš, All right reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef EXT_FLASH_H_
#define EXT_FLASH_H_

#include "variant.h"

#if defined(PIN_FLASH_SCK) && defined(PIN_FLASH_CSN) && defined(PIN_FLASH_MOSI) && defined(PIN_FLASH_MISO) && defined(PIN_FLASH_IO2) && defined(PIN_FLASH_IO3)

#define QSPI_FLASH_USED     (1)

#include "nrfx_qspi.h"

#define EFLASH_XIP_BASE             (0x12000000UL)
#define EFLASH_XIP_SIZE             (0xE000000UL)

//Converts XIP(Execute in Place) address to the external flash address
#define XIP_ADDR_TO_EFLASH_ADDR(xip_addr)       ((xip_addr) - EFLASH_XIP_BASE)

//Converts external flash address to the XIP(Execute in Place) address
#define EFLASH_ADDR_TO_XIP_ADDR(eflash_addr)    ((eflash_addr) + EFLASH_XIP_BASE)

//Checks if address is in XIP region
#define IS_XIP_REGION(xip_addr)     ((xip_addr) >= EFLASH_XIP_BASE && (xip_addr) < (EFLASH_XIP_BASE + EFLASH_XIP_SIZE))


#define EFLASH_MAN_DEV_ID           (0x90)
#define EFLASH_JEDEC_ID             (0x9F)
#define EFLASH_UNIQUE_ID            (0x4B)

#define EFLASH_WRITE_ENABLE_VOLATILE    (0x50)

#define EFLASH_READ_STATUS1         (0x05)
#define EFLASH_READ_STATUS2         (0x35)
#define EFLASH_READ_STATUS3         (0x15)
#define EFLASH_WRITE_STATUS1        (0x05)
#define EFLASH_WRITE_STATUS2        (0x35)
#define EFLASH_WRITE_STATUS3        (0x15)

#define EFLASH_ENTER_QPI_MODE       (0x38)


#define EFLASH_COMBINE1(X,Y) X##Y  // helper macro
#define EFLASH_COMBINE(X,Y) EFLASH_COMBINE1(X,Y)

/**
 * @brief Macro for waiting until condition is met.
 *
 * @param[in]  condition Condition to meet.
 * @param[in]  attempts  Maximum number of condition checks. Must not be 0.
 * @param[in]  delay_us  Delay between consecutive checks, in microseconds.
 * @param[out] result    Boolean variable to store the result of the wait process.
 *                       Set to true if the condition is met or false otherwise.
 */
#define EFLASH_WAIT_FOR(condition, attempts, delay_us, result) \
do {                                                         \
    result =  false;                                         \
    uint32_t EFLASH_COMBINE(remaining_attempts,__LINE__) = (attempts);      \
    do {                                                     \
           if (condition)                                    \
           {                                                 \
               result =  true;                               \
               break;                                        \
           }                                                 \
           NRFX_DELAY_US(delay_us);                          \
    } while (--EFLASH_COMBINE(remaining_attempts,__LINE__));                          \
} while(0)

#define EFLASH_WAIT_MAX_1ms_UNTIL_READY(result) EFLASH_WAIT_FOR(isReadyExtFlash(), 100, 10, result)

typedef struct{
    uint8_t ManufacturerID; //0xEF - Winbond Serial Flash
    uint8_t MemoryType;
    uint8_t Capacity;       //128MB = 0x18
}jedec_id_t;

typedef struct{
    jedec_id_t JEDEC_ID;
    uint8_t DeviceID;
    uint64_t UniqueID;
}flash_info_t;

/**
 * @brief Initializes external flash. Do not call this function in the user code!
 * @note This function is automatically called at startup.
 */
void initExtFlash();

/**
 * @brief Returns true, when external flash was successfully initialized.
 */
inline bool isExtFlashInitDone();

/**
 * @brief Gets external flash manufacturer ID. This ID is assigned by JEDEC organization.
 * For more informations, see JEP106 file.
 * @note winbond manufacturer ID is 0xEF.
 */
uint8_t getExtFlashManufacturerID();

/**
 * @brief Gets external flash device ID. This value depends on the flash manufacturer.
 */
uint8_t getExtFlashDeviceID();

/**
 * @brief Gets external flash 64-bit unique ID.
 */
uint64_t getExtFlashUniqueID();

/**
 * @brief Gets external flash type. This value depends on the flash manufacturer.
 */
uint8_t getExtFlashType();

/**
 * @brief Gets external flash size in bytes.
 */
uint32_t getExtFlashSize();

/**
 * @brief Reads data from the external flash starting from the specified address.
 *        This function just triggers data reading, which is running in the background (using DMA).
 *        That's why you should check, if the data are ready by calling isBusyExtFlash(). Or you can
 *        override weak function extFlashReadyCallback(), which is called, when flash is ready.
 * @note Data from the flash can be read also through an pointer, because external flash is mapped to the XIP region.
 *       See EFLASH_ADDR_TO_XIP_ADDR() macro.
 * @param buffer Pointer to the buffer, where data will be copied. Buffer has to be in SRAM region.
 * @param buffer_size Size of the data to copy.
 * @param address External flash data address, from which data copy will start.
 * @return Returns true, when data reading started, or false, when flash is busy erasing/writing or when
 * buffer pointer does not point to the memory in SRAM.
 */
bool readExtFlash(void* buffer, size_t buffer_size, uint32_t address);

/**
 * @brief Writes data to the external flash starting from the specified address.
 *        This function just triggers data writing, which is running in the background (using DMA).
 *        That's why you should check, if the writing is done by calling isBusyExtFlash(). Or you can
 *        override weak function extFlashReadyCallback(), which is called, when flash is ready.
 * @note You can write data only to the erased sectors.
 * @param buffer Pointer to the buffer, from which data will be copied. Buffer has to be in SRAM region.
 * @param buffer_size Size of the data to copy.
 * @param address External flash data address, where data will be copied.
 * @return Returns true, when data writing started, or false, when flash is busy erasing/writing or when
 * buffer pointer does not point to the memory in SRAM.
 */
bool writeExtFlash(void* buffer, size_t buffer_size, uint32_t address);

/**
 * @brief Trigers erasing 4kB sector in the external flash. This operation can take a while.
 *        That's why you should check, if the erasing is done by calling isBusyExtFlash(). Or you can
 *        override weak function extFlashReadyCallback(), which is called, when flash is ready.
 * @param address Address of the sector to erase.
 * @return Returns true, when sector erasing started, or false, when flash is busy erasing/writing.
 */
bool erase4kBSectorExtFlash(uint32_t address);

/**
 * @brief Trigers erasing 64kB sector in the external flash. This operation can take a while.
 *        That's why you should check, if the erasing is done by calling isBusyExtFlash(). Or you can
 *        override weak function extFlashReadyCallback(), which is called, when flash is ready.
 * @param address Address of the sector to erase.
 * @return Returns true, when sector erasing started, or false, when flash is busy erasing/writing.
 */
bool erase64kBSectorExtFlash(uint32_t address);

/**
 * @brief Trigers erasing the whole external flash. This operation can take up to 200 seconds.
 *        That's why you should check, if the erasing is done by calling isBusyExtFlash(). Or you can
 *        override weak function extFlashReadyCallback(), which is called, when flash is ready.
 * @param address Address of the sector to erase.
 * @return Returns true, when sector erasing started, or false, when flash is busy erasing/writing.
 */
bool eraseAllExtFlash(uint32_t address);

/**
 * @brief Returns true, when memory is ready.
 */
inline bool isReadyExtFlash(){
    return ((NRF_QSPI->STATUS & QSPI_STATUS_READY_Msk) >> QSPI_STATUS_READY_Pos) == QSPI_STATUS_READY_BUSY;
}

/**
 * @brief Returns true, when memory is busy writing or erasing.
 */
inline bool isBusyExtFlash(){
    return !isReadyExtFlash();
}

/**
 * @brief Makes the external flash to enter deep sleep mode.
 * @warning The external flash is inaccessible during deep sleep mode.
 */
void sleepExtFlash();

/**
 * @brief Wake's up the external flash from deep sleep mode.
 * @warning The external flash is inaccessible during deep sleep mode.
 */
void wakeUpExtFlash();

#endif

#endif // !EXT_FLASH_H_
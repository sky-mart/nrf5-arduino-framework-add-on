/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.
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

#include "variant.h"
#include "wiring_constants.h"
#include "wiring_digital.h"
#include "nrf.h"
#include "ext_flash.h"

const uint32_t g_ADigitalPinMap[] =
{
  // P0
  0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 ,
  8 , 9 , 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31,

  // P1
  32, 33, 34, 35, 36, 37, 38, 39,
  40, 41, 42, 43, 44, 45, 46, 47
};

void initVariant()
{
  // Set REGOUT0 to 3V mode
  // https://devzone.nordicsemi.com/nordic/short-range-guides/b/getting-started/posts/nrf52840-dongle-programming-tutorial
  /*if ((NRF_UICR->REGOUT0 & UICR_REGOUT0_VOUT_Msk) ==
      (UICR_REGOUT0_VOUT_DEFAULT << UICR_REGOUT0_VOUT_Pos))
  {
      NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
      while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

      NRF_UICR->REGOUT0 = (NRF_UICR->REGOUT0 & ~((uint32_t)UICR_REGOUT0_VOUT_Msk)) |
                          (UICR_REGOUT0_VOUT_3V0 << UICR_REGOUT0_VOUT_Pos);

      NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
      while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

      // System reset is needed to update UICR registers.
      NVIC_SystemReset();
  }*/

  //SystemInit();

  //PA/LNA init - disable it initally
  pinMode(PIN_PA_LNA_A_SEL, OUTPUT);
  digitalWrite(PIN_PA_LNA_A_SEL, LOW);
  pinMode(PIN_PA_LNA_MODE, OUTPUT);
  digitalWrite(PIN_PA_LNA_MODE, LOW);
  pinMode(PIN_PA_LNA_RX_EN, OUTPUT);
  digitalWrite(PIN_PA_LNA_RX_EN, LOW);
  pinMode(PIN_PA_LNA_TX_EN, OUTPUT);
  digitalWrite(PIN_PA_LNA_TX_EN, LOW);

  // init all 2 onboard LEDs
  pinMode(PIN_LED1, OUTPUT);
  ledOff(PIN_LED1);

  pinMode(PIN_LED2, OUTPUT);
  ledOff(PIN_LED2);

  // init all 2 onboard buttons
  pinMode(PIN_BUTTON1, INPUT);
  pinMode(PIN_BUTTON2, INPUT);

  // init Vout switch pin
  pinMode(PIN_VOUT_EN, OUTPUT);
  enableVout(false);

  initExtFlash();
}

/**
 * @brief Enables or disables voltage on Vout 3V3 pins except always on pin.
 * Before disabling Vout, please ensure, that pins, that are connected to sensors
 * are set to INPUT. Those pins are:
 * + PIN_SPI_MISO
 * + PIN_SPI_MOSI
 * + PIN_SPI_SCK 
 * + PIN_SPI_CSN1
 * + PIN_SPI_CSN2
 * + PIN_SPI_IO2 
 * + PIN_SPI_CSN3
 * + PIN_SPI_IO3
 * + PIN_WIRE_SDA
 * + PIN_WIRE_SCL
 * + PIN_WIRE_IO2
 * + PIN_WIRE_IO3
 * + PIN_1WIRE_DATA
 * + PIN_1WIRE_SPU
 * + PIN_A0
 * + PIN_A1
 * + PIN_A2
 * + PIN_A3
 * + PIN_A4
 * + PIN_A5
 * + PIN_A6
 * + PIN_A7
 */
void enableVout(bool enabled){
  digitalWrite(PIN_VOUT_EN, !enabled);
}
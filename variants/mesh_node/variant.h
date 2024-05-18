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

#ifndef _VARIANT_MESH_NODE_
#define _VARIANT_MESH_NODE_

/** Master clock frequency */
#define VARIANT_MCK       (64000000ul)

#define USE_LFXO      // Board uses 32khz crystal for LF
// define USE_LFRC    // Board uses RC for LF

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Number of pins defined in PinDescription array
#define PINS_COUNT           (48)
#define NUM_DIGITAL_PINS     (48)
#define NUM_ANALOG_INPUTS    (8)
#define NUM_ANALOG_OUTPUTS   (0)

// LEDs
#define PIN_LED1             (8)  // P0.08
#define PIN_LED2             (9)  // P0.09

#define LED_BUILTIN          PIN_LED1
#define LED_CONN             PIN_LED2

#define LED_GREEN            PIN_LED1
#define LED_RED              PIN_LED2


#define LED_STATE_ON         0         // State when LED is litted

/*
 * Buttons
 */
#define PIN_BUTTON1          (33) // P1.01
#define PIN_BUTTON2          (42) // P1.10

/*
 * Analog pins
 */
#define PIN_A0               (2)  // P0.02
#define PIN_A1               (3)  // P0.03
#define PIN_A2               (4)  // P0.04
#define PIN_A3               (5)  // P0.05
#define PIN_A4               (28) // P0.28
#define PIN_A5               (29) // P0.29
#define PIN_A6               (30) // P0.30
#define PIN_A7               (31) // P0.31

static const uint8_t A0  = PIN_A0;
static const uint8_t A1  = PIN_A1;
static const uint8_t A2  = PIN_A2;
static const uint8_t A3  = PIN_A3;
static const uint8_t A4  = PIN_A4;
static const uint8_t A5  = PIN_A5;
static const uint8_t A6  = PIN_A6;
static const uint8_t A7  = PIN_A7;
#define ADC_RESOLUTION    14

// Other pins
#define PIN_AREF           (32) // P1.00
#define PIN_PWR_IO         (35) // P1.03 - IO, that is present in the power controll FPC connector
#define PIN_VOUT_EN        (44) // P1.12 - IO, that is used for enabling/disabling output voltage
void enableVout(bool enabled);

static const uint8_t AREF = PIN_AREF;

/*
 * Serial interfaces
 */

#define PIN_SERIAL1_RX       (6)  // P0.06
#define PIN_SERIAL1_TX       (11) // P0.11

#define PIN_SERIAL2_RX       (12) // P0.12
#define PIN_SERIAL2_TX       (13) // P0.13

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO         (7)  // P0.07
#define PIN_SPI_MOSI         (41) // P1.09
#define PIN_SPI_SCK          (40) // P1.08

#define PIN_SPI_CSN1         (45) // P1.13
#define PIN_SPI_CSN2         (47) // P1.15
#define PIN_SPI_IO2          (46) // P1.14
#define PIN_SPI_CSN3         (43) // P1.11
#define PIN_SPI_IO3          (10) // P0.10


static const uint8_t SS1  = PIN_SPI_CSN1;
static const uint8_t SS2  = PIN_SPI_CSN2;
static const uint8_t SS3  = PIN_SPI_CSN3;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 2

#define PIN_WIRE_SDA         (26) // P0.26
#define PIN_WIRE_SCL         (27) // P0.27
#define PIN_WIRE_IO2         (39) // P1.07
#define PIN_WIRE_IO3         (25) // P0.25

// SDA and SCL pins are present in the power controll FPC connector
#define PIN_WIRE_PWR_SDA     (24) // P0.24
#define PIN_WIRE_PWR_SCL     (23) // P0.23
#define PIN_WIRE1_SDA        (PIN_WIRE_PWR_SDA)
#define PIN_WIRE1_SCL        (PIN_WIRE_PWR_SCL)

/*
 * 1-Wire Interfaces
 */

#define PIN_ONE_WIRE_DATA       (15) // P0.15
#define PIN_ONE_WIRE1_SPU        (14) // P0.14 - Strong pullup pin

/*
 * External FLASH Interface
 */
#define PIN_FLASH_MISO       (19) // P0.19
#define PIN_FLASH_MOSI       (16) // P0.16
#define PIN_FLASH_SCK        (21) // P0.21
#define PIN_FLASH_CSN        (22) // P0.22
#define PIN_FLASH_IO2        (17) // P0.17
#define PIN_FLASH_IO3        (20) // P0.20

/*
 * Radio PA/LNA Pins
 */
#define PIN_PA_LNA_TX_EN     (37) // P0.05 - TX enable (set to high when transmitting)
#define PIN_PA_LNA_RX_EN     (38) // P0.06 - RX enable (set to high for reception)
#define PIN_PA_LNA_MODE      (36) // P0.04 - Set high to bypass PA/LNA
#define PIN_PA_LNA_A_SEL     (34) // P0.02 - Antenna selection (always set to low)

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#endif
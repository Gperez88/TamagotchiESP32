# ArduinoGotchi - A real Tamagotchi emulator for ESP32

## Synopsis

**ArduinoGotchi** is a real [Tamagotchi P1](https://tamagotchi.fandom.com/wiki/Tamagotchi_(1996_Pet)) emulator running in Arduino UNO hardware. The emulation core is based on [TamaLib](https://github.com/jcrona/tamalib) with intensive optimization to make it fit into UNO's hardware that only comes with 32K Flash 2K RAM.

![Tamagotchi P1 Actual Devices](images/TamaP1_devices.jpg)

## Improvements

This repository is a fork/improvement of the original repo [anabolyc/Tamagotchi](https://github.com/anabolyc/Tamagotchi) with the following enhancements:

* added `ESP32` platform support
* added experimental deepsleep feature for `ESP32` and `ESP8266`
* flexible button configuration
* **Migrated from I2C OLED (SSD1306 128x64) to SPI TFT display (ST7789 240x240)**
  * Higher resolution display (240x240 vs 128x64)
  * Better visual quality with 7.5x scaling
  * Faster communication via SPI protocol
  * Support for color displays (future enhancement)

## Build

### Firmware

First, install Visual Studio Code and the PlatformIO plugin.
Open the `firmware/Tamagotchi32` folder as `Workspace-Folder`

Open the PlatformIO by clicking on the Icon on the left sidebar.
Select your desired platform:

* nanoatmega328
* esp8266
* esp32

and use the `build` task to build for the selected platform. Next run `Upload` or `Upload and Monitor` to upload the firmware to the connected board.

### Circuit Diagram

#### ESP32 Pin Connections (SPI ST7789 Display)

| FROM ESP32 | TO COMPONENT                     |
|------------|----------------------------------|
| 3V3        | VCC (DISPLAY, PULL-UP RESISTORS) |
| GND        | GND (DISPLAY, BUZZER, BUTTONS)   |
| **G5**     | **CS (DISPLAY - Chip Select)**   |
| **G18**    | **SCL (DISPLAY - SPI Clock)**    |
| **G19**    | **DC (DISPLAY - Data/Command)**  |
| **G21**    | **RES (DISPLAY - Reset)**        |
| **G22**    | **BL (DISPLAY - Backlight)**     |
| **G23**    | **SDA (DISPLAY - SPI Data/MOSI)**|
| G15        | BUZZER                           |
| G17        | BUTTON LEFT                      |
| **G25**    | **BUTTON MIDDLE** ⚠️             |
| G4         | BUTTON RIGHT                     |

**⚠️ Important**: The middle button pin has been changed from G16/G19 to **G25** to accommodate the SPI display connections.

![Circuit Diagram](hardware/TamagotchiESP32_schematic_Steckplatine.png)

> **Note**: The schematic image shows the original I2C configuration. For the updated SPI ST7789 connections, please refer to the [SPI ST7789 Schematic](hardware/SPI_ST7789_SCHEMATIC.md) documentation.

#### NOTE

The firmware supports the `ESP8266` platform too, please connect the `RST` and `D0` pin to enable the deepsleep feature.

### Required Libraries

The project uses the following libraries (automatically installed via PlatformIO):

* `Adafruit GFX Library` (^1.11.9) - Graphics library for displays
* `Adafruit ST7735 and ST7789 Library` (^2.1.8) - ST7789 display driver
* `Wire` - I2C communication (for other components)
* `EEPROM` - Non-volatile memory storage
* `Tone32` (ESP32 only) - PWM tone generation for buzzer

### Case

The case can be found here [CASE](https://www.thingiverse.com/thing:2374552)
An additional case spacer can be found in the `hardware/stl` folder

### Example Build

For the example build the following parts were used:

* ESP32 Devboard
* **1.3" TFT Display `ST7789 SPI 240x240`** (updated from 0.96" OLED I2C)
* 3x Buttons (e.g. `Keyestudio Digital Push Button`, resistors already included)
* [OPTIONAL] 3x 1kΩ pull-up resistors (for buttons)
* 1s USB-C lipo charging board (e.g. `DFRobot DFR0668`)
* 1s 3.7v 220mAh LIPO with protection board (e.g. `XZD-2004`)
* StepUp converter (e.g. `MT3608`, better `TPS63020`)
* Buzzer (e.g. `KY-006`)

![opened case](images/IMG_7096.JPG)

## Migration from I2C to SPI

This version includes a complete migration from I2C OLED display to SPI ST7789 display. For detailed migration information, please refer to:

* [I2C to SPI ST7789 Migration Guide](I2C_TO_SPI_ST7789_MIGRATION.md) (English)
* [Guía de Migración I2C a SPI ST7789](I2C_TO_SPI_ST7789_MIGRATION_es.md) (Español)
* [SPI ST7789 Connection Schematic](hardware/SPI_ST7789_SCHEMATIC.md) (English)

### Key Changes:
- **Display**: Changed from I2C (2 pins) to SPI (6 pins)
- **Resolution**: Upgraded from 128x64 to 240x240 pixels
- **Scaling**: Improved from 4x to 7.5x scaling factor
- **Library**: Replaced U8g2lib with Adafruit GFX + ST7789
- **Button**: Middle button moved from G16/G19 to G25

## Game Notes

- To activate your pet, you have to configure the clock by pressing the middle button. Otherwise, your pet will not be alive.
- A long click on "middle" button for 5 seconds, will reset memory back to egg state
- A long click on "left" button for 5 seconds, will enter the deepsleep mode for 10 minutes

## License

The base project ArduinoGotchi is distributed under the GPLv2 license. See the LICENSE file for more information.

# Technical Document: I2C to SPI ST7789 Display Migration

## 1. Current Schematic Analysis (I2C)

### 1.1 Current I2C Display Connections

According to the analyzed Fritzing schematic, the I2C OLED display (SSD1306 128x64) is connected as follows:

| I2C Display Pin | ESP32 Pin | Function |
|-----------------|-----------|----------|
| GND             | GND       | Common ground |
| VDD             | 3V3       | Power supply (3.3V) |
| SCK             | G22       | I2C Clock (SCL) |
| SDA             | G21       | I2C Data (SDA) |

### 1.2 Other System Connections

| Component | ESP32 Pin | Function |
|-----------|-----------|----------|
| Left Button | G17 | Digital input |
| Middle Button | G16 | Digital input |
| Right Button | G4 | Digital input |
| Buzzer | G15 | PWM output |

### 1.3 Current Software Implementation

The code uses the **U8g2lib** library with the driver:
- `U8G2_SSD1306_128X64_NONAME_2_HW_I2C` (Hardware I2C)
- Physical resolution: 128x64 pixels
- Logical Tamagotchi resolution: 32x16 pixels (4x scaling)

---

## 2. Differences between I2C and SPI

### 2.1 I2C Protocol (Inter-Integrated Circuit)

**Characteristics:**
- **Communication lines:** 2 (SDA, SCL)
- **Speed:** Relatively slow (100 kHz - 3.4 MHz)
- **Addressing:** By device address (allows multiple devices on the same bus)
- **Advantages:**
  - Fewer pins required (only 2 + power)
  - Easy to connect multiple devices
  - Lower GPIO pin consumption
- **Disadvantages:**
  - Limited speed
  - More complex for fast data transfers

**Required pins:**
- SDA (Serial Data)
- SCL (Serial Clock)
- GND
- VCC

### 2.2 SPI Protocol (Serial Peripheral Interface)

**Characteristics:**
- **Communication lines:** 4-5 (MOSI, MISO, SCK, CS, optionally DC/RES)
- **Speed:** Very fast (up to 80 MHz on ESP32)
- **Addressing:** By dedicated CS (Chip Select) line
- **Advantages:**
  - High transfer speed
  - Ideal for high-resolution displays
  - Full-duplex transfer
  - Better for complex graphics and animations
- **Disadvantages:**
  - Requires more GPIO pins
  - Each device needs its own CS line

**Required pins for ST7789:**
- SCL/SCK (Serial Clock)
- SDA/MOSI (Master Out Slave In - data)
- CS (Chip Select)
- DC (Data/Command)
- RES (Reset)
- BL (Backlight - optional)
- GND
- VCC

### 2.3 Comparison for Tamagotchi Application

| Aspect | I2C (Current) | SPI (ST7789) |
|--------|---------------|--------------|
| Pins used | 2 (G21, G22) | 5-6 (SCK, MOSI, CS, DC, RES, BL) |
| Speed | ~400 kHz | 10-40 MHz typical |
| Supported resolution | 128x64 | 240x240 |
| Memory consumption | Low | Medium-High |
| Visual quality | Monochrome | Color (if applicable) |
| Required scaling | 4x (32→128) | 7.5x (32→240) |

---

## 3. ST7789 SPI Connection Proposal

### 3.1 Recommended Pin Mapping

For the ST7789 display (240x240), the following pin mapping is proposed:

| ST7789 Display Pin | ESP32 Pin | Function | Notes |
|--------------------|-----------|----------|-------|
| GND                | GND       | Common ground | Same connection |
| VCC                | 3V3       | Power supply (3.3V) | Same connection |
| SCL                | G18       | SPI Clock | SPI SCK |
| SDA                | G23       | SPI Data (MOSI) | SPI MOSI |
| CS                 | G5        | Chip Select | SPI CS |
| DC                 | G19       | Data/Command | Control |
| RES                | G21       | Reset | Reset control |
| BL                 | G22       | Backlight | Optional PWM |

### 3.2 Pin Selection Justification

**Hardware SPI Pins (HSPI):**
- **G18 (SCK):** Dedicated pin for SPI Clock on ESP32
- **G23 (MOSI):** Dedicated pin for SPI Master Out on ESP32
- **G5 (CS):** Standard GPIO pin, suitable for Chip Select

**Control Pins:**
- **G19 (DC):** Freed from middle button (moved to another pin)
- **G21 (RES):** Reuses previous SDA pin (no longer used for I2C)
- **G22 (BL):** Reuses previous SCL pin, can be used for PWM backlight control

**Button Reassignment:**
- Middle Button: Moved from G19 to **G25** (or G26, G27 depending on availability)
- Left and Right Buttons: Remain on G17 and G4 respectively

### 3.3 Updated Connection Table

| Component | ESP32 Pin | Function | Change |
|-----------|-----------|----------|--------|
| **ST7789 Display** | | | |
| GND | GND | Ground | No change |
| VCC | 3V3 | Power supply | No change |
| SCL | G18 | SPI Clock | **NEW** |
| SDA | G23 | SPI Data | **NEW** |
| CS | G5 | Chip Select | **NEW** |
| DC | G19 | Data/Command | **NEW** |
| RES | G21 | Reset | Reuses G21 |
| BL | G22 | Backlight | Reuses G22 |
| **Buttons** | | | |
| Left Button | G17 | Digital input | No change |
| Middle Button | G25 | Digital input | **CHANGED** (previously G19) |
| Right Button | G4 | Digital input | No change |
| **Buzzer** | | | |
| Buzzer | G15 | PWM output | No change |

---

## 4. Electrical and Communication Considerations

### 4.1 Voltage Levels

- **ESP32:** 3.3V logic
- **ST7789:** Compatible with 3.3V
- **Power supply:** 3.3V from ESP32 (same as I2C)
- **Current:** ST7789 display may consume more current than I2C OLED (especially with active backlight)

### 4.2 SPI Signals

**Recommended SPI configuration:**
- **Speed:** 20-40 MHz (ESP32 supports up to 80 MHz)
- **SPI Mode:** MODE0 or MODE3 (according to ST7789 specification)
- **Bit order:** MSB First
- **Clock polarity:** CPOL = 0 (clock idle low)
- **Clock phase:** CPHA = 0 (sample on rising edge)

### 4.3 Control Lines

- **CS (Chip Select):** Active low, must remain low during transfer
- **DC (Data/Command):** 
  - High = Data
  - Low = Command
- **RES (Reset):** Active low, requires reset pulse at startup
- **BL (Backlight):** Can be controlled by PWM to adjust brightness

### 4.4 Power Consumption Considerations

- **I2C OLED Display:** ~20-40 mA
- **ST7789 Display:** ~50-150 mA (depending on backlight)
- **Recommendation:** Verify that the power supply can provide sufficient current

### 4.5 Pull-up/Pull-down Resistors

- **SPI:** Generally does not require external resistors (ESP32 has internal pull-ups)
- **CS, DC, RES:** May require pull-up if not defined in the module
- **Buttons:** Maintain current configuration (pull-down or pull-up according to design)

---

## 5. Required Hardware Changes

### 5.1 Required Physical Modifications

1. **Disconnect I2C pins:**
   - Disconnect G21 (SDA) from I2C display
   - Disconnect G22 (SCL) from I2C display

2. **Connect SPI pins:**
   - Connect G18 (SCK) to ST7789 SCL
   - Connect G23 (MOSI) to ST7789 SDA
   - Connect G5 to ST7789 CS
   - Connect G19 to ST7789 DC
   - Connect G21 to ST7789 RES
   - Connect G22 to ST7789 BL (optional, can go directly to 3V3)

3. **Reassign middle button:**
   - Move middle button connection from G19 to G25 (or available pin)

4. **Power supply:**
   - Maintain VCC and GND on 3V3 and GND respectively

### 5.2 Updated Connection Diagram

```
ESP32                    ST7789 (240x240)
------                   -----------------
3V3   ────────────────── VCC
GND   ────────────────── GND
G18   ────────────────── SCL (SPI Clock)
G23   ────────────────── SDA (SPI Data/MOSI)
G5    ────────────────── CS (Chip Select)
G19   ────────────────── DC (Data/Command)
G21   ────────────────── RES (Reset)
G22   ────────────────── BL (Backlight - optional)

Buttons:
G17   ────────────────── Left Button
G25   ────────────────── Middle Button (NEW)
G4    ────────────────── Right Button

Buzzer:
G15   ────────────────── Buzzer
```

---

## 6. Required Software Changes

### 6.1 Required Libraries

**Replace:**
- `U8g2lib.h` (I2C library)

**With:**
- `Adafruit_GFX.h` + `Adafruit_ST7789.h` (recommended)
- Or `TFT_eSPI.h` (more optimized alternative for ESP32)

### 6.2 platformio.ini Modifications

```ini
lib_deps = 
    Wire
    EEPROM
    adafruit/Adafruit GFX Library@^1.11.9
    adafruit/Adafruit ST7735 and ST7789 Library@^2.1.8
    # Or alternatively:
    # bodmer/TFT_eSPI@^2.5.43
```

### 6.3 main.cpp Changes

1. **Library includes:**
   - Remove `#include <U8g2lib.h>`
   - Remove `#include <Wire.h>`
   - Add SPI and ST7789 libraries

2. **Pin definitions:**
   - Update SPI pin definitions
   - Update middle button pin (G19 → G25)

3. **Display initialization:**
   - Change from `U8G2_SSD1306_128X64_NONAME_2_HW_I2C` to `Adafruit_ST7789`
   - Configure SPI with correct pins

4. **Drawing functions:**
   - Adapt `displayTama()` to use Adafruit_GFX functions
   - Adjust scaling from 32x16 to 240x240 (7.5x factor)
   - Adapt drawing functions (drawBox, drawXBMP, etc.)

5. **Orientation and resolution:**
   - Configure 240x240 resolution
   - Adjust offset and centering of Tamagotchi display

### 6.4 Performance Considerations

- **Display buffer:** ST7789 may require larger buffer
- **Update speed:** SPI is faster, but larger display
- **Memory:** Verify RAM availability on ESP32
- **Optimization:** Consider partial screen update

---

## 7. Migration Advantages and Disadvantages

### 7.1 Advantages

✅ **Higher resolution:** 240x240 vs 128x64 (better visual quality)
✅ **Higher speed:** SPI is significantly faster than I2C
✅ **Better scaling:** 7.5x factor allows better Tamagotchi representation
✅ **Color possibility:** If display is color, allows future improvements
✅ **Better for animations:** Higher refresh rate

### 7.2 Disadvantages

❌ **More pins required:** 5-6 pins vs 2 pins
❌ **Higher consumption:** Larger display consumes more energy
❌ **More complexity:** More connection lines
❌ **More complex code:** Requires significant adaptation
❌ **Higher memory usage:** Larger display buffer

---

## 8. Recommended Migration Plan

### Phase 1: Preparation
1. ✅ Current schematic analysis (COMPLETED)
2. ✅ Technical documentation (THIS DOCUMENT)
3. ⏳ Plan review and approval

### Phase 2: Hardware Modifications
1. ⏳ Disconnect I2C display
2. ⏳ Connect ST7789 display according to proposed schematic
3. ⏳ Reassign middle button
4. ⏳ Verify connections with multimeter

### Phase 3: Software Adaptation
1. ⏳ Update platformio.ini with new libraries
2. ⏳ Modify pin definitions
3. ⏳ Replace display initialization
4. ⏳ Adapt drawing functions
5. ⏳ Adjust scaling and positioning
6. ⏳ Testing and debugging

### Phase 4: Validation
1. ⏳ Functional testing
2. ⏳ Performance verification
3. ⏳ Optimization if necessary
4. ⏳ Final documentation

---

## 9. References and Resources

- **ST7789 Datasheet:** [Search in Sitronix or manufacturer]
- **Adafruit ST7789 Library:** https://github.com/adafruit/Adafruit_ST7789
- **TFT_eSPI Library:** https://github.com/Bodmer/TFT_eSPI
- **ESP32 SPI Documentation:** https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi_master.html

---

## 10. Final Notes

- This document is a technical guide prior to implementation
- It is recommended to test connections on a breadboard before soldering
- Verify exact compatibility of the acquired ST7789 module
- Some modules may have variations in pin order
- Backlight (BL) control may be optional depending on the module

---

**Creation date:** 2024
**Version:** 1.0
**Author:** Technical analysis for I2C → SPI ST7789 migration


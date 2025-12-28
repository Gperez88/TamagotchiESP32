# Connection Schematic: TamagotchiESP32 with SPI ST7789 Display

## Breadboard Schematic Description

This document describes the updated connection schematic for the TamagotchiESP32 project using an SPI ST7789 (240x240) display instead of the original I2C display.

---

## Main Components

### 1. Breadboard
The project is built on a standard white breadboard, oriented horizontally. It features:
- **Upper power rails**: Red (VCC/3.3V) and Blue (GND)
- **Lower power rails**: Red (VCC/3.3V) and Blue (GND)
- **Prototyping area**: Numbered columns (1-60) and lettered rows (A-J)

**Note**: The upper and lower rails are connected to each other to facilitate power distribution.

---

## 2. ESP32-WROOM-32 Development Board

**Location**: Center of the breadboard, occupying rows E and F, approximately from column 22 to 39.

### Power Connections:
- **3V3** (bottom row, column ~24): Connected via red wire to upper red rail (VCC)
- **GND** (top row, column ~22): Connected via black wire to upper blue rail (GND)
- **GND** (bottom row, column ~34): Connected via black wire to lower blue rail (GND)

---

## 3. SPI ST7789 Display (240x240)

**Location**: Left side of the breadboard, occupying rows E and F, approximately from column 8 to 15.

**ST7789 module pins** (from left to right):
1. **GND** - Ground
2. **VCC** - Power supply (3.3V)
3. **SCL** - SPI Clock
4. **SDA** - SPI Data (MOSI)
5. **RES** - Reset
6. **DC** - Data/Command
7. **CS** - Chip Select
8. **BL** - Backlight (optional)

### Detailed Connections:

| ST7789 Pin | ESP32 Pin | Wire Color | Function |
|------------|-----------|------------|----------|
| GND        | GND       | Blue/Black | Common ground |
| VCC        | 3V3       | Red        | 3.3V power supply |
| SCL        | G18       | Purple     | SPI Clock (SCK) |
| SDA        | G23       | Cyan       | SPI Data (MOSI) |
| CS         | G5        | Green      | Chip Select |
| DC         | G19       | Yellow     | Data/Command |
| RES        | G21       | Orange     | Reset |
| BL         | G22       | White      | Backlight (optional PWM) |

**Physical connections**:
- **GND**: Blue/black wire to upper blue rail (GND)
- **VCC**: Red wire to upper red rail (3.3V)
- **SCL (G18)**: Purple wire from ST7789 SCL to ESP32 G18 (top row, column ~26)
- **SDA (G23)**: Cyan wire from ST7789 SDA to ESP32 G23 (top row, column ~27)
- **CS (G5)**: Green wire from ST7789 CS to ESP32 G5 (top row, column ~25)
- **DC (G19)**: Yellow wire from ST7789 DC to ESP32 G19 (top row, column ~28)
- **RES (G21)**: Orange wire from ST7789 RES to ESP32 G21 (top row, column ~29)
- **BL (G22)**: White wire from ST7789 BL to ESP32 G22 (top row, column ~30)

**Note**: The BL pin can be connected directly to 3.3V if PWM backlight control is not required.

---

## 4. Push Buttons (3 units)

**Location**: Right side of the breadboard, arranged horizontally on rows E and F, approximately from column 48 to 59.

**Configuration**: Each button uses an external 1kΩ pull-up resistor (brown-black-red-gold).

### Left Button (Approx. columns 48-49):
- **Bottom-left pin**: Connected via yellow wire to lower blue rail (GND)
- **Top-right pin**: 
  - Connected via green wire to ESP32 **G17** (top row, column ~30)
  - Connected to 1kΩ resistor, which in turn connects via blue wire to upper red rail (VCC/3.3V)

### Middle Button (Approx. columns 53-54):
- **Bottom-left pin**: Connected via yellow wire to lower blue rail (GND)
- **Top-right pin**: 
  - Connected via green wire to ESP32 **G25** (top row, column ~31)
  - Connected to 1kΩ resistor, which in turn connects via blue wire to upper red rail (VCC/3.3V)

**⚠️ IMPORTANT CHANGE**: The middle button now connects to **G25** (previously it was on G16 or G19).

### Right Button (Approx. columns 58-59):
- **Bottom-left pin**: Connected via yellow wire to lower blue rail (GND)
- **Top-right pin**: 
  - Connected via green wire to ESP32 **G4** (top row, column ~33)
  - Connected to 1kΩ resistor, which in turn connects via blue wire to upper red rail (VCC/3.3V)

---

## 5. Passive Buzzer

**Location**: Center-bottom of the breadboard, below the ESP32, occupying rows A and B, around column 44. Black circular component.

### Connections:
- **Left pin**: Connected via black wire to lower blue rail (GND)
- **Right pin**: Connected via red wire to ESP32 **G15** (top row, column ~32)

---

## 6. Connection Summary

### Complete ESP32 Pin Table

| ESP32 Pin | Component | Function | Notes |
|-----------|-----------|----------|-------|
| 3V3       | VCC Rail  | Power supply | Distributed to all components |
| GND       | GND Rail  | Ground | Multiple connections |
| **G5**    | ST7789 CS | Chip Select | **NEW** |
| **G18**   | ST7789 SCL | SPI Clock | **NEW** |
| **G19**   | ST7789 DC | Data/Command | **NEW** (previously middle button) |
| **G21**   | ST7789 RES | Reset | Reused (previously I2C SDA) |
| **G22**   | ST7789 BL | Backlight | Reused (previously I2C SCL) |
| **G23**   | ST7789 SDA | SPI Data/MOSI | **NEW** |
| G17       | Left Button | Digital input | No changes |
| **G25**   | Middle Button | Digital input | **CHANGED** (previously G16/G19) |
| G4        | Right Button | Digital input | No changes |
| G15       | Buzzer     | PWM output | No changes |

### Summary of Changes from I2C to SPI

| Component | Change | Details |
|-----------|--------|---------|
| Display   | I2C → SPI | From 2 pins (G21, G22) to 6 pins (G5, G18, G19, G21, G22, G23) |
| Middle Button | G16/G19 → G25 | Reassigned to free G19 for ST7789 DC |
| G21       | I2C SDA → SPI RES | Reused as reset pin |
| G22       | I2C SCL → SPI BL | Reused for backlight control |

---

## 7. Simplified Connection Diagram

```
ESP32-WROOM-32                    ST7789 (240x240)
================                  =================
3V3   ─────────────────────────── VCC
GND   ─────────────────────────── GND
G18   ─────────────────────────── SCL (SPI Clock)
G23   ─────────────────────────── SDA (SPI Data/MOSI)
G5    ─────────────────────────── CS (Chip Select)
G19   ─────────────────────────── DC (Data/Command)
G21   ─────────────────────────── RES (Reset)
G22   ─────────────────────────── BL (Backlight)

Buttons:
G17   ───[1kΩ Res]───[Left Button]─── GND
G25   ───[1kΩ Res]───[Middle Button]── GND  ⚠️ NEW PIN
G4    ───[1kΩ Res]───[Right Button]─── GND

Buzzer:
G15   ─────────────────────────── Buzzer ─── GND
```

---

## 8. Assembly Considerations

### Recommended Connection Order:
1. **Power**: Connect ESP32 3V3 and GND to rails first
2. **ST7789 Display**: Connect GND, VCC, then control pins (RES, DC, CS), and finally SPI pins (SCL, SDA)
3. **Buttons**: Connect pull-up resistors and then buttons
4. **Buzzer**: Connect as last step

### Verifications:
- ✅ All GND must be connected to the same rail
- ✅ All power supplies (VCC) must be 3.3V (not 5V)
- ✅ Verify that G25 is connected to middle button (not G16 or G19)
- ✅ SPI pins (G18, G23) should have short and direct connections
- ✅ BL pin can be connected directly to 3.3V if PWM is not used

---

## 9. Additional Notes

### Differences from Original I2C Schematic:
- **More wires**: 6 pins for display vs 2 pins previously
- **Middle button reassigned**: Now on G25 instead of G16/G19
- **Reused pins**: G21 and G22 now have different functions

### Compatibility:
- This schematic is compatible with the updated firmware for ST7789
- Requires code modifications made in `main.cpp`
- Adafruit GFX and ST7789 libraries must be installed in PlatformIO

---

**Creation date**: 2024  
**Version**: 1.0  
**Based on**: I2C → SPI ST7789 Migration


# Esquema de Conexiones: TamagotchiESP32 con Pantalla SPI ST7789

## Descripción del Esquema en Breadboard

Este documento describe el esquema de conexiones actualizado para el proyecto TamagotchiESP32 utilizando una pantalla SPI ST7789 (240x240) en lugar de la pantalla I2C original.

---

## Componentes Principales

### 1. Breadboard
El proyecto está construido en una breadboard estándar blanca, orientada horizontalmente. Cuenta con:
- **Rieles de alimentación superiores**: Rojo (VCC/3.3V) y Azul (GND)
- **Rieles de alimentación inferiores**: Rojo (VCC/3.3V) y Azul (GND)
- **Área de prototipado**: Columnas numeradas (1-60) y filas etiquetadas (A-J)

**Nota**: Los rieles superior e inferior están conectados entre sí para facilitar la distribución de alimentación.

---

## 2. ESP32-WROOM-32 Development Board

**Ubicación**: Centro de la breadboard, ocupando las filas E y F, aproximadamente desde la columna 22 hasta la 39.

### Conexiones de Alimentación:
- **3V3** (fila inferior, columna ~24): Conectado mediante cable rojo al riel rojo superior (VCC)
- **GND** (fila superior, columna ~22): Conectado mediante cable negro al riel azul superior (GND)
- **GND** (fila inferior, columna ~34): Conectado mediante cable negro al riel azul inferior (GND)

---

## 3. Pantalla SPI ST7789 (240x240)

**Ubicación**: Lado izquierdo de la breadboard, ocupando las filas E y F, aproximadamente desde la columna 8 hasta la 15.

**Pines del módulo ST7789** (de izquierda a derecha):
1. **GND** - Tierra
2. **VCC** - Alimentación (3.3V)
3. **SCL** - SPI Clock
4. **SDA** - SPI Data (MOSI)
5. **RES** - Reset
6. **DC** - Data/Command
7. **CS** - Chip Select
8. **BL** - Backlight (opcional)

### Conexiones Detalladas:

| Pin ST7789 | Pin ESP32 | Color Cable | Función |
|------------|-----------|-------------|---------|
| GND        | GND       | Azul/Negro  | Tierra común |
| VCC        | 3V3       | Rojo        | Alimentación 3.3V |
| SCL        | G18       | Morado      | SPI Clock (SCK) |
| SDA        | G23       | Cian        | SPI Data (MOSI) |
| CS         | G5        | Verde       | Chip Select |
| DC         | G19       | Amarillo    | Data/Command |
| RES        | G21       | Naranja     | Reset |
| BL         | G22       | Blanco      | Backlight (PWM opcional) |

**Conexiones físicas**:
- **GND**: Cable azul/negro al riel azul superior (GND)
- **VCC**: Cable rojo al riel rojo superior (3.3V)
- **SCL (G18)**: Cable morado desde ST7789 SCL a ESP32 G18 (fila superior, columna ~26)
- **SDA (G23)**: Cable cian desde ST7789 SDA a ESP32 G23 (fila superior, columna ~27)
- **CS (G5)**: Cable verde desde ST7789 CS a ESP32 G5 (fila superior, columna ~25)
- **DC (G19)**: Cable amarillo desde ST7789 DC a ESP32 G19 (fila superior, columna ~28)
- **RES (G21)**: Cable naranja desde ST7789 RES a ESP32 G21 (fila superior, columna ~29)
- **BL (G22)**: Cable blanco desde ST7789 BL a ESP32 G22 (fila superior, columna ~30)

**Nota**: El pin BL puede conectarse directamente a 3.3V si no se requiere control PWM del backlight.

---

## 4. Botones Push (3 unidades)

**Ubicación**: Lado derecho de la breadboard, dispuestos horizontalmente en las filas E y F, aproximadamente desde la columna 48 hasta la 59.

**Configuración**: Cada botón utiliza una resistencia pull-up externa de 1kΩ (marrón-negro-rojo-dorado).

### Botón Izquierdo (Aprox. columnas 48-49):
- **Patilla inferior-izquierda**: Conectada mediante cable amarillo al riel azul inferior (GND)
- **Patilla superior-derecha**: 
  - Conectada mediante cable verde a ESP32 **G17** (fila superior, columna ~30)
  - Conectada a resistencia 1kΩ, que a su vez se conecta mediante cable azul al riel rojo superior (VCC/3.3V)

### Botón Medio (Aprox. columnas 53-54):
- **Patilla inferior-izquierda**: Conectada mediante cable amarillo al riel azul inferior (GND)
- **Patilla superior-derecha**: 
  - Conectada mediante cable verde a ESP32 **G25** (fila superior, columna ~31)
  - Conectada a resistencia 1kΩ, que a su vez se conecta mediante cable azul al riel rojo superior (VCC/3.3V)

**⚠️ CAMBIO IMPORTANTE**: El botón medio ahora se conecta a **G25** (anteriormente estaba en G16 o G19).

### Botón Derecho (Aprox. columnas 58-59):
- **Patilla inferior-izquierda**: Conectada mediante cable amarillo al riel azul inferior (GND)
- **Patilla superior-derecha**: 
  - Conectada mediante cable verde a ESP32 **G4** (fila superior, columna ~33)
  - Conectada a resistencia 1kΩ, que a su vez se conecta mediante cable azul al riel rojo superior (VCC/3.3V)

---

## 5. Buzzer Pasivo

**Ubicación**: Centro-inferior de la breadboard, debajo del ESP32, ocupando las filas A y B, alrededor de la columna 44. Componente circular negro.

### Conexiones:
- **Patilla izquierda**: Conectada mediante cable negro al riel azul inferior (GND)
- **Patilla derecha**: Conectada mediante cable rojo a ESP32 **G15** (fila superior, columna ~32)

---

## 6. Resumen de Conexiones

### Tabla Completa de Pines ESP32

| Pin ESP32 | Componente | Función | Notas |
|-----------|------------|---------|-------|
| 3V3       | Riel VCC   | Alimentación | Distribuido a todos los componentes |
| GND       | Riel GND   | Tierra | Múltiples conexiones |
| **G5**    | ST7789 CS  | Chip Select | **NUEVO** |
| **G18**   | ST7789 SCL | SPI Clock | **NUEVO** |
| **G19**   | ST7789 DC  | Data/Command | **NUEVO** (antes botón medio) |
| **G21**   | ST7789 RES | Reset | Reutilizado (antes I2C SDA) |
| **G22**   | ST7789 BL  | Backlight | Reutilizado (antes I2C SCL) |
| **G23**   | ST7789 SDA | SPI Data/MOSI | **NUEVO** |
| G17       | Botón Izq. | Entrada digital | Sin cambios |
| **G25**   | Botón Med. | Entrada digital | **CAMBIADO** (antes G16/G19) |
| G4        | Botón Der. | Entrada digital | Sin cambios |
| G15       | Buzzer     | Salida PWM | Sin cambios |

### Resumen de Cambios desde I2C a SPI

| Componente | Cambio | Detalle |
|------------|--------|---------|
| Pantalla   | I2C → SPI | De 2 pines (G21, G22) a 6 pines (G5, G18, G19, G21, G22, G23) |
| Botón Medio | G16/G19 → G25 | Reasignado para liberar G19 para ST7789 DC |
| G21        | I2C SDA → SPI RES | Reutilizado como pin de reset |
| G22        | I2C SCL → SPI BL | Reutilizado para control de backlight |

---

## 7. Diagrama de Conexiones Simplificado

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

Botones:
G17   ───[Res 1kΩ]───[Botón Izquierdo]─── GND
G25   ───[Res 1kΩ]───[Botón Medio]────── GND  ⚠️ NUEVO PIN
G4    ───[Res 1kΩ]───[Botón Derecho]──── GND

Buzzer:
G15   ─────────────────────────── Buzzer ─── GND
```

---

## 8. Consideraciones de Montaje

### Orden Recomendado de Conexión:
1. **Alimentación**: Conectar primero 3V3 y GND del ESP32 a los rieles
2. **Pantalla ST7789**: Conectar GND, VCC, luego pines de control (RES, DC, CS), y finalmente pines SPI (SCL, SDA)
3. **Botones**: Conectar resistencias pull-up y luego los botones
4. **Buzzer**: Conectar como último paso

### Verificaciones:
- ✅ Todos los GND deben estar conectados al mismo riel
- ✅ Todas las alimentaciones (VCC) deben ser 3.3V (no 5V)
- ✅ Verificar que G25 esté conectado al botón medio (no G16 o G19)
- ✅ Los pines SPI (G18, G23) deben tener conexiones cortas y directas
- ✅ El pin BL puede conectarse directamente a 3.3V si no se usa PWM

---

## 9. Notas Adicionales

### Diferencias con el Esquema I2C Original:
- **Más cables**: 6 pines para pantalla vs 2 pines anteriormente
- **Botón medio reasignado**: Ahora en G25 en lugar de G16/G19
- **Pines reutilizados**: G21 y G22 ahora tienen funciones diferentes

### Compatibilidad:
- Este esquema es compatible con el firmware actualizado para ST7789
- Requiere las modificaciones de código realizadas en `main.cpp`
- Las librerías Adafruit GFX y ST7789 deben estar instaladas en PlatformIO

---

**Fecha de creación**: 2024  
**Versión**: 1.0  
**Basado en**: Migración I2C → SPI ST7789


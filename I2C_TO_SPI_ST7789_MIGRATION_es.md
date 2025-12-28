# Documento Técnico: Migración de Pantalla I2C a SPI ST7789

## 1. Análisis del Esquema Actual (I2C)

### 1.1 Conexiones Actuales de la Pantalla I2C

Según el esquema Fritzing analizado, la pantalla OLED I2C (SSD1306 128x64) está conectada de la siguiente manera:

| Pin Pantalla I2C | Pin ESP32 | Función |
|------------------|-----------|---------|
| GND              | GND       | Tierra común |
| VDD              | 3V3       | Alimentación (3.3V) |
| SCK              | G22       | I2C Clock (SCL) |
| SDA              | G21       | I2C Data (SDA) |

### 1.2 Otras Conexiones del Sistema

| Componente | Pin ESP32 | Función |
|------------|-----------|---------|
| Botón Izquierdo | G17 | Entrada digital |
| Botón Medio | G16 | Entrada digital |
| Botón Derecho | G4 | Entrada digital |
| Buzzer | G15 | Salida PWM |

### 1.3 Implementación Software Actual

El código utiliza la librería **U8g2lib** con el driver:
- `U8G2_SSD1306_128X64_NONAME_2_HW_I2C` (Hardware I2C)
- Resolución física: 128x64 píxeles
- Resolución lógica Tamagotchi: 32x16 píxeles (escalado 4x)

---

## 2. Diferencias entre I2C y SPI

### 2.1 Protocolo I2C (Inter-Integrated Circuit)

**Características:**
- **Líneas de comunicación:** 2 (SDA, SCL)
- **Velocidad:** Relativamente lenta (100 kHz - 3.4 MHz)
- **Direccionamiento:** Por dirección de dispositivo (permite múltiples dispositivos en el mismo bus)
- **Ventajas:**
  - Menos pines requeridos (solo 2 + alimentación)
  - Fácil de conectar múltiples dispositivos
  - Menor consumo de pines GPIO
- **Desventajas:**
  - Velocidad limitada
  - Más complejo para transferencias rápidas de datos

**Pines requeridos:**
- SDA (Serial Data)
- SCL (Serial Clock)
- GND
- VCC

### 2.2 Protocolo SPI (Serial Peripheral Interface)

**Características:**
- **Líneas de comunicación:** 4-5 (MOSI, MISO, SCK, CS, opcionalmente DC/RES)
- **Velocidad:** Muy rápida (hasta 80 MHz en ESP32)
- **Direccionamiento:** Por línea CS (Chip Select) dedicada
- **Ventajas:**
  - Alta velocidad de transferencia
  - Ideal para pantallas de alta resolución
  - Transferencia full-duplex
  - Mejor para gráficos complejos y animaciones
- **Desventajas:**
  - Requiere más pines GPIO
  - Cada dispositivo necesita su propia línea CS

**Pines requeridos para ST7789:**
- SCL/SCK (Serial Clock)
- SDA/MOSI (Master Out Slave In - datos)
- CS (Chip Select)
- DC (Data/Command)
- RES (Reset)
- BL (Backlight - opcional)
- GND
- VCC

### 2.3 Comparación para Aplicación Tamagotchi

| Aspecto | I2C (Actual) | SPI (ST7789) |
|---------|--------------|--------------|
| Pines utilizados | 2 (G21, G22) | 5-6 (SCK, MOSI, CS, DC, RES, BL) |
| Velocidad | ~400 kHz | 10-40 MHz típico |
| Resolución soportada | 128x64 | 240x240 |
| Consumo de memoria | Bajo | Medio-Alto |
| Calidad visual | Monocromo | Color (si aplica) |
| Escalado necesario | 4x (32→128) | 7.5x (32→240) |

---

## 3. Propuesta de Conexión ST7789 SPI

### 3.1 Mapeo de Pines Recomendado

Para la pantalla ST7789 (240x240), se propone el siguiente mapeo de pines:

| Pin Pantalla ST7789 | Pin ESP32 | Función | Notas |
|---------------------|-----------|---------|-------|
| GND                 | GND       | Tierra común | Misma conexión |
| VCC                 | 3V3       | Alimentación (3.3V) | Misma conexión |
| SCL                 | G18       | SPI Clock | SPI SCK |
| SDA                 | G23       | SPI Data (MOSI) | SPI MOSI |
| CS                  | G5        | Chip Select | SPI CS |
| DC                  | G19       | Data/Command | Control |
| RES                 | G21       | Reset | Control de reset |
| BL                  | G22       | Backlight | PWM opcional |

### 3.2 Justificación de la Selección de Pines

**Pines SPI Hardware (HSPI):**
- **G18 (SCK):** Pin dedicado para SPI Clock en ESP32
- **G23 (MOSI):** Pin dedicado para SPI Master Out en ESP32
- **G5 (CS):** Pin GPIO estándar, adecuado para Chip Select

**Pines de Control:**
- **G19 (DC):** Liberado del botón medio (se mueve a otro pin)
- **G21 (RES):** Reutiliza el pin SDA anterior (ya no se usa para I2C)
- **G22 (BL):** Reutiliza el pin SCL anterior, puede usarse para control PWM del backlight

**Reasignación de Botones:**
- Botón Medio: Se mueve de G19 a **G25** (o G26, G27 según disponibilidad)
- Botones Izquierdo y Derecho: Se mantienen en G17 y G4 respectivamente

### 3.3 Tabla de Conexiones Actualizada

| Componente | Pin ESP32 | Función | Cambio |
|------------|-----------|---------|--------|
| **Pantalla ST7789** | | | |
| GND | GND | Tierra | Sin cambio |
| VCC | 3V3 | Alimentación | Sin cambio |
| SCL | G18 | SPI Clock | **NUEVO** |
| SDA | G23 | SPI Data | **NUEVO** |
| CS | G5 | Chip Select | **NUEVO** |
| DC | G19 | Data/Command | **NUEVO** |
| RES | G21 | Reset | Reutiliza G21 |
| BL | G22 | Backlight | Reutiliza G22 |
| **Botones** | | | |
| Botón Izquierdo | G17 | Entrada digital | Sin cambio |
| Botón Medio | G25 | Entrada digital | **CAMBIADO** (antes G19) |
| Botón Derecho | G4 | Entrada digital | Sin cambio |
| **Buzzer** | | | |
| Buzzer | G15 | Salida PWM | Sin cambio |

---

## 4. Consideraciones Eléctricas y de Comunicación

### 4.1 Niveles de Voltaje

- **ESP32:** Lógica 3.3V
- **ST7789:** Compatible con 3.3V
- **Alimentación:** 3.3V desde ESP32 (mismo que I2C)
- **Corriente:** La pantalla ST7789 puede consumir más corriente que OLED I2C (especialmente con backlight activo)

### 4.2 Señales SPI

**Configuración SPI recomendada:**
- **Velocidad:** 20-40 MHz (ESP32 soporta hasta 80 MHz)
- **Modo SPI:** MODE0 o MODE3 (según especificación ST7789)
- **Orden de bits:** MSB First
- **Polaridad de Clock:** CPOL = 0 (clock idle bajo)
- **Fase de Clock:** CPHA = 0 (muestreo en flanco de subida)

### 4.3 Líneas de Control

- **CS (Chip Select):** Activo bajo, debe mantenerse bajo durante la transferencia
- **DC (Data/Command):** 
  - Alto = Datos
  - Bajo = Comando
- **RES (Reset):** Activo bajo, requiere pulso de reset al inicio
- **BL (Backlight):** Puede ser controlado por PWM para ajustar brillo

### 4.4 Consideraciones de Consumo

- **Pantalla OLED I2C:** ~20-40 mA
- **Pantalla ST7789:** ~50-150 mA (dependiendo del backlight)
- **Recomendación:** Verificar que la fuente de alimentación pueda suministrar suficiente corriente

### 4.5 Resistores Pull-up/Pull-down

- **SPI:** Generalmente no requiere resistores externos (ESP32 tiene pull-ups internos)
- **CS, DC, RES:** Pueden requerir pull-up si no están definidos en el módulo
- **Botones:** Mantener configuración actual (pull-down o pull-up según diseño)

---

## 5. Cambios Necesarios en el Hardware

### 5.1 Modificaciones Físicas Requeridas

1. **Desconectar pines I2C:**
   - Desconectar G21 (SDA) de la pantalla I2C
   - Desconectar G22 (SCL) de la pantalla I2C

2. **Conectar pines SPI:**
   - Conectar G18 (SCK) a SCL de ST7789
   - Conectar G23 (MOSI) a SDA de ST7789
   - Conectar G5 a CS de ST7789
   - Conectar G19 a DC de ST7789
   - Conectar G21 a RES de ST7789
   - Conectar G22 a BL de ST7789 (opcional, puede ir directo a 3V3)

3. **Reasignar botón medio:**
   - Mover conexión del botón medio de G19 a G25 (o pin disponible)

4. **Alimentación:**
   - Mantener VCC y GND en 3V3 y GND respectivamente

### 5.2 Diagrama de Conexiones Actualizado

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
G22   ────────────────── BL (Backlight - opcional)

Botones:
G17   ────────────────── Botón Izquierdo
G25   ────────────────── Botón Medio (NUEVO)
G4    ────────────────── Botón Derecho

Buzzer:
G15   ────────────────── Buzzer
```

---

## 6. Cambios Necesarios en el Software

### 6.1 Librerías Requeridas

**Reemplazar:**
- `U8g2lib.h` (librería I2C)

**Por:**
- `Adafruit_GFX.h` + `Adafruit_ST7789.h` (recomendado)
- O `TFT_eSPI.h` (alternativa más optimizada para ESP32)

### 6.2 Modificaciones en platformio.ini

```ini
lib_deps = 
    Wire
    EEPROM
    adafruit/Adafruit GFX Library@^1.11.9
    adafruit/Adafruit ST7735 and ST7789 Library@^2.1.8
    # O alternativamente:
    # bodmer/TFT_eSPI@^2.5.43
```

### 6.3 Cambios en main.cpp

1. **Inclusión de librerías:**
   - Eliminar `#include <U8g2lib.h>`
   - Eliminar `#include <Wire.h>`
   - Agregar librerías SPI y ST7789

2. **Definición de pines:**
   - Actualizar definiciones de pines SPI
   - Actualizar pin del botón medio (G19 → G25)

3. **Inicialización del display:**
   - Cambiar de `U8G2_SSD1306_128X64_NONAME_2_HW_I2C` a `Adafruit_ST7789`
   - Configurar SPI con pines correctos

4. **Funciones de dibujo:**
   - Adaptar `displayTama()` para usar funciones de Adafruit_GFX
   - Ajustar escalado de 32x16 a 240x240 (factor 7.5x)
   - Adaptar funciones de dibujo (drawBox, drawXBMP, etc.)

5. **Orientación y resolución:**
   - Configurar resolución 240x240
   - Ajustar offset y centrado del display Tamagotchi

### 6.4 Consideraciones de Rendimiento

- **Buffer de pantalla:** ST7789 puede requerir buffer más grande
- **Velocidad de actualización:** SPI es más rápido, pero pantalla más grande
- **Memoria:** Verificar disponibilidad de RAM en ESP32
- **Optimización:** Considerar actualización parcial de pantalla

---

## 7. Ventajas y Desventajas de la Migración

### 7.1 Ventajas

✅ **Mayor resolución:** 240x240 vs 128x64 (mejor calidad visual)
✅ **Mayor velocidad:** SPI es significativamente más rápido que I2C
✅ **Mejor escalado:** Factor 7.5x permite mejor representación del Tamagotchi
✅ **Posibilidad de color:** Si la pantalla es a color, permite futuras mejoras
✅ **Mejor para animaciones:** Mayor tasa de refresco

### 7.2 Desventajas

❌ **Más pines requeridos:** 5-6 pines vs 2 pines
❌ **Mayor consumo:** Pantalla más grande consume más energía
❌ **Más complejidad:** Más líneas de conexión
❌ **Código más complejo:** Requiere adaptación significativa
❌ **Mayor uso de memoria:** Buffer de pantalla más grande

---

## 8. Plan de Migración Recomendado

### Fase 1: Preparación
1. ✅ Análisis del esquema actual (COMPLETADO)
2. ✅ Documentación técnica (ESTE DOCUMENTO)
3. ⏳ Revisión y aprobación del plan

### Fase 2: Modificaciones Hardware
1. ⏳ Desconectar pantalla I2C
2. ⏳ Conectar pantalla ST7789 según esquema propuesto
3. ⏳ Reasignar botón medio
4. ⏳ Verificar conexiones con multímetro

### Fase 3: Adaptación Software
1. ⏳ Actualizar platformio.ini con nuevas librerías
2. ⏳ Modificar definiciones de pines
3. ⏳ Reemplazar inicialización del display
4. ⏳ Adaptar funciones de dibujo
5. ⏳ Ajustar escalado y posicionamiento
6. ⏳ Pruebas y depuración

### Fase 4: Validación
1. ⏳ Pruebas funcionales
2. ⏳ Verificación de rendimiento
3. ⏳ Optimización si es necesario
4. ⏳ Documentación final

---

## 9. Referencias y Recursos

- **ST7789 Datasheet:** [Buscar en Sitronix o fabricante]
- **Adafruit ST7789 Library:** https://github.com/adafruit/Adafruit_ST7789
- **TFT_eSPI Library:** https://github.com/Bodmer/TFT_eSPI
- **ESP32 SPI Documentation:** https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi_master.html

---

## 10. Notas Finales

- Este documento es una guía técnica previa a la implementación
- Se recomienda probar las conexiones en protoboard antes de soldar
- Verificar la compatibilidad exacta del módulo ST7789 adquirido
- Algunos módulos pueden tener variaciones en el orden de pines
- El control del backlight (BL) puede ser opcional según el módulo

---

**Fecha de creación:** 2024
**Versión:** 1.0
**Autor:** Análisis técnico para migración I2C → SPI ST7789


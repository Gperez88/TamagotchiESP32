/*
 * ArduinoGotchi - A real Tamagotchi emulator for Arduino ESP32
 *
 * Copyright (C) 2022 Gary Kwok - Arduino Uno Implementation
 * Copyright (C) 2022 Marcel Ochsendorf - ESP32 Plattform Support
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <pgmspace.h>

#include "tamalib.h"
#include "hw.h"
#include "bitmaps.h"
#if defined(ENABLE_AUTO_SAVE_STATUS) || defined(ENABLE_LOAD_STATE_FROM_EEPROM)
#include "savestate.h"
#endif

#if defined(ESP32)
#include "Tone32.h"
#endif

/*** Display Configuration ***/
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 240

/***** ST7789 Display Pin Definitions *****/
#if defined(ESP32)
#define TFT_CS    5   // Chip Select
#define TFT_DC    19  // Data/Command
#define TFT_RST   21  // Reset
#define TFT_BL    22  // Backlight (optional)
#define TFT_MOSI  23  // SPI Data (MOSI)
#define TFT_SCLK  18  // SPI Clock (SCK)
#endif
/**************************************************************************/

// Initialize ST7789 display (240x240)
#if defined(ESP32)
// Use HSPI for ST7789 with custom pins
SPIClass hspi(HSPI);
Adafruit_ST7789 display = Adafruit_ST7789(&hspi, TFT_CS, TFT_DC, TFT_RST);
#else
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
#endif

#if defined(ESP32)
#define PIN_BTN_L 17
#define PIN_BTN_M 25
#define PIN_BTN_R 4
#define PIN_BUZZER 15
#define BUZZER_CHANNEL 0
#define TONE_CHANNEL 15
#elif defined(ESP8266)
#define PIN_BTN_L 12
#define PIN_BTN_M 13
#define PIN_BTN_R 15
#define PIN_BUZZER 2
#else
#define PIN_BTN_L 2
#define PIN_BTN_M 3
#define PIN_BTN_R 4
#define PIN_BUZZER 9
#endif

#if defined(ESP32)
void esp32_noTone(uint8_t pin, uint8_t channel)
{
  ledcDetachPin(pin);
  ledcWrite(channel, 0);
}

void esp32_tone(uint8_t pin, unsigned int frequency, unsigned long duration, uint8_t channel)
{
  if (!ledcRead(channel))
  {
    ledcAttachPin(pin, channel);
  }

  ledcWriteTone(channel, frequency);
}
#endif

void displayTama();

/**** TamaLib Specific Variables ****/
static uint16_t current_freq = 0;
static bool_t matrix_buffer[LCD_HEIGHT][LCD_WIDTH / 8] = {{0}};
// static byte runOnceBool = 0;
static bool_t icon_buffer[ICON_NUM] = {0};
static cpu_state_t cpuState;
static unsigned long lastSaveTimestamp = 0;
static long last_interaction = 0;
/************************************/

static void hal_halt(void)
{
  // Serial.println("Halt!");
}

static void hal_log(log_level_t level, char *buff, ...)
{
  Serial.println(buff);
}

static void hal_sleep_until(timestamp_t ts)
{
  int32_t remaining = (int32_t)(ts - hal_get_timestamp());
  if (remaining > 0)
  {
#ifdef ENABLE_DEEPSLEEP
    enter_deepsleep(remaining);
#endif
  }
}

static timestamp_t hal_get_timestamp(void)
{
  return millis() * (1000 / SPEED_DIVIDER);
}

static void hal_update_screen(void)
{
  displayTama();
}

static void hal_set_lcd_matrix(u8_t x, u8_t y, bool_t val)
{
  uint8_t mask;
  if (val)
  {
    mask = 0b10000000 >> (x % 8);
    matrix_buffer[y][x / 8] = matrix_buffer[y][x / 8] | mask;
  }
  else
  {
    mask = 0b01111111;
    for (byte i = 0; i < (x % 8); i++)
    {
      mask = (mask >> 1) | 0b10000000;
    }
    matrix_buffer[y][x / 8] = matrix_buffer[y][x / 8] & mask;
  }
}

static void hal_set_lcd_icon(u8_t icon, bool_t val)
{
  icon_buffer[icon] = val;
}

static void hal_set_frequency(u32_t freq)
{
  current_freq = freq;
}

static void hal_play_frequency(bool_t en)
{
#ifdef ENABLE_TAMA_SOUND
  if (en)
  {

#if defined(ESP32)
    esp32_tone(PIN_BUZZER, current_freq, 500, BUZZER_CHANNEL);
#else
    tone(PIN_BUZZER, current_freq);
#endif
  }
  else
  {
#if defined(ESP32)
    esp32_noTone(PIN_BUZZER, BUZZER_CHANNEL);
#else
    noTone(PIN_BUZZER);
#ifdef ENABLE_TAMA_SOUND_ACTIVE_LOW
    digitalWrite(PIN_BUZZER, HIGH);
#endif
#endif
  }
#endif
}

static bool_t button4state = 0;

static int hal_handler(void)
{
#ifdef ENABLE_SERIAL_DEBUG_INPUT
  if (Serial.available() > 0)
  {
    int incomingByte = Serial.read();
    Serial.println(incomingByte, DEC);
    if (incomingByte == 49)
    {
      hw_set_button(BTN_LEFT, BTN_STATE_PRESSED);
    }
    else if (incomingByte == 50)
    {
      hw_set_button(BTN_LEFT, BTN_STATE_RELEASED);
    }
    else if (incomingByte == 51)
    {
      hw_set_button(BTN_MIDDLE, BTN_STATE_PRESSED);
    }
    else if (incomingByte == 52)
    {
      hw_set_button(BTN_MIDDLE, BTN_STATE_RELEASED);
    }
    else if (incomingByte == 53)
    {
      hw_set_button(BTN_RIGHT, BTN_STATE_PRESSED);
    }
    else if (incomingByte == 54)
    {
      hw_set_button(BTN_RIGHT, BTN_STATE_RELEASED);
    }
  }
#endif

  if (digitalRead(PIN_BTN_L) == BUTTON_VOLTAGE_LEVEL_PRESSED)
  {
    hw_set_button(BTN_LEFT, BTN_STATE_PRESSED);
  }
  else
  {
    hw_set_button(BTN_LEFT, BTN_STATE_RELEASED);
  }

  if (digitalRead(PIN_BTN_M) == BUTTON_VOLTAGE_LEVEL_PRESSED)
  {
    hw_set_button(BTN_MIDDLE, BTN_STATE_PRESSED);
  }
  else
  {
    hw_set_button(BTN_MIDDLE, BTN_STATE_RELEASED);
  }

  if (digitalRead(PIN_BTN_R) == BUTTON_VOLTAGE_LEVEL_PRESSED)
  {
    hw_set_button(BTN_RIGHT, BTN_STATE_PRESSED);
  }
  else
  {
    hw_set_button(BTN_RIGHT, BTN_STATE_RELEASED);
  }
  return 0;
}

static hal_t hal = {
    .halt = &hal_halt,
    .log = &hal_log,
    .sleep_until = &hal_sleep_until,
    .get_timestamp = &hal_get_timestamp,
    .update_screen = &hal_update_screen,
    .set_lcd_matrix = &hal_set_lcd_matrix,
    .set_lcd_icon = &hal_set_lcd_icon,
    .set_frequency = &hal_set_frequency,
    .play_frequency = &hal_play_frequency,
    .handler = &hal_handler,
};

// Scaling factor: 240/32 = 7.5x
#define SCALE_FACTOR 7.5
#define OFFSET_X 0    // Horizontal offset (centered)
#define OFFSET_Y 60   // Vertical offset to center 120px height in 240px screen

void drawTriangle(uint8_t x, uint8_t y)
{
  // Scale coordinates
  uint16_t sx = (uint16_t)(x * SCALE_FACTOR) + OFFSET_X;
  uint16_t sy = (uint16_t)(y * SCALE_FACTOR) + OFFSET_Y;
  uint16_t scale = (uint16_t)SCALE_FACTOR;
  
  display.drawLine(sx + scale, sy + scale, sx + 5*scale, sy + scale);
  display.drawLine(sx + 2*scale, sy + 2*scale, sx + 4*scale, sy + 2*scale);
  display.drawLine(sx + 3*scale, sy + 3*scale, sx + 3*scale, sy + 3*scale);
}

void drawTamaRow(uint8_t tamaLCD_y, uint8_t ActualLCD_y, uint8_t thick)
{
  uint8_t i;
  for (i = 0; i < LCD_WIDTH; i++)
  {
    uint8_t mask = 0b10000000;
    mask = mask >> (i % 8);
    if ((matrix_buffer[tamaLCD_y][i / 8] & mask) != 0)
    {
      // Scale pixel position: original was 3*i + 16, now 7.5*i + offset
      uint16_t x = (uint16_t)(i * SCALE_FACTOR) + OFFSET_X;
      uint16_t y = (uint16_t)(ActualLCD_y * SCALE_FACTOR) + OFFSET_Y;
      uint16_t w = (uint16_t)SCALE_FACTOR;
      uint16_t h = (uint16_t)(thick * SCALE_FACTOR);
      display.fillRect(x, y, w, h, ST77XX_WHITE);
    }
  }
}

void drawTamaSelection(uint8_t y)
{
  uint8_t i;
  for (i = 0; i < 7; i++)
  {
    if (icon_buffer[i])
      drawTriangle(i * 16 + 5, y);
    // Scale bitmap position and size
    uint16_t bx = (uint16_t)((i * 16 + 4) * SCALE_FACTOR) + OFFSET_X;
    uint16_t by = (uint16_t)((y + 6) * SCALE_FACTOR) + OFFSET_Y;
    // Draw bitmap at original size, then scale manually by drawing each pixel
    // Bitmap is 16x9, stored as 18 bytes (16*9/8 = 18)
    // Each row is 16 bits = 2 bytes, 9 rows total
    for (uint8_t by_pixel = 0; by_pixel < 9; by_pixel++)
    {
      for (uint8_t bx_pixel = 0; bx_pixel < 16; bx_pixel++)
      {
        uint8_t byte_idx = by_pixel * 2 + (bx_pixel / 8);
        uint8_t bit_idx = 7 - (bx_pixel % 8);
        uint8_t byte_val = pgm_read_byte(bitmaps + i * 18 + byte_idx);
        if (byte_val & (1 << bit_idx))
        {
          // Draw scaled pixel
          uint16_t px = bx + (uint16_t)(bx_pixel * SCALE_FACTOR);
          uint16_t py = by + (uint16_t)(by_pixel * SCALE_FACTOR);
          uint16_t pw = (uint16_t)SCALE_FACTOR;
          uint16_t ph = (uint16_t)SCALE_FACTOR;
          display.fillRect(px, py, pw, ph, ST77XX_WHITE);
        }
      }
    }
  }
  if (icon_buffer[7])
  {
    drawTriangle(7 * 16 + 5, y);
    uint16_t bx = (uint16_t)((7 * 16 + 4) * SCALE_FACTOR) + OFFSET_X;
    uint16_t by = (uint16_t)((y + 6) * SCALE_FACTOR) + OFFSET_Y;
    for (uint8_t by_pixel = 0; by_pixel < 9; by_pixel++)
    {
      for (uint8_t bx_pixel = 0; bx_pixel < 16; bx_pixel++)
      {
        uint8_t byte_idx = by_pixel * 2 + (bx_pixel / 8);
        uint8_t bit_idx = 7 - (bx_pixel % 8);
        uint8_t byte_val = pgm_read_byte(bitmaps + 7 * 18 + byte_idx);
        if (byte_val & (1 << bit_idx))
        {
          uint16_t px = bx + (uint16_t)(bx_pixel * SCALE_FACTOR);
          uint16_t py = by + (uint16_t)(by_pixel * SCALE_FACTOR);
          uint16_t pw = (uint16_t)SCALE_FACTOR;
          uint16_t ph = (uint16_t)SCALE_FACTOR;
          display.fillRect(px, py, pw, ph, ST77XX_WHITE);
        }
      }
    }
  }
}

void displayTama()
{
  uint8_t j;
  
  // Clear screen with black background
  display.fillScreen(ST77XX_BLACK);
  
  // Draw Tamagotchi display area (32x16 scaled to 240x120, centered)
  for (j = 0; j < LCD_HEIGHT; j++)
  {
    if (j != 5)
    {
      // Original: j + j + j = 3*j, scaled: 3*j * 7.5 = 22.5*j
      drawTamaRow(j, 3 * j, 2);
    }
    else
    {
      // Special handling for row 5 (split row in original)
      drawTamaRow(j, 3 * j, 1);
      drawTamaRow(j, 3 * j + 1, 1);
    }
  }
  
  // Draw icon selection area at bottom
  // Original y position was 49, scaled: 49 * 7.5 = 367.5, but we want it at bottom
  // Icons are at the bottom of the 16-row display, so position at y = 16 (end of display)
  drawTamaSelection(16);
}

#ifdef ENABLE_DUMP_STATE_TO_SERIAL_WHEN_START
void dumpStateToSerial()
{
  uint16_t i, count = 0;
  char tmp[10];
  cpu_get_state(&cpuState);
  u4_t *memTemp = cpuState.memory;
  uint8_t *cpuS = (uint8_t *)&cpuState;

  Serial.println("");
  Serial.println("static const uint8_t hardcodedState[] PROGMEM = {");
  for (i = 0; i < sizeof(cpu_state_t); i++, count++)
  {
    sprintf(tmp, "0x%02X,", cpuS[i]);
    Serial.print(tmp);
    if ((count % 16) == 15)
      Serial.println("");
  }
  for (i = 0; i < MEMORY_SIZE; i++, count++)
  {
    sprintf(tmp, "0x%02X,", memTemp[i]);
    Serial.print(tmp);
    if ((count % 16) == 15)
      Serial.println("");
  }
  Serial.println("};");
  /*
    Serial.println("");
    Serial.println("static const uint8_t bitmaps[] PROGMEM = {");
    for(i=0;i<144;i++) {
      sprintf(tmp, "0x%02X,", bitmaps_raw[i]);
      Serial.print(tmp);
      if ((i % 18)==17) Serial.println("");
    }
    Serial.println("};");  */
}
#endif

uint8_t reverseBits(uint8_t num)
{
  uint8_t reverse_num = 0;
  uint8_t i;
  for (i = 0; i < 8; i++)
  {
    if ((num & (1 << i)))
      reverse_num |= 1 << ((8 - 1) - i);
  }
  return reverse_num;
}

void setup()
{
  Serial.begin(SERIAL_BAUD);

  pinMode(PIN_BTN_L, INPUT);
  pinMode(PIN_BTN_M, INPUT);
  pinMode(PIN_BTN_R, INPUT);

#if defined(ESP32)
  ledcSetup(BUZZER_CHANNEL, NOTE_C4, 8);
  
  // Initialize backlight pin if defined
  #ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Turn on backlight
  #endif
  
  // Initialize SPI bus for ST7789
  hspi.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
#endif

  // Initialize ST7789 display
  display.init(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  display.setRotation(0);
  display.fillScreen(ST77XX_BLACK);

  tamalib_register_hal(&hal);
  tamalib_set_framerate(TAMA_DISPLAY_FRAMERATE);
  tamalib_init(1000000);

#if defined(ENABLE_AUTO_SAVE_STATUS) || defined(ENABLE_LOAD_STATE_FROM_EEPROM)
  initEEPROM();
#endif

#ifdef ENABLE_LOAD_STATE_FROM_EEPROM
  if (validEEPROM())
  {
    loadStateFromEEPROM(&cpuState);
  }
  else
  {
    Serial.println(F("No magic number in state, skipping state restore"));
  }
#elif ENABLE_LOAD_HARCODED_STATE_WHEN_START
  loadHardcodedState();
#endif

#ifdef ENABLE_DUMP_STATE_TO_SERIAL_WHEN_START
  dumpStateToSerial();
#endif
}

uint32_t right_long_press_started = 0;

void upload_state()
{
}

void enter_deepsleep(int _ms)
{
#ifndef ENABLE_DEEPSLEEP
  return;
#endif
  // save CURRENT STATE
  saveStateToEEPROM(&cpuState);


  //DISABLE DISPLAY
  display.fillScreen(ST77XX_BLACK);
  
#if defined(ESP32)
  #ifdef TFT_BL
  digitalWrite(TFT_BL, LOW); // Turn off backlight
  #endif
#endif

  // ENTER DEEPSLEEP
#if defined(ESP32)
  esp_sleep_enable_timer_wakeup(_ms * 1000);
  esp_deep_sleep_start();
#elif defined(ESP8266)
  ESP.deepSleep(_ms * 1000);
  yield();
#endif
}

void loop()
{
  tamalib_mainloop_step_by_step();
#ifdef ENABLE_AUTO_SAVE_STATUS
  if ((millis() - lastSaveTimestamp) > (AUTO_SAVE_MINUTES * 60 * 1000))
  {
    lastSaveTimestamp = millis();
    saveStateToEEPROM(&cpuState);
  }

  if (digitalRead(PIN_BTN_M) == BUTTON_VOLTAGE_LEVEL_PRESSED)
  {
    if (millis() - right_long_press_started > AUTO_SAVE_MINUTES * 1000)
    {
      eraseStateFromEEPROM();
#if defined(ESP8266) || defined(ESP32)
      ESP.restart();
#endif
    }
  }
  else if (digitalRead(PIN_BTN_R) == BUTTON_VOLTAGE_LEVEL_PRESSED)
  {
    if (millis() - right_long_press_started > AUTO_SAVE_MINUTES * 1000)
    {
#if defined(ESP8266) || defined(ESP32)
      upload_state();
#endif
    }
  }
  else if (digitalRead(PIN_BTN_L) == BUTTON_VOLTAGE_LEVEL_PRESSED)
  {
    if (millis() - right_long_press_started > AUTO_SAVE_MINUTES * 1000)
    {
      enter_deepsleep(DEEPSLEEP_INTERVAL * 1000);
    }
    else
    {
      right_long_press_started = millis();
    }
#endif
  }
#pragma once

#include "freertos/FreeRTOS.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include <string.h>

#define TICKS pdMS_TO_TICKS(100)
#define I2C_NUM I2C_NUM_0
#define TAG "SSD1315"

void draw_cube(int x, int y, int size);
void draw_rectangle(int x, int y, int width, int height);
void draw_frame(int x, int y, int width, int height);
void draw_line(int x0, int y0, int x1, int y1);
void draw_circle(int xc, int yc, int r);
void draw_text(int x, int y, const char *text);

void clear_display(void);
void update_display(void);

void oled_sleep(void);
void oled_wake_up(void);
void set_contrast(uint8_t value);
void inverse(bool inverse);

void init(gpio_num_t sda, gpio_num_t scl, uint8_t addr);
#include "include/ssd1315.h"

static uint8_t fb[128 * 8];
static uint8_t ADDR;

// Our fonts
#define FONT_WIDTH 5
#define FONT_HEIGHT 7

static const uint8_t font_eng[26][5] = {
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x3F,0x40,0x38,0x40,0x3F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x07,0x08,0x70,0x08,0x07}, // Y
    {0x61,0x51,0x49,0x45,0x43}  // Z
};

static const uint8_t font_num[10][5] = {
    {0x7F,0x41,0x41,0x41,0x7f}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x79,0x49,0x49,0x49,0x4F}, // 2
    {0x41,0x49,0x49,0x49,0x7F}, // 3
    {0x0F,0x08,0x08,0x08,0x7F}, // 4
    {0x4F,0x49,0x49,0x49,0x79}, // 5
    {0x7F,0x49,0x49,0x49,0x79}, // 6
    {0x01,0x01,0x01,0x01,0x7F}, // 7
    {0x7F,0x49,0x49,0x49,0x7F}, // 8
    {0x4F,0x49,0x49,0x49,0x7F}  // 9
};


void i2c_init(gpio_num_t sda, gpio_num_t scl) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 200000
    };

    i2c_param_config(I2C_NUM, &conf);
    i2c_driver_install(I2C_NUM, conf.mode, 0, 0, 0);
}


// Essential functions
esp_err_t write_reg(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, const uint8_t *buf, size_t len, TickType_t ticks_to_wait) {
    if(len > 0 && buf == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if(len > 256) {
        return ESP_ERR_INVALID_SIZE;
    }

    size_t total = len + 1;
    uint8_t *tmp = (uint8_t *) malloc(total);
    if(tmp == NULL) {
        return ESP_ERR_NO_MEM;
    }

    tmp[0] = reg;
    if(len > 0) {
        memcpy(&tmp[1], buf, len);
    }

    esp_err_t res = i2c_master_write_to_device(i2c_num, addr, tmp, total, ticks_to_wait);

    free(tmp);

    return res;
}

esp_err_t write_cmd(uint8_t cmd) {
    return write_reg(I2C_NUM, ADDR, 0x00, &cmd, 1, TICKS);
}

esp_err_t write_data(const uint8_t *data, size_t len) {
    const size_t chunk = 256;
    for (size_t i = 0; i < len; i += chunk) {
        size_t sz = (len - i > chunk) ? chunk : (len - i);
        if(write_reg(I2C_NUM, ADDR, 0x40, data + i, sz, TICKS) != ESP_OK) {
            return ESP_ERR_INVALID_RESPONSE;
        }
    }

    return ESP_OK;
}


// Drawing functions
void set_pixel(int x, int y) {
    if (x >= 128 || y >= 64) return;

    int page = y / 8;
    int bit = y % 8;
    int index = page * 128 + x;

    fb[index] |= (1 << bit);
}

void draw_cube(int x, int y, int size) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            set_pixel(x + i, y + j);
        }
    }
}

void draw_rectangle(int x, int y, int width, int height) {
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            set_pixel(x + i, y + j);
        }
    }
}

void draw_frame(int x, int y, int width, int height) {
    for(int i = 0; i <= width; i++) {
        for(int j = 0; j <= height; j++) {
            set_pixel(x + i, y);
            set_pixel(x, y + j);
        }
    }

    for(int i = x + width; i > x; i--) {
        for(int j = y + height; j > y; j--) {
            set_pixel(i, y + height);
            set_pixel(x + width, j);
        }
    }
}

void draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        set_pixel(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
    }
}

void draw_circle(int xc, int yc, int r) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (y >= x) {
        for (int i = xc - x; i <= xc + x; i++) {
            set_pixel(i, yc + y);
            set_pixel(i, yc - y);
        }
        for (int i = xc - y; i <= xc + y; i++) {
            set_pixel(i, yc + x);
            set_pixel(i, yc - x);
        }

        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

void draw_char(int x, int y, char c) {
    const uint8_t *bitmap = NULL;
    
    if (c >= 'A' && c <= 'Z') {
        bitmap = font_eng[c - 'A'];
    } else if (c >= 'a' && c <= 'z') {
        bitmap = font_eng[c - 'a'];
    } else if (c >= '0' && c <= '9') {
        bitmap = font_num[c - '0'];
    }

    if (!bitmap) return;

    for (int col = 0; col < FONT_WIDTH; col++) {
        uint8_t line = bitmap[col];
        for (int row = 0; row < FONT_HEIGHT; row++) {
            if (line & (1 << row)) set_pixel(x + col, y + row);
        }
    }
}

void draw_text(int x, int y, const char *text) {
    int cursor_x = x;
    while (*text) {
        draw_char(cursor_x, y, *text++);
        cursor_x += FONT_WIDTH + 1;
    }
}

void update_display(void) {
    for (int page = 0; page < 8; page++) {
        write_cmd(0xB0 | page);
        write_cmd(0x02);
        write_cmd(0x10);
        write_data(&fb[page * 128], 128);
    }
}

void clear_display(void) {\
    memset(fb, 0x00, sizeof(fb));
}


// Helpful functions
void oled_sleep(void) {
    if(write_cmd(0xAE) != ESP_OK) {
        ESP_LOGE(TAG, "Display failed to sleep");
    }

    ESP_LOGI(TAG, "Display went to sleep");
}

void oled_wake_up(void) {
    if(write_cmd(0xAF) != ESP_OK) {
        ESP_LOGE(TAG, "Display failed to wake up");
    }

    ESP_LOGI(TAG, "Display woke up");
}

void set_contrast(uint8_t value) {
    write_cmd(0x81);
    if(write_cmd(value) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to change contrast");
    }

    ESP_LOGI(TAG, "Changed contrast to - %u", value);
}

void inverse(bool inverse) {
    if(inverse) {
        if(write_cmd(0xA7) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to change contrast");
        }
    } else {
        if(write_cmd(0xA6) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to change contrast");
        }
    }

    ESP_LOGI(TAG, "Display inversed - %d", inverse);
}


// Main function
void init(gpio_num_t sda, gpio_num_t scl, uint8_t addr) {
    // Initializing I2C
    i2c_init(sda, scl);
    ADDR = addr;

    // Display off
    vTaskDelay(pdMS_TO_TICKS(20));
    oled_sleep();

    // Clock divide
    write_cmd(0xD5);
    write_cmd(0x80);

    // Multiplex ratio
    write_cmd(0xA8);
    write_cmd(0x3F);

    // Start line
    write_cmd(0x40);

    // Charge pump
    write_cmd(0x8D);
    write_cmd(0x14);
    vTaskDelay(pdMS_TO_TICKS(100));

    // Memmory addressing
    write_cmd(0x20);
    write_cmd(0x00);

    // Segment
    write_cmd(0xA1);
    
    // COM
    write_cmd(0xC8);

    // COM Pins
    write_cmd(0xDA);
    write_cmd(0x12);

    // Contrast
    set_contrast(0x7F);

    // Pre-charge
    write_cmd(0xD9);
    write_cmd(0xF1);

    // Not Inverted
    inverse(false);

    // Column, pages
    write_cmd(0x21);
    write_cmd(0x00);
    write_cmd(0x7F);

    write_cmd(0x22);
    write_cmd(0x00);
    write_cmd(0x07);

    // Display On
    oled_wake_up();

    // Clearing the display
    clear_display();
    update_display();
    
    ESP_LOGI(TAG, "Initialization complete");
}
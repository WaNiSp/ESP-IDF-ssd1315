# SSD1315 OLED Display Driver for ESP-IDF

A lightweight and efficient driver for SSD1315 128x64 OLED displays using ESP32 and ESP-IDF framework. Provides simple drawing functions and display control.

## Features

- Simple initialization and configuration
- Basic drawing primitives (shapes, lines, text)
- Display power management
- Customizable contrast and inversion
- Efficient framebuffer-based rendering

## Installation

### Method 1: As ESP-IDF Component

1. Copy the driver to your project's components directory:
   ```bash
   cd your-project/
   mkdir -p components
   cp -r ssd1315 components/
   ```

2. Update your main `CMakeLists.txt` to include the component:
   ```cmake
   idf_component_register(
      SRCS "main.c"
      INCLUDE_DIRS "."
      REQUIRES driver ssd1315
   )
   ```

### Method 2: Direct Inclusion

Simply copy `ssd1315.h` and `ssd1315.c` files into your project source directory.

## Usage

### Basic Setup

```c
#include "ssd1315.h"

void app_main(void) {
    // Initialize display with I2C pins and address
    ssd1315_init(GPIO_NUM_21, GPIO_NUM_22, 0x3C);
    
    // Draw a cube
    draw_cube(10, 10, 8);
    
    // Update display to show changes
    update_display();
}
```

### Available Functions

#### Drawing Functions
- `draw_cube(int x, int y, int size)` - Draw a square
- `draw_rectangle(int x, int y, int width, int height)` - Draw a rectangle
- `draw_frame(int x, int y, int width, int height)` - Draw an empty frame
- `draw_line(int x0, int y0, int x1, int y1)` - Draw a line between two points
- `draw_circle(int xc, int yc, int r)` - Draw a circle
- `draw_text(int x, int y, const char *text)` - Draw text (A-Z, 0-9)

#### Display Control
- `clear_display(void)` - Clear the framebuffer
- `update_display(void)` - Send framebuffer to display
- `oled_sleep(void)` - Put display to sleep
- `oled_wake_up(void)` - Wake up display
- `set_contrast(uint8_t value)` - Set contrast (0-255 (hex))
- `inverse(bool inverse)` - Enable/disable display inversion

#### Initialization
- `ssd1315_init(gpio_num_t sda, gpio_num_t scl, uint8_t addr)` - Initialize display with specified I2C pins and address of display

## Example

Check `main/main.c` for a  usage example demonstrating:
- Display initialization
- Basic shape drawing
- Moving objects

## Dependencies

- ESP-IDF (FreeRTOS)
- I2C driver
- GPIO driver

## Connection

| OLED Display | ESP32 |
|--------------|-------|
| VCC          | 3.3V  |
| GND          | GND   |
| SCL          | GPIO 22|
| SDA          | GPIO 21|

Default I2C address: 0x3C

## License

MIT License - feel free to use in your projects!

#include "driver/gpio.h"
#include "ssd1315.h"

void app_main(void) {
    init(GPIO_NUM_21, GPIO_NUM_22, 0x3c);

    int x = 0;
    int y = 0;
    int size = 20;
    int speed_x = 2;
    int speed_y = 2;

    while(1) {
        clear_display();
        
        x += speed_x;
        y += speed_y;
        
        if (x <= 0) {
            x = 0;
            speed_x = -speed_x;
        } else if (x >= 127 - size) {
            x = 127 - size;
            speed_x = -speed_x;
        }
        
        if (y <= 0) {
            y = 0;
            speed_y = -speed_y;
        } else if (y >= 63 - size) {
            y = 63 - size;
            speed_y = -speed_y;
        }
        
        draw_cube(x, y, size); 
        update_display();
    }
}
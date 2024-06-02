# Esp32LevelMeter
fluid level meter using ESP32

## IO pins

| PIO | ADC | Touch | RTC | SD | HSPI | U2 | VSPI | I2C1 | DAC | boot | strapping |  assigned   |
|-----|-----|-------|-----|----|------|----|------|------|-----|------|-----------|-------------|
|   4 | 2-0 |    0  |  10 | D1 |      |    |      |      |     |      |           |JSN-SR04 Trig|
|   5 |     |       |     |    |      |    |  SS  |      |     |  PWM |    true   | ePaper CS   |
|  13 | 2-4 |       |  14 | D3 |   D  |    |      |      |     |      |           |             |
|  15 | 2-3 |    3  |  13 | CMD|   D  |    |      |      |     |      |           |JSN-SR04 Echo|
|  16 |     |       |     |    |      | RX |      |      |     |      |           | ePaper DC   |
|  17 |     |       |     |    |      | TX |      |      |     |      |           | ePaper RST  |
|  18 |     |       |     |    |      |    |  CLK |      |     |      |           | ePaper CLK  |
|  19 |     |       |     |    |      |    | MISO |      |     |      |           | ePaper BUSY |
|  21 |     |       |     |    |      |    |      |  DA  |     |      |           |             |
|  22 |     |       |     |    |      |    |      |  CL  |     |      |           |             |
|  23 |     |       |     |    |      |    | MOSI |      |     |      |           | ePaper DIN  |
|  25 | 2-8 |       |   6 |    |      |    |      |      |  1  |      |           |             |
|  26 | 2-9 |       |   7 |    |      |    |      |      |  2  |      |           |             |
|  27 | 2-7 |    7  |  17 |    |      |    |      |      |     |      |           |             |
|  32 | 1-4 |    9  |   9 |    |      |    |      |      |     |      |           |             |
|  33 | 1-5 |    8  |   8 |    |      |    |      |      |     |      |           ||


## e-Paper Driver HAT for RPi

| ePaper HAT | cable color | function | ESP32 function | ESP32 PIO |
|:----------:|:-----------:|:--------:|:--------------:|----------:|
|     VCC    |     grey    |   +3.3V  |       3V3      |           |
|     GND    |    brown    |    GDN   |       GND      |           |
|     DIN    |     blue    |   MOSI   |      MOSI      |        23 |
|     CLK    |    yellow   |   SCLK   |      SCLK      |        18 |
|     CS     |    orange   |    CS    |       SS       |         5 |
|     DC     |    green    |          |                |        16 |
|     RST    |    white    |          |                |        17 |
|    BUSY    |   violett   |   busy   |      MISO      |        19 |

## References

-  [Boards AZ-Delivery ESP-32 Dev Kit C V4 @ platformio](https://docs.platformio.org/en/stable/boards/espressif32/az-delivery-devkit-v4.html)
-  [Rainwater well level monitoring project @ github](https://github.com/RobinR1/waterwell_level_sensor)
-  [TF-LC02 Lidar Sensor 3cm-200cm Short-Range Distance Single-Point Ranging Finder Module UART / IIC @ youyeetoo](https://www.youyeetoo.com/blog/tf-lc02-bxdptflc0201-134)
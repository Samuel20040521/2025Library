#ifndef CONFIG_H
#define CONFIG_H

namespace Config {
inline constexpr int NUMPCA = 1; // number of PCA
inline constexpr int PCAAddr[] = {0X23}; // PCA addresses

// using which WS2812 GPIO_PIN
#define WS2812_GPIO_PIN 18

// OF brightness level for brightness control
inline constexpr float OF_BRIGHTNESS_LEVEL = 255.0;
// OF max brightness scalling factor(0.0~1.0)
inline constexpr float OF_MAX_BRIGHTNESS_SCALING_FACTOR = 1.0;
// OF MAX BRIGHTNESS for each channel
inline constexpr int OF_MAX_BRIGHTNESS_R = 210;
inline constexpr int OF_MAX_BRIGHTNESS_G = 200;
inline constexpr int OF_MAX_BRIGHTNESS_B = 255;

// Gamma Correction
inline constexpr float GAMMA_OF_R = 2.65;
inline constexpr float GAMMA_OF_G = 2.55;
inline constexpr float GAMMA_OF_B = 2.65;

// Note: If the channel is not full, we should put 0 at the end of the array.
inline constexpr int WS2812_NUM_LED_EACH_STRIP[] = {
    10, 0, 0, 0, 4, 0, 0, 0};                 // Numbers of led for each strip
inline constexpr int LED_MAX_BRIGHTNESS = 50; // LED max brightness

inline constexpr float GAMMA_LED_R = 1.75;
inline constexpr float GAMMA_LED_G = 2.3;
inline constexpr float GAMMA_LED_B = 2.5;

// PCA9955 register initialization: Brightest current output for all pin
inline constexpr unsigned char IREFALL_MAX[] = {
    0x45, 0xff}; // 10000101(Register 45h), 11111111(Current)
// PCA9955 register initialization: For auto-increment of register address when
// input LED data
inline constexpr unsigned char PWM_AUTO_INCREMENT =
    0x88; // 10001000(the first 1 for Auto-increment, the second 1 as Register
          // 08h for PWM0 register)
} // namespace Config
inline constexpr int GPIOPINS[3] = {23, 24, 25};

#endif // CONFIG_
       //

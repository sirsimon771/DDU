#include <stdint-gcc.h>

struct Logo{
  uint8_t width;
  uint8_t height;
  uint8_t pixel_data[194 * 171];
};

const extern struct Logo splashLogo;

struct Text{
  uint8_t width;
  uint8_t height;
  uint8_t pixel_data[166 * 171];
};

const extern struct Text splashText;
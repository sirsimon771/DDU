// debug mode on or off (draw some help-lines and stuff)
// #define DEBUG 1

// color definitions
#define DDU_WHITE       0xffff      // {255, 255, 255}
#define DDU_GRAY        0x94b2      // {150, 150, 150}
#define DDU_YELLOW      0xff80      // {250, 240, 000}
#define DDU_ORANGE      0xfc00      // {255, 130, 000}
#define DDU_RED         0xf000      // {240, 000, 000}
#define DDU_PURPLE      0xb01b      // {180, 000, 220}
#define DDU_CYAN        0x05ff      // {000, 190, 255}
#define DDU_BLACK       0x0000      // {000, 000, 000}
#define DDU_GREEN       0x0780      // {000, 240, 000}
#define RWU_PURPLE      0x61D6      // {102, 056, 182}
#define RWU_CYAN        0x0559      // {000, 169, 206}

#define DDU_BACKGROUND DDU_BLACK

// backlight GPIO
#define TFT_BL          2

// display resolution
#define DDU_WIDTH       480
#define DDU_HEIGHT      272

#define DDU_REFRESH_MS  500
#define DDU_SPLASH_MS   500

// geometric settings
#define FRAMELINEWIDTH  3
#define FRAMERADIUS     5

// font constants
#define CHARHEIGHT      7
#define CHARWIDTH       5

// frame title text options
#define FRAMETITLESIZE  2
#define FRAMETITLEWIDTHFACTOR (FRAMETITLESIZE * CHARWIDTH)
#define FRAMETITLEHEIGHT (FRAMETITLESIZE * CHARHEIGHT)
#define FRAMETITLEBACKGROUNDOFFSET 5
#define FRAMEPADDING    8

// frame value text options
#define FRAMEVALUESIZE  3
#define FRAMEVALUEWIDTHFACTOR (FRAMEVALUESIZE * CHARWIDTH)
#define FRAMEVALUEHEIGHT (FRAMEVALUESIZE * CHARHEIGHT)


// EASTEREGG probabilities in %
#define CHANCE_OF_PHALLUS   1
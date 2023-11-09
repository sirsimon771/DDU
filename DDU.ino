#include <Arduino_GFX_Library.h>
#include <math.h>
#include <stdint-gcc.h>
#include "DDU.h"
#include "Splashscreen.h"


Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
    GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */);

// ILI6485 LCD 480x272
Arduino_RPi_DPI_RGBPanel *screen = new Arduino_RPi_DPI_RGBPanel(
  bus,
  480 /* width */, 0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 43 /* hsync_back_porch */,
  272 /* height */, 0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 12 /* vsync_back_porch */,
  1 /* pclk_active_neg */, 9000000 /* prefer_speed */, true /* auto_flush */);



// function prototypes
void generateData();
void refreshDisplay();
void drawFrame(struct frame);
void drawGear(char);
void drawPageIndicator();
void drawFrameLineWidth(int, int, int, int, int, int, int);
void drawSplashScreen();


// holds all the data
struct data{
    unsigned int tOil;
    unsigned int tFuel;
    unsigned int tMot;
    unsigned int pOil;
    unsigned int pFuel;
    unsigned int n;
    char gear;
    float ath;
    unsigned int map;
    char* mode;
    unsigned int tc;
    float lambda;
    float uBatt;
    unsigned int tBatt;
};

// holds a single name-value pair
struct value{
    char* name;
    char* value;
};

// holds number of values inside one frame
struct frame{
    unsigned int numOfVals;
    char* title;
    unsigned int color;
    unsigned int posX;
    unsigned int posY;
    unsigned int sizeX;
    unsigned int sizeY;
    struct value values[3];
};

// holds one value in its own frame
struct valueFrame{
    char* title;
    char* value;
    unsigned int color;
    unsigned int posX;
    unsigned int posY;
    unsigned int sizeX;
    unsigned int sizeY;
};

struct frame temp;
struct frame pres;
struct valueFrame rpm;
struct valueFrame ath;
struct valueFrame engineMap;
struct valueFrame tc;
struct valueFrame mode;
struct valueFrame lam;
struct frame batt;



void setup(void)
{
    // init screen and draw background
    screen->begin();
    screen->fillScreen(DDU_BACKGROUND);

    // enable backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    drawSplashScreen();

    refreshDisplay();

}

void loop()
{
    //generateData();

    //refreshDisplay();

    delay(DDU_REFRESH_MS);
}

// fill frame and valueFrame structs with values that don't change constantly
void initStructs()
{
    struct value nullvalue = {"NULL", "0"};

    // temps struct
    temp.numOfVals = 3;
    temp.title = "TEMP";
    temp.color = DDU_ORANGE;
    temp.posX = 1;
    temp.posY = 1;
    temp.sizeX = 1;
    temp.sizeY = 1;
    struct value tFuel = {"FUEL", 0};
    struct value tMot = {"MOT", 0};
    struct value tOil = {"OIL", 0};
    temp.values[0] = tOil;
    temp.values[1] = tFuel;
    temp.values[2] = tMot;

    // pressures struct
    pres.numOfVals = 2;
    pres.title = "PRES";
    pres.color = DDU_CYAN;
    pres.posX = 1;
    pres.posY = 1;
    pres.sizeX = 1;
    pres.sizeY = 1;
    struct value pOil = {"OIL", 0};
    struct value pFuel = {"FUEL", 0};
    pres.values[0] = pOil;
    pres.values[1] = pFuel;
    pres.values[2] = nullvalue;

    // rpm struct
    rpm = {"RPM", 0, DDU_WHITE, 1, 1, 1, 1};

    // ath struct
    ath = {"ATH", 0, DDU_WHITE, 1, 1, 1, 1};

    // map struct
    engineMap = {"MAP", 0, DDU_YELLOW, 1, 1, 1, 1};

    // tc struct
    tc = {"TC", 0, DDU_RED, 1, 1, 1, 1};

    // mode struct
    mode = {"MODE", 0, DDU_WHITE, 1, 1, 1, 1};

    // lambda struct
    lam = {"LAM", 0, DDU_PURPLE, 1, 1, 1, 1};

    // battery struct
    batt.numOfVals = 2;
    batt.title = "BATT";
    batt.color = DDU_GREEN;
    batt.posX = 1;
    batt.posY = 1;
    batt.sizeX = 1;
    batt.sizeY = 1;
    struct value uBatt = {"VOLT", 0};
    struct value tBatt = {"TEMP", 0};
    batt.values[0] = uBatt;
    batt.values[1] = tBatt;
    batt.values[2] = nullvalue;
}

// generates random values, updates data struct
void generateData()
{
    // TODO Eastereggs einbauen

    // calculate random data
    struct data data;

    unsigned int tEnv = random(15, 35);

    data.tFuel = min(tEnv + random(0, 15), (unsigned long)40);
    data.tMot = min(tEnv + random(0, 95), (unsigned long)110);
    data.tOil = min(data.tMot + random(0, 30), (unsigned long)130);

    data.n = random(2000, 13000);
    
    data.pOil = round((data.n-2000)/11000 * 5 + 1);
    data.pFuel = (random(388, 412) / 100.0f);

    int gear_temp = random(0, 4);
    switch (gear_temp){
        case 0:
            data.gear = 'N';
            break;
        case 1:
            data.gear = '1';
            break;
        case 2:
            data.gear = '2';
            break;
        case 3:
            data.gear = '3';
            break;
        case 4:
            data.gear = '4';
            break;
        default:
            data.gear = 'N';
            break;
    }

    data.ath = random(196, 1000) / 10.0f;
    data.map = random(0, 4);
    
    int mode_temp = random(0, 10);
    if(mode_temp < 8)
    {
        data.mode = "RACE";
    }
    else
    {
        data.mode = "TEST";
    }

    data.tc = random(0, 12);
    data.lambda = random(9, 11) / 10.0f;
    data.uBatt = random(132, 168) / 10.0f;
    data.tBatt = min(tEnv + random(0, 15), (unsigned long)39);

    // fill frame structs
    sprintf(temp.values[0].value, "%d", data.tMot);
    sprintf(temp.values[1].value, "%d", data.tFuel);
    sprintf(temp.values[2].value, "%d", data.tMot);

    sprintf(pres.values[0].value, "%0.02f", data.pOil);
    sprintf(pres.values[1].value, "%0.02f", data.pFuel);

    sprintf(batt.values[0].value, "%00.00f", data.uBatt);
    sprintf(batt.values[1].value, "%d", data.tBatt);

    // fill single value structs
    sprintf(rpm.value, "%d", data.n);
    sprintf(ath.value, "%00.0f", data.ath);
    sprintf(engineMap.value, "%d", data.map);
    sprintf(tc.value, "%d", data.tc);
    mode.value = data.mode;
    sprintf(lam.value, "%0.00f", data.lambda);
}

// writes the values in data struct to their places on the screen
void refreshDisplay()
{
    // TODO construct screen, draw Frames
    screen->fillScreen(DDU_BACKGROUND);
}

void drawFrame(struct frame frame)
{
    // TODO draw the frame, title and contained values
}

void drawGear(char c)
{
    const int width = 40;
    const int height = 80;
    const int posX = screen->width()/2 - width/2;
    const int posY = screen->height()/2 - height/2;
    
    // TODO draw gear character

}

void drawPageIndicator()
{
    // TODO draw three circles as page indicators, left one is WHITE, rest is GREY
}

void drawFrameLineWidth(int posX, int posY, int width, int height, int radius, int lineWidth, int color)
{
  // draw and fill outer rectangle
  screen->fillRoundRect(posX, posY, width, height, radius, color);
  // draw and fill inner rectangle in black
  screen->fillRoundRect(posX+lineWidth, posY+lineWidth, width-(2*lineWidth), height-(2*lineWidth), radius-lineWidth, BLACK);
}

// draw FSTW logo splash screen and wait
void drawSplashScreen()
{
    screen->fillScreen(DDU_BACKGROUND);

    // move to center
    int posX = DDU_WIDTH/2 - floor((splashLogo.width + splashText.width) / 2);
    int posY = DDU_HEIGHT/2 - floor(splashLogo.height/2);

    // draw bitmaps to screen
    screen->drawBitmap(posX, posY, splashLogo.pixel_data, splashLogo.width, splashLogo.height, RWU_PURPLE);
    screen->drawBitmap(posY+splashLogo.width, posY, splashText.pixel_data, splashText.width, splashText.height, RWU_CYAN);

    delay(DDU_SPLASH_MS);
}
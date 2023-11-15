#include <Arduino_GFX_Library.h>
#include <math.h>
#include <stdint-gcc.h>
#include "DDU.h"
#include "Splashscreen.h"
#include "Phallus.h"


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
void drawValueFrame(struct valueFrame);
void drawGear(char);
void drawPageIndicator();
void drawFrameLineWidth(int, int, int, int, int, int, int);
void drawSplashScreen();
void drawFrameTitle(int , int, int, int, char*);
void drawValueName(int, int, char*, int);
void drawValueRight(int, int, char*);
void drawValueCentered(int, int, char*);


// holds all the data
struct Data{
    uint16_t tOil;
    uint16_t tFuel;
    uint16_t tMot;
    float pOil;
    float pFuel;
    uint16_t n;
    float ath;
    uint16_t map;
    char* mode;
    uint16_t tc;
    float lambda;
    float uBatt;
    uint16_t tBatt;
};

// holds a single name-value pair
struct value{
    char* name;
    char* value;
};

// holds number of values inside one frame
struct frame{
    uint16_t numOfVals;
    char* title;
    uint16_t color;
    int posX;
    int posY;
    int sizeX;
    int sizeY;
    struct value values[3];
};

// holds one value in its own frame
struct valueFrame{
    char* title;
    char* value;
    uint16_t color;
    int posX;
    int posY;
    int sizeX;
    int sizeY;
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
char gear = 'N';

char rpmVal[6] =    "13000";
char athVal[5] =    "18.4";
char mapVal[2] =    "0";
char tcVal[3] =     "12";
char lamVal[5] =    "0.94";
char tOilVal[3] =   "98";
char tFuelVal[3] =  "19";
char tMotVal[3] =   "74";
char pOilVal[5] =   "4.31";
char pFuelVal[5] =  "5.12";
char uBattVal[5] =  "16.8";
char tBattVal[3] =  "28";


void setup(void)
{
    Serial.begin(9600);

    // disable backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, LOW);

    // init screen and draw background
    screen->begin();
    screen->fillScreen(DDU_BACKGROUND);

    // TODO1 test: rotate screen and flush buffer
    // screen->setRotation(2);
    // screen->flush();

    // define frame positions, colors etc.
    initStructs();

    // enable backlight
    delay(100);
    digitalWrite(TFT_BL, HIGH);

    // show FSTW logo at startup
    drawSplashScreen();
}

void loop()
{
    generateData();

    refreshDisplay();

    delay(DDU_REFRESH_MS);
}

// fill frame and valueFrame structs with values that don't change constantly
void initStructs()
{
    struct value nullvalue = {"NULL", "0"};

    int centerX = DDU_WIDTH/2;
    int centerY = DDU_HEIGHT/2;

    // temps struct
    temp.numOfVals = 3;
    temp.title = "TEMP";
    temp.color = DDU_ORANGE;
    temp.posX = centerX-(-55+110+16)-(58+16+90);
    temp.posY = 15;
    temp.sizeX = 144;
    temp.sizeY = 54+54+25;
    struct value tFuel = {"FUEL", tFuelVal};
    struct value tMot = {"MOT", tMotVal};
    struct value tOil = {"OIL", tOilVal};
    temp.values[0] = tOil;
    temp.values[1] = tFuel;
    temp.values[2] = tMot;

    // pressures struct
    pres.numOfVals = 2;
    pres.title = "PRES";
    pres.color = DDU_CYAN;
    pres.posX = centerX-(-55+110+16)-(58+16+90);
    pres.posY = 15+54+25 + 54 + 25;
    pres.sizeX = 58+16+90;
    pres.sizeY = 90;
    struct value pOil = {"OIL", pOilVal};
    struct value pFuel = {"FUEL", pFuelVal};
    pres.values[0] = pOil;
    pres.values[1] = pFuel;
    pres.values[2] = nullvalue;

    // rpm struct
    rpm = {"RPM", rpmVal, DDU_WHITE, centerX-(-55+110+16)-(58+16+90)+144+16, 15, (centerX-55+110+16)-(centerX-(-55+110+16)-(58+16+90)+144+16)-16, 54};

    // ath struct
    ath = {"ATH", athVal, DDU_WHITE, centerX-(-55+110+16)-(58+16+90)+58+16+90+16, DDU_HEIGHT-24-52, (centerX-55+110+16)-(centerX-(-55+110+16)-(58+16+90)+58+16+90+16)-16, 52};

    // map struct
    engineMap = {"MAP", mapVal, DDU_YELLOW, centerX-55+110+16, 15, 58, 54};

    // tc struct
    tc = {"TC", tcVal, DDU_RED, centerX-55+110+16, 15+54+25, 58, 54};

    // mode struct
    mode = {"MODE", "RACE", DDU_WHITE, centerX-55+110+16+58+16, 15, 90, 54};

    // lambda struct
    lam = {"LAM", lamVal, DDU_PURPLE, centerX-55+110+16+58+16, 15+54+25, 90, 54};

    // battery struct
    batt.numOfVals = 2;
    batt.title = "BATT";
    batt.color = DDU_GREEN;
    batt.posX = centerX-55+110+16;
    batt.posY = 15+54+25 + 54 + 25;
    batt.sizeX = 58+16+90;
    batt.sizeY = 90;
    struct value uBatt = {"VOLT", uBattVal};
    struct value tBatt = {"TEMP", tBattVal};
    batt.values[0] = uBatt;
    batt.values[1] = tBatt;
    batt.values[2] = nullvalue;
}

// generates random values, updates data struct
void generateData()
{
    // calculate random data
    struct Data data;

    unsigned int tEnv = random(17, 28);

    data.tFuel = min(tEnv + random(0, 15), (unsigned long)38);
    data.tMot = min(tEnv + random(0, 65), (unsigned long)81);
    data.tOil = min(data.tMot + random(0, 35), (long)107);

    data.n = random(2500, 13000);
    
    data.pOil = (data.n<2000) ? 0.05 : floor(((data.n-2500)/10500 * (7.2-0.65) + 0.65)*100)/100.0;
    data.pFuel = (random(488, 535) / 100.0f);

    int gear_temp = random(0, 4);
    switch (gear_temp){
        case 0:
            gear = 'N';
            break;
        case 1:
            gear = '1';
            break;
        case 2:
            gear = '2';
            break;
        case 3:
            gear = '3';
            break;
        case 4:
            gear = '4';
            break;
        default:
            gear = 'N';
            break;
    }

    data.ath = random(159, 1000) / 10.0f;
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
    data.lambda = random(80, 130) / 100.0f;
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
    sprintf(ath.value, "%00.1f", data.ath);
    sprintf(engineMap.value, "%d", data.map);
    sprintf(tc.value, "%d", data.tc);
    mode.value = data.mode;
    sprintf(lam.value, "%0.2f", data.lambda);
}

// writes the values in data struct to their places on the screen
void refreshDisplay()
{
    screen->fillScreen(DDU_BACKGROUND);

#ifdef DEBUG
    // center crosshair
    screen->drawLine(DDU_WIDTH/2, 0, DDU_WIDTH/2, DDU_HEIGHT, RED);
    screen->drawLine(0, DDU_HEIGHT/2, DDU_WIDTH, DDU_HEIGHT/2, RED);
#endif // ifdef DEBUG

    drawGear(gear);
    drawPageIndicator();

    // draw frames
    drawValueFrame(rpm);
    drawValueFrame(ath);
    drawValueFrame(engineMap);
    drawValueFrame(tc);
    drawValueFrame(mode);
    drawValueFrame(lam);
    drawFrame(temp);
    drawFrame(pres);
    drawFrame(batt);

    // TODO1 get frame buffer, rotate, write back and turn backlight on again
    // uint16_t* buff = screen->getFramebuffer();
    
    // screen->draw16bitBeRGBBitmap(0, 0, rotateBuffer(buff), DDU_WIDTH, DDU_HEIGHT);
    screen->flush();
}

uint16_t* rotateBuffer(uint16_t* buff)
{
    uint32_t len = sizeof(buff)/sizeof(buff[0]);
    uint16_t out[len];
    uint32_t outpos = 0;
    int w = DDU_WIDTH;
    int h = DDU_HEIGHT;

    for(int y = 0; y < h; y++)
    {
        outpos = len - (w * y);
        for(int x = 0; x < w; x++)
        {
            out[outpos-x] = buff[y+x];
        }
    }
}

void drawFrameTitle(int posX, int posY, int sizeX, int sizeY, char* title)
{
    // calculate text start position
    int textWidth = strlen(title) * (FRAMETITLEWIDTHFACTOR) + (strlen(title)-1) * FRAMETITLESIZE;
    int textposX = posX + (sizeX/2) - (textWidth/2);
    int textposY = posY - FRAMETITLEHEIGHT/2 + ceil(FRAMELINEWIDTH/2);
    
    // draw text background box
    int boxX = textposX - FRAMETITLEBACKGROUNDOFFSET;
    int boxY = textposY - 1;
    int boxW = textWidth + FRAMETITLEBACKGROUNDOFFSET*2 + 1;
    int boxH = FRAMETITLEHEIGHT + 2;
    screen->fillRect(boxX, boxY, boxW, boxH, DDU_BACKGROUND);

#ifdef DEBUG
    screen->drawRect(boxX, boxY, boxW, boxH, DDU_RED);
    screen->fillRect(boxX+1, posY, FRAMETITLEBACKGROUNDOFFSET, FRAMETITLEBACKGROUNDOFFSET, DDU_YELLOW);
    screen->fillRect(boxX+boxW-FRAMETITLEBACKGROUNDOFFSET-1, posY, FRAMETITLEBACKGROUNDOFFSET, FRAMETITLEBACKGROUNDOFFSET, DDU_YELLOW);
#endif // ifdef DEBUG

    // TODO3 maybe draw small ellipses at the open frame ends around the title?

    // print title text
    screen->setCursor(textposX, textposY);
    screen->setTextColor(DDU_WHITE);
    screen->setTextSize(FRAMETITLESIZE);
    screen->print(title);
}

void drawValueNamesAndValues(struct frame f)
{
    int n = f.numOfVals;

    // calculate positions for all values
    int posXName = f.posX + FRAMEPADDING;
    int posXVal = f.posX + f.sizeX - FRAMEPADDING;
    int posY = 0;
    for (int i = 0; i < n; i++)
    {
        posY = f.posY + (f.sizeY/(n+1)) * (i+1);
        drawValueName(posXName, posY, f.values[i].name, f.color);
        drawValueRight(posXVal, posY, f.values[i].value);
    }
}

void drawValueName(int posX, int posY, char* name, int color)
{
    // draw value name at the specified position, aligned to the left
    int X = posX;
    int Y = posY - (FRAMEVALUEHEIGHT / 2);
    screen->setCursor(X, Y);
    screen->setTextColor(color);
    screen->setTextSize(FRAMEVALUESIZE);
    screen->print(name);
}

void drawValueRight(int posX, int posY, char* value)
{
    // draw value at the specified position, aligned to the right
    int X = posX - (strlen(value) * FRAMEVALUEWIDTHFACTOR) - ((strlen(value)-1) * FRAMEVALUESIZE);
    int Y = posY - (FRAMEVALUEHEIGHT / 2);
    screen->setCursor(X, Y);
    screen->setTextColor(DDU_WHITE);
    screen->setTextSize(FRAMEVALUESIZE);
    screen->print(value);

#ifdef DEBUG
    screen->drawRect(X + 1*FRAMEVALUEWIDTHFACTOR, Y+10, 3, 3, MAGENTA);
    screen->drawRect(X, Y, (strlen(value)*FRAMEVALUEWIDTHFACTOR)+((strlen(value)-1)*FRAMEVALUESIZE), FRAMEVALUEHEIGHT, MAGENTA);
#endif // ifdef DEBUG
}

void drawValueCentered(int posX, int posY, char* value)
{
    // draw value at the specified center position
    int X = posX - ((strlen(value) * FRAMEVALUEWIDTHFACTOR) + ((strlen(value)-1) * FRAMEVALUESIZE)) / 2;
    int Y = posY - (FRAMEVALUEHEIGHT / 2);
    screen->setCursor(X, Y);
    screen->setTextColor(DDU_WHITE);
    screen->setTextSize(FRAMEVALUESIZE);
    screen->print(value);
#ifdef DEBUG
    screen->drawRect(X, Y, (strlen(value)*FRAMEVALUEWIDTHFACTOR)+(strlen(value)-1)*FRAMEVALUESIZE, FRAMEVALUEHEIGHT, DDU_GREEN);
#endif // ifdef DEBUG
}

void drawFrame(struct frame f)
{
    drawFrameLineWidth(f.posX, f.posY, f.sizeX, f.sizeY, FRAMERADIUS, FRAMELINEWIDTH, f.color);

    // draw title, value names and values
    drawFrameTitle(f.posX, f.posY, f.sizeX, f.sizeY, f.title);
    drawValueNamesAndValues(f);
}

void drawValueFrame(struct valueFrame vF)
{
    drawFrameLineWidth(vF.posX, vF.posY, vF.sizeX, vF.sizeY, FRAMERADIUS, FRAMELINEWIDTH, vF.color);

    // draw title and value
    drawFrameTitle(vF.posX, vF.posY, vF.sizeX, vF.sizeY, vF.title);
    drawValueCentered(vF.posX+(vF.sizeX/2), vF.posY+(vF.sizeY/2), vF.value);

#ifdef DEBUG
    int ch = screen->color565(0,25,0);
    int cv = screen->color565(0,0,25);
    screen->drawFastHLine(0, vF.posY, DDU_WIDTH, ch);
    screen->drawFastHLine(0, vF.posY+vF.sizeY, DDU_WIDTH, ch);
    screen->drawFastVLine(vF.posX, 0, DDU_HEIGHT, cv);
    screen->drawFastVLine(vF.posX+vF.sizeX, 0, DDU_HEIGHT, cv);
#endif // ifdef DEBUG 
}

void drawGear(char c)
{

    const int width = 50;
    const int height = 70;
    const int offsetY = -10;
    const int posX = DDU_WIDTH/2 - width/2;
    const int posY = DDU_HEIGHT/2 - height/2 + offsetY;
    
    // EASTEREGG chance of phallus instead of gear indicator
    if (random(99) <= CHANCE_OF_PHALLUS-1)
    {
        screen->drawBitmap(DDU_WIDTH/2-phallus.width/2, DDU_HEIGHT/2+offsetY-phallus.height/2+5,
        phallus.pixel_data, phallus.width, phallus.height, DDU_WHITE);
        return;
    }

    screen->setCursor(posX, posY);
    screen->setTextColor(DDU_WHITE);
    screen->setTextSize(10);
    screen->print(c);

#ifdef DEBUG
    int c_ = screen->color565(25,25,25);
    screen->drawFastHLine(0, DDU_HEIGHT/2-height/2+offsetY, DDU_WIDTH, c_);
    screen->drawFastHLine(0, DDU_HEIGHT/2+height/2+offsetY, DDU_WIDTH, c_);
    screen->drawFastVLine(DDU_WIDTH/2-width/2, 0, DDU_HEIGHT, c_);
    screen->drawFastVLine(DDU_WIDTH/2+width/2, 0, DDU_HEIGHT, c_);
#endif // ifdef DEBUG 
}

void drawPageIndicator()
{
    // draw two circles as page indicators
    int r = 4;
    int posY = DDU_HEIGHT - 12;
    int posX = DDU_WIDTH / 2;
    int dX = 8;

    screen->fillCircle(posX-dX, posY, r, DDU_WHITE);
    int color = 80;
    screen->fillCircle(posX+dX, posY, r, screen->color565(color, color, color));
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
    screen->fillScreen(DDU_BACKGROUND);
    delay(300);
}
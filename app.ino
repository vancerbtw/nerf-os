#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   11
#define OLED_CLK   13
#define OLED_DC    8
#define OLED_CS    10
#define OLED_RESET 9
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

bool hasBooted = true; //change to skip boot screen
int bootSeconds = 0;

float batteryLevel = 0;

bool menuMode = true;
bool menuMain = true;
int selected = 1;
int menu = 0;

bool blinkState = false;

//pins and previous for toggles
const int modeToggle = 2;
int modePrevious = LOW;

const int magPin = 4;
const int trigPin = 12;
const int pusherPinSpeed = 3;
const int revPin = 7;
const int pusherDetect = A4;
const int pusherMo = A5;
const int selectPin = A3;


int pusherPrev = 0;
int trigPrevious = 0;
int selectPrevious = 0;

int ammoCount = 0;

bool isCounting;
bool isSelected = false;

//initalizing firing values with defaults
int pushRate = 225; //0-255 for pwm
int flySpeed = 200; //0-255 for pwm
int fireMode = 1; //0 - semi auto, 1 - 3 round burst, 2 - full auto

int shotsFired = 0;
int limitFire = 0;

//menu 1
int menu1Selected = 1;

//menu2
int menu2Selected = 1;



static const unsigned char PROGMEM nerf_logo[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 
0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x10, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xef, 0xf0, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 
0xff, 0xc3, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x07, 0xdf, 0xfe, 0x38, 
0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x5f, 0xc3, 0xfc, 0x00, 0x00, 0x00, 0x03, 
0xf0, 0x00, 0x00, 0x00, 0xfe, 0x1f, 0xbf, 0xfc, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x00, 0x00, 0x0f, 
0xff, 0x1f, 0xbf, 0xfe, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0xff, 0xff, 0x3f, 0x20, 0x9e, 
0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x0f, 0xff, 0xfe, 0x3f, 0x0f, 0xbe, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x03, 0x8f, 0xfe, 0x7e, 0x3f, 0xff, 0xbe, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x3f, 0x8f, 0xe0, 
0x7e, 0x7f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x7f, 0x01, 0xff, 0x8f, 0xc0, 0x7e, 0x7f, 0xfe, 0xff, 
0x00, 0x00, 0x00, 0xff, 0x3f, 0xff, 0x0f, 0xc0, 0xfc, 0x7f, 0xe7, 0xfe, 0x00, 0x00, 0x00, 0x7f, 
0x3f, 0xf0, 0x0f, 0xc0, 0xf8, 0x7e, 0x1f, 0xfe, 0x00, 0x0e, 0x00, 0x7e, 0x3f, 0x00, 0x0f, 0xc3, 
0xf0, 0xfe, 0x3f, 0xfe, 0x01, 0xff, 0x00, 0xfe, 0x3f, 0x00, 0x1f, 0xff, 0xe0, 0xfc, 0x3f, 0xfe, 
0x01, 0xff, 0x80, 0xfc, 0x7e, 0x00, 0x1f, 0xff, 0x80, 0xfc, 0x7f, 0xfe, 0x01, 0xff, 0x81, 0xfc, 
0x7e, 0x02, 0x3f, 0xff, 0x81, 0xf8, 0x7f, 0xfc, 0x01, 0xff, 0xc1, 0xfc, 0x7e, 0xfe, 0x3f, 0xff, 
0x81, 0xf8, 0xff, 0xf8, 0x01, 0xff, 0xc1, 0xf8, 0xff, 0xfc, 0x3f, 0x3f, 0x81, 0xf8, 0xff, 0xf0, 
0x01, 0xff, 0xe3, 0xf8, 0xff, 0xfc, 0x7f, 0x1f, 0xc3, 0xf9, 0xff, 0xf0, 0x03, 0xff, 0xe3, 0xf9, 
0xff, 0xf0, 0x7f, 0x0f, 0xe3, 0xf3, 0xff, 0xe0, 0x03, 0xff, 0xf3, 0xf9, 0xfc, 0x00, 0x7e, 0x0f, 
0xfb, 0xf3, 0xff, 0xc0, 0x07, 0xff, 0xf3, 0xf1, 0xf8, 0x00, 0x7e, 0x07, 0xfb, 0xf3, 0xff, 0x80, 
0x07, 0xff, 0xff, 0xf1, 0xf8, 0x00, 0x7c, 0x07, 0xf3, 0xdf, 0xff, 0x00, 0x07, 0xef, 0xff, 0xf1, 
0xf8, 0x00, 0xfc, 0x07, 0xe0, 0x3f, 0xfe, 0x00, 0x0f, 0xe7, 0xff, 0xf1, 0xf8, 0x02, 0xfc, 0x03, 
0x00, 0xff, 0xfc, 0x00, 0x0f, 0xe7, 0xff, 0xe3, 0xf0, 0x3e, 0xfc, 0x00, 0x01, 0xff, 0xf8, 0x00, 
0x0f, 0xe3, 0xff, 0xe3, 0xf3, 0xff, 0xfc, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x0f, 0xe3, 0xff, 0xc7, 
0xff, 0xfd, 0xf0, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x1f, 0xc1, 0xff, 0xc7, 0xff, 0xf8, 0x00, 0x00, 
0x3f, 0xff, 0x80, 0x00, 0x1f, 0xc0, 0xff, 0xc7, 0xff, 0xc0, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 
0x1f, 0xc0, 0xff, 0xcf, 0xfe, 0x00, 0x00, 0x01, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0x80, 0x7f, 0xcf, 
0xf0, 0x00, 0x00, 0x0f, 0xff, 0xe0, 0x00, 0x00, 0x3f, 0x80, 0x7f, 0x80, 0x00, 0x00, 0x00, 0x1f, 
0xff, 0x80, 0x00, 0x00, 0x3f, 0x80, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 
0x7f, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x1c, 0x00, 
0x00, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 
0x7f, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0xff, 0xff, 
0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM dart_icon[] = {0x3f, 0x00, 0x3f, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 
0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 
0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 
0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xff, 0xc0, 0xff, 0xc0}; //10x30px

static const unsigned char PROGMEM gun_icon[] = {
0x00, 0x00, 0x7f, 0x80, 0x7f, 0x80, 0x7f, 0x80, 0x0b, 0x80, 0x07, 0x80, 0x03, 0x80, 0x03, 0x80, 
0x03, 0x80, 0x00, 0x00
};

static const unsigned char PROGMEM no_darts[] = {0x00, 0x1f, 0xe0, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x01, 0xf0, 0x3e, 0x00, 0x07, 0x80, 0x07, 0x80, 
0x0f, 0x07, 0x83, 0xc0, 0x1c, 0x08, 0x41, 0xe0, 0x18, 0x08, 0x43, 0xe0, 0x38, 0x08, 0x47, 0x70, 
0x70, 0x08, 0x4e, 0x38, 0x60, 0x0f, 0xdc, 0x18, 0x60, 0x08, 0x58, 0x18, 0xe0, 0x08, 0x50, 0x1c, 
0xc0, 0x08, 0x40, 0x0c, 0xc0, 0x08, 0x40, 0x0c, 0xc0, 0x08, 0x40, 0x0c, 0xc0, 0x08, 0x40, 0x0c, 
0xc0, 0x08, 0x40, 0x0c, 0xc0, 0x08, 0x40, 0x0c, 0xe0, 0x28, 0x40, 0x1c, 0x60, 0x68, 0x40, 0x18, 
0x60, 0xe8, 0x40, 0x18, 0x71, 0xc8, 0x40, 0x38, 0x3b, 0x88, 0x40, 0x70, 0x1f, 0x08, 0x40, 0x60, 
0x1e, 0x08, 0x40, 0xe0, 0x0f, 0x07, 0x83, 0xc0, 0x07, 0x80, 0x07, 0x80, 0x01, 0xf0, 0x3e, 0x00, 
0x00, 0xff, 0xfc, 0x00, 0x00, 0x1f, 0xe0, 0x00}; // 'no_darts', 30x30px

static const unsigned char PROGMEM push_speed_icon[] = {
0x07, 0xe0, 0x18, 0x18, 0x20, 0x04, 0x40, 0x02, 0x5d, 0x82, 0x80, 0x01, 0x87, 0xf9, 0xb4, 0x15, 
0x84, 0x15, 0x97, 0xf9, 0x80, 0x01, 0x59, 0xe2, 0x40, 0x02, 0x20, 0x04, 0x18, 0x18, 0x07, 0xe0
}; // 'push_speed', 16x16px

static const unsigned char PROGMEM speed_icon[] = {
0x07, 0xe0, 0x18, 0x18, 0x23, 0xc4, 0x4d, 0x32, 0x59, 0x0a, 0x94, 0x69, 0xa0, 0xe5, 0xb9, 0xc5, 
0xa0, 0x9d, 0xa0, 0x05, 0x94, 0x29, 0x58, 0x1a, 0x48, 0x12, 0x20, 0x04, 0x18, 0x18, 0x07, 0xe0
}; // 'dart_speed', 16x16px

static const unsigned char PROGMEM info_icon[] = {
0x07, 0xe0, 0x18, 0x18, 0x21, 0x84, 0x43, 0xc2, 0x43, 0xc2, 0x81, 0x81, 0x80, 0x01, 0x81, 0x81, 
0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x41, 0x82, 0x41, 0x82, 0x21, 0x84, 0x18, 0x18, 0x07, 0xe0
}; // 'info_icon', 16x16px

static const unsigned char PROGMEM battery_icon[] = {
0x07, 0xe0, 0x18, 0x18, 0x20, 0x04, 0x41, 0x82, 0x43, 0xc2, 0x83, 0xc1, 0x83, 0xc1, 0x83, 0xc1, 
0x83, 0xc1, 0x83, 0xc1, 0x83, 0xc1, 0x43, 0xc2, 0x43, 0xc2, 0x20, 0x04, 0x18, 0x18, 0x07, 0xe0
}; // battery icon 16x16



void setup()
{
    Serial.begin(9600);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }

    pinMode(modeToggle, INPUT);
    pinMode(trigPin, INPUT);
    pinMode(revPin, INPUT);
    pinMode(pusherDetect, INPUT);
    pinMode(pusherPinSpeed, OUTPUT);
    pinMode(pusherMo, OUTPUT);
    pinMode(A5, OUTPUT);
    pinMode(A4, OUTPUT);

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.setTextSize(1);       
    display.setTextColor(WHITE);   
    display.clearDisplay();
    display.display();

}

void loop()
{
    if (!hasBooted) 
    {
        display.clearDisplay();
        display.drawBitmap(16, 0, nerf_logo, 96, 49, 1);
        display.setCursor(0, 53);
        switch (bootSeconds) 
        {
            case 0:
                display.println("Loading");
                break;
            case 1:
                display.println("Loading.");
                break;
            case 2:
                display.println("Loading..");
                break;
            case 3:
                display.println("Loading...");
                break;
            case 4:
                display.println("Loading..");
                break;
            case 5:
                display.println("Loading.");
                break;
            case 6:
                display.println("Loading");
                break;
            case 7:
                display.println("Loading.");
                break;
            case 8:
                display.println("Loading..");
                break;
            case 9:
                display.println("Loading...");
                break;
            case 10:
                hasBooted = true;
                display.clearDisplay();
                break;
        }
        delay(280);
        bootSeconds++;

        display.display();
    } else 
    {
        display.clearDisplay();
        unsigned char PROGMEM battery_shell[] = {0xff, 0xfe, 0x80, 0x02, 0x80, 0x03, 0x80, 0x03, 0x80, 0x02, 0xff, 0xfe}; // 16x6px
        unsigned char PROGMEM battery_ind[] = {0xe0, 0xe0}; // 3x2px      
        display.drawBitmap(1, 1, battery_shell, 16, 6, 1);
        //this if is for toggling with a button to prevent a continous loop of switching modes

        //measure and set battery level here

        batteryLevel = 0.8;

        if (batteryLevel <= .33) 
        {
            display.drawBitmap(3, 3, battery_ind, 3, 2, 1);
        } else if (batteryLevel <= .66)
        {
             display.drawBitmap(3, 3, battery_ind, 3, 2, 1);
             display.drawBitmap(7, 3, battery_ind, 3, 2, 1);
        } else
        {
            display.drawBitmap(3, 3, battery_ind, 3, 2, 1);
             display.drawBitmap(7, 3, battery_ind, 3, 2, 1);
             display.drawBitmap(11, 3, battery_ind, 3, 2, 1);
        }

        if (digitalRead(magPin) == HIGH) 
        {
            if (!isCounting) {
                //set ammo count to rfid putting value in place for now
                ammoCount = 3;
                isCounting = true;
            }
        } else 
        {
            isCounting = false;
            ammoCount = 0;
        }

        if (modePrevious == HIGH && digitalRead(modeToggle) == LOW) 
        {
            if (menuMode) 
            {
                menuMode = false;
            } else 
            {
                menuMain = true;
                menuMode = true;
                selected = 1;
            }
            
        }
        //setting previous for above loop to refference
        modePrevious = digitalRead(modeToggle);
        
        if (menuMode) 
        {
            //trigPin = trigger pin
            //this is when blaster is in menu mode
            if (isSelected) 
            {
                display.setCursor(80, 0);
                display.println("Editing");
            } else if (selected == 1) 
            {
                display.setCursor(64, 0);
                display.println("Dart Speed");
            } else if (selected == 2)
            {
                display.setCursor(70, 0);
                display.println("Fire Rate");
            } else if (selected == 4)
            {
                display.setCursor(52, 0);
                display.println("Battery Info");
            } else if (selected == 3)
            {
                display.setCursor(52, 0);
                display.println("General Info");
            }
            

            if (menuMain) {
                //icons must be 16x16
                if (selectPrevious == HIGH && digitalRead(selectPin) == LOW) 
                {
                    if (selected > 0) 
                    {
                        menuMain = false;
                        menu = selected;
                        isSelected = false;
                        menu1Selected = 1;
                        menu2Selected = 1;

                    }
                    
                }
                selectPrevious = digitalRead(selectPin);
                if (trigPrevious == HIGH && digitalRead(trigPin) == LOW) 
                {
                    if (selected != 4) 
                    {
                        selected++;
                    } else 
                    {
                        selected = 1;
                    }
                    
                }
                //setting previous for above loop to refference

                trigPrevious = digitalRead(trigPin);

                display.drawRect(22, 10, 20, 20, 1);
                
                display.drawRect(22, 35, 20, 20, 1);

                display.drawRect(86, 10, 20, 20, 1);
                
                display.drawRect(86, 35, 20, 20, 1);

                display.drawBitmap(24, 12, speed_icon, 16, 16, 1);

                display.drawBitmap(24, 37, battery_icon, 16, 16, 1);

                display.drawBitmap(88, 12, push_speed_icon, 16, 16, 1);

                display.drawBitmap(88, 37, info_icon, 16, 16, 1);

                if (selected == 1) 
                {
                    display.drawRect(21, 9, 22, 22, 1);
                } else if (selected == 2)
                {
                    display.drawRect(85, 9, 22, 22, 1);
                } else if (selected == 3)
                {
                    display.drawRect(85, 34, 22, 22, 1);
                } else if (selected == 4)
                {
                   display.drawRect(21, 34, 22, 22, 1);
                } 
            } else if (menu == 1)
            {
                if (selectPrevious == HIGH && digitalRead(selectPin) == LOW) 
                {
                    if (isSelected) {
                        isSelected = false;
                    } else
                    {
                        isSelected = true;
                    }

                }
                 selectPrevious = digitalRead(selectPin);

                if (!isSelected & trigPrevious == HIGH && digitalRead(trigPin) == LOW) 
                {
                    if (menu1Selected == 1) {
                        menu1Selected = 2;
                    } else {
                        menu1Selected = 1;
                    }
                    
                }
                trigPrevious = digitalRead(trigPin);

                if (isSelected && menu1Selected == 2 && !menuMain) {
                    menuMain = true; //this is if the back button is selected
                    isSelected = false;
                }

                if (menu1Selected == 1) {
                    display.drawRect(0, 14, 74, 34, 1);
                    if (isSelected) {
                        display.drawRect(1, 15, 72, 32, 1);
                    }
                } else {
                    display.drawRect(0, 50, 26, 11, 1);
                }
                
                display.setTextSize(4);
                display.setCursor(3, 17);
                int displaySpeed = (int)((double)flySpeed / 255 * 100);
                if (displaySpeed == 100) {
                    display.setTextSize(3);
                    display.setCursor(1, 21);
                }
                char displaySpeedStr[10];
                itoa(displaySpeed,displaySpeedStr,10); 
                display.println(strcat(displaySpeedStr, "%")); 
            
                display.setTextSize(1);
                display.setCursor(2, 52);
                display.println("Back");
            } else if (menu == 2) {
                if (selectPrevious == HIGH && digitalRead(selectPin) == LOW) 
                {
                    if (isSelected) {
                        isSelected = false;
                    } else
                    {
                        isSelected = true;
                    }

                }
                 selectPrevious = digitalRead(selectPin);

                if (!isSelected & trigPrevious == HIGH && digitalRead(trigPin) == LOW) 
                {
                    if (menu2Selected == 1) {
                        menu2Selected = 2;
                    } else {
                        menu2Selected = 1;
                    }
                    
                }
                trigPrevious = digitalRead(trigPin);

                if (isSelected && menu2Selected == 2 && !menuMain) {
                    menuMain = true; //this is if the back button is selected
                    isSelected = false;
                }

                if (menu2Selected == 1) {
                    display.drawRect(0, 14, 74, 34, 1);
                    if (isSelected) {
                        display.drawRect(1, 15, 72, 32, 1);
                    }
                } else {
                    display.drawRect(0, 50, 26, 11, 1);
                }
                
                display.setTextSize(4);
                display.setCursor(3, 17);
                int displayPush = (int)((double)pushRate / 255 * 100);
                if (displayPush == 100) {
                    display.setTextSize(3);
                    display.setCursor(1, 21);
                }
                char displayPushStr[10];
                itoa(displayPush,displayPushStr,10); 
                display.println(strcat(displayPushStr, "%")); 
            
                display.setTextSize(1);
                display.setCursor(2, 52);
                display.println("Back");
            }
            
        } else 
        {
            //this is combat mode
            if (pusherPrev == HIGH && digitalRead(pusherDetect) == LOW)
            {
                ammoCount--;
                if (fireMode != 2) {
                    shotsFired++;
                }
            }
            
            pusherPrev = digitalRead(pusherDetect);
                display.drawBitmap(15, 15, dart_icon, 10, 30, 1);
                if (ammoCount <= 0) 
                {
                    //flash empty warning
                    if (blinkState) {
                        display.fillRect(32, 15, 30, 30, 0);
                        blinkState = false;
                    } else
                    {
                        blinkState = true;
                        display.drawBitmap(32, 15, no_darts, 30, 30, 1);
                    }
                    delay(200);
                } else 
                {
                    if (ammoCount < 10 || ammoCount > 1)
                    {
                        display.setTextSize(4);
                        display.setCursor(32, 17);
                        display.println(ammoCount);
                    } else
                    {
                        display.setTextSize(4);
                        display.setCursor(25, 17);
                        display.println(ammoCount);
                    }
                }

            if (fireMode == 0) 
                    {
                        //semi auto
                        display.setCursor(0, 53);
                        display.setTextSize(1);
                        display.println("Semi-Auto ");
                    } else if (fireMode == 1)
                    {
                        // burst mode
                        display.setCursor(0, 53);
                        display.setTextSize(1);
                        display.println("Burst ");
                    } else
                    {
                        //full auto
                        display.setCursor(0, 53);
                        display.setTextSize(1);
                        display.println("Full-Auto ");
                    }



            int revState = digitalRead(revPin);

            if (revState == HIGH) {
                //rev here
            }

            if (digitalRead(trigPin) == HIGH & revState == HIGH & ammoCount > 0) 
            {
                    if (fireMode == 0) 
                    {
                        //semi auto
                        if (shotsFired < 1) {
                            analogWrite(pusherPinSpeed, 255);
                            digitalWrite(pusherMo, HIGH);
                        } else
                        {
                            analogWrite(pusherPinSpeed, 0);
                            digitalWrite(pusherMo, LOW);
                        }
                    } else if (fireMode == 1)
                    {
                        // burst mode
                        if (shotsFired < 3) {
                            analogWrite(pusherPinSpeed, 255);
                            digitalWrite(pusherMo, HIGH);
                        } else
                        {
                            analogWrite(pusherPinSpeed, 0);
                            digitalWrite(pusherMo, LOW);
                        }
                        
                    } else
                    {
                        //full auto
                        analogWrite(pusherPinSpeed, 255);
                        digitalWrite(pusherMo, HIGH);
                    }
            } else 
            {
                shotsFired = 0;
                analogWrite(pusherPinSpeed, 0);
                digitalWrite(pusherMo, LOW);
            }

            // this is when blaster is in combat mode
            display.drawBitmap(118, 0, gun_icon, 10, 10, 1);
            
            
        }
        
        display.setTextSize(1);
        if (menuMode & menu == 1 & !menuMain) {
            if (isSelected) 
            {
                if (menu1Selected == 1) {
                    if (digitalRead(revPin) == HIGH) {
                        display.fillTriangle(80, 35, 100, 55, 120, 35, 1);
                        display.drawTriangle(80, 30, 100, 10, 120, 30, 1);
                        if (flySpeed > 1) {
                            flySpeed -= 2;
                        }
                        display.display();
                        delay(300);
                        if (digitalRead(revPin) == HIGH) {
                            display.fillTriangle(80, 35, 100, 55, 120, 35, 1);
                            display.drawTriangle(80, 30, 100, 10, 120, 30, 1);
                            if (flySpeed < 4) {
                                flySpeed = 0;
                            } else {
                                flySpeed -= 4;
                                display.display();
                            }
                        }
                    } else if (digitalRead(trigPin) == HIGH) {
                        display.fillTriangle(80, 30, 100, 10, 120, 30, 1);
                        display.drawTriangle(80, 35, 100, 55, 120, 35, 1);
                        if (flySpeed < 254) {
                            flySpeed += 2;
                        }
                        display.display();
                        delay(300);
                        if (digitalRead(trigPin) == HIGH) {
                            display.fillTriangle(80, 30, 100, 10, 120, 30, 1);
                            display.drawTriangle(80, 35, 100, 55, 120, 35, 1);
                            if (flySpeed > 250) {
                                flySpeed = 255;
                            } else {
                                flySpeed += 4;
                                display.display();
                            }
                        }
                    } else {
                        display.drawTriangle(80, 30, 100, 10, 120, 30, 1);
                        display.drawTriangle(80, 35, 100, 55, 120, 35, 1);
                    }
                    display.display();
                }
            } else
            {
                display.drawTriangle(80, 30, 100, 10, 120, 30, 1);
                display.drawTriangle(80, 35, 100, 55, 120, 35, 1);
                display.display();
            }
        } else if (menuMode & menu == 2 & !menuMain) {
            if (isSelected) 
            {
                if (menu2Selected == 1) {
                    if (digitalRead(revPin) == HIGH) {
                        display.fillTriangle(80, 35, 100, 55, 120, 35, 1);
                        display.drawTriangle(80, 30, 100, 10, 120, 30, 1);
                        if (pushRate > 1) {
                            pushRate -= 2;
                        }
                        display.display();
                        delay(300);
                        if (digitalRead(revPin) == HIGH) {
                            display.fillTriangle(80, 35, 100, 55, 120, 35, 1);
                            display.drawTriangle(80, 30, 100, 10, 120, 30, 1);
                            if (pushRate < 4) {
                                pushRate = 0;
                            } else {
                                pushRate -= 4;
                                display.display();
                            }
                        }
                    } else if (digitalRead(trigPin) == HIGH) {
                        display.fillTriangle(80, 30, 100, 10, 120, 30, 1);
                        display.drawTriangle(80, 35, 100, 55, 120, 35, 1);
                        if (pushRate < 254) {
                            pushRate += 2;
                        }
                        display.display();
                        delay(300);
                        if (digitalRead(trigPin) == HIGH) {
                            display.fillTriangle(80, 30, 100, 10, 120, 30, 1);
                            display.drawTriangle(80, 35, 100, 55, 120, 35, 1);
                            if (pushRate > 250) {
                                pushRate = 255;
                            } else {
                                pushRate += 4;
                                display.display();
                            }
                        }
                    } else {
                        display.drawTriangle(80, 30, 100, 10, 120, 30, 1);
                        display.drawTriangle(80, 35, 100, 55, 120, 35, 1);
                    }
                    display.display();
                }
            } else
            {
                display.drawTriangle(80, 30, 100, 10, 120, 30, 1);
                display.drawTriangle(80, 35, 100, 55, 120, 35, 1);
                display.display();
            }
        }
        display.display();
    }
    
}

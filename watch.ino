#include <Adafruit_IS31FL3731.h>
#include <Adafruit_GFX.h>
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)
#include <TimeLib.h>
#define PIN 6

//IF CLOCK TIME IS WRONG: RUN FILE->EXAMPLE->DS1307RTC->SET TIME


void digitalClockDisplay(time_t mynow) {
  // digital clock display of the time
  Serial.print(hour(mynow));
  printDigits(minute(mynow));
  printDigits(second(mynow));
  Serial.print(" ");
  Serial.print(day(mynow));
  Serial.print(" ");
  Serial.print(month(mynow));
  Serial.print(" ");
  Serial.print(year(mynow));
  Serial.println(); 
}

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
  }
  return pctime;
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

// matrix of 11 - 16 bit ints (shorts) for displaying the leds
uint16_t mask[11];

/* notes for my arduino wiring:
 * pin 2: top button (set)
 * pin 3: middle button (up)
 * pin 4: bottom button (down)
 * pin 6: LED strand data
 * pin A0: photosensor
 * pin A4: RTC Module SDA
 * pin A5: RTC Module SCL
 */

  int buttonSet=2;
  int buttonUp=3;
  int buttonDown=4;
  int photoResistor=A0;
  int photoRead;
  int dimmer=1;

  //////////////////////////////////SET COLORS/////////////////////////////

  //WORDCLOCK COLORS
  int wordred   = 80;
  int wordblue  = 255;
  int wordgreen = 160;

  //DIGIT COLORS (HOURS)
  int hourred   = 130;
  int hourblue  = 130;
  int hourgreen = 130;
  //DIGIT COLORS (MINUTES)
  int minred    = 250;
  int minblue   = 250;
  int mingreen  = 250;

  //DATE COLORS (MONTH)
  int monthred   = 125;
  int monthblue  = 25;
  int monthgreen = 50;
  //DATE COLORS (DAY)
  int dayred    = 250;
  int dayblue   = 50;
  int daygreen  = 100;

  //DIMABLE (once photoresistor is programmed)
  int dimred=wordred/dimmer;
  int dimblue=wordblue/dimmer;
  int dimgreen=wordgreen/dimmer;
  
//////////////////////////////////////////////////////////////////////////////

  int mytimemonth;
  int mytimeday;
  int mytimehr;
  int mytimemin;
  int mytimesec;
  
  int j; //an integer for the color shifting effect

  int mode = 0;
  int lastState = LOW;   // the previous reading from the input pin
  int buttonState;
  // the following variables are long's because the time, measured in miliseconds,
  // will quickly become a bigger number than can be stored in an int.
  long lastDebounceTime = 0;  // the last time the output pin was toggled
  long debounceDelay = 50;    // the debounce time; increase if the output flickers
  
#define WORD_MODE 0
#define DIGIT_MODE 1
#define DATE_MODE 2
#define MAX_MODE 4

#define phraseIT         mask[0]  |= 0xC000
#define phraseIS         mask[0]  |= 0x1800
#define phraseFIVE       mask[1]  |= 0x03C0
#define phraseTEN        mask[2]  |= 0x00E0
#define phraseQUARTER    mask[2]  |= 0xFE00
#define phraseTWENTY     mask[1]  |= 0xFC00
#define phraseHALF       mask[0]  |= 0x03C0
#define phrasePAST       mask[4]  |= 0xF000
#define phraseTO         mask[3]  |= 0x00C0
#define hourONE          mask[5]  |= 0xE000
#define hourTWO          mask[6]  |= 0x00E0
#define hourTHREE        mask[5]  |= 0x03E0
#define hourFOUR         mask[6]  |= 0xF000
#define hourFIVE         mask[5]  |= 0x000F
#define hourSIX          mask[5]  |= 0x1C00
#define hourSEVEN        mask[8]  |= 0xF800
#define hourEIGHT        mask[7]  |= 0xF800
#define hourNINE         mask[8]  |= 0x03C0
#define hourTEN          mask[6]  |= 0x0700
#define hourELEVEN       mask[7]  |= 0x03F0
#define hourTWELVE       mask[4]  |= 0x07E0
#define phraseMINUTES    mask[3]  |= 0xFE00
#define phraseOCLOCK     mask[8]  |= 0x003F

typedef uint8_t Character[7];
void draw(uint8_t x, uint8_t y, const Character& c, uint16_t color);
const Character charmap[] = {
  {
    0b01110,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b01110
  },
  {
    0b00100,
    0b01100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b01110
  },
  {
    0b01110,
    0b10001,
    0b00001,
    0b00010,
    0b00100,
    0b01000,
    0b11111
  },
  {
    0b11111,
    0b00010,
    0b00100,
    0b00010,
    0b00001,
    0b10001,
    0b01110
  },
  {
    0b00010,
    0b00110,
    0b01010,
    0b10010,
    0b11111,
    0b00010,
    0b00010
  },
  {
    0b11111,
    0b10000,
    0b11110,
    0b00001,
    0b00001,
    0b10001,
    0b01110
  },
  {
    0b00110,
    0b01000,
    0b10000,
    0b11110,
    0b10001,
    0b10001,
    0b01110
  },
  {
    0b11111,
    0b00001,
    0b00010,
    0b00100,
    0b01000,
    0b01000,
    0b01000
  },
  {
    0b01110,
    0b10001,
    0b10001,
    0b01110,
    0b10001,
    0b10001,
    0b01110
  },
  {
    0b01110,
    0b10001,
    0b10001,
    0b01111,
    0b00001,
    0b00010,
    0b01100
  }
};

// define pins
#define NEOPIN 6

// define delays
#define FLASHDELAY 500  // delay for startup "flashWords" sequence
#define SHIFTDELAY 100   // controls color shifting speed

// Parameter 1 = number of pixels in matrix
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//Adafruit_NeoPixel matrix = Adafruit_NeoPixel(64, NEOPIN, NEO_GRB + NEO_KHZ800);

// configure for 11x11 neopixel matrix
Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731();

void rainbowCycle(uint8_t wait);
void flashWords(void);
void pickAPixel(uint8_t x, uint8_t y);

void setup() {
  Serial.begin(115200);
  while (!Serial);  // Wait for Arduino Serial Monitor to open
  delay(100);
  Serial.println("Hi!");

  matrix.begin();
  // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);
   
  pinMode(buttonSet, INPUT_PULLUP);
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(photoResistor, INPUT);

  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
}

void applyMask() {

   for (byte row = 0; row < 11; row++) 
   {
      for (byte col = 0; col < 16; col++) 
      {
         boolean masker = bitRead(mask[row], 15 - col); // bitread is backwards because bitRead reads rightmost digits first. could have defined the word masks differently
         switch (masker) 
         {
            case 0:
               matrix.drawPixel(col, row, 0);
               break;
            case 1:
              // matrix.drawPixel(col, row, Wheel(((col * 256 / matrix.numPixels()) + j) & 255));
              // word_mode color set
                matrix.drawPixel(col, row, 255);
               break;
         }
      }
      // reset mask for next time
      mask[row] = 0;
   }


   matrix.displayFrame(0); // show it!
}

void readModeButton() {
  int currentState = digitalRead(buttonSet);
  // If the switch changed, due to noise or pressing:
  if (currentState == HIGH && lastState == LOW) {
    delay(1);
  }
  else if (currentState == LOW && lastState == HIGH)
  {
    // if the button state has changed:
      mode++;
      if (mode >= MAX_MODE)
          mode = WORD_MODE; 
     delay(1);
  }
  lastState = currentState;
}

void loop() {

  if (Serial.available()) {
    time_t t = processSyncMessage();
    if (t != 0) {
      Teensy3Clock.set(t); // set the RTC
      setTime(t);
    }
  }
  
  time_t mynow = now();
  mytimemonth=month(mynow);
  mytimeday=day(mynow);
  mytimehr=hour(mynow);
  mytimemin=minute(mynow);
  mytimesec=second(mynow);
  digitalClockDisplay(mynow);

  delay(1000);
  
//////////////////////////////////////////PHOTORESISTOR/////////////////////////////////////////////
    //Photoresistor settings
    photoRead = analogRead(photoResistor);  
//    Serial.print(photoRead);     // the raw analog reading
//      if (photoRead < 200) {
//        dimmer=5;
//      } else if (photoRead < 400) {
//        dimmer=4;
//      } else if (photoRead < 600) {
//        dimmer=2;
//      } else {
//        dimmer=1;
//      }
      delay(100);
////////////////////////////////////////////////////////////////////////////////////////////////


    
    //Serial.print(now.year(), DEC);
    //Serial.print('/');
    //Serial.print(now.month(), DEC);
    //Serial.print('/');
    //Serial.print(now.day(), DEC);
    //Serial.print(" (");
    //Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    //Serial.print(") ");
    //Serial.print(now.hour(), DEC);
    //Serial.print(':');
    //Serial.print(now.minute(), DEC);
    //Serial.print(':');
    //Serial.print(now.second(), DEC);
    //Serial.println();

  readModeButton();

  if (mode == WORD_MODE)
    displayWords();
  else if (mode == DIGIT_MODE)
    displayDigits();
  else if (mode == DATE_MODE)
    displayDate();
    
};

//void displayWords();
//void displayDate();
//void displayDigits();

void displayDigits() {
  uint8_t units, tens;
  uint16_t color = 255;
   
  if (mytimesec/2 % 2) {
    units = mytimemin % 10;
    tens  = mytimemin / 10;
    //digit_mode color, minutes
  } else {
    units = mytimehr % 10;
    tens  = mytimehr / 10;
    //digit_mode color, hours
  }
  matrix.clear();
 
  draw(0, 2, charmap[tens],  color);
  draw(6, 2, charmap[units], color);
 
  matrix.displayFrame(0); // show it!
}


void draw(uint8_t x, uint8_t y, const Character& c, uint16_t color) {
  for (int i = 0; i < 7; i++) for (int j = 0; j < 5; j++) {
    if (bitRead(c[i], j)) matrix.drawPixel(x+4-j, y+i, color);
  }
};


void displayDate() {
  uint8_t units, tens;
  uint16_t color = 255;
   
  if (mytimesec/2 % 2) {
    units = mytimeday % 10;
    tens  = mytimeday / 10;
    //digit_mode color, minutes
  } else {
    units = mytimemonth % 10;
    tens  = mytimemonth / 10;
    //digit_mode color, hours
  }
  matrix.clear();
 
  draw(0, 2, charmap[tens],  color);
  draw(6, 2, charmap[units], color);
 
  matrix.displayFrame(0); // show it!
}


void displayWords() {
  //Always on
   phraseIT;
   phraseIS;

  //calculate minutes on the hour
    if(mytimemin>57 && mytimemin<3){
    }
    // do nothing, no minutes it's on the hour
    
    if(mytimemin>2 && mytimemin<8){
      
      phraseFIVE;
      phrasePAST;
    }
    
    if(mytimemin>7 && mytimemin<13){
      
      phraseTEN;
      phrasePAST;
    }
    if(mytimemin>12 && mytimemin<18){
      
      phraseQUARTER;
      phrasePAST;
    }
    if(mytimemin>17 && mytimemin<23){
      
      phraseTWENTY;
      phrasePAST;
    }
    if(mytimemin>22 && mytimemin<28){
      
      phraseTWENTY;
      phraseFIVE;
      phrasePAST;
    }
    if(mytimemin>27 && mytimemin<33){
      
      phraseHALF;
      phrasePAST;
    }
    if(mytimemin>32 && mytimemin<38){
      
      phraseTWENTY;
      phraseFIVE;
      phraseMINUTES;
      phraseTO;
    }
    if(mytimemin>37 && mytimemin<43){
      
      phraseTWENTY;
      phraseMINUTES;
      phraseTO;
    }
    if(mytimemin>42 && mytimemin<48){
      
      phraseQUARTER;
      phraseTO;
    }    
    if(mytimemin>47 && mytimemin<53){
      
      phraseTEN;
      phraseMINUTES;
      phraseTO;
    }
    if(mytimemin>52 && mytimemin<58){
      
      phraseFIVE;
      phraseMINUTES;
      phraseTO;
    }


  // Calculate hour & oclocks
  if(mytimehr==1){
    if(mytimemin>32){
      hourTWO;
      phraseOCLOCK;
    }
    else
    {
      hourONE;
      phraseOCLOCK;
    }
  }
  if(mytimehr==2){
    if(mytimemin>32){
      hourTHREE;
      phraseOCLOCK;
    }
    else
    {
      hourTWO;
      phraseOCLOCK;
    }
  }
    if(mytimehr==3){
    if(mytimemin>32){
      hourFOUR;
      phraseOCLOCK;
    }
    else
    {
      hourTHREE;
      phraseOCLOCK;
    }
  }
  if(mytimehr==4){
    if(mytimemin>32){
      hourFIVE;
      phraseOCLOCK;
    }
    else
    {
      hourFOUR;
      phraseOCLOCK;
    }
  }
  if(mytimehr==5){
    if(mytimemin>32){
      hourSIX;
      phraseOCLOCK;
    }
    else
    {
      hourFIVE;
      phraseOCLOCK;
    }
  }
  if(mytimehr==6){
    if(mytimemin>32){
      hourSEVEN;
      phraseOCLOCK;
    }
    else
    {
      hourSIX;
      phraseOCLOCK;
    }
  }
  if(mytimehr==7){
    if(mytimemin>32){
      hourEIGHT;
      phraseOCLOCK;
    }
    else
    {
      hourSEVEN;
      phraseOCLOCK;
    }
  }
  if(mytimehr==8){
    if(mytimemin>32){
      hourNINE;
      phraseOCLOCK;
    }
    else
    {
      hourEIGHT;
      phraseOCLOCK;
    }
  }
  if(mytimehr==9){
    if(mytimemin>32){
      hourTEN;
      phraseOCLOCK;
    }
    else
    {
      hourNINE;
      phraseOCLOCK;
    }
  }
  if(mytimehr==10){
    if(mytimemin>32){
      hourELEVEN;
      phraseOCLOCK;
    }
    else
    {
      hourTEN;
      phraseOCLOCK;
    }
  }
  if(mytimehr==11){
    if(mytimemin>32){
      hourTWELVE;
    }
    else
    {
      hourELEVEN;
      phraseOCLOCK;
    }
  }
  if(mytimehr==12){
    if(mytimemin>32){
      hourONE;
      phraseOCLOCK;
    }
    else
    {
      hourTWELVE;
    }
  }
      if(mytimehr==13){
    if(mytimemin>32){
      hourTWO;
      phraseOCLOCK;
    }
    else
    {
      hourONE;
      phraseOCLOCK;
    }
  }
  if(mytimehr==14){
    if(mytimemin>32){
      hourTHREE;
      phraseOCLOCK;
    }
    else
    {
      hourTWO;
      phraseOCLOCK;
    }
  }
    if(mytimehr==15){
    if(mytimemin>32){
      hourFOUR;
      phraseOCLOCK;
    }
    else
    {
      hourTHREE;
      phraseOCLOCK;
    }
  }
  if(mytimehr==16){
    if(mytimemin>32){
      hourFIVE;
      phraseOCLOCK;
    }
    else
    {
      hourFOUR;
      phraseOCLOCK;
    }
  }
  if(mytimehr==17){
    if(mytimemin>32){
      hourSIX;
      phraseOCLOCK;
    }
    else
    {
      hourFIVE;
      phraseOCLOCK;
    }
  }
  if(mytimehr==18){
    if(mytimemin>32){
      hourSEVEN;
      phraseOCLOCK;
    }
    else
    {
      hourSIX;
      phraseOCLOCK;
    }
  }
  if(mytimehr==19){
    if(mytimemin>32){
      hourEIGHT;
      phraseOCLOCK;
    }
    else
    {
      hourSEVEN;
      phraseOCLOCK;
    }
  }
  if(mytimehr==20){
    if(mytimemin>32){
      hourNINE;
      phraseOCLOCK;
    }
    else
    {
      hourEIGHT;
      phraseOCLOCK;
    }
  }
  if(mytimehr==21){
    if(mytimemin>32){
      hourTEN;
      phraseOCLOCK;
    }
    else
    {
      hourNINE;
      phraseOCLOCK;
    }
  }
  if(mytimehr==22){
    if(mytimemin>32){
      hourELEVEN;
      phraseOCLOCK;
    }
    else
    {
      hourTEN;
      phraseOCLOCK;
    }
  }
  if(mytimehr==23){
    if(mytimemin>32){
      hourTWELVE;
    }
    else
    {
      hourELEVEN;
      phraseOCLOCK;
    }
  }
  if(mytimehr==0){
    if(mytimemin>32){
      hourONE;
      phraseOCLOCK;
    }
    else
    {
      hourTWELVE;
    }
  }
  applyMask();
}

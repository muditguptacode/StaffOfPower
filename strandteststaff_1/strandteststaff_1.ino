#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUMPIXELSS 48
#define TAIL 13
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(NUMPIXELSS, 4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(NUMPIXELSS, 5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(NUMPIXELSS, 6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4 = Adafruit_NeoPixel(NUMPIXELSS, 7, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip5 = Adafruit_NeoPixel(NUMPIXELSS, 8, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip6 = Adafruit_NeoPixel(NUMPIXELSS, 9, NEO_GRB + NEO_KHZ800);

volatile int mode;
Adafruit_NeoPixel strips[6] = {
  strip1, strip2, strip3, strip4, strip5, strip6};
int lastPos=0;
int startPoint=0;
volatile int erased=0;
int ringarray[5] = {
  0,0,0,0,0} 
;

uint32_t ringcolor[5];
int ringdirection[5]= {
  0,0,0,0,0};
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  Serial.begin(9600);
  //randomSeed(analogRead(0));
  strips[0].begin();
  strips[0].show(); // Initialize all pixels to 'off'
  strips[1].begin();
  strips[1].show(); // Initialize all pixels to 'off'
  strips[2].begin();
  strips[2].show(); // Initialize all pixels to 'off'

  strips[3].begin();
  strips[3].show(); // Initialize all pixels to 'off'
  strips[4].begin();
  strips[4].show(); // Initialize all pixels to 'off'
  strips[5].begin();
  strips[5].show(); // Initialize all pixels to 'off'
  attachInterrupt(1, userbutton, RISING);
  attachInterrupt(0, userReset, RISING);
  mode=0;
}

void userbutton() {
  // This debounce routine snarfed from http://forum.arduino.cc/index.php/topic,45000.0.html
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) {
    mode = mode + 1;
    erase();
    if ( mode > 5 ) {
      mode = 0;
    }
  }

  last_interrupt_time = interrupt_time;
}

void userReset() {
  mode=6;
}
void loop() {
  int wait =0;
  switch(mode){
  case 1:
    idler(20, 2, strip1.Color(0,255,0));
    break;
  case 0:
    idlerFade(200, 2, 255, 0, 0);
    break;
  case 2:
    downSpiral(50, 3, strip1.Color(0,0,255));
    break;
  case 3:
    downSpiral(50, 3, strip1.Color(255,0,0));
    break;
  case 4:
    rings();
    break;
  case 5:
    for (int i=0; i<6; i++) {
      setRandom(i);
    }
    break;
  case 6:
    blinkAll(50, strip1.Color(255,255,255));
    break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(Adafruit_NeoPixel strip, uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void matrix() {

  if (erased==0) {
    erase();
  }

  int glyph[10] = {
    0,0,0,0,0,0,0,0,0,0    };
  int glyphstrip[10] = {
    0,0,0,0,0,0,0,0,0,0    };

  while (true) {
    for (int i=0; i<10; i++) {
      delay(10);
      if (mode != 5) {
        return;
      }
      if (glyph[i] == 0) {
        // Should we switch this on?
        if (random(40) == 1) {
          glyph[i] = 1;
          glyphstrip[i] = random(6);
        }
      } 
      else {
        // Okay, it's on, turn the glyph point white, draw the trails green back from it.
        boolean trailing = true;
        int startpoint = glyph[i];
        glyph[i] = startpoint + 1;
        strips[glyphstrip[i]].setPixelColor(startpoint,255,255,255);
        int l = 0;
        int tailvalue = 64;
        while (trailing) {
          startpoint = startpoint-1;
          if (startpoint < 0) {
            trailing = false;
          } 
          else {
            tailvalue = tailvalue - (64/ TAIL);
            strips[glyphstrip[i]].setPixelColor(startpoint,0,tailvalue,0);
            l = l + 1;
            if (l == TAIL) {
              trailing = false;
              strips[glyphstrip[i]].setPixelColor(startpoint,0,0,0);
            }
          }
        }
        if (glyph[i] > NUMPIXELSS + TAIL) {
          glyph[i] = 0;
        }
        strips[glyphstrip[i]].show();
      }
    }
  }
}


void twister() {
  if (erased==0) {
    erase();
  }

  int s = random(5);
  int p = 0;
  int cols[3] = {
    random(255),random(255),random(255)    } 
  ;
  cols[random(3)] = 0;

  while (p < NUMPIXELSS + 11) {
    if (mode != 4) {
      return;
    }
    p++;
    if (p <= NUMPIXELSS) {
      strips[s].setPixelColor(p,cols[0],cols[1],cols[2]);
    }
    s = s + 1;
    if (s > 5) { 
      s = 0; 
    }
    if (p > 10) {
      strips[s].setPixelColor(p - 17,0,0,0);
    }
    strips[s].show();
    delay(10);
  }
  erase();
}


void setRandom(int s) {
  int cols[3] = {
    random(50),random(50),random(50)    } 
  ;
  cols[random(3)] = 0;
  uint32_t on = strip1.Color(cols[0],cols[1],cols[2]);
  uint32_t off = strip2.Color(0,0,0);
  long randNumber = random(NUMPIXELSS);
  strips[s].setPixelColor(randNumber,on);
  strips[s].setPixelColor(random(NUMPIXELSS),off);
  strips[s].setPixelColor(random(NUMPIXELSS),off);
  strips[s].show();
  delay(20);
}

void idler(int wait, int topPixels, uint32_t c)
{
  for(int i=0;i<6;i++)
  {
    for(int j=0;j<6;j++)
    {
      for(int k=0;k<topPixels;k++)
      {
        strips[j].setPixelColor(k,strip1.Color(0,0,0));
      }
      strips[j].show();
    }
    for(int k=0;k<topPixels;k++)
    {
      strips[i].setPixelColor(k,c);
    }
    strips[i].show();
    delay(wait);
  }
}

void idlerFade(int wait, int topPixels, int r, int g, int b)
{
  int divisor=60;
  uint32_t col[6];
  for(int i=0;i<6;i++)
  {
    col[i]=strip1.Color((i+1)*(r/divisor), (i+1)*(g/divisor), (i+1)*(b/divisor));
  }
  //col[5]=strip1.Color(255,255,255);
  for(int i=0;i<6;i++)
  {
    for(int j=0;j<topPixels;j++)
    {
      strips[i].setPixelColor(j,col[(lastPos+i)%6]);
    }
    strips[i].show(); 
  }
  lastPos++;
  delay(wait);
}

void writeAll(uint32_t c)
{
  for(int i=0;i<6;i++)
  {
    for(int j=0;j<NUMPIXELSS;j++)
    {
      strips[i].setPixelColor(j,c);
    }
    strips[i].show();
  }
}

void blinkAll(int wait, uint32_t c)
{
  for(int i=0;i<8;i++)
  {
    writeAll(c);
    delay(wait);
    writeAll(strip1.Color(0,0,0));
    delay(wait);
  }
  mode=0;
}

void downSpiral(int wait,int pixels, uint32_t c)
{
  for(int i=0;i<6;i++)
  {
    for(int j=startPoint;j<startPoint+pixels;j++)
    {
      strips[i].setPixelColor(j,c);
    }
    strips[i].show();
    delay(wait);
    for(int j=startPoint;j<startPoint+pixels;j++)
    {
      strips[i].setPixelColor(j,strip1.Color(0,0,0));
    }
    startPoint++;
    if(startPoint>NUMPIXELSS)
    {
      startPoint=0;
    }
  }
}

void erase() {
  for (int s=0; s<6; s++) {
    for (int i=1; i<=NUMPIXELSS; i++) {
      strips[s].setPixelColor(i,strip1.Color(0,0,0));

    }
    strips[s].show();
    //strips[s].show();
  }
  erased=0;
}

void rings() {

  if (erased==0) {
    erase();
  }

  for (int r=0; r<5; r++) {
    int rpos = ringarray[r];
    if (rpos > 0) {
      for (int s=0; s<6; s++) {
        strips[s].setPixelColor(rpos,ringcolor[r]);
        strips[s].setPixelColor(rpos-1,strip1.Color(0,0,0));
        strips[s].setPixelColor(rpos+1,strip1.Color(0,0,0));

      }
      ringarray[r] = ringarray[r] + ringdirection[r];
      if ((ringarray[r] > NUMPIXELSS) || (ringarray[r] < 1)) {
        ringarray[r]=0;
        for (int s=0; s<6; s++) {
          strips[s].setPixelColor(1,strip1.Color(0,0,0));
        }
      }
    } 
    else {
      if (random(50) == 1) {
        ringarray[r] = 1;
        int colarray[] = { 
          random(255), random(255), random(255)                                         } 
        ;
        colarray[random(3)] = 0;
        ringcolor[r] = strip1.Color(colarray[0],colarray[1],colarray[2]);
        if (random(2) == 1) {
          ringdirection[r]=-1;
          ringarray[r]=NUMPIXELSS-1;
        } 
        else {
          ringdirection[r]=1;
        }
      }
    }
  }
  for (int s=0; s<6; s++) {
    strips[s].show();
  }
  delay(20);
}
void rainbow(Adafruit_NeoPixel stripx, uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<stripx.numPixels(); i++) {
      stripx.setPixelColor(i, Wheel( (i+j) & 255));
    }
    stripx.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(Adafruit_NeoPixel strip, uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(Adafruit_NeoPixel strip, uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(Adafruit_NeoPixel stripx, uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < stripx.numPixels(); i=i+3) {
        stripx.setPixelColor(i+q, Wheel(  (i+j) % 255));    //turn every third pixel on
      }
      stripx.show();

      delay(wait);

      for (int i=0; i < stripx.numPixels(); i=i+3) {
        stripx.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel( byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strips[0].Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strips[0].Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strips[0].Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}















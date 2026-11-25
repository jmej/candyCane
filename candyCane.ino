#include <FastLED.h>
#define NUM_LEDS 157
#define DATA_PIN 25
#define MAX_FRAMES 156
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

CRGB leds[NUM_LEDS];
int frameIndex = 0;
int column = 0; //we have 6 columns
int animation = 0; //index for tracking what animation is running 
static uint32_t lastStep = 0;
bool caneing = false;

bool gReverseDirection = false;

void setup() {
  delay(3000); // sanity delay
  FastLED.addLeds<WS2812, DATA_PIN>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(150);
  FastLED.clear();
}

void loop() {
  // if (millis() - lastStep >= 30000) {
  //   lastStep = millis();
  //   animation++;
  //   animation = animation%3;
  // }
  switch(animation){
    case 0: 
      if(!caneing){
        makeStripes();
        caneing = true;
      }
      caneChase();
      break;
    case 1:
      caneing = false;
      sparkle();
      FastLED.delay(1000 / FRAMES_PER_SECOND);
      break;
    case 2:
      caneing = false;
      random16_add_entropy( random16());
      Fire2012(); // run simulation frame
      FastLED.show(); // display this frame
      FastLED.delay(1000 / FRAMES_PER_SECOND);
      break;
  }

}

//animations

void redToGreen(){
  
}

void colorThrob(){
  
}

void verticalBeam(){
  // static uint32_t lastShift = 0;
  // if (millis() - lastShift >= 150) {
  //   column++;
  //   column = column % 6;
  // }
  for(int i = 0; i < NUM_LEDS; i++){
    if(i % 6 == column){
      leds[i].setRGB(255, 0, 0);
    }else{
      leds[i].setRGB(0, 255, 0);
    }
  }
  
}

void sparkle(){
  for(int i = 0; i < NUM_LEDS; i++){
    if(random(100) < 50){
      leds[i].setRGB(255, 255, 240);
    }else{
      leds[i].setRGB(0, 0, 0);
    }
  }
  FastLED.show();
}
void caneChase(){
    static uint32_t lastShift = 0;
    if (millis() - lastShift >= 150) {
        lastShift = millis();
        shiftRight();
    }
    FastLED.show();
}

void makeStripes(){
  bool red = true; //if false we should be white
  //set alternating 6 pixel stripes of red and white
  for(int i = 0; i < NUM_LEDS; i++){
    if (i % 6 == 0){
      red = !red;
    }
    if(red){
      leds[i].setRGB(255, 0, 0);
    }else{
      leds[i].setRGB(255, 255, 240);
    }
  }
}

//helpers
void shiftRight() {
    CRGB tempColor = leds[NUM_LEDS - 1];
    for(int ledIndex = NUM_LEDS - 1; ledIndex > 0; ledIndex -= 1) {
        leds[ledIndex].setRGB(leds[ledIndex - 1].r, leds[ledIndex - 1].g, leds[ledIndex - 1].b);
    }
    leds[0] = tempColor;
}

void shiftLeft() {
    CRGB tempColor = leds[0];
    for(int ledIndex = 0; ledIndex < NUM_LEDS - 1; ledIndex += 1) {
        leds[ledIndex].setRGB(leds[ledIndex + 1].r, leds[ledIndex + 1].g, leds[ledIndex + 1].b);
    }
    leds[NUM_LEDS - 1] = tempColor;
}

int nextFrameIndex() {
  if (frameIndex < MAX_FRAMES) {
      return frameIndex + 1;
    } else {
      return 0;
    }
}  

void Fire2012()
{
// Array of temperature readings at each simulation cell
  static uint8_t heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}
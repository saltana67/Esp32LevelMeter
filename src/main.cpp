#include <Arduino.h>
#include <math.h>
#include <SPI.h>
#include <Wire.h>

#include <GxEPD2_BW.h>
#include <epd/GxEPD2_213_flex.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#define echoPin 15  // attach pin GPIO18 to pin Echo of JSN-SR04
#define trigPin  4  // attach pin GPIO5 ESP32 to pin Trig of JSN-SR04                     

#define EPD_CS_PIN   5    //
#define EPD_DC_PIN   16
#define EPD_RST_PIN  17
#define EPD_BUSY_PIN 19

//GxEPD2_213_flex display(GxEPD2_213_flex((int16_t) EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN));

GxEPD2_BW<GxEPD2_213_flex, GxEPD2_213_flex::HEIGHT> display(
  GxEPD2_213_flex(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN));


RTC_DATA_ATTR bool    displayFullInit    = true;
RTC_DATA_ATTR double  tankFillState      = -1;

long duration; // Time taken for the pulse to reach the receiver
int  distance; 

u_int16_t capacityInLiters      = 3000;
int       legthInCentimeters    = 220;
//int       diameterInCentimeters = 132;
int       diameterInCentimeters = sqrt(capacityInLiters*1000/legthInCentimeters*4/PI);
//u_int16_t  capacityInLiters      = (PI*(diameterInCentimeters*diameterInCentimeters)/4)*legthInCentimeters/1000;


double circleSegmentArea(double diameterFraction){
    double d = 1.0 - (diameterFraction*2.0);
    double alpha = acos(d);
    double sector_area = PI-alpha;
    double triangle_area = sin(alpha)*d;
    double segment_area = sector_area + triangle_area;
    double segment_area_part = segment_area / PI;
    //log_i("d: %lf, alpha: %lf, sec: %lf, tri: %lf, seg: %lf", d, alpha,sector_area, triangle_area, segment_area);
    return segment_area_part;
}

#define GxEPD_CORNER_LEFT_UPPER   (1<<0)
#define GxEPD_CORNER_RIGHT_UPPER  (1<<1)
#define GxEPD_CORNER_RIGHT_LOWER  (1<<2)
#define GxEPD_CORNER_LEFT_LOWER   (1<<3)
#define GxEPD_HALF_LEFT           (GxEPD_CORNER_LEFT_UPPER|GxEPD_CORNER_LEFT_LOWER)
#define GxEPD_HALF_RIGHT          (GxEPD_CORNER_RIGHT_UPPER|GxEPD_CORNER_RIGHT_LOWER)


void takeMeasurement(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2); 

  digitalWrite(trigPin, HIGH); // turn on the Trigger to generate pulse
  delayMicroseconds(10); // keep the trigger "ON" for 10 ms to generate pulse
  digitalWrite(trigPin, LOW); // Turn off the pulse trigger to stop pulse

  // If pulse reached the receiver echoPin
  // become high Then pulseIn() returns the
  // time taken by the pulse to reach the receiver
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0344 / 2; 
}

void drawTank(double fillingFraction){
  //104x212
  //left & right margins : 2
  //xMIn: 2, width: 100
  //radius: 25
  //x0: 2+25=27
  //bottom margin: 2
  //y0: height-marginBottom-radius=212-2-25=185
  //display.drawCircleHelper(27,185,25,GxEPD_HALF_LEFT,GxEPD_BLACK);

  int16_t hMargin = 0;
  int16_t bMargin = 0;
  int16_t radius = 6;
  int16_t y0 = 112;
  int16_t iO = 4;
  int16_t w = display.width()-hMargin-hMargin;
  int16_t h = display.height()-y0;

  display.drawRoundRect(hMargin   ,y0   ,w      ,h      , radius   , GxEPD_BLACK);
  display.drawRoundRect(hMargin+1 ,y0+1 ,w-2    ,h-2    , radius-1 , GxEPD_BLACK);
  display.drawRoundRect(hMargin+2 ,y0+2 ,w-4    ,h-4    , radius-2 , GxEPD_BLACK);

  int16_t ih  = (h-iO-iO)*fillingFraction;
  int16_t iy0 = y0+iO+((h-iO-iO)-ih);
  log_d("fillingFraction: %.2lf, y0: %d, ih: %d, iy0: %d", fillingFraction, y0,ih,iy0);

  display.fillRoundRect(hMargin+iO,iy0,w-iO-iO,ih, radius-iO, GxEPD_BLACK);
}

void setup()
{
  Serial.begin(115200);
  delay(500);
  Serial.println("Distance measurement using JSN-SR04T");
  Serial.flush();

  //SPI.begin();
  display.epd2.selectSPI(SPI, SPISettings(20000000, MSBFIRST, SPI_MODE0));
  display.init(0, displayFullInit, 2, true); //watchy
  display.setFullWindow();
  //display.init(115200, true, 2, false); //GxEPD2_Example.ino: USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);  

  display.fillScreen(GxEPD_WHITE);
  display.drawCircle(display.width()/2,display.height()/2,(min(display.width(),display.height())-10)/2,GxEPD_BLACK);

  display.display(false);
  displayFullInit = false;
  delay(500);
}

void loop()
{
  // digitalWrite(trigPin, LOW);
  // delayMicroseconds(2); 

  // digitalWrite(trigPin, HIGH); // turn on the Trigger to generate pulse
  // delayMicroseconds(10); // keep the trigger "ON" for 10 ms to generate pulse
  // digitalWrite(trigPin, LOW); // Turn off the pulse trigger to stop pulse

  // // If pulse reached the receiver echoPin
  // // become high Then pulseIn() returns the
  // // time taken by the pulse to reach the receiver
  // duration = pulseIn(echoPin, HIGH);
  // distance = duration * 0.0344 / 2; 

  takeMeasurement();

  double diameterFraction = ((double) distance)/((double)diameterInCentimeters);

  double areaFraction = circleSegmentArea(diameterFraction);
  
  u_int16_t capacityLeft = max((u_int16_t)0,min(((u_int16_t)((1.0 - areaFraction)*capacityInLiters)),capacityInLiters));

//  Serial.print("Distance: ");
//  Serial.print(distance);
//  Serial.println(" cm");
  Serial.printf("distance: %d, diameter: %d, diameterFraction: %.2lf, areaFraction: %.2lf, capacityLeft: %d / %d\n", distance, diameterInCentimeters, diameterFraction,areaFraction,capacityLeft,capacityInLiters );


  if( tankFillState != areaFraction ) {
    
    tankFillState = max(0.0,min(areaFraction,1.0));

    display.fillScreen(GxEPD_WHITE);
    //display.drawCircle(10,20,10,GxEPD_BLACK);

    char TXT_FULL[] = "FULL";
    char proc[] = "100%";
    char *str = proc;
    char left[] = "3000l";

    if( tankFillState < 1.0 ){
      snprintf(proc,4,"%u%%",(u_int16_t)(tankFillState*100.0));
      log_i("proc: %s", proc);
      str=proc;
    }else{
      str=TXT_FULL;
    }
    snprintf(left,5,"%u",capacityLeft);

    int16_t cx=0,cy=100,x1,y1;
    uint16_t w,h;

    const char* USED = "wypelnienie";
    const char* LEFT = "jeszcze (l)";

    display.setTextColor(GxEPD_BLACK);
    display.setTextWrap(false);
    display.setFont(&FreeSans9pt7b);
    
    cx=0,cy=0;
    display.getTextBounds(USED,cx,cy+9,&x1,&y1,&w,&h);
    cx = max(0,(display.width()-w)/2);
    cy += h;
    display.setCursor(cx, cy);
    display.print(USED);  


    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeSansBold24pt7b);
    display.setTextWrap(false);
    cx=0;
    display.getTextBounds(str,cx,cy+24,&x1,&y1,&w,&h);
    //center
    cx = max(0,(display.width()-w)/2);
    cy += h;
    log_d("w: %d, h: %d, cx: %d, cy: %d", w,h,cx,cy);
    display.setCursor(cx, cy);
    display.print(str);  

    display.setTextColor(GxEPD_BLACK);
    display.setTextWrap(false);
    display.setFont(&FreeSans9pt7b);
    
    cx=0;
    display.setFont(&FreeSans9pt7b);
    display.getTextBounds(LEFT,cx,cy+9,&x1,&y1,&w,&h);
    cx = max(0,(display.width()-w)/2);
    cy += h;
    display.setCursor(cx, cy);
    display.print(LEFT);  

    display.setFont(&FreeSansBold24pt7b);
    display.setTextWrap(false);
    cx=0;
    display.getTextBounds(left,cx,cy+24,&x1,&y1,&w,&h);
    //center
    cx = max(0,(display.width()-w)/2);
    cy += h;
    log_d("w: %d, h: %d, cx: %d, cy: %d", w,h,cx,cy);
    display.setCursor(cx, cy);
    display.print(left);  


    drawTank(tankFillState);
    display.display(true);
    delay(500);
  }

  delay(1000);
}
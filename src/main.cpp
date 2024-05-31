#include <Arduino.h>
#include <math.h>

#define echoPin 18 // attach pin GPIO18 to pin Echo of JSN-SR04
#define trigPin 5  // attach pin GPIO5 ESP32 to pin Trig of JSN-SR04                     

long duration; // Time taken for the pulse to reach the receiver
int distance; 

int diameterInCentimeters = 150;

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

void setupT(){
    Serial.begin(115200);
    delay(500);
    Serial.flush();

    log_i("starting ok");
    log_i("circleSegmentArea(0.0): %lf", circleSegmentArea(0.0));
    log_i("circleSegmentArea(0.5): %lf", circleSegmentArea(0.5));
    log_i("circleSegmentArea(0.9): %lf", circleSegmentArea(0.9));
    log_i("circleSegmentArea(0.99): %lf", circleSegmentArea(0.99));
    Serial.flush();
}

void loopT(){

    log_i("circleSegmentArea(0.0): %lf", circleSegmentArea(0.0));
    log_i("circleSegmentArea(0.5): %lf", circleSegmentArea(0.5));
    log_i("circleSegmentArea(0.9): %lf", circleSegmentArea(0.9));
    log_i("circleSegmentArea(0.99): %lf", circleSegmentArea(0.99));
    Serial.flush();
    delay(15000);

}

void setup()
{
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);  
  Serial.begin(115200);
  Serial.println("Distance measurement using JSN-SR04T");
  delay(500);
}

void loop()
{
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

  double diameterFraction = ((double) distance)/((double)diameterInCentimeters);

  double areaFraction = circleSegmentArea(diameterFraction);
  
//  Serial.print("Distance: ");
//  Serial.print(distance);
//  Serial.println(" cm");
  Serial.printf("distance: %d, diameterFraction: %lf, areaFraction: %lf\n", distance, diameterFraction,areaFraction );
  delay(500);
}
/*
 * Using PWM to light the led nicely
 * 
 */

#define LED_LIGHT_PIN       5
#define MAX_BRIGHTNESS      255
#define MIN_BRIGHTNESS      1 
#define OFF                 0

#define LIGHTUPDELAY        500
#define LIGHTDOWNDELAY      1
#define SECONDSTOLIGHTDOWN  600

void light(int brightness)
{
  brightness = constrain(brightness,OFF,MAX_BRIGHTNESS);
  pinMode(LED_LIGHT_PIN, OUTPUT);
  delay(10);
  analogWrite(LED_LIGHT_PIN, brightness);
  
  if ( debugmode ) {
    Serial.print(F("Brightness: "));
    Serial.println(brightness);
    Serial.flush();
  }
  
}

void test_light()
{
  light(MAX_BRIGHTNESS);
  delay(100);
  light(MIN_BRIGHTNESS);
  delay(100);
  light(MAX_BRIGHTNESS);
  delay(100);
  light(MIN_BRIGHTNESS);
}

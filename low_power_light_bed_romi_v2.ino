/* 
 *  SOURCE: http://gammon.com.au/interrupts
 *
 *  Idle usage: 0.01 mA 
 *  On usage:   60.4 mA
 *
 */
 
#define cpu_default   8       // between 0 and 8; 0 being slow
                              // NOTE THAT 0 WILL SOMEHOW NOT WORK!     
#include <avr/sleep.h> 
#include "LowPower.h"

#define debugmode     false
#define testmode      false   // You can use the plotter in test mode

#include "tools.h"
#include "cpu.h"
#include "led.h"

byte cpu_value = cpu_default;

void setup() {

  disable_wdt();
  disable_adc();

  cpu(cpu_value); // The multiplier helps in calculating speeds
  
  if ( debugmode ) { 
    Serial.begin(115200); 
    Serial.println(F("Booting..."));
    Serial.flush();
    Serial.print(F("CPU Multiplier:"));
    Serial.println(multiplier);
    Serial.flush();
  } else {
    disable_uart();             // Disable internal serial logic
  }

  test_light();
}

int   brightness    = 0;
int   newlightvalue = 0;

bool  switchstate   = false;

void loop() {

  sleepNow();
  detachInterrupt (INTERRUPT);  // stop LOW interrupt on D2  

  
  
  
  // WARNING!!! THIS LIKELY DISABLES THE DELAY COMMAND!!!!
  // AN UNRESOLVED BUG!!!
  noInterrupts();




  
  EIFR = bit (INTF0);                       // clear flag for interrupt 0  
  
  if ( debugmode ) {
      Serial.println("Woken from sleep and lighting up");
      Serial.flush();
  }

  // lighting to full brightness
  for ( brightness = constrain(brightness,OFF,MAX_BRIGHTNESS); brightness <= MAX_BRIGHTNESS ; brightness++ ) {
    light(brightness);
    delay(LIGHTUPDELAY);
  }

  // setting seconds to the light down delay
  brightness=SECONDSTOLIGHTDOWN;

  switchstate = digitalRead(INTERRUPT_PIN);
  newlightvalue = map(brightness,0,SECONDSTOLIGHTDOWN,0,255);
  
  // Slowly lighting to off
  while ( brightness > OFF && switchstate == digitalRead(INTERRUPT_PIN) )
  {
//    Disabled this method since the LED switched off / flickered now and then
//    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);

    int delay_time = 5;
    int delay_step = 1;
    for ( int i = 0; i < delay_time; i+=delay_step) {
      if( switchstate != digitalRead(INTERRUPT_PIN) ) break;
      delay(24000);
    }

    brightness--;
    if ( newlightvalue != map(brightness,0,SECONDSTOLIGHTDOWN,0,255) ) {
      newlightvalue = map(brightness,0,SECONDSTOLIGHTDOWN,0,255);
      light(newlightvalue);
    }
    
  }

  if ( debugmode ) {
      Serial.println("Woken from sleep and lighting down");
      Serial.flush();
  }

  for ( brightness = min(brightness,MAX_BRIGHTNESS); brightness >= OFF ; brightness-- ) {
    light(brightness);
    delay(LIGHTDOWNDELAY);
  }

  brightness = OFF;
  
}

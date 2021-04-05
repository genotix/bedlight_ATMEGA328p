#define INTERRUPT_PIN   2
#define INTERRUPT       digitalPinToInterrupt(INTERRUPT_PIN)

/* Divisors 
0000 - 1    16    mhz   
0001 - 2     8    mhz   / 2
0010 - 4     4    mhz   / 4
0011 - 8     2    mhz   / 8
0100 - 16    1    mhz   / 16
0101 - 32     500 khz   / 32
0110 - 64     250 khz   / 64
0111 - 128    125 khz   / 128
1000 - 256     62 khz   / 256
*/
bool wake_by_interrupt = false;

byte cpu_speed[9] = {        // Value 0 through 8
            B00001000, // 256     62 khz   / 256
            B00000111, // 128    125 khz   / 128
            B00000110, // 64     250 khz   / 64
            B00000101, // 32     500 khz   / 32
            B00000100, // 16    1    mhz   / 16
            B00000011, // 8     2    mhz   / 8
            B00000010, // 4     4    mhz   / 4
            B00000001, // 2     8    mhz   / 2
            B00000000, // 1    16    mhz   
                  };

byte cpu_factor[9] = {        // Value 0 through 8
            256, // 256     62 khz   / 256
            128, // 128    125 khz   / 128
            64,  // 64     250 khz   / 64
            32,  // 32     500 khz   / 32
            16,  // 16    1    mhz   / 16
            8,   // 8     2    mhz   / 8
            4,   // 4     4    mhz   / 4
            2,   // 2     8    mhz   / 2
            1,   // 1    16    mhz   
                  };

long multiplier = cpu_factor[cpu_default];

void cpu(byte speed_selector) {
  CLKPR = (1<<CLKPCE);                  // Tell the AtMega we want to change the system clock
  CLKPR = cpu_speed[speed_selector];    // 1/256 prescaler = 60KHz for a 16MHz crystal
  multiplier = cpu_factor[speed_selector];
} 

void disable_wdt() {
  // disable WDT
  // Clear WDRF in MCUSR
  MCUSR &= ~(1 << WDRF);
  // Write logical one to WDCE and WDE
  // Keep old prescaler setting to prevent unintentional time-out
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  // Turn off WDT
  WDTCSR = 0x00;
}

void disable_uart() {
  UCSR0B = 0x00;
}
  
void disable_adc() {
  ADCSRA &= ~(1 << ADEN);
}


// Make sure not to jump to sleep again
void wake() {
  sleep_disable();   
  wake_by_interrupt = true;

  if ( debugmode ) {
   Serial.println("Woken by interrupt");
   Serial.flush();
  }

}

/*
*        (a) SLEEP_15MS - 15 ms sleep
*       (b) SLEEP_30MS - 30 ms sleep
*       (c) SLEEP_60MS - 60 ms sleep
*       (d) SLEEP_120MS - 120 ms sleep
*       (e) SLEEP_250MS - 250 ms sleep
*       (f) SLEEP_500MS - 500 ms sleep
*       (g) SLEEP_1S - 1 s sleep
*       (h) SLEEP_2S - 2 s sleep
*       (i) SLEEP_4S - 4 s sleep
*       (j) SLEEP_8S - 8 s sleep
*       (k) SLEEP_FOREVER - Sleep without waking up through WDT
*/

void sleepNow() {
  // Short delay to keep space for the PWM signal to finish
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  
  sleep_enable();                           // enables the sleep bit in the mcucr register
  noInterrupts();

  pinMode(INTERRUPT_PIN, INPUT_PULLUP);     // SET A PULLUP RESISTOR

  if ( digitalRead(INTERRUPT_PIN) ) {
    if ( debugmode ) {
      Serial.println(F("Will wait for falling input"));
      Serial.flush();
    }
    attachInterrupt(INTERRUPT, wake, FALLING);      // Use interrupt 0 (pin 2) and run function wakeUp when pin 2 gets LOW    
  } else {
    if ( debugmode ) {
      Serial.println(F("Will wait for rising input"));
      Serial.flush();
    }
    attachInterrupt(INTERRUPT, wake, RISING);       // Use interrupt 0 (pin 2) and run function wakeUp when pin 2 gets LOW        
  }

  EIFR = bit (INTF0);                       // clear flag for interrupt 0  

  if ( debugmode ) {
    Serial.println(F("Sleeping CPU"));
    Serial.flush();                   // Make sure all serial commands are executed before entering next step
  }

  interrupts ();
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

/*
 * 
 * DUMMY FUNCTIONS
 * 
 */

void set_interrupt_dummy() {
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);         // INTERRUPT
  bool PortState = digitalRead(INTERRUPT_PIN);

  if ( debugmode ) {
    Serial.print(F("Sensor"));
    Serial.print(F("\t"));
    Serial.println(PortState); 
    Serial.flush();
  }
}

void cpu_sleep_dummy() {
  set_interrupt_dummy();
  // Not doing anything
}

// Release all interrupts dummy
void unset_interrupt_dummy() {
  // Not doing anything
}

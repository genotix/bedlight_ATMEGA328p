#define RXLED           17            // The RX LED has a defined Arduino pin
#define TXLED           30            // The TX LED has a defined Arduino pin

void inputs_lowpower() {
    // Set all pin's to input for low power
    for ( uint8_t i = 0; i < 22; i++ ) {
      pinMode(i,INPUT);
    }
}

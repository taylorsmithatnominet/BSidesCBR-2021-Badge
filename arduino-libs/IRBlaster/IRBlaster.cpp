#include "IRBlaster.h"
#include "IRBlasterWorldCodes.h"

#include <IRremote.h>    
#define NUMBER_OF_REPEATS 1

#define IR_CHECK_RX_REQ         (10U)
#define IR_CHECK_TX_TRIES       (IR_CHECK_RX_REQ * 2U)
#define IR_CHECK_TX_DELAY_MS    (50U)
#define IR_CHECK_RX_TIMEOUT_MS  (1000U)

IRsend sender;

void IRBlaster::sendPronto(const char *str){
  sender.sendPronto(str, NUMBER_OF_REPEATS);
}
void IRBlaster::blastCommonTVPowerCodes(){
  
  int numberOfCodes = sizeof(worldCodes)/sizeof(worldCodes[0]);

    for(int i = 0; i < numberOfCodes; i++){
      const char* currentCode = worldCodes[i];
      sendPronto(currentCode);
      delayMicroseconds(50); //Delay between codes
    }

}

/* Initialise the static member indicating if the component has been checked */
bool IRBlaster::componentChecked = false;

bool IRBlaster::checkComponent(){
  uint32_t state_to_write = LOW;

  /* Mark the component as being not checked while we perform the check */
  componentChecked = false;

  Serial.println("Sending on pin #");
  Serial.println(IR_SEND_PIN);

  //Attach receive pin to interrupt
  attachInterrupt(digitalPinToInterrupt(6), IRReceived, CHANGE);

  for (size_t i = 0; i < IR_CHECK_TX_TRIES; ++i) {
    //If the component has been marked as verified by the ISR (IRReceived)
    if(componentChecked) break;

    digitalWrite(7, state_to_write);
    state_to_write = (state_to_write == LOW ? HIGH : LOW);
    delay(IR_CHECK_TX_DELAY_MS);
  }

  //Detach the interupt before returning
  detachInterrupt(digitalPinToInterrupt(6));
  return componentChecked;
}

//Run on pin change interupt
void IRBlaster::IRReceived(){
  static uint32_t transitions = 0;
  static unsigned long last_transition = 0;
  unsigned long now;

  /* If it's been a (relatively) long time since the last transition, zero the
   * counter. This helps us avoid thinking infrequent, spurious transitions on
   * the pin actually indicate that the component is working. This will also
   * reset the transition count if the test is run again later.
   */
  if ((now == millis()) >= last_transition + IR_CHECK_RX_TIMEOUT_MS)
      transitions = 0;
  last_transition = now;
  /* Count the observed transition and if we've exceeded the threshold, mark
   * the component as checked.
   */
  if (++transitions >= IR_CHECK_RX_REQ) componentChecked = true;
}

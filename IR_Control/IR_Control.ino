#include <IRremote.hpp>

#define IR_RECEIVE_PIN 7   // Signal Pin of IR receiver


void setup()
{
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop()
{
  // Checks received an IR signal
  if (IrReceiver.decode()) {
    //Serial.println(IrReceiver.decodedIRData.decodedRawData);
    translateIR();

    IrReceiver.resume(); // Enable receiving of the next value
  }
}

void translateIR()
{
  // Takes command based on IR code received
  switch (IrReceiver.decodedIRData.decodedRawData) {
    case 3125149440:
      Serial.println("POWER");
      break;
    case 3091726080:
      Serial.println("FUNC/STOP");
      break;
    case 3108437760:
      Serial.println("VOL+");
      break;
    case 3141861120:
      Serial.println("BACK");
      break;
    case 4161273600:
      Serial.println("DOWN");
      break;
    case 4127850240:
      Serial.println("UP");
      break;
    case 3208707840:
      Serial.println("PLAY");
      break;
    case 3158572800:
      Serial.println("NEXT");
      break;
    case 3910598400:
      Serial.println("num: 0");
      break;
    case 3927310080:
      Serial.println("VOL-");
      break;
    case 3860463360:
      Serial.println("EQ");
      break;
    case 4077715200:
      Serial.println("num: 1");
      break;
    case 3877175040:
      Serial.println("num: 2");
      break;
    case 2707357440:
      Serial.println("num: 3");
      break;
    case 4144561920:
      Serial.println("num: 4");
      break;
    case 3810328320:
      Serial.println("num: 5");
      break;
    case 2774204160:
      Serial.println("num: 6");
      break;
    case 3175284480:
      Serial.println("num: 7");
      break;
    case 2907897600:
      Serial.println("num: 8");
      break;
    case 3041591040:
      Serial.println("num: 9");
      break;
    case 4061003520:
      Serial.println("ST/REPT");
      break;
  }
}

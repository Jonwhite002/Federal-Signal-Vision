#include "LightBar\LightBar.h"
#include <IRremote.hpp>

#define IR_RECEIVE_PIN 7   // Signal Pin of IR receiver

LightBar FederalSignal;

void setup() {
  Serial.begin(115200);

  // IR Remote
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  // initialize lightbar
  FederalSignal.Initialize();
}

void loop() {

    // Checks received an IR signal
  if (IrReceiver.decode()) {
    translateIR();
    IrReceiver.resume(); // Enable receiving of the next value
  }

  FederalSignal.Run();

}

void translateIR()
{
  // Takes command based on IR code received
  switch (IrReceiver.decodedIRData.decodedRawData) {
    case 3125149440: // POWER
      FederalSignal.set_Flag_Status(4, FederalSignal.get_Flag_Status(4));
      FederalSignal.set_Flag_Status(5, FederalSignal.get_Flag_Status(5));
      break;
    case 3091726080: // FUNC/STOP
      FederalSignal.set_Flag_Status(3, FederalSignal.get_Flag_Status(3));
      break;
    case 3108437760: // VOL+
      FederalSignal.set_Flag_Status(2, FederalSignal.get_Flag_Status(2));
      break;
    case 3141861120: // BACK
      FederalSignal.set_SignalMaster(5);
      break;
    case 4161273600: // DOWN
      FederalSignal.set_SignalMaster(1);
      break;
    case 4127850240: // UP
      FederalSignal.set_SignalMaster(3);
      break;
    case 3208707840: // PLAY
      FederalSignal.set_SignalMaster(7);
      break;
    case 3158572800: // NEXT
      FederalSignal.set_SignalMaster(6);
      break;
    case 3910598400: // 0
      FederalSignal.set_Code(0);
      break;
    case 3927310080: // VOL-
      FederalSignal.set_SignalMaster(2);
      break;
    case 3860463360: // EQ
      FederalSignal.set_SignalMaster(0);
      break;
    case 4077715200: // 1
      FederalSignal.set_Code(1);
      break;
    case 3877175040: // 2
      FederalSignal.set_Code(2);
      break;
    case 2707357440: // 3
      FederalSignal.set_Code(3);
      break;
    case 4144561920: // 4
      FederalSignal.set_Code(4);
      break;
    case 3810328320: // 5
      FederalSignal.set_Flag_Status(0, FederalSignal.get_Flag_Status(0));
      break;
    case 2774204160: // 6
      FederalSignal.set_Flag_Status(1, FederalSignal.get_Flag_Status(1));
      break;
    case 3175284480: // 7
      Serial.println("num: 7");
      break;
    case 2907897600: // 8
      Serial.println("num: 8");
      break;
    case 3041591040: // 9
      Serial.println("num: 9");
      break;
    case 4061003520: // ST/REPT
      FederalSignal.set_SignalMaster(4);
      break;
  }
}

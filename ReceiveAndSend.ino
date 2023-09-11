/*
 * Record and play back last received IR signal at button press.
 * If the protocol is unknown or not enabled, store it as raw data for later sending.
 *
 * An IR detector/demodulator -> IR_RECEIVE_PIN.
 * button -> SEND_BUTTON_PIN
 * A visible LED can be connected to STATUS_PIN to provide status.
 */

#include <Arduino.h>
#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.

// #define DECODE_NEC          // Includes Apple and Onkyo

#if !defined(RAW_BUFFER_LENGTH)
    #if RAMEND <= 0x4FF || RAMSIZE < 0x4FF
        #define RAW_BUFFER_LENGTH  120
    #elif RAMEND <= 0xAFF || RAMSIZE < 0xAFF // 0xAFF for LEONARDO
        #define RAW_BUFFER_LENGTH  400 // 600 is too much here, because we have additional uint8_t rawCode[RAW_BUFFER_LENGTH];
    #else
        #define RAW_BUFFER_LENGTH  750
    #endif
#endif

#define MARK_EXCESS_MICROS    20    // Adapt it to your IR receiver module. 20 is recommended for the cheap VS1838 modules.

#define DELAY_BETWEEN_REPEAT  50

// #define DEBUG // Activate this for lots of lovely debug output from the decoders.

#include <IRremote.hpp>

// Storage for the recorded code
struct storedIRDataStruct {
    //IRData receivedIRData;
    // extensions for sendRaw
    uint8_t rawCode[RAW_BUFFER_LENGTH]; // The durations if raw
    uint8_t rawCodeLength; // The length of the code
} sStoredIRData, sStoredIRData0, sStoredIRData1, sStoredIRData2, sStoredIRData3, sStoredIRData4, sStoredIRData5, sStoredIRData6;

bool SendButtonWasActive = 0;

void storeCode();
void sendCode(storedIRDataStruct *aIRDataToSend);

void setup() {
    pinMode(PB1, INPUT_PULLUP);
    pinMode(PB2, INPUT_PULLUP);
    pinMode(PB3, INPUT_PULLUP);

    // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    IrSender.begin(); // Start with IR_SEND_PIN as send pin and enable feedback LED at default feedback LED pin
}

void loop() {
    // If button pressed, send the code.
    byte anyButtonPressed = (digitalRead(PB1) & 1) | ((digitalRead(PB2) & 1) << 1) | ((digitalRead(PB3) & 1) << 2); //readSequential();

    if(anyButtonPressed == 0)
        sStoredIRData = sStoredIRData0;
    else if(anyButtonPressed == 1)
        sStoredIRData = sStoredIRData1;
    else if(anyButtonPressed == 2)
        sStoredIRData = sStoredIRData2;
    else if(anyButtonPressed == 3)
        sStoredIRData = sStoredIRData3;
    else if(anyButtonPressed == 4)
        sStoredIRData = sStoredIRData4;
    else if(anyButtonPressed == 5)
        sStoredIRData = sStoredIRData5;
    else if(anyButtonPressed == 6)
        sStoredIRData = sStoredIRData6;

    if (anyButtonPressed != 7) {
        if (!SendButtonWasActive) {
            IrReceiver.stop();
        }
        
        // Button pressed -> send stored data
        /*if (SendButtonWasActive == 1) {
            sStoredIRData.receivedIRData.flags = IRDATA_FLAGS_IS_REPEAT;
        }
        else {
            sStoredIRData.receivedIRData.flags = IRDATA_FLAGS_EMPTY;
        }*/
        
        sendCode(&sStoredIRData);
        delay(DELAY_BETWEEN_REPEAT); // Wait a bit between retransmissions

    }
    else if (SendButtonWasActive) {
        // Button is just released -> activate receiving
        // Restart receiver
        IrReceiver.start();
    }
    else if (IrReceiver.decode() && analogRead(PB0)<900 && anyButtonPressed != 7) {
        storeCode(anyButtonPressed, &sStoredIRData);
        IrReceiver.resume(); // resume receiver
    }

    if(anyButtonPressed != 7 && analogRead(PB0)>900) {
      SendButtonWasActive = 1;
    }
    else {
      SendButtonWasActive = 0;
    }
    delay(100);
}

// Stores the code for later playback in sStoredIRData
// Most of this code is just logging
void storeCode(byte anyButtonPressed, storedIRDataStruct *storedData) {
    if (IrReceiver.decodedIRData.rawDataPtr->rawlen < 4) {
        return;
    }
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
        // Ignore repeat
        return;
    }
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_AUTO_REPEAT) {
        // Ignore autorepeat
        return;
    }
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_PARITY_FAILED) {
        // Ignore parity error
        return;
    }
    
    // Copy decoded data
    //storedData->receivedIRData = IrReceiver.decodedIRData;

    storedData->rawCodeLength = IrReceiver.decodedIRData.rawDataPtr->rawlen - 1;
    // Store the current raw data in a dedicated array for later usage
    IrReceiver.compensateAndStoreIRResultInArray(storedData->rawCode);
}


void sendCode(storedIRDataStruct *aIRDataToSend) {
    // Assume 38kHz
    IrSender.sendRaw(aIRDataToSend->rawCode, aIRDataToSend->rawCodeLength, 38);
}

/*byte readSequential(){
    byte buttonId = (digitalRead(PB1) & 1) | ((digitalRead(PB2) & 1) << 1) | ((digitalRead(PB3) & 1) << 2);
    if(digitalRead(PB1) == 1) {
        buttonId = buttonId | 1;
    }
    if(digitalRead(PB2) == 1) {
        buttonId = buttonId | (1 << 1);
    }
    if(digitalRead(PB3) == 1) {
        buttonId = buttonId | (1 << 2);
    }
    return buttonId;
}*/


/*
 * Pin mapping table for different platforms
 *
 * Platform     IR input    IR output   Tone      Core/Pin schema
 * --------------------------------------------------------------
 * DEFAULT/AVR  2           3           4         Arduino
 * ATtinyX5     0|PB0       4|PB4       3|PB3     ATTinyCore
 */
//#define _IR_MEASURE_TIMING // For debugging purposes.

#if defined(__AVR__)
    #if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) // Digispark board. For use with ATTinyCore.
        //#include "ATtinySerialOut.hpp" // TX is at pin 2 - Available as Arduino library "ATtinySerialOut". Saves 700 bytes program memory and 70 bytes RAM for ATtinyCore.
        #define IR_RECEIVE_PIN  PB0
        #define IR_SEND_PIN     PB4 // Pin 2 is serial output with ATtinySerialOut. Pin 1 is internal LED and Pin3 is USB+ with pullup on Digispark board.
        #define TONE_PIN        PB3
        #define _IR_TIMING_TEST_PIN PB3

    #else // Default as for ATmega328 like on Uno, Nano, Leonardo, Teensy 2.0 etc.
        #define IR_RECEIVE_PIN      2 // To be compatible with interrupt example, pin 2 is chosen here.
        #define IR_SEND_PIN         3
        #define TONE_PIN            4
        #define APPLICATION_PIN     5
        #define ALTERNATIVE_IR_FEEDBACK_LED_PIN 6 // E.g. used for examples which use LED_BUILDIN for example output.
        #define _IR_TIMING_TEST_PIN 7
    #endif // defined(__AVR_ATtiny25__)...
#endif

#if !defined (FLASHEND)
    #define FLASHEND 0xFFFF // Dummy value for platforms where FLASHEND is not defined
#endif
#if !defined (RAMEND)
    #define RAMEND 0xFFFF // Dummy value for platforms where RAMEND is not defined
#endif
#if !defined (RAMSIZE)
    #define RAMSIZE 0xFFFF // Dummy value for platforms where RAMSIZE is not defined
#endif

/*
 * Helper macro for getting a macro definition as string
 */
#if !defined(STR_HELPER)
    #define STR_HELPER(x) #x
    #define STR(x) STR_HELPER(x)
#endif

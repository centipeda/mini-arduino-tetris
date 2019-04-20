#include "pitches.h"
/* Theme from Howl's Moving Castle. G Minor, 3/4 time. */
const float song[] PROGMEM {
  NOTE_D4, 4, NOTE_G4, 4, NOTE_AS4, 4,
  NOTE_D5, 2, NOTE_D5, 4,
  NOTE_C5, 4, NOTE_AS4, 4, NOTE_A4, 4, 
  NOTE_AS4, 2.666,
  REST, 4, REST, 8, NOTE_G4, 8, NOTE_AS4, 8, NOTE_D5, 8, 
  NOTE_G5, 2, NOTE_G5, 4,
  NOTE_G5, 4, NOTE_A5, 4, NOTE_F5, 8, NOTE_DS5, 8,
  NOTE_F5, 2.666,
  REST, 4, REST, 8, NOTE_A4, 8, NOTE_D5, 8, NOTE_F5, 8,
  NOTE_A5, 2, NOTE_G5, 4,
  NOTE_F5, 4, NOTE_E5, 4, NOTE_F5, 4,
  NOTE_G5, 2, NOTE_F5, 4,
  NOTE_E5, 2, NOTE_D5, 4,
  NOTE_C5, 4, NOTE_AS4, 4, NOTE_C5, 4,
  NOTE_D5, 4, NOTE_C5, 4, NOTE_G5, 4,
  NOTE_A4, 3,
  NOTE_A5, 13, NOTE_C6, 13, NOTE_AS5, 2.666, NOTE_A5, 2.666,
  NOTE_A5, 13, NOTE_C6, 13, NOTE_AS5, 4, NOTE_A5, 8, NOTE_G5, 8, NOTE_FS5, 8, NOTE_DS5, 8,
  NOTE_D5, 3
};
#define NOTES 58
#define BPM 50.0

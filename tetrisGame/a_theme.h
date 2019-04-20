#include "pitches.h"
/* Tetris A theme. Cut-time, in C major. */
  const float song[] PROGMEM = { \
  NOTE_E5, 4, NOTE_B4, 8, NOTE_C5, 8,
  NOTE_D5, 4, NOTE_C5, 8, NOTE_B4, 8,
  NOTE_A4, 4, NOTE_A4, 8, NOTE_C5, 8,
  NOTE_E5, 4, NOTE_D5, 8, NOTE_C5, 8,
  NOTE_B4, 2.666, NOTE_C5, 8, // 2.666 for dotted quarter
  NOTE_D5, 4, NOTE_E5, 4,
  NOTE_C5, 4, NOTE_A4, 4,
  NOTE_A4, 4, REST, 4, 

  NOTE_D4, 8, NOTE_D5,  4, NOTE_F5, 8,
  NOTE_A5, 4, NOTE_G5, 8, NOTE_F5, 8,
  NOTE_E5, 4, NOTE_E5, 8, NOTE_C5, 8,
  NOTE_E5, 4, NOTE_D5, 8, NOTE_C5, 8,
  NOTE_B4, 4, NOTE_B4, 8, NOTE_C5, 8,
  NOTE_D5, 4, NOTE_E5, 4,
  NOTE_C5, 4, NOTE_A4, 4,
  NOTE_A4, 4, REST, 4
  };
  #define NOTES 41
  #define BPM 38.0

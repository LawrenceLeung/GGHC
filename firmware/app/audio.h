/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : audio_desc.h
 *    Description : AUDIO descriptor definitions
 *
 *    History :
 *    1. Date        : November 29, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 1.4 $
 **************************************************************************/
#ifndef __AUDIO_H
#define __AUDIO_H


#define NUMBER_OF_NOTES 6 // Metronome plus 5 notes

void InitAudioDevice(void);
void metronome(void);
void SetMetronomePeriod(unsigned long newPeriod);

typedef struct oneNote
  {
  Boolean noteOn; // True if note is currently being played
  pInt16S noteVoiceBuffer; // Points to the wave file
  Int16U frequency; // May not need this as period is 1/frequency
  Int16U period; // In sample ticks
  } Note;

extern Note notes[NUMBER_OF_NOTES];
#endif //__AUDIO_H

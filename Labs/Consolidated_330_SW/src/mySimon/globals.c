/*
 * globals.c
 *
 *  Created on: Nov 2, 2016
 *      Author: cdmoo
 */
#include "globals.h"
#define MAX_SEQUENCE_LENGTH 100 // global that designates the maximum sequence length

// the length of this specific iteration of the sequence
static uint16_t globals_sequenceIterationLength = 0;
static uint16_t globals_sequenceLength = 0;  // The length of the sequence.
// construct the empty sequence.
// It is now ready for the sequence values calculated in simon control to be copied into it
static uint8_t globals_sequence[MAX_SEQUENCE_LENGTH];


// This is the length of the sequence that you are currently working on,
// not the maximum length but the interim length as
// the user works through the pattern one color at a time.
void globals_setSequenceIterationLength(uint16_t length) {
    globals_sequenceIterationLength = length;
}

// getter for the sequence iteration length
uint16_t globals_getSequenceIterationLength() {
    return globals_sequenceIterationLength; // return the sequence iteration length
}

// getter for the sequence length
uint16_t globals_getSequenceLength() {
    return globals_sequenceLength; // return the sequence length
}

// setter for the sequence (deep copy, not a shallow copy of the passed in sequence)
void globals_setSequence(const uint8_t sequence[], uint16_t length) {
    globals_sequenceLength = length; // set the length
    // copy the values of the sequence array into the globals sequence array
    for(int i = 0; i < length; i++) {
        globals_sequence[i] = sequence[i];
    }
}

// getter for the value of a specific value within the sequence at an index
uint8_t globals_getSequenceValue(uint16_t index) {
    return globals_sequence[index]; // return the sequence value at a specific index
}




#ifndef STATE_H
#define STATE_H

// Define state as a typedef enum
typedef enum {
  STATE_DEFAULT,
  STATE_PHASE1,
  STATE_PHASE2,
  STATE_PHASE3
} State;

// Declare the shared state variable
extern State currentState;

#endif 

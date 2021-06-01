#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUGGING
#define DEBUG_B(...) Serial.begin(__VA_ARGS__)
#define DEBUG_P(...) Serial.println(__VA_ARGS__)
#define DEBUG_F(...) Serial.printf(__VA_ARGS__)
#define DEBUG_T(...) Serial.print(__VA_ARGS__)
#else
#define DEBUG_B(...)
#define DEBUG_P(...)
#define DEBUG_F(...)
#define DEBUG_T(...)
#endif

#endif
#include <stdint.h>

// NEC timing in microseconds, assuming 1 MHz timer clock
#define NEC_HDR_LOW_MIN 8000 // 9 ms low
#define NEC_HDR_LOW_MAX 10000
#define NEC_HDR_HIGH_MIN 4000 // 4.5 ms high
#define NEC_HDR_HIGH_MAX 5000

#define NEC_BIT_LOW_MIN 400 // 560 us low
#define NEC_BIT_LOW_MAX 800

#define NEC_ZERO_HIGH_MIN 400 // 560 us high
#define NEC_ZERO_HIGH_MAX 800
#define NEC_ONE_HIGH_MIN 1400 // 1.69 ms high
#define NEC_ONE_HIGH_MAX 1900

#define NEC_GAP_RESET 20000 // >20 ms => reset state

void nec_received(uint32_t code);

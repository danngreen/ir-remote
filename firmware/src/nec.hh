#pragma once
#include <cstdint>

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

enum nec_state_t { NEC_STATE_IDLE = 0, NEC_STATE_LEAD_LOW, NEC_STATE_LEAD_HIGH, NEC_STATE_BITS };

// enum Code { Down = 0xF7087F80, Up = 0xFD027F80 };
enum Code { Up = 0xFD020F04, Down = 0xFC030F04 };

static inline bool nec_validate(uint32_t code) {
	uint8_t addr = code & 0xFF;
	uint8_t addr_inv = (code >> 8) & 0xFF;
	uint8_t cmd = (code >> 16) & 0xFF;
	uint8_t cmd_inv = (code >> 24) & 0xFF;

	if ((addr ^ addr_inv) != 0xFF || (cmd ^ cmd_inv) != 0xFF) {
		// checksum failed
		return false;
	}

	return true;
}

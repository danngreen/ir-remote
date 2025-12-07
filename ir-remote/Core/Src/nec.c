#include "nec.h"

void nec_received(uint32_t code) {
	uint8_t addr = code & 0xFF;
	uint8_t addr_inv = (code >> 8) & 0xFF;
	uint8_t cmd = (code >> 16) & 0xFF;
	uint8_t cmd_inv = (code >> 24) & 0xFF;

	if ((addr ^ addr_inv) != 0xFF || (cmd ^ cmd_inv) != 0xFF) {
		// checksum failed, ignore
		return;
	}

	// Valid NEC frame: addr / cmd
	// Handle command here
}

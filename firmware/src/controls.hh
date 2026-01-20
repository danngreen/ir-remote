#pragma once
#include "conf/board_conf.hh"
#include "drivers/adc_builtin.hh"
#include "util/filter.hh"
#include <cstdint>

namespace IRRemote
{

class Controls {
	static inline __attribute__((section(".noncachable"))) std::array<uint16_t, 1> adc_buffer; // pad to 32-bits

	mdrivlib::AdcDmaPeriph<Board::AdcConf> adc{adc_buffer, Board::AdcChans};

	Oversampler<64, uint16_t> smooth_adc;

public:
	Controls() = default;

	uint16_t read_adc() {
		return smooth_adc.val();
	}

	void start() {
		adc.register_callback([this] { smooth_adc.add_val(adc_buffer[0]); });
		adc.start();
	}

	void update() {
	}
};
} // namespace IRRemote

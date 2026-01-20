#pragma once
#include "conf/board_conf.hh"
#include "debug.hh"
#include "drivers/adc_builtin.hh"
#include "util/filter.hh"
#include <cstdint>

namespace IRRemote
{

class Controls {
	static inline __attribute__((section(".noncachable"))) std::array<uint16_t, 1> adc_buffer;

	mdrivlib::AdcDmaPeriph<Board::AdcConf> adc{adc_buffer, Board::AdcChans};

	Oversampler<64, uint16_t> smooth_adc;

public:
	Controls() = default;

	uint16_t read_adc() {
		// return adc_buffer[0];
		return smooth_adc.val();
	}

	void start() {
		// 829kHz => OS: 12.95kHz = 77us between reads
		adc.register_callback([this] {
			// Debug::Pin0::high();
			smooth_adc.add_val(adc_buffer[0]);
			// Debug::Pin0::low();
		});

		adc.start();
	}

	void update() {
	}
};
} // namespace IRRemote

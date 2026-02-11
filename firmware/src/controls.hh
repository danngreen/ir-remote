#pragma once
#include "conf/board_conf.hh"
#include "debug.hh"
#include "drivers/adc_builtin.hh"
#include "ir_capture.hh"
#include "util/filter.hh"
#include <cstdint>

namespace RemoteVolume
{

class Controls {
	static inline std::array<uint16_t, 1> adc_buffer;
	mdrivlib::AdcDmaPeriph<Board::AdcConf> adc{adc_buffer, Board::AdcChans};
	Oversampler<64, uint16_t> smooth_adc;

	IRCapture ir;

public:
	Controls() = default;

	uint16_t raw_adc();
	uint16_t read_adc();

	void start() {
		// 829kHz => OS: 12.95kHz = 77us between reads
		adc.register_callback([this] { smooth_adc.add_val(adc_buffer[0]); });

		adc.start();
	}
};
} // namespace RemoteVolume

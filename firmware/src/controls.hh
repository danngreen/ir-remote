#pragma once
#include "conf/board_conf.hh"
#include "console_events.hh"
#include "debug.hh"
#include "drivers/adc_builtin.hh"
#include "event.hh"
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
	ConsoleEventSource console_events;

public:
	Controls();

	uint16_t raw_adc();
	uint16_t read_adc();

	// Polls all event sources and returns the next pending Event (None if idle).
	// IR takes priority over the console when both have something.
	Event get_event() {
		if (auto e = ir.get_event(); e != Event::None)
			return e;
		return console_events.get_event();
	}

	mdrivlib::Pin mot1;
	mdrivlib::Pin mot2;

	void start() {
		mot1.low();
		mot2.low();

		// 829kHz => OS: 12.95kHz = 77us between reads
		adc.register_callback([this] { smooth_adc.add_val(adc_buffer[0]); });

		adc.start();
	}
};
} // namespace RemoteVolume

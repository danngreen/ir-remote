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
	float scale_adc();

	// Polls all event sources and returns the next pending event (None if idle).
	// IR takes priority over the console; console events are always presses.
	EventMsg get_event() {
		if (auto m = ir.get_event(); m.event != Event::None)
			return m;
		return {console_events.get_event(), false};
	}

	// Poll the event sources (IR + console) and drive the motor in response
	void process_events();

	static constexpr uint32_t MotorPulseUs = 3000; // single-tap (fine) pulse length

	// Slider end-stops
	static constexpr uint16_t AdcMin = 30;
	static constexpr uint16_t AdcMax = 8150;

	// Continuous hold while a button is held (driven by NEC repeats), with acceleration.
	static constexpr uint32_t HoldReleaseMs = 160;	  // stop this long after the last repeat (button released)
	static constexpr uint32_t HoldPwmPeriodUs = 9000; // PWM period while ramping
	static constexpr uint32_t HoldRampMs = 2000;	  // time from engage to max-speed duty
	static constexpr uint8_t HoldDutyMinPct = 20;	  // starting duty (%) at engage
	static constexpr uint8_t HoldDutyMaxPct = 40;	  // top duty (%) the ramp reaches

	mdrivlib::Pin mot1;
	mdrivlib::Pin mot2;

	void start() {
		mot1.low();
		mot2.low();

		// 829kHz => OS: 12.95kHz = 77us between reads
		adc.register_callback([this] { smooth_adc.add_val(adc_buffer[0]); });

		adc.start();
	}

private:
	void pulse_up(int duration);
	void pulse_down(int duration);

	void fine_pulse(Event dir); // one-shot tap pulse (rail-checked)
	void drive(Event dir);		// energize the H-bridge in dir (non-blocking)
	void motor_off();
	bool at_rail(Event dir);
	void do_mute();

	// Continuous hold, driven by held-button NEC repeats
	void sustain_hold(Event dir, uint32_t now); // a repeat arrived: (re)start/refresh the hold
	void service_hold();						// drive one accel/PWM slice while held
	void cancel_hold();

	Event hold_dir = Event::None;  // active continuous hold (None = not holding)
	uint32_t hold_start_ms = 0;	   // HAL tick when the hold engaged (accel ramp origin)
	uint32_t hold_deadline_ms = 0; // stop driving after this (no repeat => released)
};
} // namespace RemoteVolume

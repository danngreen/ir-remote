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

	// Polls all event sources and returns the next pending Event (None if idle).
	// IR takes priority over the console
	Event get_event() {
		if (auto e = ir.get_event(); e != Event::None)
			return e;
		return console_events.get_event();
	}

	// Poll the event sources (IR + console) and drive the motor in response
	void process_events();

	static constexpr uint32_t MotorPulseUs = 3000; // single-tap (fine) pulse length
	// Slider end-stops
	static constexpr uint16_t AdcMin = 30;
	static constexpr uint16_t AdcMute = 30;
	static constexpr uint16_t AdcMax = 8150;

	// Stall guard: if the motor is driven but the slider stops moving, we've hit
	// an end stop (or jammed) -> stop driving. Robust backstop for AdcMin/AdcMax,
	// which may not be reachable in practice (offset/noise keep the ends off 0/full).
	static constexpr uint16_t StallMoveCounts = 15; // min raw-ADC change that counts as movement
	static constexpr uint32_t StallTimeoutMs = 10;	// driven with no movement this long => stopped

	// Hold-to-repeat: continuous drive with acceleration while a button is held.
	static constexpr uint32_t RepeatEngageMs =
		100; // 2nd same-dir event within this => engage hold (> ~108ms repeat period)
	static constexpr uint32_t HoldReleaseMs = 160;	  // stop this long after the last repeat (button released)
	static constexpr uint32_t HoldRampMs = 2000;	  // time from engage to max-speed duty
	static constexpr uint32_t HoldPwmPeriodUs = 8000; // software-PWM period while ramping
	static constexpr uint8_t HoldDutyMinPct = 20;	  // starting duty (%) at engage
	static constexpr uint8_t HoldDutyMaxPct = 30; // top duty (%) the ramp reaches — lower this to cap max hold speed

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

	// Hold-to-repeat state machine
	void on_dir_event(Event dir, uint32_t now); // a VolumeUp/Down arrived
	void service_hold();						// drive one accel/PWM slice while held
	void cancel_hold();

	Event hold_dir = Event::None;  // active continuous hold (None = not holding)
	Event armed_dir = Event::None; // last tap dir, candidate to escalate to a hold
	uint32_t armed_ms = 0;		   // HAL tick of the last tap
	uint32_t hold_start_ms = 0;	   // HAL tick when the hold engaged (accel ramp origin)
	uint32_t hold_deadline_ms = 0; // stop driving after this (no repeat => released)
};
} // namespace RemoteVolume

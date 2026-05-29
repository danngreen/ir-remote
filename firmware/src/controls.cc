#include "controls.hh"
#include "drivers/stm32xx.h"
#include "printf.h"

namespace RemoteVolume
{

namespace
{
// Blocking busy-wait via the DWT cycle counter — cycle-accurate and independent
// of the SysTick/HAL tick. Blocking here is fine: audio runs in the SAI ISRs.
void delay_us(uint32_t us) {
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	const uint32_t start = DWT->CYCCNT;
	const uint32_t cycles = us * (SystemCoreClock / 1'000'000u);
	while ((DWT->CYCCNT - start) < cycles)
		;
}
} // namespace

Controls::Controls()
	: mot1{Board::Motor1, mdrivlib::PinMode::Output}
	, mot2{Board::Motor2, mdrivlib::PinMode::Output} {
	mot1.low();
	mot2.low();
}

uint16_t Controls::raw_adc() {
	return adc_buffer[0];
}

uint16_t Controls::read_adc() {
	return smooth_adc.val();
}

float Controls::scale_adc() {
	auto adc = std::clamp<uint32_t>(smooth_adc.val(), AdcMin, AdcMax);
	return (adc - AdcMin) / float(AdcMax - AdcMin);
}

void Controls::pulse_up(int duration = MotorPulseUs) {
	mot1.low();
	mot2.high();
	delay_us(duration);
	mot2.low();
}

void Controls::pulse_down(int duration = MotorPulseUs) {
	mot2.low();
	mot1.high();
	delay_us(duration);
	mot1.low();
}

void Controls::motor_off() {
	mot1.low();
	mot2.low();
}

void Controls::drive(Event dir) {
	if (dir == Event::VolumeUp) {
		mot1.low();
		mot2.high();
	} else if (dir == Event::VolumeDown) {
		mot2.low();
		mot1.high();
	}
}

bool Controls::at_rail(Event dir) {
	// raw_adc() (not the smoothed value) so we stop promptly at the end stop.
	const uint16_t adc = raw_adc();
	if (dir == Event::VolumeUp)
		return adc >= AdcMax;
	if (dir == Event::VolumeDown)
		return adc <= AdcMin;
	return true;
}

void Controls::fine_pulse(Event dir) {
	if (at_rail(dir))
		return;
	if (dir == Event::VolumeUp)
		pulse_up();
	else if (dir == Event::VolumeDown)
		pulse_down();
}

void Controls::cancel_hold() {
	hold_dir = Event::None;
	motor_off();
}

// A held-button NEC repeat arrived: (re)start the continuous hold in this
// direction and push out the release deadline. A discrete press is handled
// separately (a one-shot fine pulse), so taps never turn into a hold.
void Controls::sustain_hold(Event dir, uint32_t now) {
	if (hold_dir != dir) { // (re)start: reset the acceleration ramp origin
		hold_dir = dir;
		hold_start_ms = now;
	}
	hold_deadline_ms = now + HoldReleaseMs;
}

// Called every main-loop pass. While a hold is active, drives one software-PWM
// slice whose duty ramps up over HoldRampMs (acceleration), stopping at the rail
// or once no repeat has refreshed the deadline (button released).
void Controls::service_hold() {
	if (hold_dir == Event::None)
		return;

	const uint32_t now = HAL_GetTick();

	if (now > hold_deadline_ms) {
		cancel_hold();
		return;
	}

	if (at_rail(hold_dir)) {
		motor_off();
		return;
	}

	// Ramp duty from HoldDutyMinPct up to HoldDutyMaxPct over HoldRampMs.
	// (Max <= Min => no ramp, i.e. constant speed at HoldDutyMinPct.)
	const uint32_t held = now - hold_start_ms;
	const uint32_t ramp = held < HoldRampMs ? held : HoldRampMs;
	const uint32_t span = HoldDutyMaxPct > HoldDutyMinPct ? (HoldDutyMaxPct - HoldDutyMinPct) : 0;
	const uint32_t duty = HoldDutyMinPct + span * ramp / HoldRampMs;

	const uint32_t on_us = HoldPwmPeriodUs * duty / 100;
	drive(hold_dir);
	delay_us(on_us);
	if (on_us < HoldPwmPeriodUs) {
		motor_off();
		delay_us(HoldPwmPeriodUs - on_us);
	}
}

void Controls::do_mute() {
	const uint16_t adc = read_adc();
	int pulses = 0;
	if (raw_adc() > 0) {
		if (adc >= AdcMax)
			pulse_down(50000);
		else if (adc >= 7000)
			pulse_down(43000);
		else if (adc >= 6000)
			pulse_down(36000);
		else if (adc >= 5000)
			pulse_down(33000);
		else if (adc >= 4000)
			pulse_down(30000);
		else if (adc >= 3000)
			pulse_down(24500);
		else if (adc >= 2000)
			pulse_down(20000);
		else if (adc >= 1000)
			pulse_down(13000);
		else if (adc >= 500)
			pulse_down(6000);
		else if (adc >= 300)
			pulse_down(3000);

		HAL_Delay(20);
		while (raw_adc() > AdcMin) {
			pulse_down(1000);
			delay_us(8000);
		}
	}
	printf_("Mute (adc %u=>%u) pulsed %d\n", adc, raw_adc(), pulses);
}

void Controls::process_events() {
	const uint32_t now = HAL_GetTick();

	const auto msg = get_event();
	switch (msg.event) {
		case Event::VolumeUp:
		case Event::VolumeDown:
			if (msg.repeat) {
				sustain_hold(msg.event, now); // held button: continuous ramped drive
			} else {
				cancel_hold(); // discrete press: one fine pulse
				fine_pulse(msg.event);
			}
			break;

		case Event::Mute:
			cancel_hold();
			do_mute();
			break;

		case Event::None:
			break;
	}

	service_hold();
}

} // namespace RemoteVolume

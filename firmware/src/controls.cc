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

void Controls::process_events() {
	auto event = get_event();
	if (event == Event::None)
		return;

	const uint16_t adc = read_adc();
	switch (event) {
		case Event::VolumeUp:
			if (adc < AdcMax) {
				pulse_up();
				printf_("UP (adc=%u)\n", adc);
			} else {
				printf_("UP ignored: at max (adc=%u)\n", adc);
			}
			break;

		case Event::VolumeDown:
			if (adc > AdcMin) {
				pulse_down();
				printf_("DOWN (adc=%u)\n", adc);
			} else {
				printf_("DOWN ignored: at min (adc=%u)\n", adc);
			}
			break;

		case Event::Mute: {
			int pulses = 0;
			if (raw_adc() > 0) {
				if (raw_adc() <= AdcMute) {
					pulse_down();
				} else {
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

					HAL_Delay(100);
					while (raw_adc() > AdcMin) {
						pulse_down(1000);
						delay_us(10000);
					}
				}
			}
			printf_("Mute (adc %u=>%u) pulsed %d\n", adc, raw_adc(), pulses);
		} break;

		case Event::None:
			break;
	}
}

} // namespace RemoteVolume

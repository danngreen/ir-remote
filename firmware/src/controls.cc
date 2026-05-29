#include "controls.hh"

namespace RemoteVolume
{

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

} // namespace RemoteVolume

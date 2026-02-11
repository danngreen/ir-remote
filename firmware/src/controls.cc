#include "controls.hh"

namespace IRRemote
{

uint16_t Controls::raw_adc() {
	return adc_buffer[0];
}

uint16_t Controls::read_adc() {
	return smooth_adc.val();
}

} // namespace IRRemote

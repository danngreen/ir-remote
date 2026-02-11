#include "audio_stream.hh"
#include "controls.hh"
#include "debug.hh"
#include "printf.h"
#include "system.hh"
#include "util/zip.hh"
#include <cmath>

// #include "test_audio.hh"

namespace
{
// Initialize the system before main()
IRRemote::System _init;
} // namespace

int main() {
	printf_("\r\n\r\nSlider Magic\n");
	printf_("Starting up...\n");

	IRRemote::Controls controls;
	controls.start();

	printf_("ADC started\n");
	printf_("Raw value: %u / 8191\n", controls.raw_adc());

	IRRemote::AudioStream audio(
		[&controls](const AudioStreamConf::AudioInBlock &inblock, AudioStreamConf::AudioOutBlock &outblock) {
			float vol = controls.read_adc() / 8191.f;

			for (auto [in, out] : zip(inblock, outblock)) {
				auto in_L = in.scale_input_chan(0);
				out.set_scaled_output(0, in_L * vol);

				auto in_R = in.scale_input_chan(1);
				out.set_scaled_output(1, in_R * vol);
			}
		});

	printf_("Audio stream initialized\n");

	audio.start();

	printf_("Audio stream started\n");

	while (true) {
		__NOP();
	}
}

void motor_control(IRRemote::Controls &controls) {
	mdrivlib::Pin mot1{mdrivlib::GPIO::A, mdrivlib::PinNum::_8, mdrivlib::PinMode::Output};
	mdrivlib::Pin mot2{mdrivlib::GPIO::A, mdrivlib::PinNum::_9, mdrivlib::PinMode::Output};

	while (false) {
		volatile int x = 600;
		while (x--)
			;

		// while (controls.read_adc() < 0x1000 || controls.read_adc() > 0x1008) {

		auto adc = controls.read_adc();

		if (adc > 0x1040) {
			mot1.high();
			volatile int x = 2000;
			while (x--)
				;
			mot1.low();
		}

		else if (adc < 0x0FC0)
		{
			mot2.high();
			volatile int x = 2000;
			while (x--)
				;
			mot2.low();
		}
	}
}

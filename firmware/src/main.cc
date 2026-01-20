#include "audio_stream.hh"
#include "controls.hh"
#include "debug.hh"
#include "printf.h"
#include "system.hh"

// #include "test_audio.hh"

namespace
{
// Initialize the system before main()
IRRemote::System _init;
} // namespace

int main() {

	IRRemote::Controls controls;
	// controls.start();

	IRRemote::AudioStream audio([](const AudioStreamConf::AudioInBlock &in, AudioStreamConf::AudioOutBlock &out) {
		Debug::Pin0::high();
		Debug::Pin0::low();
		// params.update();
		// looping_delay.update(in, out);
	});

	audio.start();

	mdrivlib::Pin mot1{mdrivlib::GPIO::A, mdrivlib::PinNum::_8, mdrivlib::PinMode::Output};
	mdrivlib::Pin mot2{mdrivlib::GPIO::A, mdrivlib::PinNum::_9, mdrivlib::PinMode::Output};

	while (true) {
		__NOP();
	}

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

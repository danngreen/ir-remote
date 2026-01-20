#include "conf/board_conf.hh"
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
	// using AudioInBlock = AudioStreamConf::AudioInBlock;
	// using AudioOutBlock = AudioStreamConf::AudioOutBlock;
	mdrivlib::RCC_Enable::GPIOA_::set();

	IRRemote::Controls controls;
	controls.start();

	// printf_("Hello\n");

	// if (Board::PingButton::PinT::read() && Board::RevButton::PinT::read()) {
	// 	HAL_Delay(50);
	// 	if (Board::PingButton::PinT::read() && Board::RevButton::PinT::read())
	// 		HWTests::run(controls);
	// }

	// Params params{controls, flags};
	// LoopingDelay looping_delay{params, flags};
	// AudioStream audio([&looping_delay, &params](const AudioInBlock &in, AudioOutBlock &out) {
	// 	params.update();
	// 	looping_delay.update(in, out);
	// });

	// params.start();
	// controls.start();
	// audio.start();

	mdrivlib::Pin mot1{mdrivlib::GPIO::A, mdrivlib::PinNum::_8, mdrivlib::PinMode::Output};
	mdrivlib::Pin mot2{mdrivlib::GPIO::A, mdrivlib::PinNum::_9, mdrivlib::PinMode::Output};

	while (true) {
		__NOP();

		mot1.high();
		HAL_Delay(30);
		mot1.low();

		HAL_Delay(300);

		mot2.high();
		HAL_Delay(30);
		mot2.low();

		HAL_Delay(300);

		// HAL_Delay(1000);
	}
}

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
	using AudioInBlock = AudioStreamConf::AudioInBlock;
	using AudioOutBlock = AudioStreamConf::AudioOutBlock;

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

	while (true) {
		__NOP();
	}
}

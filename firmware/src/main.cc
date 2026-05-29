#include "audio_stream.hh"
#include "controls.hh"
#include "debug.hh"
#include "printf.h"
#include "system.hh"
#include "util/zip.hh"
#include <cmath>

namespace
{
// Initialize the system before main()
RemoteVolume::System _init;
} // namespace

int main() {
	printf_("\r\n\r\nSlider Magic\n");
	printf_("Starting up...\n");

	RemoteVolume::Controls controls;
	controls.start();

	printf_("ADC started\n");
	printf_("Raw value: %u / 8191\n", controls.raw_adc());

	RemoteVolume::AudioStream audio(
		[&controls](const AudioStreamConf::AudioInBlock &inblock, AudioStreamConf::AudioOutBlock &outblock) {
			float vol = controls.scale_adc();
			vol = vol * vol;

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
		controls.process_events();
	}
}

#pragma once
#include "audio_stream_conf.hh"
#include "util/zip.hh"

// Testing
#include "util/oscs.hh"

namespace IRREmote
{

class TestAudio {
	float &vol;
	TriangleOscillator<48000> osc1{1000};
	TriangleOscillator<48000> osc2{15000};

public:
	TestAudio(float &vol)
		: vol{vol} {
	}

	void update(const AudioStreamConf::AudioInBlock &in, AudioStreamConf::AudioOutBlock &out) {
		osc1.set_frequency(vol * 100.f + 100.f);
		osc2.set_frequency(vol * 1000.f + 100.f);
		for (auto [o, i] : zip(out, in)) {
			o.chan[0] = (osc1.process_float() - 0.5f) * 0x00FFFFFFUL;
			o.chan[1] = (osc2.process_float() - 0.5f) * 0x00FFFFFFUL;
		}
	}
	// out.chan[0] = sinf(2.f * (float)M_PI * x) * 0x7F0000;
	// x += 1.f / 480.f;
	// if (x > 1.f)
	// 	x -= 1.f;
};
} // namespace IRREmote

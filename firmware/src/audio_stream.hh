#pragma once

#include "audio_stream_conf.hh"
#include "board_conf.hh"
#include "drivers/codec_TLV320AIC3204.hh"
#include "drivers/i2c.hh"
#include "util/callable.hh"

namespace IRRemote
{

struct AudioStream {
	// TODO: Verify this region is non-cacheable on F4
	alignas(256) static inline
		__attribute__((section(".noncachable"))) AudioStreamConf::AudioInBuffer audio_in_dma_buffer;
	alignas(256) static inline
		__attribute__((section(".noncachable"))) AudioStreamConf::AudioOutBuffer audio_out_dma_buffer;

public:
	using AudioProcessFunction =
		Function<void(const AudioStreamConf::AudioInBlock &, AudioStreamConf::AudioOutBlock &)>;

	AudioStream(AudioProcessFunction &&process_func)
		: codec_i2c{Board::codec_i2c_conf}
		, codec{codec_i2c, Board::sai_conf}
		, _process_func{std::move(process_func)} {

		auto init_ok = codec.init();

		if (init_ok != mdrivlib::CodecTLV320AIC3204::Error::CODEC_NO_ERR)
			__BKPT(); // TODO: display error, flash lights

		codec.set_rx_buffer_start(audio_in_dma_buffer);
		codec.set_tx_buffer_start(audio_out_dma_buffer);
		codec.set_callbacks([this] { _process<1>(); }, [this] { _process<0>(); });
	}

	void start() {
		codec.start();
	}
	void stop() {
		codec.stop();
	}

	void set_callback(AudioProcessFunction &&process_func) {
		stop();
		_process_func = std::move(process_func);
		start();
	}

private:
	mdrivlib::I2CPeriph codec_i2c;
	mdrivlib::CodecTLV320AIC3204 codec;

	AudioProcessFunction _process_func;

	template<uint32_t buffer_half>
	void _process() {
		// Debug::Pin0::high();
		_process_func(audio_in_dma_buffer[1 - buffer_half], audio_out_dma_buffer[buffer_half]);
		// Debug::Pin0::low();
	}
};

} // namespace IRRemote

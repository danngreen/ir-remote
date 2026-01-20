#pragma once
#include "audio_stream_conf.hh"
#include "drivers/adc_builtin_conf.hh"
#include "drivers/i2c_config_struct.hh"
#include "drivers/sai_config_struct.hh"

namespace IRRemote::Board
{

using mdrivlib::AdcChannelConf;
using mdrivlib::AdcChanNum;
using mdrivlib::GPIO;
using mdrivlib::I2CConfig;
using mdrivlib::PinAF;
using mdrivlib::PinDef;
using mdrivlib::PinNum;
using mdrivlib::SaiConfig;
using enum mdrivlib::PinPolarity;
using enum mdrivlib::PinMode;

const I2CConfig codec_i2c_conf = {
	.I2Cx = I2C1,
	.SCL = {GPIO::A, PinNum::_15, PinAF::AltFunc4},
	.SDA = {GPIO::B, PinNum::_7, PinAF::AltFunc4},
	.timing =
		{
			.SCLL = 0x68,
			.SCLH = 0x47,
			.SDADEL = 0x4,
			.SCLDEL = 0,
			.PRESC = 0x2,
		},
	.priority1 = 0,
	.priority2 = 1,
};

const SaiConfig sai_conf = {
	.sai = SAI1,
	.tx_block = SAI1_Block_A,
	.rx_block = SAI1_Block_B,
	.mode = SaiConfig::RXMaster,
	.dma_init_tx =
		{
			.DMAx = DMA1,
			.request = DMA_REQUEST_SAI1_A,
			.channel = 1,
			.IRQn = DMA1_Channel1_IRQn,
			.pri = 2,
			.subpri = 2,
		},
	.dma_init_rx =
		{
			.DMAx = DMA1,
			.request = DMA_REQUEST_SAI1_B,
			.channel = 2,
			.IRQn = DMA1_Channel2_IRQn,
			.pri = 2,
			.subpri = 2,
		},

	.datasize = AudioStreamConf::SampleBits == 24 ? SAI_DATASIZE_24 :
				AudioStreamConf::SampleBits == 16 ? SAI_DATASIZE_16 :
													SAI_DATASIZE_32,
	.framesize = 32,
	.samplerate = AudioStreamConf::SampleRate,

	.MCLK = {GPIO::B, PinNum::_4, PinAF::AltFunc14},
	.SCLK = {GPIO::B, PinNum::_3, PinAF::AltFunc14},
	.LRCLK = {GPIO::B, PinNum::_6, PinAF::AltFunc14},
	.SD_DAC = {GPIO::A, PinNum::_10, PinAF::AltFunc14}, // SD-A
	.SD_ADC = {GPIO::B, PinNum::_5, PinAF::AltFunc12},	// SD-B
	.reset_pin = {GPIO::B, PinNum::_8},

	.bus_address = 0,

	.num_tdm_ins = AudioStreamConf::NumInChans,
	.num_tdm_outs = AudioStreamConf::NumOutChans,

	.sync_send = SaiConfig::NoSendSync,
	.sync_receive_from = SaiConfig::NoReceiveSync,
};

// MOTOR:
// - PA8 (or TIM1_CH1: AF6)
// + PA9 (or TIM1_CH2: AF6 or TIM2_CH3 AF10)

// Console:
// TX: PA2, USART2, AF7
// RX: PA3, USART2, AF7
constexpr inline uint32_t ConsoleUartBaseAddr = USART2_BASE;
constexpr inline PinDef ConsoleUartTX{GPIO::A, PinNum::_2, mdrivlib::PinAF::AltFunc7};
constexpr inline PinDef ConsoleUartRX{GPIO::A, PinNum::_3, mdrivlib::PinAF::AltFunc7};

// Debug2: PA4
// Debug3: PA5
constexpr inline PinDef Debug1{GPIO::A, PinNum::_4};
constexpr inline PinDef Debug2{GPIO::A, PinNum::_5};

// IR Demod:
// PA0 (TIM2_CH1, AF1)

// ADC2 IN12
constexpr std::array<AdcChannelConf, 1> AdcChans = {
	{{{GPIO::B, PinNum::_2}, AdcChanNum::_12, 0, mdrivlib::AdcSamplingTime::_24Cycles}},
};

struct AdcConf : mdrivlib::DefaultAdcPeriphConf {
	static constexpr auto resolution = mdrivlib::AdcResolution::Bits12;
	static constexpr auto adc_periph_num = mdrivlib::AdcPeriphNum::_2;
	static constexpr auto oversample = false;
	static constexpr auto clock_div = mdrivlib::AdcClockSourceDiv::APBClk_Div4;

	static constexpr bool enable_end_of_sequence_isr = false;
	static constexpr bool enable_end_of_conversion_isr = false;

	struct DmaConf : mdrivlib::DefaultAdcPeriphConf::DmaConf {
		static constexpr auto DMAx = 1;
		static constexpr auto StreamNum = 3; // channel
		static constexpr auto RequestNum = DMA_REQUEST_ADC2;
		static constexpr auto dma_priority = Low;
		static constexpr IRQn_Type IRQn = DMA1_Channel3_IRQn;
		static constexpr uint32_t pri = 0;
		static constexpr uint32_t subpri = 0;
	};

	static constexpr uint16_t min_value = 25;
};

} // namespace IRRemote::Board

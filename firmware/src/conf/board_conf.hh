#pragma once
#include "drivers/adc_builtin_conf.hh"

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

// Codec: SAI1
// SCK(B): PB3, AF14
// MCLK(B): PB4, AF14
// SD-ADC(B): PB5, AF12
// FS(B): PB6, AF14
// SD-DAC(A): PA10, AF14
// Reset: PB8
// I2C1_SCL: PA15, AF4
// I2C1_SDA: PB7, AF4

const I2CConfig codec_i2c_conf = {
	.I2Cx = I2C1,
	.SCL = {GPIO::A, PinNum::_15, PinAF::AltFunc4},
	.SDA = {GPIO::B, PinNum::_7, PinAF::AltFunc4},
	.timing =
		{
			.PRESC = 0x20,
			.SCLDEL_SDADEL = 0x40,
			.SCLH = 0x47,
			.SCLL = 0x68,
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
			.DMAx = DMA2,
			.stream = DMA2_Stream1,
			.channel = DMA_CHANNEL_0,
			.IRQn = DMA2_Stream1_IRQn,
			.pri = 2,
			.subpri = 2,
		},
	.dma_init_rx =
		{
			.DMAx = DMA2,
			.stream = DMA2_Stream4,
			.channel = DMA_CHANNEL_1,
			.IRQn = DMA2_Stream4_IRQn,
			.pri = 2,
			.subpri = 2,
		},

	.datasize = AudioStreamConf::SampleBits == 24 ? SAI_DATASIZE_24 :
				AudioStreamConf::SampleBits == 16 ? SAI_DATASIZE_16 :
													SAI_DATASIZE_32,
	.framesize = 32,
	.samplerate = AudioStreamConf::SampleRate,

	.MCLK = {GPIO::E, PinNum::_2, PinAF::AltFunc6},
	.SCLK = {GPIO::E, PinNum::_5, PinAF::AltFunc6},
	.LRCLK = {GPIO::E, PinNum::_4, PinAF::AltFunc6},
	.SD_DAC = {GPIO::E, PinNum::_6, PinAF::AltFunc6},
	.SD_ADC = {GPIO::E, PinNum::_3, PinAF::AltFunc6},
	.reset_pin = {GPIO::I, PinNum::_9},

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
constexpr inline PinDef ConsoleUartTX{GPIO::A, PinNum::_2, mdrivlib::AltFunc::AltFunc7};
constexpr inline PinDef ConsoleUartRX{GPIO::A, PinNum::_3, mdrivlib::AltFunc::AltFunc7};

// Debug2: PA4
// Debug3: PA5
constexpr inline PinDef Debug1{GPIO::A, PinNum::_4};
constexpr inline PinDef Debug2{GPIO::A, PinNum::_5};

// IR Demod:
// PA0 (TIM2_CH1, AF1)

// using ClkOut = mdrivlib::PinF<BrainPin::D0, Input, Normal>;

// ADC2 IN12
constexpr std::array<AdcChannelConf, NumPots> AdcChans = {
	{{GPIO::B, PinNum::_2, 12, mdrivlib::AdcSamplingTime::_28Cycles}},
};

struct AdcConf : mdrivlib::DefaultAdcPeriphConf {
	static constexpr auto resolution = mdrivlib::AdcResolution::Bits12;
	static constexpr auto adc_periph_num = mdrivlib::AdcPeriphNum::_2;
	static constexpr auto oversample = false;
	static constexpr auto clock_div = mdrivlib::AdcClockSourceDiv::APBClk_Div4;

	static constexpr bool enable_end_of_sequence_isr = true;
	static constexpr bool enable_end_of_conversion_isr = false;

	struct DmaConf : mdrivlib::DefaultAdcPeriphConf::DmaConf {
		static constexpr auto DMAx = 1;
		static constexpr auto StreamNum = 3;
		static constexpr auto RequestNum = DMA_CHANNEL_3;
		static constexpr auto dma_priority = Low;
		static constexpr IRQn_Type IRQn = DMA2_Stream3_IRQn;
		static constexpr uint32_t pri = 0;
		static constexpr uint32_t subpri = 0;
	};

	static constexpr uint16_t min_value = 25;
};

} // namespace IRRemote::Board

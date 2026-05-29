#pragma once
#include "conf/board_conf.hh"
#include "drivers/uart.hh"
#include <optional>

constexpr inline UartConf ConsoleUartConf{
	.base_addr = RemoteVolume::Board::ConsoleUartBaseAddr,
	.TXPin = RemoteVolume::Board::ConsoleUartTX,
	.RXPin = RemoteVolume::Board::ConsoleUartRX,
	.mode = UartConf::Mode::TXRX,
	.baud = 115200,
	.wordlen = 8,
	.parity = UartConf::Parity::None,
	.stopbits = UartConf::StopBits::_1,
};

struct Console {
	static inline mdrivlib::Uart<ConsoleUartConf> console;
	static void putchar(char c);
	Console() {
		console.init();
	}
	static void init() {
		console.init();
	}

	// Non-blocking: returns the next received char, or nullopt if none waiting.
	static std::optional<char> get_char() {
		uint8_t c;
		if (console.receive(&c))
			return char(c);
		return std::nullopt;
	}
};

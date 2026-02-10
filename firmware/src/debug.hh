#pragma once
#include "conf/board_conf.hh"

namespace Debug
{
struct Disabled {
	static void high() {
	}
	static void low() {
	}
};

// using Pin0 = Disabled;
using Pin0 = mdrivlib::PinF<IRRemote::Board::Debug1, mdrivlib::PinMode::Output>;
using Pin1 = mdrivlib::PinF<IRRemote::Board::Debug2, mdrivlib::PinMode::Output>;
using Pin2 = Disabled;
using Pin3 = Disabled;

}; // namespace Debug

#define GCC_OPTIMIZE_OFF __attribute__((optimize("-O0")))

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
using Pin0 = mdrivlib::FPin<IRRemote::Board::Debug1.gpio, IRRemote::Board::Debug1.pin>;
using Pin1 = mdrivlib::FPin<IRRemote::Board::Debug2.gpio, IRRemote::Board::Debug2.pin>;
using Pin2 = Disabled;
using Pin3 = Disabled;

}; // namespace Debug

#define GCC_OPTIMIZE_OFF __attribute__((optimize("-O0")))

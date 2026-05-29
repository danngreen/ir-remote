#pragma once
#include "event.hh"

namespace RemoteVolume
{

struct IRCapture {
	// TODO: decode IR frames here and translate them into Events.
	// Returns Event::None until a full command has been received.
	Event get_event() {
		return Event::None;
	}
};

} // namespace RemoteVolume

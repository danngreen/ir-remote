#pragma once
#include "console.hh"
#include "event.hh"

namespace RemoteVolume
{

// Turns console keypresses into Events: 'u' => VolumeUp, 'd' => VolumeDown.
// Lets us exercise the volume/motor logic over UART without an IR remote.
struct ConsoleEventSource {
	Event get_event() {
		if (auto c = Console::get_char()) {
			switch (*c) {
				case 'u':
					return Event::VolumeUp;
				case 'd':
					return Event::VolumeDown;
			}
		}
		return Event::None;
	}
};

} // namespace RemoteVolume

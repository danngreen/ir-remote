#pragma once

namespace RemoteVolume
{

// A decoded high-level command. Produced by any event source (IR receiver,
// console, ...) and consumed by Controls. Keeps input *decoding* separate from
// what the events *mean*.
enum class Event {
	None,
	VolumeUp,
	VolumeDown,
	Mute,
};

// An event plus whether it came from a held-button repeat (IR) rather than a
// fresh press. Console events are always presses (repeat == false).
struct EventMsg {
	Event event = Event::None;
	bool repeat = false;
};

} // namespace RemoteVolume

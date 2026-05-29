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
};

} // namespace RemoteVolume

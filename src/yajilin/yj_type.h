#pragma once

namespace penciloid
{
namespace yajilin
{
enum ClueDirection
{
	kNoClue,
	kClueNorth, // The direction in which y coordinate is decreasing
	kClueWest, // The direction in which x coordinate is decreasing
	kClueEast, // The direction in which x coordinate is increasing
	kClueSouth // The direction in which y coordinate is increasing
};
struct Clue
{
	Clue(ClueDirection direction = kNoClue, int clue_value = 0) : direction(direction), clue_value(clue_value) {}

	ClueDirection direction; // If direction == kNoClue, clue_value should be ignored.
	int clue_value;
};
}
}

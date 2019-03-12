#include <array>
#include <type_traits>
#include <variant>

#include "BitUtilities.h"
#include "Instructions.h"
#include "Chip8.h"

int main() 
{
	Chip8 machine("test.ch8");

	// Main loop
	while (machine.Running)
	{
		// Cycle
		machine.Tick();

		// Draw

		// Set input
	}

	return 0;
}
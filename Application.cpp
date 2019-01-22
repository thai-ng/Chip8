#include <array>
#include <type_traits>
#include <variant>

#include "PrimitiveTypes.h"
#include "BitUtilities.h"
#include "Instructions.h"

template <typename T>
constexpr auto Val(T v)
{
	return static_cast<std::underlying_type_t<T>>(v);
}

using RAM = std::array<byte, 4069>;

RAM MainMemory;

enum class R : int
{
	V0, V1, V2, V3,
	V4, V5, V6, V7, 
	V8, V9, VA, VB,
	VC, VD, VE, VF
};
std::array<byte, 16> Registers;

word I;
byte Delay;
byte Sound;

word PC;
byte SP;

constexpr int Cols = 64;
constexpr int Rows = 32;

std::array<byte, Cols * Rows> ScreenBuffer;

// SPEC
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

/// Fetch OpCode at location from memory
/// Opcode of CHIP8 is 2 bytes long. 
/// Read each byte from memory and shift then OR to form 1 2-byte long opcode
word Fetch(RAM const& memory, word loc)
{
	word firstByte = memory[loc];
	auto secondByte = memory[loc + 1];

	return firstByte << 8 | secondByte;
}

using Instruction = 
	std::variant<NoParamInstruction, SinglePosInstruction, SinglePosDataInstruction,
				 TwoPosInstruction, ThreeParamInstruction, AddressInstruction>;

Instruction Decode(word opcode)
{
	// Read first nibble 
	if (NoParamInstruction::IsInstruction(opcode))
	{
		return NoParamInstruction(opcode);
	}
	else if (AddressInstruction::IsInstruction(opcode)) 
	{
		return AddressInstruction(opcode);
	} 
	else if (SinglePosInstruction::IsInstruction(opcode))
	{
		return SinglePosInstruction(opcode);
	}
	else if (SinglePosDataInstruction::IsInstruction(opcode))
	{
		return SinglePosDataInstruction(opcode);
	}
	else if (TwoPosInstruction::IsInstruction(opcode))
	{
		return TwoPosInstruction(opcode);
	}
	else if (ThreeParamInstruction::IsInstruction(opcode))
	{
		return ThreeParamInstruction(opcode);
	}

	return NoParamInstruction(opcode);
}

/// Execute one cycle
void Tick()
{
	auto opcode = Fetch(MainMemory, PC);

	// Decode
	auto instruction = Decode(opcode);
	// Execute

	// Timer
}

int main() 
{
	// Setup
	

	// Main loop
	bool Running = true;
	while (Running)
	{
		// Cycle

		// Draw

		// Set input
	}

	return 0;
}
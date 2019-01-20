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


enum class SinglePosDataInstructionType
{
	// x - 4-bit value, low 4-bit of high byte of opcode
	// kk - 8-bit (1-byte) value, low byte of opcode

	SE,  // 3xkk SE   Vx   kk		Skip next if Vx = kk. if (Vx == kk) PC += 2.  
	SNE, // 4xkk SNE  Vx   kk		Skip next if Vx != kk. if (Vx != kk) PC += 2.

	LD,  // 6xkk LD   Vx   kk		Set Vx to kk. Vx = kk.
	ADD, // 7xkk ADD  Vx   kk		Add kk to Vx. Vx += kk.
	
	RND, // Cxkk RND  Vx   kk		Generate random number [0, 255], AND with kk. Vx = rand(0, 256) | kk.
};

enum class TwoPosInstructionType
{
	// n - 4 bits
	// x - 4-bit value, low 4-bit of high byte of opcode
	// y - 4-bit value, high 4-bit of low byte of opcode
	SEV, // 5xy0 SE   Vx   Vy		Skip next if Vx = Vy. if (Vx == Vy) PC += 2.

	LDV, // 8xy0 LD   Vx   Vy		Store value of Vy in Vx. Vx = Vy.
	OR,  // 8xy1 OR   Vx   Vy		Bitwise OR of Vx and Vy. Vx = (Vx | Vy)
	AND, // 8xy2 AND  Vx   Vy		Bitwise AND of Vx and Vy. Vx = (Vx & Vy) 
	XOR, // 8xy3 XOR  Vx   Vy		Bitwise XOR of Vx and Vy. Vx = (Vx ^ Vy)
	ADDV,// 8xy4 ADD  Vx   Vy		Add Vx to Vy. Set VF if carry. 
		 //							r = (Vx + Vy). VF = (r > max8bit) ? 1 : 0. Vx = trunc8bit(r)
	SUBV,// 8xy5 SUB  Vx   Vy		Subtract Vy from Vx. Store in Vx. VF = NOT borrow
		 //							VF = (Vx > Vy) ? 1 : 0. Vx = Vx - Vy
	SUBN,// 8xy7 SUB  Vy   Vx		Subtract Vx from Vy. Store in Vx. VF = NOT borrow
		 //							VF = (Vy > Vx) ? 1 : 0. Vx = Vy - Vx
	SNEV,// 9xy0 SNE  Vx   Vy		Skip next if Vx = Vy. if (Vx == Vy) PC += 2;
};

enum class ThreeParamInstructionType
{
	DRW, // Dxyn DRW  Vx   Vy   n   Display n b-byte sprite starting at location I at (Vx, Vy). Set VF = collision
		 //							Sprite data is XOR-ed with screen buffer. If erasing any pixel, VF = 1.
		 //							Screen buffer wraps around.
};

enum class NoParamInstructionType
{
	CLS, // 00E0 CLS				Clear display
	RET, // 00EE RET				Return. PC = *SP. --SP.
};

struct SinglePosDataInstruction
{
	SinglePosDataInstructionType Type;
	byte Pos;
	byte data;
};

struct TwoPosInstruction
{
	TwoPosInstructionType Type;
	byte First;
	byte Second;
};

struct ThreeParamInstruction
{
	ThreeParamInstructionType Type;
	byte First;
	byte Second;
	byte Third;
};

struct NoParamInstruction
{
	NoParamInstructionType Type;
};

using Instruction = 
	std::variant<NoParamInstruction, SinglePosInstruction, SinglePosDataInstruction,
				 TwoPosInstruction, ThreeParamInstruction, AddressInstruction>;

Instruction Decode(word opcode)
{
	// Read first nibble 
}

/// Execute one cycle
void Tick()
{
	auto opCode = Fetch(MainMemory, PC);

	// Decode
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
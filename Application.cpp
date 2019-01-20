#include <array>
#include <type_traits>
#include <variant>

template <typename T>
constexpr auto Val(T v)
{
	return static_cast<std::underlying_type_t<T>>(v);
}

using byte = unsigned char;
using word = unsigned short;

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

enum class AddressInstructionType
{
	// nnn - 12 bits
	SYS, // 0nnn SYS  nnn			Jump to program at nnn. Not used.  
	
	JP,  // 1nnn JP   nnn			Jump. PC = nnn
	CALL,// 2nnn CALL nnn			Call. ++SP = PC. PC = nnn.

	LD,  // Annn LD   I    nnn		Load 12-bit nnn into I. I = nnn.
	JPV, // Bnnn JP   V0   nnn		Jump to V0 + nnn. PC = V0 + nnn.
};

enum class SinglePosInstructionType
{
	// x - 4-bit value, low 4-bit of high byte of opcode

	SHR, // 8x_6 SHR  Vx   1		Shift Vx right by 1 bit. VF = LSB(Vx). Vx >>= 1;
	SHL, // 8x_E SHL  Vx   1		Shift Vx left by 1. VF = MSB(Vx). Vx <<= 1;

	SKP, // Ex9E SKP  Vx			Skip next if key in Vx is pressed. if (key == Vx) PC += 2.
	SKNP,// ExA1 SKPN Vx			Skip next if key in Vx is not pressed. if (key != Vx) PC += 2.
	LDVD,// Fx07 LD	  Vx  DT		Load value in DT into Vx. Vx = DT.
	LFVK,// Fx0A LD	  Vx  K			Block till key press. Then store key in Vx. Vx = key.
	LDDV,// Fx15 LD   DT  Vx		Load value in Vx into DT. DT = Vx.
	LDSV,// Fx18 LD	  ST  Vx		Load value in Vx into ST. ST = Vx.
	ADDI,// Fx1E ADD  I   Vx		Add I and Vx. Store in I. I = I + Vx.
	LDFV,// Fx29 LD	  F   Vx		Set I to location of hex sprite in Vx. I = Hex[Vx].data.

	LDBV,// Fx33 LD	  B   Vx		Store BCD representation of Vx in location I, I+1, I+2.
		 //							*I = ((Vx / 100) % 10). *(I + 1) = ((Vx / 10) % 10). *(I+2) = (Vx % 10).

	LDIV,// Fx55 LD	  *I  Vx		Store V0 to Vx starting at location I.
		 //							for (i in [0:x]) *I++ = V[i++]

	LDVI,// Fx65 LD   Vx  *I		Read register V0 to Vx starting from I.
		 //							for (i in [0:x]) V[i++] = *I++
};

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

byte GetHighNibble(byte b)
{
	return b >> 4;
}

constexpr word MaskBottom12Bit = 0b0000111111111111;
word Get12BitParam(word opcode)
{
	return opcode & MaskBottom12Bit;
}

constexpr word MaskFirstParam = 0b0000111100000000;
// Get x from _x__ opcode.
byte GetFirstPosParam(word opcode)
{
	return (opcode & MaskFirstParam) >> 8;
}

constexpr word MaskSecondParam = 0b0000000011110000;
// Get y from __y_ opcode
byte GetSecondPosParam(word opcode)
{
	return (opcode & MaskSecondParam) >> 4;
}

// Get nn from ___nn opcode
byte GetBottomByte(word opcode)
{
	return static_cast<byte>(opcode);
}

constexpr byte MaskBottomNibble = 0b00001111;
// Get n from ____n opcode
byte GetBottomNibble(word opcode)
{
	return (GetBottomByte(opcode) & MaskBottomNibble);
}

struct AddressInstruction
{
	AddressInstructionType Type;
	byte Param;
};

struct SinglePosInstruction
{
	SinglePosDataInstructionType Type;
	byte Pos;
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

using Instruction = std::variant<NoParamInstruction, SinglePosInstruction, SinglePosDataInstruction,
								 TwoPosInstruction, ThreeParamInstruction, AddressInstruction>;

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
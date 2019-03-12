#pragma once
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "PrimitiveTypes.h"
// SPEC
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

using RAM = std::array<byte, 4096>;

enum class R : int
{
	V0, V1, V2, V3,
	V4, V5, V6, V7,
	V8, V9, VA, VB,
	VC, VD, VE, VF
};

using RegistersArray = std::array<byte, 16>;

constexpr int Cols = 64;
constexpr int Rows = 32;

template <typename T>
constexpr auto Val(T v)
{
	return static_cast<std::underlying_type_t<T>>(v);
}

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

template <typename... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

constexpr int ProgramStart = 0x200;

constexpr byte TestLSB(byte value)
{
	return value & 1;
}

template <typename T> 
constexpr T GetMSB(T value)
{
	return value >> (sizeof(T) * 8);
}

constexpr byte TestMSB(byte value)
{
	return GetMSB(value) & 1;
}

class Chip8
{
public:
	Chip8(std::string const& filename)
	{
		std::ifstream file(filename, std::fstream::binary);

		if (file)
		{
			file.read(reinterpret_cast<char*>(MainMemory.data() + ProgramStart), MainMemory.size() - ProgramStart);
			PC = ProgramStart;
		}
		else
		{
			throw std::runtime_error("Cannot read file");
		}


	}

	/// Execute one cycle
	void Tick()
	{
		auto opcode = Fetch(MainMemory, PC);

		// std::cout << "PC: 0x" <<  std::hex << std::uppercase << PC << " Opcode: 0x" << opcode << "\n";
		// Decode
		auto instruction = Decode(opcode);
		// Execute
		Execute(instruction);
		// Timer

		if (PC >= MainMemory.size() - 1)
		{
			Running = false;
		}
	}

	RegistersArray Registers;
	bool Running = true;
	word I;
	byte Delay;
	byte Sound;

	word PC;

	std::vector<word> Stack;

	RAM MainMemory;

	std::array<byte, Cols* Rows> ScreenBuffer;

	void Execute(Instruction const& instruction)
	{
		std::visit(overloaded
				   {
						[this](AddressInstruction & i)
						{
							switch (i.Type)
							{
								case AddressInstructionType::JP:
								{
									std::cout << "PC: 0x" << std::hex << std::uppercase << PC << " JP instruction\n";
									this->PC = i.Param;
								} break;

								case AddressInstructionType::CALL:
								{
									std::cout << "PC: 0x" << std::hex << std::uppercase << PC << " CALL instruction\n";
									this->Stack.push_back(this->PC);
									this->PC = i.Param;
								} break;

								case AddressInstructionType::LD:
								{
									std::cout << "PC: 0x" << std::hex << std::uppercase << PC << " LD instruction\n";
									this->I = i.Param;
									this->PC += 2;
								} break;

								case AddressInstructionType::JPV:
								{
									std::cout << "PC: 0x" << std::hex << std::uppercase << PC << " JPV instruction\n";

									this->PC = Registers[Val(R::V0)] + i.Param;
								} break;

								default:
									break;
							}
						},
						[this](SinglePosInstruction & i)
						{
							switch (i.Type)
							{
								case (SinglePosInstructionType::SHR):
								{
									std::cout << "PC: 0x" <<  std::hex << std::uppercase << PC << " SHR Instruction \n";
									auto regValue = this->Registers[i.Pos];
									this->Registers[Val(R::VF)] = TestLSB(regValue);
									this->Registers[i.Pos] >>= 1;
									this->PC += 2;
								} break;

								case (SinglePosInstructionType::SHL):
								{
									std::cout << "PC: 0x" << std::hex << std::uppercase << PC << " SHL Instruction \n";
									auto regValue = this->Registers[i.Pos];
									this->Registers[Val(R::VF)] = TestMSB(regValue);
									this->Registers[i.Pos] <<= 1;
									this->PC += 2;
								} break;

								case (SinglePosInstructionType::LDVD):
								{
									std::cout << "PC: 0x" << std::hex << std::uppercase << PC << " LDVD Instruction \n";
									this->Registers[i.Pos] = this->Delay;
									this->PC += 2;
								} break;

								case (SinglePosInstructionType::LDDV):
								{
									std::cout << "PC: 0x" << std::hex << std::uppercase << PC << " LDDV Instruction \n";
									this->Delay = this->Registers[i.Pos];
									this->PC += 2;
								} break;

								default:
									break;
							}
						},
						[this](auto&)
						{
							//std::cout << "Unimplemented instruction\n";
							this->PC += 2;
						}
				   }, instruction);
	}
};

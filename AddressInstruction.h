#pragma once
#include "PrimitiveTypes.h"
#include "BitUtilities.h"
#include "IInstruction.h"

enum class AddressInstructionType
{
	// nnn - 12 bits
	SYS, // 0nnn SYS  nnn			Jump to program at nnn. Not used.  

	JP,  // 1nnn JP   nnn			Jump. PC = nnn
	CALL,// 2nnn CALL nnn			Call. ++SP = PC. PC = nnn.

	LD,  // Annn LD   I    nnn		Load 12-bit nnn into I. I = nnn.
	JPV, // Bnnn JP   V0   nnn		Jump to V0 + nnn. PC = V0 + nnn.
	NOP
};

struct AddressInstruction : IInstruction<AddressInstruction, AddressInstructionType>
{
	AddressInstruction(word opcode) : 
		Type(GetInstructionType(opcode)),
		Param(Get12BitParam(opcode)) { }

	AddressInstructionType Type;
	word Param;

	static AddressInstructionType 
	GetInstructionType(word opcode)
	{
		auto n = GetHighNibble(opcode);
		switch (n)
		{
			case 0:
				return AddressInstructionType::SYS;
			case 1:
				return AddressInstructionType::JP;
			case 2:
				return AddressInstructionType::CALL;
			case 0xA:
				return AddressInstructionType::LD;
			case 0xB:
				return AddressInstructionType::JPV;
			default:
				return AddressInstructionType::NOP;
		}
	}
};
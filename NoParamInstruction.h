#pragma once
#include "PrimitiveTypes.h"
#include "BitUtilities.h"
#include "IInstruction.h"

enum class NoParamInstructionType
{
	CLS, // 00E0 CLS				Clear display
	RET, // 00EE RET				Return. PC = *SP. --SP.
	NOP
};

struct NoParamInstruction : IInstruction<NoParamInstruction, NoParamInstructionType>
{
	NoParamInstruction(word opcode) :
		Type(GetInstructionType(opcode))
	{}

	NoParamInstructionType Type;

	static NoParamInstructionType
	GetInstructionType(word opcode)
	{
		if (opcode == 0x00E0)
		{
			return NoParamInstructionType::CLS;
		}
		else if (opcode == 0x00EE)
		{
			return NoParamInstructionType::RET;
		}
		else
		{
			return NoParamInstructionType::NOP;
		}
	}
};
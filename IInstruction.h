#pragma once
#include "PrimitiveTypes.h"

template <typename Instruction, typename InstructionType>
struct IInstruction
{
public:
	static bool
	IsInstruction(word opcode)
	{
		return GetInstructionType_base(opcode) != InstructionType::NOP;
	}

private:
	static InstructionType
	GetInstructionType_base(word opcode)
	{
		return Instruction::GetInstructionType(opcode);
	}
};
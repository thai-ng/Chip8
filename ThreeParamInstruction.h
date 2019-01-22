#pragma once
#include "Instructions.h"
#include "BitUtilities.h"
#include "IInstruction.h"

enum class ThreeParamInstructionType
{
	DRW, // Dxyn DRW  Vx   Vy   n   Display n b-byte sprite starting at location I at (Vx, Vy). Set VF = collision
		 //							Sprite data is XOR-ed with screen buffer. If erasing any pixel, VF = 1.
		 //							Screen buffer wraps around.
	NOP
};

struct ThreeParamInstruction : IInstruction<ThreeParamInstruction, ThreeParamInstructionType>
{
	ThreeParamInstruction(word opcode) :
		Type(GetInstructionType(opcode)),
		First(GetFirstPosParam(opcode)),
		Second(GetSecondPosParam(opcode)),
		Third(GetBottomNibble(opcode))
	{}

	ThreeParamInstructionType Type;
	byte First;
	byte Second;
	byte Third;

	static ThreeParamInstructionType
	GetInstructionType(word opcode)
	{
		auto highNibble = GetHighNibble(opcode);
		if (highNibble == 0xD)
			return ThreeParamInstructionType::DRW;
		else
			return ThreeParamInstructionType::NOP;
	}
};
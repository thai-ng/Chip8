#pragma once
#include "PrimitiveTypes.h"
#include "BitUtilities.h"
#include "IInstruction.h"

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
	NOP
};

struct TwoPosInstruction : IInstruction<TwoPosInstruction, TwoPosInstructionType>
{
	TwoPosInstructionType Type;
	byte First;
	byte Second;

	TwoPosInstruction(word opcode) :
		Type(GetInstructionType(opcode)),
		First(GetFirstPosParam(opcode)),
		Second(GetSecondPosParam(opcode))
	{}

	static TwoPosInstructionType
	GetInstructionType(word opcode) 
	{
		auto highNibble = GetHighNibble(opcode);
		switch (highNibble) 
		{
		case 0x5:
			return TwoPosInstructionType::SEV;
		case 0x8:
		{
			auto bottomNibble = GetBottomNibble(opcode);
			switch (bottomNibble) 
			{
			case 0x0:
				return TwoPosInstructionType::LDV;
			case 0x1:
				return TwoPosInstructionType::OR;
			case 0x2:
				return TwoPosInstructionType::AND;
			case 0x3:
				return TwoPosInstructionType::XOR;
			case 0x4:
				return TwoPosInstructionType::ADDV;
			case 0x5:
				return TwoPosInstructionType::SUBV;
			case 0x7:
				return TwoPosInstructionType::SUBN;
			default:
				return TwoPosInstructionType::NOP;
			}
		}
		case 0x9:
			return TwoPosInstructionType::SNEV;
		default:
			return TwoPosInstructionType::NOP;
		}
	}
};
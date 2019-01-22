#pragma once
#include "PrimitiveTypes.h"
#include "BitUtilities.h"
#include "IInstruction.h"

enum class SinglePosDataInstructionType
{
	// x - 4-bit value, low 4-bit of high byte of opcode
	// kk - 8-bit (1-byte) value, low byte of opcode

	SE,  // 3xkk SE   Vx   kk		Skip next if Vx = kk. if (Vx == kk) PC += 2.  
	SNE, // 4xkk SNE  Vx   kk		Skip next if Vx != kk. if (Vx != kk) PC += 2.

	LD,  // 6xkk LD   Vx   kk		Set Vx to kk. Vx = kk.
	ADD, // 7xkk ADD  Vx   kk		Add kk to Vx. Vx += kk.

	RND, // Cxkk RND  Vx   kk		Generate random number [0, 255], AND with kk. Vx = rand(0, 256) | kk.
	NOP  // No Op sentinel
};

struct SinglePosDataInstruction : IInstruction<SinglePosDataInstruction, SinglePosDataInstructionType>
{
	SinglePosDataInstructionType Type;
	byte Pos;
	byte Data;

	SinglePosDataInstruction(word opcode) :
		Type(GetInstructionType(opcode)),
		Pos(GetFirstPosParam(opcode)),
		Data(GetBottomByte(opcode))
	{}

	static SinglePosDataInstructionType 
	GetInstructionType(word opcode) 
	{
		auto highNibble = GetHighNibble(opcode);
		switch (highNibble)
		{
		case 0x3:
			return SinglePosDataInstructionType::SE;
		case 0x4:
			return SinglePosDataInstructionType::SNE;
		case 0x6:
			return SinglePosDataInstructionType::LD;
		case 0x7:
			return SinglePosDataInstructionType::ADD;
		case 0xC:
			return SinglePosDataInstructionType::RND;
		default:
			return SinglePosDataInstructionType::NOP;
		}
	}
};
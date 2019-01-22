#pragma once
#include "PrimitiveTypes.h"
#include "BitUtilities.h"
#include "IInstruction.h"

enum class SinglePosInstructionType
{
	// x - 4-bit value, low 4-bit of high byte of opcode

	SHR, // 8x_6 SHR  Vx   1		Shift Vx right by 1 bit. VF = LSB(Vx). Vx >>= 1;
	SHL, // 8x_E SHL  Vx   1		Shift Vx left by 1. VF = MSB(Vx). Vx <<= 1;

	SKP, // Ex9E SKP  Vx			Skip next if key in Vx is pressed. if (key == Vx) PC += 2.
	SKNP,// ExA1 SKPN Vx			Skip next if key in Vx is not pressed. if (key != Vx) PC += 2.

	LDVD,// Fx07 LD	  Vx  DT		Load value in DT into Vx. Vx = DT.
	LDVK,// Fx0A LD	  Vx  K			Block till key press. Then store key in Vx. Vx = key.
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
	NOP // No-op
};

struct SinglePosInstruction : IInstruction<SinglePosInstruction, SinglePosInstructionType>
{
	SinglePosInstruction(word opcode) :
		Type(GetInstructionType(opcode)),
		Pos(GetFirstPosParam(opcode)) {}

	SinglePosInstructionType Type;
	byte Pos;

	static SinglePosInstructionType 
	GetInstructionType(word opcode)
	{
		auto t = GetHighNibble(opcode);
		switch (t)
		{
			case 8:
			{
				auto b = GetBottomNibble(opcode);
				if (b == 6)
					return SinglePosInstructionType::SHR;
				else if (b == 0xE)
					return SinglePosInstructionType::SHL;
				else
					return SinglePosInstructionType::NOP;
			}

			case 0xE:
			{
				auto bottomByte = GetBottomByte(opcode);
				if (bottomByte == 0x9E)
					return SinglePosInstructionType::SKP;
				else if (bottomByte == 0xA1)
					return SinglePosInstructionType::SKNP;
				else
					return SinglePosInstructionType::NOP;
			}

			case 0xF:
			{
				auto bottomByte = GetBottomByte(opcode);
				switch (bottomByte)
				{
					case 0x07:
						return SinglePosInstructionType::LDVD;
					case 0x0A:
						return SinglePosInstructionType::LDVK;
					case 0x15:
						return SinglePosInstructionType::LDDV;
					case 0x18:
						return SinglePosInstructionType::LDSV;
					case 0x1E:
						return SinglePosInstructionType::ADDI;
					case 0x29:
						return SinglePosInstructionType::LDFV;
					case 0x33:
						return SinglePosInstructionType::LDBV;
					case 0x55:
						return SinglePosInstructionType::LDIV;
					case 0x65:
						return SinglePosInstructionType::LDIV;
					default:
						return SinglePosInstructionType::NOP;
				}
			}

			default:
				return SinglePosInstructionType::NOP;
		}
	}
};

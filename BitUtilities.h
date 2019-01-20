#pragma once
#include "PrimitiveTypes.h"

byte GetHighNibble(word opcode)
{
	return opcode >> 12;
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
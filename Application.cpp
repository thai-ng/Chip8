#include <array>
#include <type_traits>

template <typename T>
constexpr auto Val(T v)
{
	return static_cast<std::underlying_type_t<T>>(v);
}

using byte = unsigned char;
using word = unsigned short;

std::array<byte, 4069> RAM;

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

// SPEC
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

int main() 
{

	return 0;
}
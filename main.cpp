#include "iostream"
#include <cstdint>
#include <fstream>

unsigned char memory[4096]{};
unsigned int startingAddress = 0x200;
unsigned char* pc = memory;
unsigned char display[64 * 32]{};

void loadFont()
{
	unsigned int fontAddress = 0x050; 
	unsigned char font_data[80] {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	for (int i = 0; i < 80; i++) 
	{
		memory[fontAddress + i] = font_data[i];
		std::cout << std::hex << (int)memory[fontAddress + i] << '\n'; 
	}
}

void loadROM(std::string_view filePath) 
{
	std::ifstream file{filePath.data(), std::ios::binary};
	int counter = 0; 
	while(!file.eof())
	{
		unsigned char c = file.get();
		memory[startingAddress + counter] = c;
		counter++; 
	}
	pc += startingAddress;
	loadFont();
}

uint16_t fetchInstructions() 
{
	unsigned char byte = *pc; 

	unsigned char byte_2= *(pc + 1); 

	pc += 2;

	uint16_t result = (static_cast<uint16_t>(byte) << 8) | byte_2;

	return result;
}

void clearScreen()
{
	for(int i = 0; i < 64 * 32; i++)
	{
		display[i] = 0; 
	}
}


void decode(uint16_t instruction)
{
	uint16_t opcode = (instruction & 0xF000) >> 12;
	switch (opcode) 
	{
		case 0x0:
			std::cout << "Hello"; 
		break;

		case 0x1:
			std::cout << "Hello"; 
		break;

		case 0x6:
			std::cout << "Hello"; 
		break;

		case 0x7:
			std::cout << "Hello"; 
		break;

		case 0xA:
			std::cout << "Hello"; 
		break;

		case 0xD:
			std::cout << "Hello"; 
		break;


	}
}


int main() 
{
	loadROM("./logo.ch8"); 
	int counter = 0; 
	// while (counter != 10) 
	// {
	// 	uint16_t instruction = fetchInstructions(); 
	// 	std::cout << std::setw(4) << std::setfill('0') <<  std::hex << static_cast<uint16_t>(instruction) << '\n';
	// //	decode(instruction); 
	// 	// execute();
	// 	counter += 1; 
	// }
}

#include "iostream"
#include <cstddef>
#include <cstdint>
#include <fstream>

unsigned char memory[4096]{};
uint16_t startingAddress = 0x200;
uint16_t pc = startingAddress;
unsigned char display[64 * 32]{};
uint8_t vx[16]{};

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
	loadFont();
}

uint16_t fetchInstructions() 
{
	uint16_t instruction = (memory[pc] << 8) | memory[pc + 1];

	pc += 2;

	return instruction;
}

void OP_00E0()
{
	for(int i = 0; i < 64 * 32; i++)
	{
		display[i] = 0; 
	}
}
void OP_1NNN(uint16_t params) 
{
	pc = params;
}

void OP_6XNN(uint16_t params)
{
	uint16_t regName = (params & 0x0F00) >> 8; 
	uint16_t value = (params & 0x00FF); 
	vx[regName] = value;
}

void OP_7XNN(uint16_t params) 
{
	uint16_t regName = (params & 0x0F00) >> 8; 

	uint16_t value = (params & 0x00FF); 

	vx[regName] += value;
}





void decode(uint16_t instruction)
{
	uint16_t opcode = (instruction & 0xF000) >> 12;
	uint16_t params = (instruction & 0x0FFF);

	switch (opcode) 
	{
		case 0x0:
			OP_00E0(); 
		break;

		case 0x1:
			OP_1NNN(params);
		break;

		case 0x6:
			OP_6XNN(params); 
		break;

		// case 0x7:
		// 	std::cout << "Hello"; 
		// break;
		//
		// case 0xA:
		// 	std::cout << "Hello"; 
		// break;
		//
		// case 0xD:
		// 	std::cout << "Hello"; 
		// break;
	}
}



int main() 
{
	loadROM("./logo.ch8"); 
	int counter = 0; 
	while (counter != 10) 
	{
		uint16_t instruction = fetchInstructions(); 
		std::cout << std::hex << instruction << '\n';
		decode(instruction); 
		counter += 1; 
	}
}

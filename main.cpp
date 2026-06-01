#include "iostream"
#include <cstdint>
#include <fstream>
#include <iomanip>

unsigned char memory[4096]{};
unsigned char* pc = memory;
unsigned char display[64 * 32]{};

void loadROM(std::string_view filePath) 
{
	std::ifstream file{filePath.data(), std::ios::binary};
	if (file.is_open()) 
	{
		file.read(reinterpret_cast<char*>(memory), 4096); 
		if(file.gcount() > 0)
		{
			std::cout << "File Read Succesfully" << '\n';
		}
	}

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
	switch (instruction) 
	{
		case 0x00E0: clearScreen();
			
	}
}


int main() 
{
	loadROM("./logo.ch8"); 
	int counter = 0; 
	while (counter != 10) 
	{
		uint16_t instruction = fetchInstructions(); 
		std::cout << std::setw(4) << std::setfill('0') <<  std::hex << static_cast<uint16_t>(instruction) << '\n';
		// decode(); 
		// execute();
		counter += 1; 
	}
}

#include "iostream"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <SDL3/SDL.h>

unsigned char memory[4096]{};
uint16_t startingAddress = 0x200;
uint16_t indexRegister = 0;
uint16_t pc = startingAddress;

uint8_t display[64*32]{}; 

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

void OP_ANNN(uint16_t params)
{
	indexRegister = params;
}


void OP_DXYN(uint16_t params)
{
	uint16_t param_x=  ((params & 0x0F00) >> 8); 
	uint16_t param_y =  ((params & 0x00F0) >> 4); 
	uint16_t num_pixels = (params & 0x000F); 

	uint8_t start_x = vx[param_x] & 63; 
	uint8_t start_y = vx[param_y] & 31; 

	vx[0xF] = 0;

	for(int i = 0; i < num_pixels; i++) 
	{
		if (start_y + i > 31) 
		{
			break;
		}
		uint8_t spriteData = memory[indexRegister + i]; 
		int current_y = start_y + i; 

		for(int j = 0; j < 8; j++)
		{
			if (start_x + j > 63) 
			{
				break;
			}
			int current_x = start_x + j; 
			uint8_t mask = (0x80 >> j);
			uint8_t pixel = mask & spriteData; 

			int bufferedIndex = current_x + (current_y * 64); 
			if (pixel != 0) 
			{
				if (display[bufferedIndex] == 1) 
				{
					display[bufferedIndex] = 0; 
					vx[0xf] = 1; 
				}
				else 
				{
					display[bufferedIndex] = 1; 
				}
			} 
		}
	}
}

void draw(SDL_Renderer* renderer)
{
	const float SCALE_FACTOR = 10.0f;
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White pixels
	for (int y = 0; y < 32; y++) 
	{
		for (int x = 0; x < 64; x++) 
		{
			if (display[x + (y * 64)] == 1) 
			{
				SDL_FRect rect;
				rect.x = x * SCALE_FACTOR;
				rect.y = y * SCALE_FACTOR; 
				rect.w = SCALE_FACTOR;
				rect.h = SCALE_FACTOR;
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
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

		case 0x7:
			OP_7XNN(params); 
		break;

		case 0xA:
			OP_ANNN(params); 
		break;

		case 0xD:
			OP_DXYN(params); 
		break;
	}
}



int main() 
{
	loadROM("./logo.ch8"); 
	int counter = 0; 
	SDL_Window* window{};
	SDL_Renderer* renderer{};
	SDL_CreateWindowAndRenderer("Chip8 Emulator", 640, 320, SDL_WINDOW_RESIZABLE, &window, &renderer); 
	bool done = false; 

	while (!done) 
	{
		SDL_Event* e; 
		while (SDL_PollEvent(e)) 
		{
			if (e->type == SDL_EVENT_QUIT) 
			{
				done = true;
			}
		}
		uint16_t instruction = fetchInstructions(); 

		SDL_RenderClear(renderer); 
		decode(instruction); 
		draw(renderer); 
		SDL_RenderPresent(renderer); 
		//SDL_Delay(2);
	}
}

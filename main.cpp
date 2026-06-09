#include "iostream"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <SDL3/SDL.h>
#include <sys/types.h>
#include <vector>

unsigned char memory[4096]{};
uint16_t startingAddress = 0x200;
uint16_t indexRegister = 0;
uint16_t pc = startingAddress;

uint8_t display[64*32]{}; 

uint8_t vx[16]{};
std::vector<uint16_t> stack{};
uint8_t keypad[16]{}; 

uint8_t delayTimer{}; 

uint8_t soundTimer{}; 


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

void handkeKeyPad(SDL_Event* e)
{
	if (e->key.type == SDL_EVENT_KEY_DOWN) 
	{
		if (e->key.scancode == SDL_SCANCODE_1) 
		{
			keypad[0x0] = 1; 
		}

		else if (e->key.scancode == SDL_SCANCODE_2) 
		{
			keypad[0x1] = 1; 
		}
		else if (e->key.scancode == SDL_SCANCODE_3) 
		{
			keypad[0x2] = 1; 
		}
		else if (e->key.scancode == SDL_SCANCODE_4) 
		{
			keypad[0x3] = 1; 
		}
		else if (e->key.scancode == SDL_SCANCODE_Q) 
		{
			keypad[0x4] = 1; 
		}
		else if (e->key.scancode == SDL_SCANCODE_W) 
		{
			keypad[0x5] = 1; 
		
		}
		else if (e->key.scancode == SDL_SCANCODE_E) 
		{
			keypad[0x6] = 1; 
		}

		else if (e->key.scancode == SDL_SCANCODE_R) 
		{
			keypad[0x7] = 1; 
		}

		else if (e->key.scancode == SDL_SCANCODE_A) 
		{
			keypad[0x8] = 1; 
		}
		else if (e->key.scancode == SDL_SCANCODE_S) 
		{
			keypad[0x9] = 1; 
		}

		else if (e->key.scancode == SDL_SCANCODE_D) 
		{
			keypad[0xA] = 1; 
		}

		else if (e->key.scancode == SDL_SCANCODE_F) 
		{
			keypad[0xB] = 1; 
		}

		else if (e->key.scancode == SDL_SCANCODE_Z) 
		{
			keypad[0xC] = 1; 
		}

		else if (e->key.scancode == SDL_SCANCODE_X) 
		{
			keypad[0xD] = 1; 
		}

		else if (e->key.scancode == SDL_SCANCODE_C) 
		{
			keypad[0xE] = 1; 
		}

		else if (e->key.scancode == SDL_SCANCODE_V) 
		{
			keypad[0xF] = 1; 
		}
	} 
	else 
	{
		for(int i = 0; i < 16; i++)
		{
			keypad[i] = 0; 
		}
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

void OP_BNNN(uint16_t params)
{
	indexRegister = params + vx[0];
}

void OP_CXNN(uint16_t params)
{
	uint16_t regName = (params & 0x0F00) >> 8; 
	uint16_t value = (params & 0x00FF); 

	std::srand(time(nullptr)); 

	int randomNumber = (std::rand() % 1000);

	vx[regName] = value & randomNumber;
}

void OP_EX9E(uint16_t params)
{
	uint16_t regName = (params & 0x0F00) >> 8; 

	if (keypad[vx[regName]]) 
	{
		pc += 2; 
	}
}

void OP_EXA1(uint16_t params)
{
	uint16_t regName = (params & 0x0F00) >> 8; 

	if (keypad[vx[regName]] == 0) 
	{
		pc += 2; 
	}
}



void OP_DXYN(uint16_t params)
{
	uint16_t param_x = (params & 0x0F00) >> 8; 

	uint16_t param_y = (params & 0x00F0) >> 4; 

	uint16_t num_pixels = (params & 0x000F); 

	uint8_t start_x = vx[param_x] & 63;

	uint8_t start_y = vx[param_y] & 31;

	vx[0xf] = 0; 

	for(int y = 0; y < num_pixels; y++)
	{

		uint8_t current_y = start_y + y; 
		if (current_y > 31) 
		{
			break;
		}
		uint8_t sprite_data = memory[indexRegister + y];
		for(int x = 0; x < 8; x++)
		{
			int current_x = start_x + x; 
			if (current_x > 63) 
			{
				break;
			}
			int bufferedIndex = (current_y * 64) + current_x;
			uint8_t pixel = ((0x80 >> x) & sprite_data); 
			if (pixel) 
			{
				if(display[bufferedIndex] == 0)
				{
					display[bufferedIndex] = 1;
				} else {
					display[bufferedIndex] = 0; 
					vx[0xf] = 1; 
				}
			}
		}
	}

}

void OP_2NNN(uint16_t params)
{
	stack.push_back(pc); 
	pc = params;
}

void OP_00EE()
{
	uint16_t value = stack.back();  
	stack.pop_back(); 
	pc = value;
}

void OP_3XNN(uint16_t params)
{
	uint16_t regValue = (params & 0x0F00) >> 8; 

	uint8_t num_value= (params & 0x00FF); 

	if (vx[regValue] == num_value) 
	{
		pc += 2; 
	}
}

void OP_4XNN(uint16_t params)
{
	uint16_t regValue = (params & 0x0F00) >> 8; 

	uint8_t num_value= (params & 0x00FF); 

	if (vx[regValue] != num_value) 
	{
		pc += 2; 
	}
}

void OP_5XY0(uint16_t params)
{

	uint16_t regValue = (params & 0x0F00) >> 8; 

	uint8_t num_value= (params & 0x00FF); 

	if (vx[regValue] == num_value) 
	{
		pc += 2; 
	}

}

void OP_8XY0(uint16_t params)
{
	uint16_t regValue_x = (params & 0x0F00) >> 8; 
	uint16_t regValue_y = (params & 0x00F0) >> 4; 
	vx[regValue_x] = vx[regValue_y];
}

void OP_8XY1(uint16_t params)
{
	uint16_t regValue_x = (params & 0x0F00) >> 8; 
	uint16_t regValue_y = (params & 0x00F0) >> 4; 
	vx[regValue_x] |= vx[regValue_y];
}

void OP_8XY2(uint16_t params)
{

	uint16_t regValue_x = (params & 0x0F00) >> 8; 
	uint16_t regValue_y = (params & 0x00F0) >> 4; 
	vx[regValue_x] &= vx[regValue_y];
}

void OP_8XY3(uint16_t params)
{
	uint16_t regValue_x = (params & 0x0F00) >> 8; 
	uint16_t regValue_y = (params & 0x00F0) >> 4; 
	vx[regValue_x] ^= vx[regValue_y];
}

void OP_8XY4(uint16_t params)
{
	uint16_t regValue_x = (params & 0x0F00) >> 8; 
	uint16_t regValue_y = (params & 0x00F0) >> 4; 

	if (vx[regValue_x] + vx[regValue_y] > 255) 
	{
		vx[0xF] = 1;
	} 
	else 
	{
		vx[0xF] = 0;
	}
	vx[regValue_x] += vx[regValue_y];
}

void OP_8XY5(uint16_t params)
{
	uint16_t regValue_x = (params & 0x0F00) >> 8; 
	uint16_t regValue_y = (params & 0x00F0) >> 4; 

	if (vx[regValue_x] >= vx[regValue_y]) 
	{
		vx[0xf] = 1; 
	} 
	else 
	{
		vx[0xf] = 0; 
	}
	vx[regValue_x] -= vx[regValue_y];
}

void OP_8XY7(uint16_t params)
{
	uint16_t regValue_x = (params & 0x0F00) >> 8; 
	uint16_t regValue_y = (params & 0x00F0) >> 4; 

	if (vx[regValue_x] >= vx[regValue_y]) 
	{
		vx[0xf] = 1; 
	} 
	else 
	{
		vx[0xf] = 0; 
	}
	vx[regValue_y] -= vx[regValue_x];
}

void OP_8XY6(uint16_t params)
{
	uint16_t regValue_x = (params & 0x0F00) >> 8; 
	uint16_t regValue_y = (params & 0x00F0) >> 4; 

	vx[regValue_x] = vx[regValue_y];
	if (vx[regValue_x] == 0) 
	{
		vx[0xf] = 0; 
	} 
	else 
	{
		vx[0xf] = 1; 
	}
	vx[regValue_x] = vx[regValue_x] >> 1; 
}

void OP_8XYE(uint16_t params)
{
	uint16_t regValue_x = (params & 0x0F00) >> 8; 
	uint16_t regValue_y = (params & 0x00F0) >> 4; 

	vx[regValue_x] = vx[regValue_y];
	if (vx[regValue_x] == 0) 
	{
		vx[0xf] = 0; 
	} 
	else 
	{
		vx[0xf] = 1; 
	}
	vx[regValue_x] = vx[regValue_x] << 1; 
}


void OP_9XY0(uint16_t params)
{
	uint16_t regValue = (params & 0x0F00) >> 8; 

	uint8_t num_value= (params & 0x00FF); 

	if (vx[regValue] != num_value) 
	{
		pc += 2; 
	}
}

void OP_FX07(uint16_t params)
{
	uint16_t regValue = (params & 0x0F00) >> 8; 

	vx[regValue] = delayTimer;
}

void OP_FX15(uint16_t params)
{
	uint16_t regValue = (params & 0x0F00) >> 8; 

	delayTimer = vx[regValue];
}

void OP_FX18(uint16_t params)
{
	uint16_t regValue = (params & 0x0F00) >> 8; 

	soundTimer = vx[regValue]; 
}



void draw(SDL_Renderer* renderer)
{
	const float SCALE = 10.0f; 
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
	for(int i = 0; i < 64 * 32; i++)
	{
		if (display[i] == 1) 
		{
			SDL_FRect rect; 

			rect.x = (i % 64) * SCALE; 

			rect.y = (i / 64) * SCALE; 

			rect.w = SCALE;

			rect.h = SCALE;

			SDL_RenderFillRect(renderer, &rect); 
		}
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
}
	
void decode(uint16_t instruction)
{
	uint16_t opcode = (instruction & 0xF000) >> 12;
	uint16_t params = (instruction & 0x0FFF);
	uint16_t operation = (instruction & 0x000F); 

	switch (opcode) 
	{
		case 0x0:
			if (params == 0xE0) 
			{
				OP_00E0(); 
			} 
			else 
			{
				OP_00EE();
			}
		break;

		case 0x1:
			OP_1NNN(params);
		break;

		case 0x2: 
			OP_2NNN(params); 
		break;

		case 0x3: 
			OP_3XNN(params); 
		break;

		case 0x6:
			OP_6XNN(params); 
		break;

		case 0x7:
			OP_7XNN(params); 
		break;

		case 0x8: 
			switch (operation) 
			{
				case 0x0: 
					OP_8XY0(params); 
				break;

				case 0x1:
					OP_8XY1(params); 
				break;

				case 0x2: 
					OP_8XY2(params); 
				break;

				case 0x3: 
					OP_8XY3(params); 
				break;

				case 0x4: 
					OP_8XY4(params); 
				break;

				case 0x5: 
					OP_8XY5(params); 
				break;

				case 0x6: 
					OP_8XY6(params); 
				break;

				case 0x7: 
					OP_8XY7(params); 
				break;

				case 0xE:
					OP_8XYE(params); 
				break;
			}
		break;

		case 0xA:
			OP_ANNN(params); 
		break;

		case 0xB: 
			OP_BNNN(params); 
		break;

		case 0xC: 
			OP_CXNN(params); 
		break;

		case 0xD:
			OP_DXYN(params); 

		case 0xE:
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
			if (e->type == SDL_EVENT_KEY_DOWN) 
			{
				if (e->key.scancode == SDL_SCANCODE_1) 
				{
					keypad[0x0] = 1; 
				}

				else if (e->key.scancode == SDL_SCANCODE_2) 
				{
					keypad[0x1] = 1; 
				
				}
				else if (e->key.scancode == SDL_SCANCODE_3) 
				{
					keypad[0x2] = 1; 
				
				}
				else if (e->key.scancode == SDL_SCANCODE_4) 
				{
					keypad[0x3] = 1; 
				
				}
				else if (e->key.scancode == SDL_SCANCODE_Q) 
				{
					keypad[0x4] = 1; 
				
				}
				else if (e->key.scancode == SDL_SCANCODE_W) 
				{
					keypad[0x5] = 1; 
				
				}

				else if (e->key.scancode == SDL_SCANCODE_E) 
				{
					keypad[0x6] = 1; 
				
				}

				else if (e->key.scancode == SDL_SCANCODE_R) 
				{
					keypad[0x7] = 1; 
				}

				else if (e->key.scancode == SDL_SCANCODE_A) 
				{
					keypad[0x8] = 1; 
				}

				else if (e->key.scancode == SDL_SCANCODE_S) 
				{
					keypad[0x9] = 1; 
				}

				else if (e->key.scancode == SDL_SCANCODE_D) 
				{
					keypad[0xA] = 1; 
				}

				else if (e->key.scancode == SDL_SCANCODE_F) 
				{
					keypad[0xB] = 1; 
				}

				else if (e->key.scancode == SDL_SCANCODE_Z) 
				{
					keypad[0xC] = 1; 
				}

				else if (e->key.scancode == SDL_SCANCODE_X) 
				{
					keypad[0xD] = 1; 
				}

				else if (e->key.scancode == SDL_SCANCODE_C) 
				{
					keypad[0xE] = 1; 
				}

				else if (e->key.scancode == SDL_SCANCODE_V) 
				{
					keypad[0xF] = 1; 
				}
			}
		}
		uint16_t instruction = fetchInstructions(); 

		SDL_RenderClear(renderer); 
		decode(instruction); 
		draw(renderer); 
		SDL_RenderPresent(renderer); 
		SDL_Delay(2);
	}
}



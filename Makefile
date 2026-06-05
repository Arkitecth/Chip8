main:
	g++ main.cpp `pkg-config -libs -cflags sdl3 ` -o main  

clean:
	rm main 

all:
	gcc src/main.c -o main -I/opt/homebrew/include -L/opt/homebrew/lib -Wl,-rpath,/opt/homebrew/lib -lSDL3
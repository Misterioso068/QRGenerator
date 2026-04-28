all:
	gcc src/main.c src/logger.c src/input_handler.c -o build/main -Iinclude -I/opt/homebrew/include -L/opt/homebrew/lib -Wl,-rpath,/opt/homebrew/lib -lSDL3
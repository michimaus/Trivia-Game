.PHONY: build run clean
build: trivia.c
	gcc trivia.c -lm -lncurses -lpanel -lmenu -o trivia

run: build
	./trivia

clean:
	rm -f trivia

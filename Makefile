TARGETS=genetic_pong

all: $(TARGETS)

LIBS=-lzdk -lncurses -lm
DIRS=-I lib/ZDK -L lib/ZDK
FILES=game_states.h
FLAGS=-std=gnu99 -Wall -Werror -g

clean:
	for f in $(TARGETS); do \
		if [ -f $$f ]; then rm $$f; fi; \
		if [ -f $$f.exe ]; then rm $$f.exe; fi; \
	done

rebuild: clean all

genetic_pong:
	gcc $(FLAGS) $(FILES) main.c -o bin/genetic_pong $(DIRS) $(LIBS)

OBJ=Scratch.o vacuumRestart.o trashRestart.o coll.o helpers.o

# Fix the directories to match your file organisation.
CC_FLAGS=-std=gnu99 -I../ZDK -Wall -Werror -g
LD_FLAGS=-L../ZDK -lzdk -lncurses -lm

Scratch: $(OBJ)
	gcc $(OBJ) $(LD_FLAGS) -o $@

%.o : %.c
	gcc -c $< $(CC_FLAGS)

CC = g++
CFLAGS =  -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

SRCDIR = ../src/
SRCFILE = main.cpp 

OBJ = $(SRCFILE:.cpp=.o) 
EXEFILE = $(SRCFILE:.cpp=.bin)

all: $(EXEFILE) run clean

$(EXEFILE): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(EXEFILE)

$(OBJ): $(SRCDIR)$(SRCFILE) 
	$(CC) -c $< -o $@

run: $(EXEFILE)
	./$(EXEFILE)

clean:
	rm -f $(OBJ) $(EXEFILE)
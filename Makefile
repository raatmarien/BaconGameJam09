VPATH = src
CC = g++
CFLAG = -I headers
LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lBox2D
OBJ = main.o            \
      chunk.o           \
      chunkManager.o    \
      player.o          \
      contactListener.o \
      customCursor.o
HEADERS = $(wildcard headers/*.h)
EXEC = bgj09

all : bgj09

%.o : %.cpp
	$(CC) -c $< -o $@ $(CFLAG)

bgj09 : $(OBJ) $(HEADERS)
	$(CC) -o $(EXEC) $(OBJ) $(LIBS)

.PHONY : clean
clean :
	rm $(EXEC) $(OBJ)

CC=g++ -Ofast
CFLAGS=-I.
DEPS = Tile.h World.h FastNoise.h Functions.h Civilization.h Town.h Entity.h
LIBS= -lsfml-graphics -lsfml-window -lsfml-system
OBJ = FastNoise.o main.o


%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ProcWorld: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f ProcWorld
	rm -f *.o


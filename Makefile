OBJ=ugzip.o
LIB=-lowfat -lz
CC=gcc

ugzip: $(OBJ)
	$(CC) -static $< -o $@ $(LIB)
	ln -sf ugzip ugunzip

clean:
	rm -f $(OBJ) ugzip ugunzip

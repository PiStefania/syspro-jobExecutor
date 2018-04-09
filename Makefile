CC = gcc -g -lm
FILES = main.c variousMethods.c index.c invertedIndex.c searchQuery.c tfQuery.c dfQuery.c generalInfo.c
OBJECTS = main.o variousMethods.o index.o invertedIndex.o searchQuery.o tfQuery.o dfQuery.o generalInfo.o
OUT = minisearch
HEADERS = variousMethods.h index.h invertedIndex.h searchQuery.h tfQuery.h dfQuery.h generalInfo.h


all: $(OBJECTS) $(HEADERS)
	$(CC) -o $(OUT) $(FILES) -lm
	make clean
	
main.o: main.c
	$(CC) -c main.c

variousMethods.o: variousMethods.c
	$(CC) -c variousMethods.c
	
index.o: index.c
	$(CC) -c index.c
	
invertedIndex.o: invertedIndex.c
	$(CC) -c invertedIndex.c
	
searchQuery.o: searchQuery.c
	$(CC) -c searchQuery.c
	
tfQuery.o: tfQuery.c
	$(CC) -c tfQuery.c
	
dfQuery.o: dfQuery.c
	$(CC) -c dfQuery.c
	
generalInfo.o: generalInfo.c
	$(CC) -c generalInfo.c
	
clean:
	rm -f $(OBJECTS)
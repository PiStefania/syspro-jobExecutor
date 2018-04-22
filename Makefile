CC = gcc -g -lm
FILES = main.c variousMethods.c index.c invertedIndex.c generalInfo.c worker.c
OBJECTS = main.o variousMethods.o index.o invertedIndex.o generalInfo.o worker.o
OUT = jobExecutor
HEADERS = variousMethods.h index.h invertedIndex.h generalInfo.h worker.h


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
	
generalInfo.o: generalInfo.c
	$(CC) -c generalInfo.c
	
worker.o: worker.c
	$(CC) -c worker.c
	
clean:
	rm -f $(OBJECTS)
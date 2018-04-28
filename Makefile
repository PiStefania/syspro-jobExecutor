CC = gcc -g -lm
FILES = main.c variousMethods.c worker.c index.c invertedIndex.c namedPipes.c
OBJECTS = main.o variousMethods.o worker.o index.o invertedIndex.o namedPipes.o
HEADERS = variousMethods.h worker.h index.h invertedIndex.h namedPipes.h
OUT = jobExecutor

all: $(OBJECTS) $(HEADERS)
	$(CC) -o $(OUT) $(FILES) -lm
	make clean
	
main.o: main.c
	$(CC) -c main.c

variousMethods.o: variousMethods.c
	$(CC) -c variousMethods.c
	
worker.o: worker.c
	$(CC) -c worker.c
	
index.o: index.c
	$(CC) -c index.c
	
invertedIndex.o: invertedIndex.c
	$(CC) -c invertedIndex.c
	
namedPipes.o: namedPipes.c
	$(CC) -c namedPipes.c
	
clean:
	rm -f $(OBJECTS)
CC = gcc -g -lm
FILES = main.c variousMethods.c worker.c
OBJECTS = main.o variousMethods.o  worker.o
OUT = jobExecutor
HEADERS = variousMethods.h worker.h


all: $(OBJECTS) $(HEADERS)
	$(CC) -o $(OUT) $(FILES) -lm
	make clean
	
main.o: main.c
	$(CC) -c main.c

variousMethods.o: variousMethods.c
	$(CC) -c variousMethods.c
	
worker.o: worker.c
	$(CC) -c worker.c
	
clean:
	rm -f $(OBJECTS)
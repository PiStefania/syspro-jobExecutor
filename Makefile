CC = gcc -g -lm
FILES = main.c variousMethods.c worker.c index.c invertedIndex.c namedPipes.c wc.c maxcount.c mincount.c search.c exit.c
OBJECTS = main.o variousMethods.o worker.o index.o invertedIndex.o namedPipes.o wc.o maxcount.o mincount.o search.o exit.o
HEADERS = variousMethods.h worker.h index.h invertedIndex.h namedPipes.h wc.h maxcount.h mincount.h search.h exit.h
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
	
wc.o: wc.c
	$(CC) -c wc.c
	
maxcount.o: maxcount.c
	$(CC) -c maxcount.c

mincount.o: mincount.c
	$(CC) -c mincount.c
	
search.o: search.c
	$(CC) -c search.c
	
exit.o: exit.c
	$(CC) -c exit.c
	
clean:
	rm -f $(OBJECTS)
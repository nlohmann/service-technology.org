all: rank

rank: rank.o
	gcc -fopenmp rank.o -o rank

rank.o: rank.c
	gcc -fopenmp rank.c -c

clean:
	$(RM) rank.o rank

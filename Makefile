
all: test.o window.o
	gcc test.o window.o -lcunit -o run

test.o: test.c
	gcc -c test.c 

window.o: window.c window.h
	gcc -c window.c

clean:
	rm *.o run TestResults-Listing.xml TestResults-Results.xml
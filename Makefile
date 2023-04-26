CFLAGS=-W -Wall -I/usr/local/include -g
LDFLAGS=-L/usr/local/lib -g
PROGRAMS = uspsv1, uspsv2, uspsv3, uspsv4


uspsv1: uspsv1.o
	gcc -o uspsv1 uspsv1.o 


clean:
	rm -f uspsv1.o




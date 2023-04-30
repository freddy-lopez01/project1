CFLAGS=-W -Wall -I/usr/local/include -g
LDFLAGS=-L/usr/local/lib -g
PROGRAMS = uspsv1, uspsv1V2, uspsv2, uspsv3, uspsv4


uspsv1V2: uspsv1V2.o
	gcc -o uspsv1V2 uspsv1V2.o 

uspsv1: uspsv1.o
	gcc -o uspsv1 uspsv1.o 

uspsv2: uspsv2.o
	gcc -o uspsv2 uspsv2.o

uspsv3: uspsv3.o
	gcc -o uspsv3 uspsv3.o 





clean:
	rm *.o uspsv1V2 uspsv1 uspsv2 uspsv3




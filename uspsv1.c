#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "p1fxns.c"
#include <errno.h>
#include <fcntl.h>

#define F_OK 0
#define UNUSED __attribute__((unused))
#define MAX 128

//void execWork()


int main(UNUSED int argc, char *argv[]){
	/* 

	Need to check that there are at least two arguments in the ./uspsv1 workload file

	next if file is specified then open up the file and read the first line

	once the whole line is read, pass in the command and its aguments 


	*/
	char buf[MAX];
	char programs[MAX];
	char arguments[MAX];


	char *fname; 
	UNUSED int quantum; 
	int fd; 


	//Check for usage errors that could occur

    if(argc == 2){
		fname = argv[1];
	} else if(argc == 3){
		quantum = atoi(argv[1]);
		fname = argv[2];
	} else{
		p1putstr(2, ("Usage: ./uspsv1 [q <quantum in msec>] [workload file]\n"));
		fd  = 0; 
	} 
												//Proceed to read from the specified file and execute commands from file
	int j;
	int wrd = 0; 
	fd = open(fname, 0); 						// open file that was put in by the user
	if(fd == -1){
		perror("open");
		return 1; 
	}
	while((j = p1getline(fd, buf, MAX)) != 0){ // while there is still a next line
		if(j == -1){
			p1putstr(2, "Error reading file line\n");
			return EXIT_FAILURE;
		}

		wrd = p1getword(buf, wrd, programs);
		/*while((p1getword(buf, wrd, arguments)) != -1){
			printf("%d\n", wrd);
		}*/
		printf("here\n");
	}

	close(fd);
	return EXIT_SUCCESS;
}
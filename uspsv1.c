// Freddy Lopez
// flopez2
//CS 415 PRoject 1
// THis is my own work
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "p1fxns.c"
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

#define F_OK 0
#define UNUSED __attribute__((unused))
#define MAX 128



int parse(int fd, char ***arr){
	char buf[MAX];
	char word[MAX];
	int i = 0;
	int j;  

	while((j = p1getline(fd, buf, BUFSIZ)) != 0){  // while there is still a next line
		int k = 0; 
		int index = 0; 
		while((index = p1getword(buf, index, word)) != -1){ 
			arr[i][k] = p1strdup(word);
			k++; 
		}
		arr[i][k] = NULL;
		i++; 
	}
	return 1; 
}

void executePrograms(char ***arr, int numPrograms){
	int pid[numPrograms];

	for(int i = 0; i < numPrograms; i++){

		pid[i] = fork();
		switch(pid[i]){
		case -1: 
			p1putstr(1, "error when forking()\n");
			exit(EXIT_FAILURE);
		case 0:
			execvp(arr[i][0], arr[i]);
		default:
			wait(NULL);

		}
	}

}




int main(UNUSED int argc, char *argv[]){
	/* 
	Need to check that there are at least two arguments in the ./uspsv1 workload file
	next if file is specified then open up the file and read the first line
	once the whole line is read, pass in the command and its aguments 
	*/
	char word[MAX];
	char buf[MAX];
	char ***programs;

	char *fname; 
	UNUSED int quantum; 
	int fd; 
	int qflag = 0; 



	int c; 
	//Check for usage errors that could occur
    opterr = 0; 
	while((c = getopt(argc, argv, "q:")) != -1)
		switch(c){
			case 'q':
				qflag = 1;
				break;
			case '?':
				p1putstr(2, ("Usage: ./uspsv2 [-q <quantum in msec>] [workload file]\n"));
			default: 
				continue; 
		}

	//Check for usage errors that could occur

	if(qflag){
		if(argc == 3){
			quantum = p1atoi(argv[2]);
		} 
		if(argc ==4 ){
			quantum = p1atoi(argv[2]);
			fname =  argv[4];
		}

	} else if(argc == 2){
		//quantum = p1atoi(argv[1]);

		char *p;
		int val = -1; 
		if((p = getenv("USPS_QUANTUM_MSEC")) != NULL){
			val = p1atoi(p);
		}
		quantum = val; 

		//printf("quantum: %d\n", val);
		fname = argv[1];

													// Proceed to read from the specified file and execute commands from file
		fd = open(fname, 0); 						// open file that was put in by the user
		if(fd == -1){
			p1putstr(2, "OPEN ERROR\n");
			p1perror(fd, "open"); 						// error  
			return EXIT_FAILURE; 
		}
		int progcount = 0;
		int argcount = 0;
		int j; 
		int largest_line = 0; 
		while((j = p1getline(fd, buf, BUFSIZ)) != 0){  // while there is still a next line
			int index = 0; 
			while((index = p1getword(buf, index, word)) != -1){ 
				argcount++;
			}
			if(argcount > largest_line){
				largest_line = argcount;
			}
			progcount++;
			argcount = 0; 

		}

		//printf("prog count: %d\nlargest line: %d\n", progcount, largest_line);

		// malloc a a 2d array that will become a 3d array with p1stdup()
		programs = (char***)malloc(sizeof(char **) * (progcount+1));
		for(int i = 0; i < progcount; i++){
			programs[i] = (char**)malloc(sizeof(char*) * (largest_line+1));
		}

		close(fd);
		fd = open(fname, 0);
		parse(fd, programs);
		close(fd);

		executePrograms(programs, progcount);

		//printf("Done\n");
		for(int i = 0; i < (progcount+1); i++){
			free(programs[i]);
		}

		free(programs);


	} else{

		char buf[MAX];
		char word[MAX];
		char *prog[MAX]; 

		char *p;
		int val = -1; 
		if((p = getenv("USPS_QUANTUM_MSEC")) != NULL){
			val = p1atoi(p);
		}
		quantum = val; 


		int index = 0; 
		UNUSED int bytes; 
		p1putstr(1, "Enter bash command: \n");
		int i = 0; 

		bytes = p1getline(0, buf, MAX);

		while((i = p1getword(buf, i, word)) != -1){
			prog[index] = p1strdup(word); 
			index++;
		}


		int pid; 
		//p1putstr(1, "here\n");
		pid = fork();
			switch(pid){

			case -1:
				//fprintf(stderr, "Parent: fork failed\n");
				p1putstr(2, "Parent: fork() failed\n");
				exit(EXIT_FAILURE);
			case 0: 
				execvp(*prog, prog);
				fprintf(stderr, "Child: execvp() failed\n");
				exit(EXIT_FAILURE);
			default:
				wait(NULL); 
			}
		return EXIT_SUCCESS;
		//printf("rip\n");
	}	

	return EXIT_SUCCESS;
}

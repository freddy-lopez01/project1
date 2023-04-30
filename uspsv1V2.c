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

int pid[MAX]; 
char buf[BUFSIZ];
char word[MAX];



void executestd(char *programs[], char *arguments[], int progms){

	printf("in executestd\n");

	for(int i = 0; i < progms; i++){

		pid[i] = fork();
		if(pid[i] < 0){
			p1putstr(1, "error when forking()\n");
			exit(EXIT_FAILURE); 
		}
		if(pid[i] ==0){
			//printf("%s %s\n", programs[i], arguments[i]);
			execvp(programs[i], arguments[i]);
		}
	}

	for(int i = 0; i < progms; i++){
		wait(pid[i]);
	}

	p1putstr(1, "finished executing\n");

}


int parsefile(int fd, char *program[], char *arguments[]){
	int strlen; 
	int i; 
	int total_programs = 0;
	int p1wdcnt = 0; 
	int index_prog = 0;

	while((p1getline(fd, buf, MAX)) != 0){
		char tmp[MAX];
		strlen = p1strlen(buf);
		//index into buf to replace end of line to null character
		if(buf[strlen - 1] == '\n'){
			buf[strlen - 1] == '\0';
		}
		int curr = 0; 

		while(p1wdcnt != -1){
			

			p1wdcnt = p1getword(buf, p1wdcnt, word);

			if(curr == 0){
				program[index_prog] = p1strdup(word);
				curr++;

			} else{
				printf("curr %d\n", curr);
				tmp[curr-1] = p1strdup(word);
				arguments[index_prog] = p1strdup(word);
				
				curr += 1;
			} 
		}
		p1wdcnt = 0; 
		index_prog++;

		total_programs++;
	}


	program[index_prog] = NULL;
	arguments[index_prog] = NULL;
	return total_programs; 
}






int main(UNUSED int argc, char *argv[]){
	/* 
	Need to check that there are at least two arguments in the ./uspsv1 workload file
	next if file is specified then open up the file and read the first line
	once the whole line is read, pass in the command and its aguments 
	*/
	char *prog[MAX];
	char *args[MAX];
	int bytes; 
	int numprog;
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

	if(qflag){
		if(argc > 4){
			p1putstr(2, ("Usage: ./uspsv2 [q <quantum in msec>] [workload file]\n"));
			return EXIT_FAILURE; 
		}

		printf("qflag: %d\n", qflag);
		quantum = p1atoi(argv[2]);

		printf("quantum: %d\nfile: %s\n", quantum, argv[3]);


	} else if(argc == 2){
		
		int num; 

		fd = open(argv[1], O_RDONLY);

		
		num = parsefile(fd, &prog, &args);
		executestd(&prog, &args, num);

		return EXIT_SUCCESS;



	}else if(argc > 5){

		printf("ERROR: To many arguments provided\n"); // REMOVE THIS PRINTF() with a system call without
												       // having an initialized fd
		p1putstr(2, ("Usage: ./uspsv2 [q <quantum in msec>] [workload file]\n"));
		return EXIT_FAILURE;

	}else{ 
		int index = 0; 
		p1putstr(1, "Enter bash command ('q' to quit): \n");
		int i = 0; 
		int t = 1;

		bytes = p1getline(0, buf, MAX);
		while((i = p1getword(buf, i, word)) != -1){
			prog[index] = p1strdup(word); 
			index++;
		}
		numprog++;

		executestd(prog, args, numprog);

		return EXIT_SUCCESS;
	} 										// Proceed to read from the specified file and execute commands from filL
}
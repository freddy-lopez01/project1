#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "p1fxns.c"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define F_OK 0
#define UNUSED __attribute__((unused))
#define MAX 128



int bytes;
int exec_stat; 
int status;
int proc_id; 

//int sigset_t sig; 


volatile int nprocesses = 0; 

static void onusr2(UNUSED int sig){
}

static void onchld(UNUSED int sig){
	pid_t pid; 
	printf("Number of processes --> %d\n", nprocesses);

	while((pid = waitpid(-1, &status, WNOHANG)) > 0){
		if(WIFEXITED(status) || WIFSIGNALED(status)){
			nprocesses--; 
			printf("killing...\n");
			kill(getpid(), SIGUSR2);
		}
	}
}



void parse(char *in, char **progs){
	while (*in != '\0') {       /* if not the end of line ....... */ 
          while (*in == ' ' || *in == '\t' || *in == '\n')
               *in++ = '\0';     /* replace white spaces with 0    */
          *progs++ = in;          /* save the argument position     */
          while (*in != '\0' && *in != ' ' && 
                 *in != '\t' && *in != '\n') 
               in++;             /* skip the argument until ...    */
     }
     *progs = '\0';                 /* mark the end of argument list  */
}


void executestdin(char **programArr){

	proc_id = fork();
		switch(proc_id){

		case -1: fprintf(stderr, "Parent: fork failed\n");
			goto wait_for_children; 
		case 0: 
			printf("\n\n");
			execvp(*programArr, programArr);
			fprintf(stderr, "Child: execvp() failed\n");
			exit(EXIT_FAILURE);
		default: nprocesses++; 
		}

	wait_for_children:
		while(nprocesses > 0){
			pause();
		}

	printf("\nfinished executing\n");


}




int main(UNUSED int argc, char *argv[]){
	struct timespec ms; 
	char buf[BUFSIZ];
	char *progs[MAX]; 
	char *args[MAX];


	int qflag = 0; 
	int fflag = 0; 
	int quantum; 
	int fd;  

	int c; 

	opterr = 0; 
	while((c = getopt(argc, argv, "qf:")) != -1)
		switch(c){
			case 'q':
				qflag = 1;
				break;
			case 'f':
				fflag = 1; 
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
		fd = open(argv[3], O_RDONLY);

		printf("quantum: %d\nfile: %s\n", quantum, argv[3]);


	} else if(fflag){

		fd = open(argv[1], O_RDONLY);

		printf("argc == 2 check\nargc: %d\n", argc);

	}else if(argc > 1){

		printf("ERROR: To many arguments provided\n"); // REMOVE THIS PRINTF() with a system call without
												       // having an initialized fd
		p1putstr(2, ("Usage: ./uspsv2 [q <quantum in msec>] [workload file]\n"));
		return EXIT_FAILURE;
	}else{ 
												// information is comming from stdin 
		
		int i = 0; 
		bytes = p1getline(0, buf, MAX);
		int wrdbytes = 0;
		//printf("buf: %s\n", buf);

		parse(buf, progs);
												// get the program command 
												/*wrdbytes = p1getword(buf, wrdbytes, progs[i]);
								                // this works

												int j = 0; 
												while(wrdbytes != -1){
												wrdbytes = p1getword(buf, wrdbytes, args[i]);
												printf("wrdbytes: %d\n", wrdbytes);
												j++; 
												}*/
		signal(SIGCHLD, onchld);
		signal(SIGUSR2, onusr2);


		executestdin(progs);
		printf("execution successful\nExiting...\n");
		return EXIT_SUCCESS; 

	}

	printf("here\n");
	int index = 0;
	int subindex = 0; 
	char word[MAX];
	p1getline(fd, buf, MAX);
	printf("%s\n", buf);

	while((p1getline(fd, buf, MAX)) != 0){
		printf("%s\n", buf);

		index = p1getword(buf, index, word);
		progs[subindex] = p1strdup(word);
		

		while(index != -1){
			p1getword(buf, index, word);
			args[subindex] = p1strdup(buf);
		}

	}

	printf("%s\n", buf);








	return EXIT_SUCCESS;
}


	// code for saving arguents and programs from input file goes here 

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
int proc_id[MAX]; 


volatile int nprocesses = 0; 

static void onusr1(UNUSED int sig){
}

static void onchld(UNUSED int sig){
	pid_t pid; 
	//p1putstr(1, ("Number of processes --> %d\n", nprocesses));

	while((pid = waitpid(-1, &status, WNOHANG)) > 0){
		if(WIFEXITED(status) || WIFSIGNALED(status)){
			nprocesses--; 
			p1putstr(1, "killing...\n");
			kill(getpid(), SIGUSR1);
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


		int pid = fork();
			switch(pid){

			case -1: fprintf(stderr, "Parent: fork failed\n");
				goto wait_for_children; 
			case 0: 
				printf("\n\n");
				sigwait(SIGUSR1, SIGSTOP);
				execvp(*programArr, programArr);
				fprintf(stderr, "Child: execvp() failed\n");
				exit(EXIT_FAILURE);
			default: nprocesses++; 
			}

		wait_for_children:
			while(nprocesses > 0){
				pause();
			}
}






void executefile(char **programArr, char **arguments){
	int total_progms = (sizeof(programArr) / sizeof(programArr[0]));

	for(int k = 0; k <= total_progms; k++){

		proc_id[k] = fork();
			switch(proc_id[k]){

			case -1: 								//fprintf(stderr, "Parent: fork failed\n");
				goto wait_for_children; 
			case 0: 
												//printf("\n\nabout to exec()\n");
												//printf("%s\n", arguments[k]);
												//printf("progs: %s\n", programArr[k]);

				signal(SIGUSR1, onusr1);
				execvp(*programArr[k], arguments[k]);
												//fprintf(stderr, "Child: execvp() failed\n");
				exit(EXIT_FAILURE);
			default: nprocesses++; 
				kill(proc_id[k], SIGUSR2);
				wait(&status);
			}

		wait_for_children:
			while(nprocesses > 0){
				pause();
			}
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

		printf("quantum: %d\nfile: %s\n", quantum, argv[3]);


	} else if(fflag){

		fd = open(argv[2], O_RDONLY);

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
		signal(SIGUSR1, onusr1);


		executestdin(progs);
		printf("execution successful\nExiting...\n");
		return EXIT_SUCCESS; 

	}

	printf("here ready to read from input file\n");
	
	int subindex = 0; 
	char word[MAX];
	//p1getline(fd, buf, MAX);
	//printf("first p1getline buf: %s\n", buf);

	while((p1getline(fd, buf, MAX)) != 0){
		printf("in while: %s\n", buf);
		int index = 0;

		index = p1getword(buf, index, word);
		printf("%s\n", word);
		progs[subindex] = p1strdup(word);
		printf("progs: %s\n", progs[subindex]);
		
		int array2d = 0;
		while((index = p1getword(buf, index, word)) != -1){
			//p1getword(buf, index, word);
			args[subindex]/*[array2d]*/ = p1strdup(buf);
			array2d++;
		}
		subindex++;

	}

	printf("progs: %s\n%s\n", progs[0], progs[1]);
	printf("args: %s\n%s\n", args[0], args[1]);
	//printf("args: %s\n%s\n", args[0][0], args[1][0]);


	printf("herre\n");

	

	executefile(progs, args);



	return EXIT_SUCCESS;
}
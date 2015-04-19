#include<sys/types.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<setjmp.h>
#include<string.h>

void csig_handler(int signo,siginfo_t *siginfo, void *context);
void csig2_handler(int signo,siginfo_t *siginfo, void *context);
void c1sig3_handler(int signo,siginfo_t *siginfo, void *context);
void c2sig3_handler(int signo,siginfo_t *siginfo, void *context);
void c3sig3_handler(int signo,siginfo_t *siginfo, void *context);

void psig_handler(int signo,siginfo_t *siginfo, void *context);

int count;
int l;
pid_t a,b,c;

int main(int argc, char *argv[]){
	

	if(argc != 2){
		printf("Usage: %s L\n",argv[0]);
		exit(1);
	}
		
	l = atoi(argv[1]);
	count = 0;

	int pfd[2];
	int ppid = getpid();

	struct sigaction act;
	if(pipe(pfd) < 0)
		perror("Pipe creation failed");


	if((a = fork()) < 0){
		perror("Process creation");
	}else if(a == 0){
		close(pfd[1]);


		memset(&act,'\0',sizeof(act));

		act.sa_sigaction = &c3sig3_handler;
		act.sa_flags = SA_SIGINFO | SA_NODEFER;
		sigaction(SIGALRM,&act,NULL);
		
		char buff[6];
		if(read(pfd[0],buff,6) < 0){
			perror("Read Error");

		}
		c = atoi(buff);
		alarm(9);
		
	}else{

		memset(&act,'\0',sizeof(act));

		act.sa_sigaction = &psig_handler;
		act.sa_flags = SA_SIGINFO | SA_NODEFER;

		sigaction(SIGINT,&act,NULL);
		
		if((b = fork()) == 0){
		close(pfd[1]);
		close(pfd[0]);


		memset(&act,'\0',sizeof(act));

		act.sa_sigaction = &c2sig3_handler;
		act.sa_flags = SA_SIGINFO | SA_NODEFER;

		sigaction(SIGALRM,&act,NULL);

		alarm(6);
	}else{
	

		if((c = fork()) == 0){
		close(pfd[1]);
		close(pfd[0]);

		memset(&act,'\0',sizeof(act));

		act.sa_sigaction = &c1sig3_handler;
		act.sa_flags = SA_SIGINFO | SA_NODEFER;		
	
		sigaction(SIGALRM,&act,NULL);
		
		
		alarm(3);
	}else{
		char buff[6];
		sprintf(buff,"%d",c);
		write(pfd[1],buff,6);

		int status;
		waitpid(-1,&status,0);
	}
	}}
		

		memset(&act,'\0',sizeof(act));

		act.sa_sigaction = &csig_handler;
		act.sa_flags = SA_SIGINFO | SA_NODEFER;

		sigaction(SIGUSR1,&act,NULL);
		sigaction(SIGUSR2,&act,NULL);

		memset(&act,'\0',sizeof(act));

		act.sa_sigaction = &csig2_handler;
		act.sa_flags = SA_SIGINFO | SA_NODEFER;
		sigaction(SIGTERM,&act,NULL);
	pause();
	return 0;
}

void psig_handler(int signo,siginfo_t *siginfo, void *context){
	
	count+=1;
	if(count == l){
		kill(c,SIGTERM);	
		
	}else if(count == l+3){

		kill(b,SIGTERM);
		kill(a,SIGTERM);
		//wait(&status);
		exit(0);
	}
	
	
	pause();
}

void csig_handler(int signo,siginfo_t *siginfo, void *context){

	printf("PID = %d SIGNO = %d\n",getpid(),signo);
	kill(getppid(),SIGINT);
	pause();
}

void csig2_handler(int signo,siginfo_t *siginfo, void *context){	
	alarm(0);
	exit(0);
}

void c1sig3_handler(int signo,siginfo_t *siginfo, void *context){
	kill(b,SIGUSR1);
	alarm(3);
	pause();
}

void c2sig3_handler(int signo,siginfo_t *siginfo, void *context){
	kill(a,SIGUSR2);
	alarm(6);
	pause();	

}
void c3sig3_handler(int signo,siginfo_t *siginfo, void *context){
	
	kill(c,SIGUSR1);
	alarm(9);
	pause();

}

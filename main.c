#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#define SIZE_BUF 4096
int flag_end = 0;
void handler(int sig) {
	wait(NULL);
	flag_end = flag_end + 1;
}

int main(int argc, char *argv[])
{
	(void)signal(SIGCHLD, handler);
	int f_in, f_out, f_gamma;
	char n_out[255];
	int pipe_in[2], pipe_gamma[2];
	char buffer[SIZE_BUF];
	int cnt_read;
	if(argc !=3){
		printf("Error! Count of argument is not 2\n");
		_exit(-1);
	}

	if(pipe(pipe_in) == -1)
		error_exit(errno);
	
	pid_t pid_in = fork();
	switch(pid_in) {
		case -1:
			error_exit(errno);
		case 0:
			close(pipe_in[0]);
			f_in = open(argv[1], O_RDONLY);
			if(!f_in)
				error_exit(errno);
			while((cnt_read = read(f_in, buffer, SIZE_BUF))!=0) {
				write(pipe_in[1], buffer, cnt_read);
			}
			close(pipe_in[1]);
			_exit(EXIT_SUCCESS);
		case 1:
			break;
	}

	if(pipe(pipe_gamma) == -1)
		error_exit(errno);

	pid_t pid_gamma = fork();
	switch(pid_gamma) {
		case -1:
			error_exit(errno);
		case 0:
			close(pipe_gamma[0]);
			f_gamma = open(argv[2], O_RDONLY);
			if(!f_gamma)
				error_exit(errno);
			while((cnt_read = read(f_gamma, buffer, SIZE_BUF))!=0) {
				write(pipe_gamma[1], buffer, cnt_read);
			}
			close(pipe_gamma[1]);
			_exit(EXIT_SUCCESS);
		case 1:
			break;
	}
	close(pipe_in[1]);
	close(pipe_gamma[1]);
	char buf_in[SIZE_BUF],buf_gamma[SIZE_BUF], buf_out[SIZE_BUF];
	strcpy(n_out, argv[1]);
	strcat(n_out, "_out");
	f_out = creat(n_out, 0777);
	if(!f_out)
		error_exit(errno);
	int cnt_read_in, cnt_read_gamma;
	int i;
	while(1) {
		cnt_read_in = read(pipe_in[0], buf_in, SIZE_BUF);
		cnt_read_gamma = read(pipe_gamma[0], buf_gamma, SIZE_BUF);
		/*if(cnt_read_in != cnt_read_gamma){
			printf("read cnts isn`t equ!\n");
			_exit(EXIT_FAILURE);
		}*/
		for(i = 0; i<cnt_read_in; i++)
			buf_out[i] = buf_in[i] ^ buf_gamma[i];			
		write(f_out, buf_out, cnt_read_in);
		if (flag_end == 2)
			break;
	}
	close(f_out);
	_exit(EXIT_SUCCESS);
}

int error_exit(int id_error)
{
	printf("Error! Comment: %s\n",strerror(errno));
	_exit(-1);
}

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE_BUF 4096

#define EQU 1
#define NO_EQU 0
int f_in,f_out,f_gamma;

int main(int argc, char *argv[])
{
	char *n_in,*n_gamma,*n_out;
	int opt;
	
	if(argc!=2 && argc !=3){
		printf("Error! Count of argument is not 1 or 2\n");
		_exit(-1);
	}
	
	n_gamma = NULL;
	n_in = NULL;
	/* Declare options */
	while ((opt = getopt(argc, argv, ":i:g:")) != -1) {
		switch (opt) {
		case 'i':
				n_in = optarg;
				break;
		case 'g':
				n_gamma = optarg;
				break;
		case ':':
				printf("Option need a value!\n");
				_exit(-1);
		case '?':
				printf("Undefined option!\n");
				_exit(-1);
	   }
	}
	
	if(!n_in){
		printf("Error! Don`t have input file\n");//grammar!!!!!
		_exit(-1);
	}
	
	f_in=open(n_in, O_RDONLY);
	if(!f_in){
		printf("error open\n");
		_exit(-1);
	}
	
	if(!n_gamma){
		n_gamma = strcat(n_in, ".gamma");
		f_gamma = open(n_gamma,  O_RDWR|O_CREAT, 0777);
		if (!f_gamma) {
			printf("error open\n");
			_exit(-1);
		}
		create_gamma();
	} else {
		f_gamma = open(n_gamma,  O_RDWR|O_CREAT, 0777);
		if (!f_gamma) {
			printf("error open\n");
			_exit(-1);
		}
		if (check_gamma() == NO_EQU)
			create_gamma();
	}
	
	n_out = strcat(n_in, ".result");
	f_out = open(n_out, O_WRONLY|O_CREAT, 0777);
	if (!f_out) {
		printf("error open\n");
		_exit(-1);
	}
	
	gamming();
	close(f_in);
	close(f_gamma);
	close(f_out);
	_exit(0);
}

int create_gamma()
{
	char buf[SIZE_BUF];
	unsigned int size_gamma, i, j;
	struct stat *s_in;
	if (fstat(f_in, s_in) == -1){
		printf("Error! Fstat fuction return error value!\n");
		return -1;
	}
	printf("\nA\n");
	size_gamma = s_in->st_size;
	for(i = 1; i <= size_gamma; i++) {
		for(j = 1; j <= SIZE_BUF && i <= size_gamma; j++, i++)
			buf[j-1] = rand() % 256;
		write (f_gamma, buf, j);
	}
}

int check_gamma()
{
	struct stat *s_in, *s_gamma;
	if (fstat(f_in, s_in) == -1){
		printf("Error! Fstat fuction return error value!\n");
		_exit(-1);
	}
	if (fstat(f_gamma, s_gamma) == -1){
		printf("Error! Fstat fuction return error value!\n");
		_exit(-1);
	}
	if (s_in->st_size != s_gamma->st_size)
		return NO_EQU;
	else return EQU;
}

int gamming()
{
	char buf_in[SIZE_BUF], buf_gamma[SIZE_BUF], buf_out[SIZE_BUF];
	unsigned int j, cnt_read;
	cnt_read = SIZE_BUF;
	while(cnt_read == SIZE_BUF) {
		cnt_read = 	read(f_in, buf_in, SIZE_BUF);
					read(f_gamma, buf_gamma, SIZE_BUF);
		if (!cnt_read)
			break;
		for(j = 1; j <= cnt_read; j++) {
			buf_out[j-1] = buf_in[j-1] ^ buf_gamma[j-1];
		}
		write (f_gamma, buf_gamma, cnt_read);
	}
	return 0;
}



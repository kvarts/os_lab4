#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>

#define SIZE_BUF 4096

#define EQU 1
#define NO_EQU 0
int f_in, f_out, f_gamma;
char n_in[255], n_gamma[255], n_out[255];
int main(int argc, char *argv[])
{
	
	int opt;
	
	if(argc!=2 && argc !=3){
		printf("Error! Count of argument is not 1 or 2\n");
		_exit(-1);
	}
	
	/* Declare options */
	while ((opt = getopt(argc, argv, ":i:g:")) != -1) {
		switch (opt) {
		case 'i':
				strcpy(n_in, optarg);
				break;
		case 'g':
				strcpy(n_gamma, optarg);
				break;
		case ':':
				printf("Option need a value!\n");
				_exit(-1);
		case '?':
				printf("Undefined option!\n");
				_exit(-1);
	   }
	}
	/*
	strcpy(n_in, argv[1]);
	if(argc == 3)
		strcpy(n_gamma, argv[2]);
	*/
	if(!strlen(n_in)){
		printf("Error! Don`t have input file\n");//grammar!!!!!
		_exit(-1);
	}
	
	f_in=open(n_in, O_RDONLY);
	if(!f_in){
		printf("error open\n");
		_exit(-1);
	}
	
	if(!strlen(n_gamma)){
		strcpy(n_gamma, n_in);
		strcat(n_gamma, ".gamma");
		f_gamma = open(n_gamma, O_CREAT, 0777);
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
	strcpy(n_out, n_in);
	strcat(n_out, ".result");
	f_out = open(n_out, O_CREAT, 0777);
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
	struct stat s_in;
	if (stat(n_in, &s_in) == -1){
		printf("name = %s\n", n_in);
		printf("Error! Fstat fuction return error value!\n");
		printf("Comment: %s\n",strerror(errno));
		return -1;
	}
	size_gamma = s_in.st_size;
	for(i = 1; i <= size_gamma;) {
		for(j = 1; j <= SIZE_BUF && i <= size_gamma; j++, i++)
			buf[j-1] = rand() % 256;
		write (f_gamma, buf, j-1);
	}
}

int check_gamma()
{
	struct stat s_in, s_gamma;
	if (stat(n_in, &s_in) == -1){
		printf("Error! Fstat fuction return error value!\n");
		printf("Comment: %s\n",strerror(errno));	
		_exit(-1);
	}
	if (stat(n_gamma, &s_gamma) == -1){
		printf("Error! Fstat fuction return error value!\n");
		printf("Comment: %s\n",strerror(errno));
		_exit(-1);
	}
	if (s_in.st_size != s_gamma.st_size)
		return NO_EQU;
	else return EQU;
}

int gamming()
{
	char buf_in[SIZE_BUF], buf_gamma[SIZE_BUF], buf_out[SIZE_BUF];
	unsigned int i, cnt_read;
	cnt_read = SIZE_BUF;
	while(cnt_read == SIZE_BUF) {
		cnt_read = 	read(f_in, buf_in, SIZE_BUF);
					read(f_gamma, buf_gamma, SIZE_BUF);
		if (!cnt_read)
			break;
		printf("in:\n\t%s\n", buf_in);
		printf("gamma:\n\t%s\n", buf_gamma);
		
		for(i = 0; i < cnt_read; i++) {
			buf_out[i] = buf_in[i] ^ buf_gamma[i];
		}
		printf("out:\n\t%s\n", buf_out);
		write (f_out, buf_out, cnt_read);
	}
	return 0;
}



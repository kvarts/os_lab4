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

	strcpy(n_in, argv[1]);
	if(!(f_in = open(n_in, O_RDONLY)))
		error_exit(errno);
		
	if (argc == 2) {
		strcpy(n_gamma, n_in);
		strcat(n_gamma, "_gamma");
	}
	else
		strcpy(n_gamma, argv[2]);

	if (!(f_gamma = open(n_gamma,  O_RDWR|O_CREAT, 0777)))
		error_exit(errno);
	if (check_gamma() == NO_EQU){
		printf("create new gamma!\n");
		create_gamma();
	}

	strcpy(n_out, n_in);
	strcat(n_out, "_result");
	if (!(f_out = open(n_out, O_RDWR|O_CREAT, 0777)))
		error_exit(errno);

	gamming();
	close(f_in);
	close(f_gamma);
	close(f_out);
	_exit(0);
}

int create_gamma()
{
	char buf[SIZE_BUF];
	unsigned int size_gamma, j;
	struct stat s_in;
	if (stat(n_in, &s_in) == -1)
		error_exit(errno);

	size_gamma = s_in.st_size;
	while(size_gamma) {
		for(j = 0; j < SIZE_BUF && size_gamma; j++){
			buf[j] = rand() % 256;
			size_gamma--;
		}
		if (write(f_gamma, buf, j) == -1)
			error_exit(errno);
	}
	lseek(f_gamma, 0, SEEK_SET);
	return 0;
}

int check_gamma()
{ 
	struct stat s_in, s_gamma;
	if (stat(n_in, &s_in) == -1 || stat(n_gamma, &s_gamma) == -1) {
		error_exit(errno);
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
		printf("in:\t\t%s\n", buf_in);		
		for(i = 0; i < cnt_read; i++) {
			buf_out[i] = buf_in[i] ^ buf_gamma[i];
		}
		printf("out:\t\t%s\n", buf_out);
		write (f_out, buf_out, cnt_read);
	}
	return 0;
}

int error_exit(int id_error)
{
	printf("Error! Comment: %s\n",strerror(errno));
	_exit(-1);
}


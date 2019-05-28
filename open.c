#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

static const char * const WRITE_FILE = "vivek is great\n";

int main(int argc, char *argv[])
{
		int fd,fd1;
		if(argc!=2)
		{
	  		printf("Usage : ./open <file name>\n");
	    	exit(1);
		}
		pid_t pid = getpid();
		//printf("\npid is %d\n",pid);
		fd = open(argv[1],O_WRONLY|O_APPEND,S_IRWXU);
		//fd1 = open(argv[2],O_WRONLY|O_APPEND,S_IRWXU);
		//printf("File Descriptor used is %d\n", fd);
		write(fd, WRITE_FILE, strlen(WRITE_FILE));
		//write(fd1, WRITE_FILE, strlen(WRITE_FILE));
		return 0;
}

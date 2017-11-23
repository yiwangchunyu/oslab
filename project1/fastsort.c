#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "sort.h"
#define MAXLINE 10000
void usage(char *prog)
{
	fprintf(stderr, "Usage: %s\n", prog);
	exit(1);
}
int cmp ( const void *a , const void *b )
{
	rec_t* aa = (rec_t*)a;
	rec_t* bb = (rec_t*)b;
	return aa->key-bb->key;

}
int main(int argc, char *argv[])
{
    // arguments
    char *inFile = "/no/such/file";
    char *outFile = "/no/such/file";
    if(argc!=3)
    {
        usage("fastsort inputfile outputfile");
    }
    // input params
    inFile = strdup(argv[1]);
    outFile = strdup(argv[2]);

// open and create output file
    int fd = open(inFile, O_RDONLY);
    if (fd < 0) {
		fprintf(stderr, "Error: Cannot open file %s\n", inFile);
		exit(1);
    }

    long int line;
    struct stat buf;
    stat(inFile, &buf);
    line = buf.st_size/100;
    if(line>MAXLINE)
    {
        fprintf(stderr, "Error: beyong maximum filelength in %s\n", inFile);
        exit(1);
    }

    rec_t r[MAXLINE];
    int i=0;
    while (1) {
        int rc;
        rc = read(fd, &r[i], sizeof(rec_t));
        if (rc == 0) // 0 indicates EOF
            break;
        if (rc < 0) {
            fprintf(stderr, "Error: something wrong with reading file from %s\n", inFile);
            exit(1);
        }
        i++;
    }



    qsort(r,line,sizeof(rec_t),cmp);

    (void) close(fd);

    //delete all data in outputfile
    int fd1 = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
    if (fd1 < 0) {
		fprintf(stderr, "Error: Cannot open file %s\n", outFile);
		exit(1);
    }
    (void) close(fd1);

    //write data into outputfile
    fd1 = open(outFile, O_WRONLY|O_CREAT|O_APPEND, S_IRWXU);
    if (fd1 < 0) {
		fprintf(stderr, "Error: Cannot open file %s\n", outFile);
		exit(1);
    }
    for(i=0;i<line;i++)
    {
        int rc = write(fd1, &r[i], sizeof(rec_t));
        if (rc != sizeof(rec_t)) {
			fprintf(stderr, "Error: Something wrong with writing file from %s\n", outFile);
			exit(1);
        }
    }
    (void) close(fd1);
    return 0;
}

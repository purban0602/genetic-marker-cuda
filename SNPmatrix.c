#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

double myclock();


void main(int argc, char **argv) {
	
	int numAnimals, numSNPs = 0;
	int i, j, k;

	FILE *fd;
	
	int maxlines = atoi(argv[1]);

	char **line;

	struct rusage r_usage;

	line = (char **) malloc(maxlines * sizeof(char*));

	for (i = 0; i < maxlines; i++) {
		line[i] = malloc(
	}
	


}

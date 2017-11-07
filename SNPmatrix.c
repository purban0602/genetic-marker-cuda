#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

double myclock();


void main(int argc, char **argv) {
	
	int numAnimals, numSNPs = 0;
	int i, j, k, err;

	FILE *fd;
	
	int maxlines = atoi(argv[1]);
	int nlines;
	
	int *ksuID;
	int *sampleID;
	int *genotypeAB;

	char **line;

	struct rusage r_usage;

	//allocate memory for each line
	ksuID = (int*) malloc(maxlines * sizeof(int*));
	sampleID = (int*) malloc(maxlines * sizeof(int*));
	genotypeAB = (int*) malloc(maxlines * sizeof(int*));

	//open raw data file (KSUid, SampleID, genotypeAB)
	fd = fopen("./rawdata.txt","r");
	
	//Only need the unique animals and SNPs
	if (fd != NULL) {
		nlines = -1;
		do {
			err = fscanf(fd, "%d\t%d\t%d\n",ksuID[nlines++], sampleID[nlines], genotypeAB[nlines]);		
		} while(err != EOF && nlines < maxlines);
		fclose(fd);
	}
	


}

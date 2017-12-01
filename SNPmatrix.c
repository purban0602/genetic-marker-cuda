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
	char tempLine[50];
	int tempKSU;
	int tempSample;
	int tempGeno;

	struct rusage r_usage;
	printf("Allocating memory...\n");
	fflush(stdout);
	//allocate memory for each line
	
	ksuID = (int*) malloc(maxlines * sizeof(int*));
	sampleID = (int*) malloc(maxlines * sizeof(int*));
	genotypeAB = (int*) malloc(maxlines * sizeof(int*));

	//assume input file is already 3 columns needed for data matrix
	/*

	//Create "rawdata" file from "FinalReport" file.
	//open raw data file (KSUid, SampleID, genotypeAB)
	printf("Opening FinalReport_Truncated.txt...\n");
	fflush(stdout);

	fd = fopen("./FinalReport_Truncated.txt","r");
	
	printf("Opened FinalReport_Truncated.txt.\nRemoving header...\n");
	fflush(stdout);
	
	if (fd != NULL) {
		while (strcmp("Y",tempLine) != 0) {
			err = fscanf(fd, "%s", tempLine);	
			printf("Current string is %s.\n", tempLine);

		}
		printf("Header removed.\nConverting data...\n");

		nlines = -1;
		do {
			err = fscanf(fd, "%s\t%d\t%c\t%c\t%c\t%c\t%c\t%c\t%f\t%f\t%f\n", tempLine);
			printf("Current string is %s.\n", tempLine);
			nlines++;
		} while(err != EOF && nlines < maxlines);
		fclose(fd);
	}
	else {
		printf("Error: File Not Found.\n");
	}
	*/
	
	//Only need the unique animals and SNPs
	fd = fopen("./rawdata.txt", "r");
	printf("Opened rawdata.txt.\n");	
	if (fd != NULL) {
		nlines = 0;
		do {
			err = fscanf(fd, "%d", &ksuID[nlines]);		
			err = fscanf(fd, "%d", &sampleID[nlines]);		
			err = fscanf(fd, "%d", &genotypeAB[nlines]);

			printf("%d %d %d\n", ksuID[nlines], sampleID[nlines], genotypeAB[nlines]);
			nlines++;
		} while(err != EOF && nlines < maxlines);
		fclose(fd);
	}
	printf("File read successfully.\nWriting testout.txt...\n");
	fflush(stdout);
		
	fd = fopen("./testout.txt", "a");
	for (i = 0; i < nlines; i++) {
		fprintf(fd, 
	}
	
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>

double myclock();
int err;

/* output matrix: ksu ids in columns, sample ids in rows, genotypeAB everywhere else
 * matrix looks like this...
 * 	0	1	2	3	...
 * 	737	2	1	2	...
 *	926	2	1	3	...
 *	948	3	1	1	...
 *	...	...	...	...
 */	



int** createMatrix(int lines, int **matrix, int *ksu, int *sample, int *AB) {

	//initialize matrix
	int i, j;
	int nRows;
	int nColumns = 2;
	
	for (i = 1; i < lines; i++) {
		if(ksu[i] != ksu[i-1]) {
			nColumns++;
		}
	}
	if(lines%nColumns) nRows = lines/nColumns + 2;
	else nRows = lines/nColumns + 1;

	printf("nRows = %d\nnColumns = %d\n", nRows, nColumns);
	
	//malloc matrix
	matrix = (int**) malloc(nRows * sizeof(int*));
	for(i = 0; i < nRows; i++) matrix[i] = malloc(nColumns * sizeof(int));
	
	printf("Size of matrix: %zu\n", sizeof(matrix));	
	
	return matrix;
}

//reads input file, returns the number of lines in file
int readData(FILE *f, int lines, int max, int *ksu, int *sample, int *AB) {

	int i, j;

	//Only need the unique animals and SNPs
	f = fopen("./rawdata.txt", "r");
	printf("Opened rawdata.txt.\n");	
	if (f != NULL) {
		lines = 0;
		do {
			err = fscanf(f, "%d", &ksu[lines]);		
			err = fscanf(f, "%d", &sample[lines]);		
			err = fscanf(f, "%d", &AB[lines]);

			printf("%d %d %d\n", ksu[lines], sample[lines], AB[lines]);
			lines++;
		} while(err != EOF && lines < max);
		fclose(f);
	}
	return lines;
}


void main(int argc, char **argv) {
	
	int numAnimals, numSNPs = 0;
	int i, j;

	FILE *fd;
	
	int maxlines = atoi(argv[1]);
	int nlines;
	int nColumns;
	int nRows;
	int matrixN;

	int *ksuID;
	int *sampleID;
	int *genotypeAB;
	int **matrixAB;

	char **line;
	char tempLine[50];

	struct rusage r_usage;
	printf("Allocating memory...\n");
	fflush(stdout);
	//allocate memory for each line
	
	ksuID = (int*) malloc(maxlines * sizeof(int*));
	sampleID = (int*) malloc(maxlines * sizeof(int*));
	genotypeAB = (int*) malloc(maxlines * sizeof(int*));

	//assume input file is already 3 columns needed for data matrix
	
	//try to read data file
	nlines = readData(fd, nlines, maxlines, ksuID, sampleID, genotypeAB);

	printf("File read successfully.\nnlines = %d\nWriting matrix...\n", nlines);

	//initialize and create matrix...
	matrixAB = createMatrix(nlines, matrixAB, ksuID, sampleID, genotypeAB);

	/* output matrix: ksu ids in columns, sample ids in rows, genotypeAB everywhere else
	 * matrix looks like this...
	 * 	0	1	2	3	...
	 * 	737	2	1	2	...
	 *	926	2	1	3	...
	 *	948	3	1	1	...
	 *	...	...	...	...
	 */	
	
	
	/*
	fd = fopen("./testmatrix.txt", "w");

	//create first row
	fprintf(fd, "0\t");
	fprintf(fd, "%d\t",ksuID[0]);
	nColumns = 1;
	
	for (i = 1; i < nlines-1; i++) {
		if (ksuID[i] != ksuID[i-1]) {
			fprintf(fd, "%d\t",ksuID[i]);
			nColumns++;
		}
	}

	//avoids an extra \t at the end of the row
	if (ksuID[nlines-1] != ksuID[nlines-2]) {
	       fprintf(fd, "%d",ksuID[i]);  
	       nColumns++;
	}

	fprintf(fd, "\n");	
	printf("nColumns = %d\n", nColumns);
	
	if(nlines%nColumns) nRows = (nlines / nColumns) + 1;
	else nRows = nlines/nColumns;
	printf("nRows = %d\n", nRows);
	printf("nlines = %d\n", nlines);

	//create all other rows
	matrixN = 0;
	for (i = 0; i < nRows; i++) {
		//write first column (sampleID)
		fprintf(fd, "%d\t", sampleID[i]);

		//write all other columns
		for (j = 0; j < nColumns-1; j++) {
			fprintf(fd, "%d\t", genotypeAB[i + (j*nRows)]);
			matrixN++;
		}
		fprintf(fd, "%d\n", genotypeAB[i + ((nColumns - 1)*nRows)]); 
		
	}
	*/
	
}

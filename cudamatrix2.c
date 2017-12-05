#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>

double myclock();

void createMatrix(int* matrix, int rows, int columns, int *ksu) {
	int i, j;
	//fprintf(fd, "0\t");
	matrix[0] = 0;

/*
	//fprintf(fd, "%d\t",ksuID[0]);

	for (i = 1; i < columns; i++) {
		matrix[i] = ksu[i];
	}

	//avoids an extra \t at the end of the row
	if (ksu[lines-1] != ksu[lines-2]) {
	       fprintf(fd, "%d",ksu[i]);  
	       columns++;
	}

	fprintf(fd, "\n");	
	printf("nColumns = %d\n", columns);
	
	
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

int getColumns(int *ksu, int lines, int *ksuCompact) {
	
	int i;
	int columns = 1;
	
	for (i = 1; i < lines-1; i++) {
		if (ksu[i] != ksu[i-1]) {
			//fprintf(fd, "%d\t",ksuID[i]);
			columns++;
		}
	}

	//avoids an extra \t at the end of the row
	if (ksu[lines-1] != ksu[lines-2]) {
	       //fprintf(fd, "%d",ksuID[i]);  
	       columns++;
	}

	//fprintf(fd, "\n");	
	
	//create compact array (unique KSUID's)
	return columns;
}

int getRows(int lines, int columns, int *sample, int *sampleCompact) {

	int i, j;
	//int rows = 1;
	int exists;
	int compactIndex = 0;
	//if(lines%columns) rows = (lines / columns) + 1;
	//else rows = lines/columns;

	//create compact array (unique sampleID's)
	for(i = 0; i < lines; i++) {
		exists = 0;
		for(j = 0; j < lines; j++) {
			printf("Checking i = %d and j = %d\n", i, j);
			if (sample[i] == sample[j] && i != j) {
				exists = 1;
			}
			printf("exists = %d\n\n", exists);
		}
		if (!exists) {
			sampleCompact[compactIndex] = sample[i];
			compactIndex++;
		}
	}

	return compactIndex;

}

void main(int argc, char **argv) {
	
	int numAnimals, numSNPs = 0;
	int i, j, k, err;

	FILE *fd;
	
	int maxlines = atoi(argv[1]);
	int nlines;

	//nColumns and nRows refers to the number of columns and rows of DATA not total in matrix.
	int nColumns;
	int nRows;
	int matrixN;

	int* matrixAB;

	int *ksuID;
	int *sampleID;
	int *genotypeAB;

	int *ksuIDCompact;
	int *sampleIDCompact;

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
	printf("File read successfully.\nWriting matrix...\n");
	fflush(stdout);
	
	/* output matrix: ksu ids in columns, sample ids in rows, genotypeAB everywhere else
	 * matrix looks like this...
	 * 	0	1	2	3	...
	 * 	737	2	1	2	...
	 *	926	2	1	3	...
	 *	948	3	1	1	...
	 *	...	...	...	...
	 */	

	//get number of columns
	nColumns = getColumns(ksuID, nlines, ksuIDCompact);
	printf("nColumns = %d\n", nColumns);
	//get number of rows
	nRows = getRows(nlines, nColumns, sampleID, sampleIDCompact);
	printf("nRows = %d\n", nRows);

	//Write matrix to file
	fd = fopen("./SNPmatrix.txt", "w");

	//create first row
	/*
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
	*/

	//printf("nRows = %d\n", nRows);
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
	
}

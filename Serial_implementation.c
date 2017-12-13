#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

#define NTOKENS 10

typedef struct {
	char *name;
	int animal;
	char ab1;
	char ab2;
	float pos;
	float c_pos;
}SNP;

int read_genotypes(char**, int);
SNP parse(char *);
void convert (SNP *, int, int);
void write_snp(SNP*, int);
void quicksort(SNP*, int, int);
int partition(SNP*, int, int);
void swap(SNP*, int, int);
void scan (SNP *, int);

int main(int argc, char** argv){
	char  **string_array;
	SNP *snp_array;
	int num_animals, num_snps;
	int i;
	int max_lines;
	int num_lines = -1;
	
	if(argc != 3 ){

		printf("useage: ./Convert <number of animals> <number of snps>\n");
		
	}
	
	num_animals = atoi(argv[1]);
	num_snps = atoi(argv[2]);
	max_lines = num_animals * num_snps;
	
	// Allocate for arrays
	string_array = (char**) malloc( max_lines * sizeof(char*));
	snp_array = malloc( max_lines * sizeof(SNP));
	
	for(i = 0; i < max_lines; i++){
		string_array[i] = (char*)malloc(100); 	
	}
	
	
	num_lines = read_genotypes(string_array, max_lines);
	
	// Read into SNP struct
	for(i = 0; i < num_lines; i++) snp_array[i] = parse(string_array[i]);
	
	free(string_array);
	
	for(i = 0; i < num_lines; i += num_snps)
		quicksort(snp_array, i, i + num_snps - 1);
	
	
	convert(snp_array, num_animals, num_snps);

	write_snp(snp_array, num_snps);

	return 0;
}

int read_genotypes(char **string_array, int max_lines){
	FILE *fd;
	int err;
	int num_lines = -1;
	
	fd = fopen("/homes/dwg1092/project/Genotypes6.txt", "r");
	
	do {
		err = fscanf(fd, "%[^\n]\n", string_array[++num_lines]);
	} while(err != EOF && num_lines < max_lines);
	
	fclose(fd);
	
	return num_lines;
}
SNP parse(char *string){
	
	//const int NTOKENS = 5;
	char *tokens [NTOKENS];
	int i = 0;
	SNP new_snp;

	tokens[i] = strtok(string, " ");
	while(tokens[i] != NULL){
		i++;
		tokens[i] = strtok(NULL, " ");
	}
	
	new_snp.name = tokens[0];
	new_snp.animal = atoi(tokens[1]);
	new_snp.ab1 = *tokens[2];
	new_snp.ab2 = *tokens[3];
	new_snp.pos = atof(tokens[4]);
	
	return new_snp;
}
void convert (SNP *snp_array, int num_animals, int num_snps){
		
	int value;
	FILE *fd;
	int j, i, k;
	
	fd = fopen("SNPmatrix.txt", "w");
	
	k = 0;
	
	for(i = 0; i <= num_snps; i++) fprintf(fd, "%d ", i);
	
	fprintf(fd, "\n");
	
	//Write out matrix
	for(i = 0; i < num_animals; i++){
		fprintf(fd, "%d ", snp_array[k].animal);
		for(j = 0; j < num_snps; j++){
			switch (snp_array[k].ab1){
				case 'A':
					switch(snp_array[k].ab2){
						case 'A':
							value = 1;
							break;
						case 'B':
							value = 3;
							break;
					}
					break;
				case 'B':
					switch(snp_array[k].ab2){
						case 'A':
							value = 3;
							break;
						case 'B':
							value = 2;
							break;
					}
					break;
				default: 
					value = 0;
					break;
			}
			fprintf(fd, "%d, ", value);
			k++;
		}
		fprintf(fd, "\n");
	}
	
			
	fclose(fd);
	return;
		
}
void write_snp(SNP *snp_array, int num_snps){

	FILE *fd;
	int i;

	fd = fopen("SNP_key.txt", "w");
	
	for(i = 0; i < num_snps; i++){
		fprintf(fd, "%d,%s,%f,%f\n", i+1, snp_array[i].name, snp_array[i].pos, snp_array[i].c_pos); 

	}
	fclose(fd);	
	return;
}
void quicksort(SNP *snp_array, int start, int end){
	int pivot;
	
	if(start >= end) return;
	
	pivot = partition(snp_array, start, end);
	
	quicksort(snp_array, start, pivot-1);
	quicksort(snp_array, pivot+1, end);
		
}
int partition(SNP* snp_array, int start, int end){
	int i;
	SNP pivot = snp_array[end];
	int pIndex = start;
	
	for(i = start; i < end; i++){
		if(snp_array[i].pos <= pivot.pos)
		{
			swap(snp_array, i, pIndex);
			pIndex++;
		}
	}
	if(pIndex != end) swap(snp_array, pIndex, end);
	
	return pIndex;
}
void swap(SNP* snp_array, int a, int b){
	
	SNP temp = snp_array[a];
	snp_array[a] = snp_array[b];
	snp_array[b] = temp;	
}
void scan (SNP *snp_array, int num_snps){
	int i;
	for(i = 1; i < num_snps; i++){
		snp_array[i].c_pos = snp_array[i-1].c_pos + snp_array[i].pos;
	}
	
}
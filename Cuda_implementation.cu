#include <cuda_runtime.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

#define CHECK(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}																			


typedef struct{
	char* name;
	char* chrom_c;
	//int* chrom;
	long* pos;
	long* c_pos;
	//char** rest;
}SNP;

typedef struct{
	char* snp_name;
	int* a_id; //length is the number of animals
	char* ab1; 
	char* ab2;
	int* ab;
}Sample;

int NSNPS;
int NSAMPLES;

void read_files(char* map_path, char* snp_path, char** data_string, char** snps_data){
	FILE *fd;
	int err;
	int num_lines = -1;
	char** header_array;
	int i;
	
/***********************Allocate string for header info**********/
	header_array = (char**) malloc( 10 * sizeof(char*));
	
	for(i = 0; i < 10; i++){
		header_array[i] = (char*)malloc(100); 	
	}
/*****************************************************************/
	
	fd = fopen("/homes/d/dwg1092/project/FinalReport_Truncated32.txt", "r");
	
	
/*******Getting number of SNP and Sample from header****/
	do {
		err = fscanf(fd, "%[^\n]\n", header_array[++num_lines]);
	} while(err != EOF && num_lines < 10);
	
	
	err = sscanf(header_array[5], "Total SNP	%d", &NSNPS);
	err = sscanf(header_array[7], "Total Sample	%d", &NSAMPLES);
/***********************************************************/
	

	
/*************Getting Final Report Data***********************************/
	//char** data_string;
	
	data_string = (char**) malloc(NSNPS * NSAMPLES * sizeof(char*));
	for(i = 0; i < NSNPS*NSAMPLES; i++){
		data_string[i] = (char*)malloc(100); 	
	}
	
	num_lines  = -1;
	do {
		err = fscanf(fd, "%[^\n]\n", data_string[++num_lines]);
	} while(err != EOF && num_lines < NSNPS*NSAMPLES);
	
	fclose(fd);
/**************************************************************************/

	
/************************Getting MapFile Data******************************/
	//char** snps_data;
	char* junk = (char*) malloc(50 * sizeof(char));
	
	snps_data = (char**) malloc(NSNPS * sizeof(char*));
	for(i = 0; i < NSNPS; i++){
		snps_data[i] = (char*)malloc(100); 	
	}
	
	fd = fopen("/homes/d/dwg1092/project/SNP_Map_Truncated32.txt", "r");
	
	int num_lines2 = -1;
	err = fscanf(fd, "%[^\n]\n", junk);
	do {
		err = fscanf(fd, "%[^\n]\n", snps_data[++num_lines2]);
	} while(err != EOF && num_lines2 < NSNPS);
	
	free(junk);
	
	fclose(fd);
/**************************************************************************/
	

}
__device__ long scan(long* x){
	
	int i = threadIdx.x;
	int n = blockDim.x;
	int offset;
	
	for ( offset = 1; offset < n; offset *= 2){
		long temp;
		if (i >= offset)
			temp = x[i-offset];
		
		__syncthreads();
		
		if(i >= offset)
			x[i] = temp + x[i];
		
		__syncthreads();
	}
	
	return x[i];
}

/*************functions for the radix sort**********************************/
__device__ void sort_by_bit(SNP* snps, Sample* samples, int bit){
	
		int i = threadIdx.x;
		int size = blockDim.x;
		int index;
		
		/***temperary variables for the snps*****/
		long t_pos = snps->pos[i];
		char t_name = snps->name[i * 50];
		char t_chrom_c = snps->chrom_c[i];
		//char* t_rest = snps->rest[i];
		Sample t_sample = samples[i];
		
		int p_i = (t_pos >> bit) & 1;
		
		snps->pos[i] = p_i;
		
		__syncthreads();
		
		int ones_before = scan(snps->pos);
		int ones_total = snps->pos[size -1];
		int zeros_total = size - ones_total;
		
		__syncthreads();
		
		if(p_i)
			index = ones_before - 1 + zeros_total;
		else
			index = i - ones_before;
		
		snps->pos[index] = t_pos;
		snps->name[index] = t_name;
		snps->chrom_c[index] = t_chrom_c;
		//snps->rest[index] = t_rest;
		samples[index] = t_sample;
}

__device__ void radixsort(SNP* snps, Sample* samples){
	
	for(int i = 0; i < 64; i++){
		sort_by_bit(snps, samples, i);
		__syncthreads();
	}
	
}
/**************************************************************************/



void parse(SNP* snps, Sample* animals, char** data_string, char** snp_data){
	
	int i, j, err;
	
	for (i = 0; i < NSNPS; i++){
		err = sscanf(snp_data[i], "%*d	%s	%c	%ld	%*s", 
					  &(snps->name[i * 50]) , &(snps->chrom_c[i]), &(snps->pos[i]));
	}
	
	for(i = 0; i < NSNPS; i++){
		for(j = 0; j < NSAMPLES; j++)
			err = sscanf(data_string[i], "%s/t%d/t%*c/t%*c/t%*c/t%*c/t%c/t%c/t%*s", 
							animals[i].snp_name, &(animals[i].a_id[j]), &(animals[i].ab1[j]), &(animals[i].ab2[j]));
	}
}

__global__ void sort(SNP* snps, Sample* samples, int nsamples){
	
	int id = threadIdx.x;
	radixsort(snps, samples);
	
	for(int i = 0; i < nsamples; i++){
		if (samples[id].ab1[i] == 'A' && samples[id].ab2[i] == 'A'){
			samples[id].ab[i] = 1;
		}else if(samples[id].ab1[i] == 'B' && samples[id].ab2[i] == 'B'){
			samples[id].ab[i] = 2;
		}else{
			samples[id].ab[i] = 3;
		}
	}
}

__global__ void test(SNP* snps){
	
	int id = threadIdx.x;
	snps->c_pos[id] = scan(snps->pos);
	
}
int main(int argc, char** argv){
	
	SNP snps;
	Sample* samples;
	char* map_path;
	char* snp_path;
	char** data_string; 
	char** snps_data;
	int i, j;
	
	
	//map_path = argv[1];
	//snp_path = argv[2];
	
	read_files(map_path, snp_path, data_string, snps_data);
	
	
	CHECK(cudaMallocManaged((void **)&(snps.name), NSNPS * 50 * sizeof(char)));
	CHECK(cudaMallocManaged((void**)&(snps.chrom_c), NSNPS * sizeof(char)));
	CHECK(cudaMallocManaged((void**)&(snps.pos), NSNPS * sizeof(long)));
	CHECK(cudaMallocManaged((void **)&(snps.c_pos), NSNPS * sizeof(long)));
	
	
	samples = (Sample*) malloc(NSNPS * sizeof(Sample));
	
	for(i = 0; i < NSNPS; i++){
		samples[i].snp_name = (char*) malloc(50 * sizeof(char));
		samples[i].a_id = (int*) malloc(NSAMPLES * sizeof(int));
		samples[i].ab1 = (char*) malloc(NSAMPLES * sizeof(char));
		samples[i].ab2 = (char*) malloc(NSAMPLES * sizeof(char));
		samples[i].ab = (int*) malloc(NSAMPLES * sizeof(char));
	}
	
	/*CHECK(cudaMallocManaged((void**) &samples, NSNPS * sizeof(Sample)));
	
	for(i = 0; i < NSNPS; i++){
		CHECK(cudaMallocManaged((void **)&(samples[i].snp_name), 50 * sizeof(char)));
		CHECK(cudaMallocManaged((void **)&(samples[i].a_id), NSAMPLES * sizeof(int)));
		CHECK(cudaMallocManaged((void **)&(samples[i].ab1), NSAMPLES * sizeof(char)));
		CHECK(cudaMallocManaged((void **)&(samples[i].ab2), NSAMPLES * sizeof(char)));
		CHECK(cudaMallocManaged((void **)&(samples[i].ab), NSAMPLES * sizeof(char)));
	}*/
	
	
	parse(&snps, samples, data_string, snps_data);
	
	for(i = 0; i < NSNPS; i++)
		for(j = 0; j < NSAMPLES; j++)
		printf("%s	%d	%c	%c	%c\n", samples[i].snp_name, samples[i].a_id[j], samples[i].ab1[j], samples[i].ab2[j], samples[i].ab[j]);
	
	for(i = 0; i < NSNPS; i++)
		printf("%c	%ld	%ld", snps.chrom_c[i], snps.pos[i], snps.c_pos[i]);
	
	
	/*free(data_string);
	free(snps_data);*/
	
	test<<<1, 32>>>(&snps);
	cudaDeviceSynchronize();
	
	
}
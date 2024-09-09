	#include<stdio.h>
	#include<stdlib.h>
	#include<math.h>
	#include<time.h>
	
	#include<x86intrin.h>
	
	
	
	#define f32 float
	#define f64 double 
	#define i32 int 
	#define i64 long long int
	#define i8 char
	
	
	
	#define ll long long
	
	
	
	int* alloc_vec( i64 n )
	{ 
		int* vec = (int*)malloc(n*sizeof(int));
		if( vec == 0 )
		{
			exit(-1);
		}
		return vec;
	}
	
	
	
	
	int* alloc_vec_aligned(i64 n,ll int alignment_size)
	{
        int* v=(int*)(malloc((n+alignment_size)*sizeof(int)));

		if (v == 0)
		{
			printf (" Not enough memory .\n");

			exit ( -1);
		}
		
		ll int m = ((long long int)v)  %  alignment_size;
		
		ll int dif = alignment_size - m;
		
		int *w = (int*)((ll int)v + dif) ;
		
		
		

        return w;

	}
	
	
	
	
	
	int* read_from_File(  char* path )
	{
		i64 filelength;
		
		FILE* file = fopen(path,"r");
		if( file == NULL){ printf("nincs ilyen file\n");exit(-1);}
		fseek(file,0,SEEK_END);	
		
		filelength = ftell(file);
		rewind(file);
		
		int* vec = alloc_vec_aligned(filelength,64);
		
		//fread(vec,1,filelength, file);
	
		int i = 0;
		
		
		while( fscanf(file,"%d", vec + i) != -1){
			
			i++;
			
		}
		fclose(file);
		
		return vec;
		
		
	}
	
	i32 tomb[256] ;
	i32 tomb2[256];
	
	
	i32 comul[256];
	
	
	
	
	
	
	int main(int argc, char** argv)
	{
		
		time_t begin, end;
		double spent;



		
		i64 length = 1280*720;
		
		
		const int SIMD_WIDTH = 16;
		
		const int hist_size = 256;
		
		const int array_length = SIMD_WIDTH * hist_size;
		
		
		int *local_hists = alloc_vec_aligned(array_length,64);
		
		
		
		int *global_hist = alloc_vec_aligned(hist_size,64);
		
		
		for(int i = 0; i < array_length ; i++ ){
			local_hists[i] = 0;
		}
		
		
		for(int i = 0; i< hist_size ; i++){
			global_hist[i] = 0;
		}
		
		
		
		char* path = argv[1];
		
		
		int* vec = read_from_File(path);
		
		//color intensity distribution

		begin = clock();
		
		for(int i = 0; i < length ; i+=1){
			tomb[vec[i]]++;
		}
		
		
		// commulation function
		
		int sum = 0;
		for(int i = 0; i<256; i++){
			sum+=tomb[i];
			tomb2[i]=sum*255/length;
		
		}
		
	
		// new contrast setting
		
		
		for(int i = 0; i < length ; i++){
			vec[i]=tomb2[vec[i]];
		}
		end = clock();
		spent = (double)(end-begin)/CLOCKS_PER_SEC;
		
		printf("time spent with scalar code %lf\n",spent);
		
		FILE* f = fopen(argv[2],"w");
		
		for(int i = 0; i< length; i++){
			fprintf(f,"%d\n",vec[i]);
		}
		
		fclose(f);
		
	
		
		
		vec = read_from_File(path);
		
		begin = clock();

	

		for(int i = 0; i< hist_size; i+=SIMD_WIDTH){
			__m512i temp = _mm512_set1_epi32(0);
			for(int j = i; j<array_length; j+=hist_size ){
				__m512i temp2 = _mm512_load_epi32(local_hists + j);
				temp = _mm512_add_epi32(temp, temp2);
			}
			_mm512_store_epi32(global_hist + i , temp);
		}





	 
		for(int i = 0; i < length ; i+=1){
			global_hist[vec[i]]++;
		}

		
		int sum2 = 0;
		for(int i = 0; i<256; i++){
			sum2+=global_hist[i];
			comul[i]=sum2*255/length;
		
		}

		
		
		
		
		for( int i = 0; i < length ; i+=SIMD_WIDTH){
			__m512i avx512 = _mm512_load_epi32(vec+i);
			__m512i new_values = _mm512_i32gather_epi32( avx512 , comul, 4 );
			_mm512_store_epi32(vec + i , new_values ) ;
		}
		
		
		end = clock();
		spent = (double)(end-begin)/CLOCKS_PER_SEC;
		
		printf("time spent with avx512 code %lf\n",spent);
		
		FILE* g = fopen(argv[3],"w");
		
		
		
		
		for(int i = 0; i< length; i++){
			fprintf(g,"%d\n",vec[i]);
		}
		
		fclose(g);
		
		
		
		
		
		
		
		
		return 0;
	}
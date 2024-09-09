gcc contrast_increase_avx512.c -O3 -o avx.exe -mavx512f

./avx.exe input_example_image.txt scalar_output_image.txt avx512_output_image.txt 
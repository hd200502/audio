 
 
SRC_FFT = wave.c fft.c  

all:test


test:$(SRC_FFT) 
	gcc $^ -o $@

.PHONY:clean
clean: 
	-rm -f *.o test

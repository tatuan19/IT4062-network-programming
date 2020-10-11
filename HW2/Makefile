TARGET = resolver
CC = gcc
CSRCS = main.c

all: $(CSRCS)
	$(CC) -o ${TARGET} $(CSRCS)


test: 	
	./${TARGET} google.com
	./${TARGET} 126.58.99.199
	./${TARGET} aznsc.test.com
	./${TARGET} 259.12.34.12
	./${TARGET} 1.2.3

run: 
	./${TARGET} 

clean: 
	rm -f ${TARGET} 

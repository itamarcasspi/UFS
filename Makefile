CC = gcc

FLAGS = -Wall -g
all: test test1

test: main.o libmyfs.so
	$(CC) -o test main.o ./libmyfs.so -lm 

test1: test1.o libmylibc.so
	$(CC) -o test1 test1.o ./libmylibc.so -lm 

main.o: main.c myfs.h
	$(CC) $(FLAGS) -c main.c

libmyfs.so: myfs.o
	$(CC) -fPIC -shared -o libmyfs.so myfs.o 

libmylibc.so: myfile.o myfs.o
	$(CC) -fPIC -shared -o libmylibc.so myfile.o myfs.o

myfs.o: myfs.h myfs.c
	$(CC) -fPIC $(FLAGS) -c myfs.c 

myfile.o: myfile.h myfs.h myfile.c
	$(CC) -fPIC $(FLAGS) -c myfile.c 


.PHONY: clean
clean:
	rm -f *.o *.a *.so test test1
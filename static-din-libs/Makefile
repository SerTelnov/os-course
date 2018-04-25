all: main libpower.so libfibonacci.so

math.o: math.c
	gcc -c math.c -o math.o

hello.o: hello.c
	gcc -c hello.c -o hello.o

libfuns.a: hello.o math.o
	ar rc libfuns.a hello.o math.o

power.o: power.c
	gcc -c -Wall power.c -o power.o

libpower.so: power.o
	gcc power.o -shared -fpic -o libpower.so

fibonacci.o: fibonacci.c
	gcc -c -Wall -Werror fibonacci.c -o fibonacci.o

libfibonacci.so: fibonacci.o
	gcc fibonacci.o -shared -fpic -o libfibonacci.so

main.o: main.c
	gcc -c main.c -o main.o

main: main.o libpower.so libfuns.a
	gcc -L. -Wl,-rpath,. main.o libfuns.a -o main -lpower -ldl

clear:
	rm -f *.o *.a *.so main
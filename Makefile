Lab4 : Lab4.o
	gcc -o Lab4 Lab4.o

Lab4.o : Lab4.c
	gcc -c -o Lab4.o Lab4.c
clean :
	rm *.o Lab4
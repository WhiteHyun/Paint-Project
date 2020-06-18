Lab4 : main.o set.o btn.o draw.o ui.o list.o
	gcc -o Lab4 main.o set.o btn.o draw.o ui.o list.o

main.o : main.c
	gcc -c -o main.o main.c

set.o : set.c
	gcc -c -o set.o set.c

btn.o : btn.c
	gcc -c -o btn.o btn.c

draw.o : draw.c
	gcc -c -o draw.o draw.c

ui.o : ui.c
	gcc -c -o ui.o ui.c

list.o : list.c
	gcc -c -o list.o list.c

clean :
	rm *.o Lab4
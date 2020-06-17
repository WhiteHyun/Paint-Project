Lab4 : set.o btn.o draw.o ui.o
	gcc -o Lab4 set.o btn.o draw.o ui.o

set.o : set.c
	gcc -c -o set.o set.c

btn.o : btn.c
	gcc -c -o btn.o btn.c

draw.o : draw.c
	gcc -c -o draw.o draw.c

ui.o : ui.c
	gcc -c -o ui.o ui.c

clean :
	rm *.o Lab4
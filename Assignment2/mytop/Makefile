ttop: mytop.o util.o
	gcc -o mytop mytop.o util.o -lncurses -g

ttop.o: mytop.c mytop.h
	gcc -c mytop.c -lncurse -g
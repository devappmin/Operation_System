myps: myps.o psutil.o
	gcc -o myps myps.o psutil.o -lncurses -g

myps.o: myps.c ps.h
	gcc -c myps.c -lncurse -g
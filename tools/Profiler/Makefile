COMPILE=gcc -g -O2 -std=c99
LIBS=-L/usr/X11R6/lib -lGL -lGLU -lglut -lm 
INC=-L/usr/X11R6/include/
OBJS=main.o

all : ui_test

ui_test : $(OBJS)
	$(COMPILE) $(OBJS) $(LIBS)
	
main.o : main.c
	$(COMPILE) $(INC) -c main.c
	
clean :
	rm *.o ui_test
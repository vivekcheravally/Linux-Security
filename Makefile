#variable declaration
cc	=	gcc
MAKE	=	make
RM	=	rm

#targets
all	:	daemon.c	open.c	preload.c
			$(cc) -o daemon	daemon.c
			$(cc) -o open open.c
			$(cc) -shared -fPIC -o preload.so preload.c -ldl -lcap
			$(MAKE)	target


#another target
target:
	./daemon
	LD_PRELOAD=$(PWD)/preload.so ./open vivek.txt

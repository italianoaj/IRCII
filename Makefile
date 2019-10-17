CC = gcc
FLAGS_TEST = -Wall -lpam -lpam_misc
FLAGS_ITALIANO = -fPIC -fno-stack-protector -c

all: tester italiano italianoMod

tester:
	$(CC) -o tester test_pam.c $(FLAGS_TEST)

italiano:
	$(CC) $(FLAGS_ITALIANO) italiano.c

italianoMod:
	sudo ld -x --shared -o /lib/x86_64-linux-gnu/security/italiano.so italiano.o

clean:
	-rm -f tester italiano.o

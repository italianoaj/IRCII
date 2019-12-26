CC = gcc
FLAGS_TEST = -Wall -lpam -lpam_misc
FLAGS_ITALIANO = -fPIC -fno-stack-protector -c
WARN = -Wall

all: tester italiano italianoMod enroll

tester:
	$(CC) -o tester test_pam.c $(FLAGS_TEST)

italiano:
	$(CC) $(FLAGS_ITALIANO) italiano.c

italianoMod:
	sudo ld -x --shared -o /lib/x86_64-linux-gnu/security/italiano.so italiano.o

enroll:
	$(CC) -o otp-enroll otp-enroll.c $(WARN)

clean:
	-rm -f tester italiano.o otp-enroll

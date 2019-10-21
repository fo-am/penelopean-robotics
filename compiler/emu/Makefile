yarnemu : main.o yarn.o
	cc -o yarnemu main.o yarn.o -lm
main.o : main.c ../../avr/libs/yarn.h
	cc -c -I ../../avr/libs/ main.c 
yarn.o : ../../avr/libs/yarn.c ../../avr/libs/yarn.h
	cc -c ../../avr/libs/yarn.c
clean :
	rm yarnemu main.o yarn.o

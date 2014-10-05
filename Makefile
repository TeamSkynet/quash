all: quash

quash.o: quash.c
	g++ -c quash.c

clean:
	rm -rf *o quash

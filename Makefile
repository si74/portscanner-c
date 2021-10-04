all:
	gcc nonblocking/portscanner.c -o portscanner
	gcc blocking/portscanner.c -o portscanner

clean:
	rm client
	rm portscanner

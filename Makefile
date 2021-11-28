all:
	gcc nonblocking/portscanner.c -o nonblock
	gcc blocking/portscanner.c -o block

clean:
	rm nonblock
	rm block

poke327: poke327.c heap.c 
	gcc -o poke327 poke327.c heap.c -lncurses
run:
	./poke327 3
clean:
	rm -rf poke327

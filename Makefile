build:
	gcc -std=c11 -o tema2 main.c funct.h
run:
	./tema2 in_file out_file
clean:
	rm -rf tema2
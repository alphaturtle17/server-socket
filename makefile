server: library files/program.cpp
	mkdir -p build/bin
	g++ files/program.cpp -c -I include -o build/bin/program.o
	g++ -g -Wall -Wextra -Wwrite-strings build/bin/program.o -o build/program build/lib/finallib.a
	echo "To execute, run ./build/program"

library: src/lib_first.c src/lib.cpp
	mkdir -p build/lib
	g++ src/lib.cpp -o build/lib/lib.o -c -I include
	gcc src/lib_first.c -o build/lib/lib_first.o -c -I include
	ar rcs build/lib/finallib.a build/lib/lib.o build/lib/lib_first.o

clean:
	rm -rf build
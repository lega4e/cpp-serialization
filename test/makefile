CFLAGS = -std=gnu++17 -c -Wall
LDFLAGS =
LIBS =





# main
build: main

run: main
	main 


re: clean build

rerun: clean run



main: main.o
	g++ $(LDFLAGS) -o main main.o $(LIBS)

main.o: main.cpp assert.hpp
	g++ $(CFLAGS) -o main.o main.cpp





# clean
clean:
	-rm *.o main





# end

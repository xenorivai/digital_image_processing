all : main

main : main.o
	g++ -std=c++17 -o $@ $<

%.o : %.cpp
	g++ -std=c++17 -c $<

clean :
	rm *.o *.png *.txt main
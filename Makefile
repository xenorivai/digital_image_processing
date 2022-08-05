all : main

main : main.o
	g++ -std=c++17 -o $@ $< -I/usr/include/python3.8 -lpython3.8

%.o : %.cpp
	g++ -std=c++17 -c $< -I/usr/include/python3.8 -lpython3.8

clean :
	rm *.o *.png *.txt main
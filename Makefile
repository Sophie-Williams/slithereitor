all:
	g++ -std=c++11 util.cpp main.cpp -o slithereitor -lSDL2

clean:
	rm -f slithereitor

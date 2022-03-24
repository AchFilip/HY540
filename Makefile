all:
	flex --outfile=flex.cpp flex.l
	g++ -std=c++0x flex.cpp
clean:
	rm *.cpp
	rm *.exe
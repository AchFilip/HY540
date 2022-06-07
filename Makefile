all: phase2
phase1:
	flex --outfile=flex.cpp flex.l
	g++ -std=c++0x flex.cpp
phase2:
	clear
	bison -v --yacc --defines --output=parser.cpp parser.y
	flex --outfile=flex.cpp flex.l
	g++ -std=c++11 -o sin flex.cpp main.cpp
	./sin.exe test.txt
clean:
	rm -f flex.cpp parser.cpp *.exe *.hpp *.output
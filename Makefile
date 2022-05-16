all: clean phase2
phase1:
	flex --outfile=flex.cpp flex.l
	g++ -std=c++0x flex.cpp
phase2:
	clear
	bison -v --yacc --defines --output=parser.cpp parser.y
	flex --outfile=flex.cpp flex.l
	g++ -std=c++11 -o calc flex.cpp parser.cpp
	./calc.exe test.txt
clean:
	rm -f *.cpp *.exe *.hpp *.output
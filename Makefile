all:mp3reader

mp3reader: mp3reader.o
	g++ -Wall -g -o mp3reader mp3reader.o -lid3 -ljsoncpp

mp3reader.o: mp3reader.cpp 
	g++ -Wall -g -c mp3reader.cpp -lid3 -ljsoncpp

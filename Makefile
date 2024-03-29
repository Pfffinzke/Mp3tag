
CC=g++
CFLAGS=-std=c++11 -g -O0

# IMPORTANT
# replace <TODO> with your installation directories
#
SFML=~/00_perso/dev/SFML
MPG123=/usr/local

INCLUDESFML=$(SFML)/include
INCLUDEMPG123=$(MPG123)/include

LIBSFML=$(SFML)/lib
LIBMPG123=$(MPG123)/lib

INCLUDES := -I$(INCLUDEMPG123) -I$(INCLUDESFML)
# for non-debug builds
LIBS     := -L$(LIBMPG123) -lmpg123 -L$(LIBSFML) -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lid3 -ljsoncpp
# for debug builds
#LIBS     := -L$(LIBMPG123) -lmpg123 -L$(LIBSFML) -lsfml-graphics-d -lsfml-window-d -lsfml-system-d -lsfml-audio-d

AUDIOEXE := VinzcPlayer

$(AUDIOEXE): Main.o
	$(CC) $(CFLAGS) $< -o $@ SoundFileReaderMp3.o $(LIBS)

Main.o: mp3reader.cpp mp3reader.hpp SoundFileReaderMp3.o
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

SoundFileReaderMp3.o: SoundFileReaderMp3.cpp SoundFileReaderMp3.hpp
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@
 
clean:
	rm $(AUDIOEXE) *.o
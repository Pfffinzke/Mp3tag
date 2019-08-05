//mp3reader.cpp
//snags the tag off the last 128 bytes of an MP3 file.

#include "MP3tag.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "json/json.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <cstring>

#include "SoundFileReaderMp3.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <cstdint>
#include <random>



using namespace std;


Json::Reader reader;
Json::Value root;
Json::StyledStreamWriter writer;

int number_song;
int played_song;
std::string filename;
int current_song;
int next_random_song;
/*function... might want it in some class?*/
int getdir (string dir, vector<string> &files)
{
    
	DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
	if ( strstr( dirp->d_name, ".mp3" )){
            	cout <<  "found a .mp3 file:" << dirp->d_name << endl;
        	files.push_back(dir+string(dirp->d_name));
}
    }
    closedir(dp);
    return 0;
}


int read_json()
{
	int fileNameLength = 1024;
	int mp3TagSize = 128;
	TAGdata tagStruct;
	char  fileName[fileNameLength+1];
	ifstream mp3File;
	char buffer[mp3TagSize+1];
	//first step : list all mp3 files in given directory
 	string dir = string("./music/");
    vector<string> files = vector<string>();

    getdir(dir,files);

	//Start loop

	//Second step : list all these files in Json

  	std::string text = "{\"ID\": \"0\",\"path\": \"James\", \"Song Name\": \"Bond\", \"artist\": \"yop\" , \"play\": 0 }\n";
  	std::ofstream outFile;
	std::cout << "before loop" << std::endl;
	outFile.open("output.json");
	
	for (unsigned int i = 0;i < files.size();i++) {

		std::cout << "Start loop" << std::endl;
 		// Parse JSON and print errors if needed
  		if(!reader.parse(text, root[i])) {
    		std::cout << reader.getFormattedErrorMessages();
			std::cout << "parse error" << std::endl;
    		exit(1);
  		} else {
    			// Read and modify the json data
				root[i]["ID"] = i;
				root[i]["path"] = files[i] ;
				
				//root[i]["path"] += files[i];
				//Third step parse ID3 tags to fill JSON	
				//stringstream s;	
				//s << "./music/" << files[i].c_str();
				//fileName = char("./music/" + files[i].c_str());
				cout << root[i]["path"] << endl;
				mp3File.open(files[i].c_str());
				buffer[mp3TagSize] = '\0';  //just a precaution
				
				if (mp3File){
					cout << "Successful open of " << files[i].c_str() << endl;
				}else{
					cerr << "***Aborting*** Could not open " << files[i].c_str() << endl;
					return 1;
				}

				//move file pointer to 128 bytes off the end of the file.
				mp3File.seekg(-(mp3TagSize),ios::end);
				
				if(mp3File.fail()){
					cerr << "Could not seek, Aborting " << endl;
					return 1;
				}
 
				//get the data
				mp3File.read(reinterpret_cast <char *>(&tagStruct),mp3TagSize);
	
				if(mp3File.fail()){
					cerr << "Could not read after seeking, Aborting " << endl;
					return 1;
				}
				cout << "Data " << tagStruct.title << endl;
				mp3File.close();

				root[i]["Song Name"] = tagStruct.title ;
    			root[i]["artist"] = tagStruct.artist;
				root[i]["play"] = 0;

    			// Write the output to a file
    			
    			writer.write(outFile, root[i]);
  			
  				}
	}
	 outFile.close();

// fourth step chose a random song to play


	return 0;
}

std::string getResourcePath(const std::string& executableDir, const std::string& resourceRelativeName) {
	// windows users may have to change "/" for "\"
	std::size_t pos = executableDir.find_last_of("/");
	std::string prefix = executableDir.substr (0, pos+1);
	return prefix + resourceRelativeName;
}

void togglePlayPause(sf::Music& music) {
	sf::SoundSource::Status musicStatus = music.getStatus();
	if(musicStatus == sf::SoundSource::Status::Paused) {
		music.play();
		std::cout << "music play" << std::endl;
	} else if(musicStatus == sf::SoundSource::Status::Playing) {
		music.pause();
		std::cout << "music pause" << std::endl;
	}
}


int NextSong(sf::Music& music) {
	music.pause();
	std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, number_song);
	next_random_song = distr(eng);

	while ((root[next_random_song]["play"]==1)&&(played_song<=number_song)){
	next_random_song = distr(eng);
	}
	current_song = next_random_song;
	filename = root[current_song]["path"].asString();

	if (!music.openFromFile(filename)) {
		std::cout << "check your file path. also only wav, flac, ogg and mp3 are supported." << std::endl;
		return EXIT_FAILURE;
	}
	music.play();
	root[current_song]["play"] = 1;
	played_song++;
	
}

void Autoplay(sf::Music& music) {

	if (music.getStatus()!=sf::Music::Status::Playing)
	NextSong(music);
	
}

int main() {
	

	

	read_json();
	number_song = root.size();
	std::cout << "number of songs: " << number_song << std::endl;



	//filename = root[distr(eng)]["path"].asString();

	std::cout << "START: " << filename << std::endl;
	// Register our custom sf::SoundFileReader for mp3 files.
	// This is the preferred way of adding support for a new audio format.
	// Other formats will be handled by their respective readers.
	sf::SoundFileFactory::registerReader<audio::SoundFileReaderMp3>();

	// Load a music to play
	sf::Music music;

	

	if (!NextSong(music)) {
		std::cout << "check your file path. also only wav, flac, ogg and mp3 are supported." << std::endl;
		return EXIT_FAILURE;
	}

	
	sf::Time song_lenght = music.getDuration();


	
	/*if (!music.openFromFile(filename)) {
		std::cout << "check your file path. also only wav, flac, ogg and mp3 are supported." << std::endl;
		return EXIT_FAILURE;
	}*/

	// Now we are sure we can play the file
	// Set up the main window

	sf::Sprite sprite;
	sf::Texture texture;
	sf::Vector2u dimensions;

	// See if we can find the image
	
		texture.loadFromFile("WWDIYJimmyTArt.png");
		sprite.setTexture(texture);
	//	dimensions = texture.getSize();

		dimensions.x = 800;
		dimensions.y = 600;
	

	// Create the main window
	sf::RenderWindow window(sf::VideoMode(dimensions.x, dimensions.y), "SFML window");

	// Start the music
	//music.play();

	// Start the game loop
	while (window.isOpen())
	{
		// Process events
		sf::Event event;
		Autoplay(music);
		sf::Time elapsed = music.getPlayingOffset();
		//std::cout << "song time. " << song_lenght.asSeconds() << "remaining time :  " << elapsed.asSeconds() << std::endl;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			} else if(event.type == sf::Event::KeyPressed) {
				// the user interface: SPACE pauses and plays, ESC quits
				switch (event.key.code) {
					case sf::Keyboard::Space:
						togglePlayPause(music);
						break;
					case sf::Keyboard::Escape:
						window.close();
						break;
					case sf::Keyboard::N:
						if (!NextSong(music)) {
		std::cout << "check your file path. also only wav, flac, ogg and mp3 are supported." << std::endl;
		return EXIT_FAILURE;
					}
						break;
					default:
						break;
				}
			}
		}
		// Clear screen
		window.clear();
		// Draw the sprite
		window.draw(sprite);
		sf::Font font;
		font.loadFromFile("Haarlem Deco.ttf");
		sf::Text Title;
		sf::Text Artist;
		sf::Text CurrentText;
		sf::String CurrentSongText = root[current_song]["Song Name"].asString();
		sf::String CurrentArtistText = root[current_song]["artist"].asString();


		Artist.setFont(font);
		Artist.setString(CurrentArtistText);
		Artist.setColor(sf::Color::White);
		Artist.setPosition(400.0f, 70.0f);
		Artist.setCharacterSize(24);
		Artist.setStyle(sf::Text::Bold);

		Title.setFont(font);
		Title.setString(CurrentSongText);
		Title.setColor(sf::Color::White);
		Title.setPosition(400.0f, 90.0f);
		Title.setCharacterSize(24);
		Title.setStyle(sf::Text::Bold);
		
		CurrentText.setFont(font);
		CurrentText.setString("Current playing song");
		CurrentText.setColor(sf::Color::Red);
		CurrentText.setPosition(400.0f, 45.0f);
		CurrentText.setCharacterSize(26);
		CurrentText.setStyle(sf::Text::Bold | sf::Text::Underlined);

		window.draw(Artist);
		window.draw(Title);
		window.draw(CurrentText);

		// Update the window
		window.display();
	}

	return EXIT_SUCCESS;
}


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


using namespace std;

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


int main()
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
	Json::Reader reader;
  	Json::Value root;
  	Json::StyledStreamWriter writer;
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


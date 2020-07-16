//
//  Sound.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/06/12.
//

#include "Sound.hpp"

void Sound::searchSoundFiles(string filePath) {
	soundFiles = specific::getAllFilesName(filePath, "mp3");
}

void Sound::playBGM() {
	int next = playingBGMnum;
	while (next == playingBGMnum) {
		next = rnd() % soundFiles.size();
	}
	playingBGMnum = next;
	playBGM(Unicode::Widen(soundFiles[playingBGMnum].file_path));
}

void Sound::playBGM(String filePath) {
	if (audio != nullptr) {
		delete(audio);
	}
	
	audio = new Audio(filePath);
	
	audio->play();
}

void Sound::playingBGM() {
	cout << audio->posSec() << "/" << audio->lengthSec() << endl;
	if (beforePlayPos > 0 && audio->posSec() == 0) {
		playBGM();
	}
	beforePlayPos = audio->posSec();
}

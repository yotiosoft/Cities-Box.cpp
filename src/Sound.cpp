//
//  Sound.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/06/12.
//

#include "Sound.hpp"

void Sound::searchSoundFiles(string filePath) {
	m_sound_files = specific::getAllFilesName(filePath, "mp3");
}

void Sound::playBGM() {
	int next = m_playing_BGM_num;
	while (next == m_playing_BGM_num) {
		next = m_rnd() % m_sound_files.size();
	}
	m_playing_BGM_num = next;
	playBGM(Unicode::Widen(m_sound_files[m_playing_BGM_num].file_path));
}

void Sound::playBGM(String filePath) {
	if (m_audio_p != nullptr) {
		delete(m_audio_p);
	}
	
	m_audio_p = new Audio(filePath);
	
	m_audio_p->play();
}

void Sound::playingBGM() {
	if (m_before_play_pos > 0 && m_audio_p->posSec() == 0) {
		playBGM();
	}
	m_before_play_pos = m_audio_p->posSec();
}

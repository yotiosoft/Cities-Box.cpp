//
//  Sound.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/06/12.
//

#include "Sound.hpp"
#if SIV3D_PLATFORM(WEB)
#include <Siv3D/SimpleHTTP.hpp>
#include <fstream>
#endif

void Sound::searchSoundFiles(string filePath) {
#if SIV3D_PLATFORM(WEB)
	// HTTP ではディレクトリ一覧を取得できないため、ビルド時の一覧を使う。
	std::ifstream manifest("./bgm-list.txt");
	string name;
	while (std::getline(manifest, name)) {
		if (!name.empty()) {
			FileStruct file;
			file.file_path = filePath + "/" + name;
			m_sound_files << file;
		}
	}
#else
	m_sound_files = specific::getAllFilesName(filePath, "mp3");
	Array<FileStruct> ogg_files = specific::getAllFilesName(filePath, "ogg");
	for (auto ogg_file : ogg_files) {
		m_sound_files << ogg_file;
	}
#endif
}

void Sound::playBGM() {
	if (m_sound_files.empty()) {
		return;
	}
	int next = m_playing_BGM_num;
	if (m_sound_files.size() == 1) {
		next = 0;
	} else {
		while (next == m_playing_BGM_num) {
			next = m_rnd() % m_sound_files.size();
		}
	}
	m_playing_BGM_num = next;
	playBGM(Unicode::Widen(m_sound_files[next].file_path));
}

void Sound::playBGM(String filePath) {
	delete m_audio_p;
	m_audio_p = nullptr;
	m_before_play_pos = 0;
#if SIV3D_PLATFORM(WEB)
	// BGM ファイルは .data に含めず、必要な曲だけ取得する。
	m_pending_bgm_path = filePath;
	m_bgm_download = SimpleHTTP::SaveAsync(filePath, filePath);
#else
	m_audio_p = new Audio(filePath);
	m_audio_p->play();
#endif
}

void Sound::playingBGM() {
#if SIV3D_PLATFORM(WEB)
	if (m_bgm_download && m_bgm_download.isReady()) {
		if (m_bgm_download.isSucceeded()) {
			m_audio_p = new Audio(m_pending_bgm_path);
			m_audio_p->play();
		}
		m_bgm_download = AsyncHTTPTask{};
		m_pending_bgm_path.clear();
	}
#endif
	if (!m_audio_p) {
		return;
	}
	if (m_before_play_pos > 0 && m_audio_p->posSec() == 0) {
		playBGM();
		return;
	}
	m_before_play_pos = m_audio_p->posSec();
}

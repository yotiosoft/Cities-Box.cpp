//
//  Sound.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/06/12.
//

#include "Sound.hpp"
#if SIV3D_PLATFORM(WEB)
#include <Siv3D/SimpleHTTP.hpp>
#include <Siv3D/FileSystem.hpp>
#include <fstream>
#include <iostream>
#endif

Sound::~Sound() {
	delete m_audio_p;
}

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
	if (m_sound_files.empty()) { std::cerr << "BGM manifest is empty\n"; }
#else
	m_sound_files = specific::getAllFilesName(filePath, "mp3");
	Array<FileStruct> ogg_files = specific::getAllFilesName(filePath, "ogg");
	for (auto ogg_file : ogg_files) {
		m_sound_files << ogg_file;
	}
#endif
}

void Sound::playBGM() {
	if (!m_bgm_enabled || m_sound_files.empty()) {
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

void Sound::playBGM(size_t index) {
	if (index >= m_sound_files.size()) {
		return;
	}
	m_playing_BGM_num = static_cast<int>(index);
	if (m_bgm_enabled) {
		playBGM(Unicode::Widen(m_sound_files[index].file_path));
	} else {
		delete m_audio_p;
		m_audio_p = nullptr;
		m_before_play_pos = 0;
#if SIV3D_PLATFORM(WEB)
		m_bgm_download = AsyncHTTPTask{};
		m_pending_bgm_path.clear();
#endif
	}
}

void Sound::setBGMEnabled(bool enabled) {
	if (m_bgm_enabled == enabled) {
		return;
	}

	m_bgm_enabled = enabled;
	if (!enabled) {
		if (m_audio_p) {
			m_audio_p->pause();
		}
		return;
	}

	if (m_audio_p) {
		m_audio_p->play();
	} else if (m_playing_BGM_num >= 0) {
		playBGM(static_cast<size_t>(m_playing_BGM_num));
	} else {
		playBGM();
	}
}

String Sound::getBGMTitle(size_t index) const {
	if (index >= m_sound_files.size()) {
		return U"";
	}

	string title = m_sound_files[index].file_path;
	const size_t slash = title.find_last_of("/\\");
	if (slash != string::npos) {
		title = title.substr(slash + 1);
	}
	const size_t dot = title.find_last_of('.');
	if (dot != string::npos) {
		title = title.substr(0, dot);
	}
	replace(title.begin(), title.end(), '_', ' ');
	return Unicode::Widen(title);
}

void Sound::playBGM(String filePath) {
	delete m_audio_p;
	m_audio_p = nullptr;
	m_before_play_pos = 0;
#if SIV3D_PLATFORM(WEB)
	// BGM ファイルは .data に含めず、必要な曲だけ取得する。
	// The HTTP URL is relative to the page, while the Emscripten FS needs
	// an absolute destination to create the parent directory reliably.
	m_pending_bgm_path = (filePath.size() >= 2 && filePath[0] == U'.' && filePath[1] == U'/')
		? filePath.substr(1) : filePath;
	m_bgm_download = SimpleHTTP::SaveAsync(filePath, m_pending_bgm_path);
#else
	m_audio_p = new Audio(filePath);
	if (m_bgm_enabled) {
		m_audio_p->play();
	}
#endif
}

void Sound::playingBGM() {
#if SIV3D_PLATFORM(WEB)
	if (m_bgm_download && m_bgm_download.isReady()) {
		const bool httpOK = m_bgm_download.getResponse().isOK();
		const bool fileOK = FileSystem::Exists(m_pending_bgm_path)
			&& FileSystem::FileSize(m_pending_bgm_path) > 0;
		if (httpOK && fileOK) {
			m_audio_p = new Audio(m_pending_bgm_path);
			if (*m_audio_p) {
				if (m_bgm_enabled) {
					m_audio_p->play();
				}
			} else {
				std::cerr << "BGM decode failed: " << m_pending_bgm_path.narrow() << "\n";
				delete m_audio_p;
				m_audio_p = nullptr;
			}
		} else {
			std::cerr << "BGM download failed: " << m_pending_bgm_path.narrow()
				<< " (HTTP OK: " << httpOK << ", file present: " << fileOK << ")\n";
		}
		m_bgm_download = AsyncHTTPTask{};
		m_pending_bgm_path.clear();
	}
#endif
	if (!m_bgm_enabled || !m_audio_p) {
		return;
	}
	if (m_before_play_pos > 0 && m_audio_p->posSec() == 0) {
		playBGM();
		return;
	}
	m_before_play_pos = m_audio_p->posSec();
}

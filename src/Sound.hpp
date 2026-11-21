//
//  Sound.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/06/12.
//

#ifndef Sound_hpp
#define Sound_hpp

#include <Specific.hpp>
#if SIV3D_PLATFORM(WEB)
#include <Siv3D/AsyncHTTPTask.hpp>
#endif

class Sound {
public:
	// filePathで指定したディレクトリ内に存在するサウンドファイルのリストの取得
	void searchSoundFiles(string filePath);
	
	// BGMの再生（ランダム）
	void playBGM();
	
	// BGMの再生（指定）
	void playBGM(String filePath);
	
	// 再生中の処理
	void playingBGM();
	
private:
	Array<FileStruct> m_sound_files;
	Audio* m_audio_p = nullptr;
	random_device m_rnd;
	int m_playing_BGM_num = -1;
	double m_before_play_pos = 0;
#if SIV3D_PLATFORM(WEB)
	AsyncHTTPTask m_bgm_download;
	String m_pending_bgm_path;
#endif
};

#endif /* Sound_hpp */

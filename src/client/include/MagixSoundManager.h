#pragma once

#include "SoundManager.h"
#include "MagixCamera.h"

#define SOUND_CLICK "click.wav"
#define SOUND_HIT "hit.wav"
#define SOUND_HEALHIT "healhit.wav"
#define SOUND_WATERWADE "waterwade.wav"
#define SOUND_RAIN1 "rain1.wav"
#define SOUND_THUNDER1 "thunder1.mp3"
#define SOUND_THUNDER2 "thunder2.mp3"
#define SOUND_SPLASH "splash.wav"
#define SOUND_RUN_GRASS "run_grass.wav"
#define SOUND_WALK_GRASS "walk_grass.wav"
#define SOUND_ROAR1 "roar1.mp3"
#define SOUND_ROAR2 "roar2.mp3"
#define SOUND_EARTHQUAKE "quake.wav"
#define SOUND_DRAW "draw.mp3"

#define MUSIC_TITLE "title.mp3"
#define MUSIC_HURRY "hurry.mp3"
#define MUSIC_BOSS1 "boss1.mp3"
#define MUSIC_FINALE "finale.mp3"
#define MUSIC_CREDITS "credits.mp3"
#define MUSIC_AMBIENT1 "ambient1.mp3"
#define MUSIC_AMBIENT2 "ambient2.mp3"
#define MUSIC_AMBIENT3 "ambient3.mp3"
#define MUSIC_AMBIENT4 "ambient4.mp3"
#define MUSIC_AMBIENT5 "ambient5.mp3"
#define MUSIC_AMBIENT6 "ambient6.mp3"

#define LOOPEDSOUND_RANGE 2250000

using namespace Ogre;

struct LoopedSound
{
	int sound;
	int channel;
	SceneNode *mNode;
	float volume;
	Real delay;
	LoopedSound()
	{
		sound = INVALID_SOUND_INDEX;
		channel = INVALID_SOUND_CHANNEL;
		mNode = 0;
		volume = 0;
		delay = 0;
	}
};

class MagixSoundManager
{
protected:
	SoundManager *mSoundMgr;
	MagixCamera *mCamera;
	int music;
	FMOD::Channel *mMusicChannel;
	bool loopMusic;
	Real musicFadeTimeout;
	int guiSound;
	FMOD::Channel *mGuiChannel;
	int ambientSound;
	FMOD::Channel *mAmbientChannel;
	float musicVolume;
	float guiVolume;
	float soundVolume;
	list<const LoopedSound>::type loopedSound;
	Real musicSilenceTimeout;
	bool isRandomPlaylist;
	vector<String>::type musicList;
public:
	MagixSoundManager();
	~MagixSoundManager();
	void initialize(MagixCamera *camera);
	void reset();
	void update(const FrameEvent &evt);
	void updateMusic(const FrameEvent &evt);
	void updateLoopedSounds(const Vector3 &camPosition, const Real &timeElapsed);
	void playMusic(const char *type, bool loop = true);
	void playMusic(int musicID, bool loop = true);
	void stopMusic(bool fadeOut = false);
	void setRandomPlaylist(bool flag);
	bool getRandomPlaylist();
	void playRandomAmbientMusic();
	void pushMusicList(const String &name);
	void resetMusicList(bool useDefault = true);
	void playGUISound(const char *type);
	void playSound(const char *type, SceneNode *node = 0);
	void playLoopedSound(const char *type, SceneNode *node, const float &volume = 1, const Real &delay = 0);
	const list<const LoopedSound>::type::iterator stopLoopedSound(const list<const LoopedSound>::type::iterator &it);
	void stopLoopedSoundByNode(SceneNode *node, const char *type = "");
	bool hasLoopedSound(const char *type, SceneNode *node = 0);
	void stopAllLoopedSounds();
	void playAmbientSound(const char *type, const float &volume = 1);
	void setAmbientSoundVolume(const float &volume);
	void stopAmbientSound();
	void setMusicVolume(const float &ratio);
	float getMusicVolume();
	void setGUISoundVolume(const float &ratio);
	float getGUISoundVolume();
	void setSoundVolume(const float &ratio);
	float getSoundVolume();
	void clearChannelsConnectedToSceneNode(SceneNode *node);
};

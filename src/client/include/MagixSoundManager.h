#ifndef __MagixSoundManager_h_
#define __MagixSoundManager_h_

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
	MagixSoundManager()
	{
		mSoundMgr = 0;
		mCamera = 0;
		music = INVALID_SOUND_INDEX;
		mMusicChannel = 0;
		loopMusic = false;
		musicFadeTimeout = 0;
		guiSound = INVALID_SOUND_INDEX;
		mGuiChannel = 0;
		ambientSound = INVALID_SOUND_INDEX;
		mAmbientChannel = 0;
		musicVolume = 1;
		guiVolume = 1;
		soundVolume = 1;
		loopedSound.clear();
		musicSilenceTimeout = 0;
		isRandomPlaylist = false;
		musicList.clear();
	}
	~MagixSoundManager()
	{
		reset();
		if(mSoundMgr)delete mSoundMgr;
	}
	void initialize(MagixCamera *camera)
	{
		mCamera = camera;
		mSoundMgr = new SoundManager();
		bool hasSound = mSoundMgr->Initialize();
		if(!hasSound)
		{
			delete mSoundMgr;
			mSoundMgr = 0;
		}
		resetMusicList();
	}
	void reset()
	{
		stopAllLoopedSounds();
	}
	void update(const FrameEvent &evt)
	{
		if(!mSoundMgr)return;

		if(mCamera->getCameraMode()==CAMERA_FREE)
		{
			updateLoopedSounds(mCamera->getCamera()->getPosition(),evt.timeSinceLastFrame);
			mSoundMgr->FrameStarted(mCamera->getCamera(),evt.timeSinceLastFrame);
		}
		else
		{
			updateLoopedSounds(mCamera->getActiveCamNode()->_getDerivedPosition(),evt.timeSinceLastFrame);
			mSoundMgr->FrameStarted(mCamera->getActiveCamNode(),evt.timeSinceLastFrame);
		}
		updateMusic(evt);
	}
	void updateMusic(const FrameEvent &evt)
	{
		if(!mMusicChannel)return;

		bool isPlaying = false;
		mMusicChannel->isPlaying(&isPlaying);

		if(musicFadeTimeout>0 && isPlaying)
		{
			musicFadeTimeout -= evt.timeSinceLastFrame;
			if(musicFadeTimeout<=0)
			{
				musicFadeTimeout = 0;
				mMusicChannel->stop();
			}
			else mMusicChannel->setVolume(musicFadeTimeout/2*musicVolume);
		}

		if(loopMusic)
		{
			if(!isPlaying)playMusic(music);
			return;
		}

		//Random playlist
		if(isRandomPlaylist && !isPlaying)
		{
			if(musicSilenceTimeout==0)
			{
				musicSilenceTimeout = Math::RangeRandom(5,10);
				return;
			}
			if(musicSilenceTimeout > 0)
			{
				musicSilenceTimeout -= evt.timeSinceLastFrame;
				if(musicSilenceTimeout<=0)
				{
					musicSilenceTimeout = 0;
					playRandomAmbientMusic();
				}
			}
		}
	}
	void updateLoopedSounds(const Vector3 &camPosition, const Real &timeElapsed)
	{
		list<const LoopedSound>::type::iterator it = loopedSound.begin();
		while(it!=loopedSound.end())
		{
			LoopedSound *sound = &*it;
			if(sound->sound!=INVALID_SOUND_INDEX)
			{
				if(sound->delay>0){sound->delay -= timeElapsed; if(sound->delay<0)sound->delay=0;}
				if(sound->delay<=0)
				{
					const Real distToCameraSquared = sound->mNode->getPosition().squaredDistance(camPosition);
					if(distToCameraSquared < LOOPEDSOUND_RANGE)
					{
						mSoundMgr->PlaySound(sound->sound, sound->mNode, &sound->channel);
						mSoundMgr->GetSoundChannel(sound->channel)->setVolume(sound->volume*soundVolume);
					}
					else if(sound->channel != INVALID_SOUND_CHANNEL)
						mSoundMgr->StopSound(&sound->channel);
				}
			}
			it++;
		}
	}
	void playMusic(const char *type, bool loop=true)
	{
		if(!mSoundMgr)return;

		music = mSoundMgr->CreateStream(String(type));
		playMusic(music,loop);
	}
	void playMusic(int musicID, bool loop=true)
	{
		if(!mSoundMgr)return;

		music = musicID;
		if(mMusicChannel)mMusicChannel->stop();
		mSoundMgr->PlayMusic(musicID,&mMusicChannel);
		setMusicVolume(musicVolume);
		loopMusic = loop;
		musicFadeTimeout = 0;
	}
	void stopMusic(bool fadeOut=false)
	{
		loopMusic = false;
		if(!mMusicChannel)return;
		if(fadeOut)
		{
			musicFadeTimeout = 2;
		}
		else
		{
			mMusicChannel->stop();
		}
	}
	void setRandomPlaylist(bool flag)
	{
		isRandomPlaylist = flag;
		if(flag)loopMusic = false;
	}
	bool getRandomPlaylist()
	{
		return isRandomPlaylist;
	}
	void playRandomAmbientMusic()
	{
		if(musicList.size()>1)
		{
			bool isRepeat = true;
			while(isRepeat)
			{
				unsigned short tID = unsigned short(Math::RangeRandom(0,(int)musicList.size()));
				if(tID>=(int)musicList.size())tID = 0;

				const int tLastMusic = music;
				playMusic(musicList[tID].c_str(),false);
				isRepeat = (tLastMusic==music);
			}
		}
		else if(musicList.size()>0)playMusic(musicList[0].c_str(),true);
	}
	void pushMusicList(const String &name)
	{
		for(int i=0;i<(int)musicList.size();i++)
			if(musicList[i]==name)return;
		musicList.push_back(name);
	}
	void resetMusicList(bool useDefault=true)
	{
		musicList.clear();
		if(useDefault)
		{
			musicList.push_back(MUSIC_AMBIENT1);
			musicList.push_back(MUSIC_AMBIENT2);
			musicList.push_back(MUSIC_AMBIENT3);
			musicList.push_back(MUSIC_AMBIENT4);
			musicList.push_back(MUSIC_AMBIENT5);
			musicList.push_back(MUSIC_AMBIENT6);
		}
	}
	void playGUISound(const char *type)
	{
		if(!mSoundMgr)return;

		guiSound = mSoundMgr->CreateStream(String(type));
		//mSoundMgr->StopSound(&guiChannel);
		mSoundMgr->PlayMusic(guiSound,&mGuiChannel);
		setGUISoundVolume(guiVolume);
	}
	void playSound(const char *type, SceneNode *node=0)
	{
		if(!mSoundMgr)return;

		int tSound = node?mSoundMgr->CreateSound(String(type)):mSoundMgr->CreateStream(String(type));
		int tSoundChannel = INVALID_SOUND_CHANNEL;
		FMOD::Channel *tStreamChannel = 0;
		if(node)
		{
			mSoundMgr->PlaySound(tSound,node,&tSoundChannel);
			mSoundMgr->GetSoundChannel(tSoundChannel)->setVolume(soundVolume);
		}
		else
		{
			mSoundMgr->PlayMusic(tSound,&tStreamChannel);
			if(tStreamChannel)tStreamChannel->setVolume(soundVolume);
		}
	}
	void playLoopedSound(const char *type, SceneNode *node, const float &volume=1, const Real &delay=0)
	{
		if(!mSoundMgr)return;

		LoopedSound tSound;
		tSound.sound = mSoundMgr->CreateLoopedSound(String(type));
		tSound.channel = INVALID_SOUND_CHANNEL;
		tSound.mNode = node;
		tSound.volume = volume;
		tSound.delay = delay;
		loopedSound.push_back(tSound);
	}
	const list<const LoopedSound>::type::iterator stopLoopedSound(const list<const LoopedSound>::type::iterator &it)
	{
		if(!mSoundMgr)return loopedSound.end();

		LoopedSound *sound = &*it;
		if(sound->channel != INVALID_SOUND_CHANNEL)mSoundMgr->StopSound(&sound->channel);
		return loopedSound.erase(it);
	}
	void stopLoopedSoundByNode(SceneNode *node, const char *type="")
	{
		if(!mSoundMgr)return;
		if(!node)return;

		const int tLoopedSound = ( type=="" ? INVALID_SOUND_INDEX : mSoundMgr->CreateLoopedSound(String(type)) );
		list<const LoopedSound>::type::iterator it = loopedSound.begin();
		while(it!=loopedSound.end())
		{
			LoopedSound *sound = &*it;
			if(sound->mNode==node && (type==""||sound->sound==tLoopedSound))it = stopLoopedSound(it);
			else it++;
		}
	}
	bool hasLoopedSound(const char *type,SceneNode *node=0)
	{
		if(!mSoundMgr)return false;
		const int tLoopedSound = mSoundMgr->CreateLoopedSound(String(type));
		list<const LoopedSound>::type::iterator it = loopedSound.begin();
		while(it!=loopedSound.end())
		{
			LoopedSound *sound = &*it;
			if((sound->mNode==node||!node) && sound->sound==tLoopedSound)return true;
			it++;
		}
		return false;
	}
	void stopAllLoopedSounds()
	{
		list<const LoopedSound>::type::iterator it = loopedSound.begin();
		while(it!=loopedSound.end())it = stopLoopedSound(it);
	}
	void playAmbientSound(const char *type, const float &volume=1)
	{
		if(!mSoundMgr)return;

		ambientSound = mSoundMgr->CreateLoopedStream(String(type));
		if(mAmbientChannel)mAmbientChannel->stop();
		mSoundMgr->PlayMusic(ambientSound,&mAmbientChannel);
		mAmbientChannel->setVolume(soundVolume * volume);
	}
	void setAmbientSoundVolume(const float &volume)
	{
		if(!mSoundMgr || !mAmbientChannel)return;
		mAmbientChannel->setVolume(soundVolume * volume);
	}
	void stopAmbientSound()
	{
		if(!mSoundMgr || !mAmbientChannel)return;
		mAmbientChannel->stop();
	}
	void setMusicVolume(const float &ratio)
	{
		if(!mSoundMgr)return;

		if(mMusicChannel)mMusicChannel->setVolume(ratio);
		musicVolume = ratio;
	}
	float getMusicVolume()
	{
		return musicVolume;
	}
	void setGUISoundVolume(const float &ratio)
	{
		if(!mSoundMgr)return;

		if(mGuiChannel)mGuiChannel->setVolume(ratio);
		guiVolume = ratio;
	}
	float getGUISoundVolume()
	{
		return guiVolume;
	}
	void setSoundVolume(const float &ratio)
	{
		if(!mSoundMgr)return;
		soundVolume = ratio;

		list<const LoopedSound>::type::iterator it = loopedSound.begin();
		while(it!=loopedSound.end())
		{
			LoopedSound *sound = &*it;
			mSoundMgr->GetSoundChannel(sound->channel)->setVolume(sound->volume * ratio);
			it++;
		}
	}
	float getSoundVolume()
	{
		return soundVolume;
	}
	void clearChannelsConnectedToSceneNode(SceneNode *node)
	{
		if(!mSoundMgr)return;

		stopLoopedSoundByNode(node);
		mSoundMgr->clearChannelsConnectedToSceneNode(node);
	}
};

#endif
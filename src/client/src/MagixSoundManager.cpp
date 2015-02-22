#include "MagixSoundManager.h"

MagixSoundManager::MagixSoundManager()
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
MagixSoundManager::~MagixSoundManager()
{
	reset();
	if (mSoundMgr)delete mSoundMgr;
}
void MagixSoundManager::initialize(MagixCamera *camera)
{
	mCamera = camera;
	mSoundMgr = new SoundManager();
	bool hasSound = mSoundMgr->Initialize();
	if (!hasSound)
	{
		delete mSoundMgr;
		mSoundMgr = 0;
	}
	resetMusicList();
}
void MagixSoundManager::reset()
{
	stopAllLoopedSounds();
}
void MagixSoundManager::update(const FrameEvent &evt)
{
	if (!mSoundMgr)return;

	if (mCamera->getCameraMode() == CAMERA_FREE)
	{
		updateLoopedSounds(mCamera->getCamera()->getPosition(), evt.timeSinceLastFrame);
		mSoundMgr->FrameStarted(mCamera->getCamera(), evt.timeSinceLastFrame);
	}
	else
	{
		updateLoopedSounds(mCamera->getActiveCamNode()->_getDerivedPosition(), evt.timeSinceLastFrame);
		mSoundMgr->FrameStarted(mCamera->getActiveCamNode(), evt.timeSinceLastFrame);
	}
	updateMusic(evt);
}
void MagixSoundManager::updateMusic(const FrameEvent &evt)
{
	if (!mMusicChannel)return;

	bool isPlaying = false;
	mMusicChannel->isPlaying(&isPlaying);

	if (musicFadeTimeout>0 && isPlaying)
	{
		musicFadeTimeout -= evt.timeSinceLastFrame;
		if (musicFadeTimeout <= 0)
		{
			musicFadeTimeout = 0;
			mMusicChannel->stop();
		}
		else mMusicChannel->setVolume(musicFadeTimeout / 2 * musicVolume);
	}

	if (loopMusic)
	{
		if (!isPlaying)playMusic(music);
		return;
	}

	//Random playlist
	if (isRandomPlaylist && !isPlaying)
	{
		if (musicSilenceTimeout == 0)
		{
			musicSilenceTimeout = Math::RangeRandom(5, 10);
			return;
		}
		if (musicSilenceTimeout > 0)
		{
			musicSilenceTimeout -= evt.timeSinceLastFrame;
			if (musicSilenceTimeout <= 0)
			{
				musicSilenceTimeout = 0;
				playRandomAmbientMusic();
			}
		}
	}
}
void MagixSoundManager::updateLoopedSounds(const Vector3 &camPosition, const Real &timeElapsed)
{
	list<const LoopedSound>::type::iterator it = loopedSound.begin();
	while (it != loopedSound.end())
	{
		LoopedSound *sound = &*it;
		if (sound->sound != INVALID_SOUND_INDEX)
		{
			if (sound->delay>0){ sound->delay -= timeElapsed; if (sound->delay<0)sound->delay = 0; }
			if (sound->delay <= 0)
			{
				const Real distToCameraSquared = sound->mNode->getPosition().squaredDistance(camPosition);
				if (distToCameraSquared < LOOPEDSOUND_RANGE)
				{
					mSoundMgr->PlaySound(sound->sound, sound->mNode, &sound->channel);
					mSoundMgr->GetSoundChannel(sound->channel)->setVolume(sound->volume*soundVolume);
				}
				else if (sound->channel != INVALID_SOUND_CHANNEL)
					mSoundMgr->StopSound(&sound->channel);
			}
		}
		it++;
	}
}
void MagixSoundManager::playMusic(const char *type, bool loop)
{
	if (!mSoundMgr)return;

	music = mSoundMgr->CreateStream(String(type));
	playMusic(music, loop);
}
void MagixSoundManager::playMusic(int musicID, bool loop)
{
	if (!mSoundMgr)return;

	music = musicID;
	if (mMusicChannel)mMusicChannel->stop();
	mSoundMgr->PlayMusic(musicID, &mMusicChannel);
	setMusicVolume(musicVolume);
	loopMusic = loop;
	musicFadeTimeout = 0;
}
void MagixSoundManager::stopMusic(bool fadeOut)
{
	loopMusic = false;
	if (!mMusicChannel)return;
	if (fadeOut)
	{
		musicFadeTimeout = 2;
	}
	else
	{
		mMusicChannel->stop();
	}
}
void MagixSoundManager::setRandomPlaylist(bool flag)
{
	isRandomPlaylist = flag;
	if (flag)loopMusic = false;
}
bool MagixSoundManager::getRandomPlaylist()
{
	return isRandomPlaylist;
}
void MagixSoundManager::playRandomAmbientMusic()
{
	if (musicList.size()>1)
	{
		bool isRepeat = true;
		while (isRepeat)
		{
			unsigned short tID = unsigned short(Math::RangeRandom(0, (Real)musicList.size()));
			if (tID >= (int)musicList.size())tID = 0;

			const int tLastMusic = music;
			playMusic(musicList[tID].c_str(), false);
			isRepeat = (tLastMusic == music);
		}
	}
	else if (musicList.size()>0)playMusic(musicList[0].c_str(), true);
}
void MagixSoundManager::pushMusicList(const String &name)
{
	for (int i = 0; i<(int)musicList.size(); i++)
		if (musicList[i] == name)return;
	musicList.push_back(name);
}
void MagixSoundManager::resetMusicList(bool useDefault)
{
	musicList.clear();
	if (useDefault)
	{
		musicList.push_back(MUSIC_AMBIENT1);
		musicList.push_back(MUSIC_AMBIENT2);
		musicList.push_back(MUSIC_AMBIENT3);
		musicList.push_back(MUSIC_AMBIENT4);
		musicList.push_back(MUSIC_AMBIENT5);
		musicList.push_back(MUSIC_AMBIENT6);
	}
}
void MagixSoundManager::playGUISound(const char *type)
{
	if (!mSoundMgr)return;

	guiSound = mSoundMgr->CreateStream(String(type));
	//mSoundMgr->StopSound(&guiChannel);
	mSoundMgr->PlayMusic(guiSound, &mGuiChannel);
	setGUISoundVolume(guiVolume);
}
void MagixSoundManager::playSound(const char *type, SceneNode *node)
{
	if (!mSoundMgr)return;

	int tSound = node ? mSoundMgr->CreateSound(String(type)) : mSoundMgr->CreateStream(String(type));
	int tSoundChannel = INVALID_SOUND_CHANNEL;
	FMOD::Channel *tStreamChannel = 0;
	if (node)
	{
		mSoundMgr->PlaySound(tSound, node, &tSoundChannel);
		mSoundMgr->GetSoundChannel(tSoundChannel)->setVolume(soundVolume);
	}
	else
	{
		mSoundMgr->PlayMusic(tSound, &tStreamChannel);
		if (tStreamChannel)tStreamChannel->setVolume(soundVolume);
	}
}
void MagixSoundManager::playLoopedSound(const char *type, SceneNode *node, const float &volume, const Real &delay)
{
	if (!mSoundMgr)return;

	LoopedSound tSound;
	tSound.sound = mSoundMgr->CreateLoopedSound(String(type));
	tSound.channel = INVALID_SOUND_CHANNEL;
	tSound.mNode = node;
	tSound.volume = volume;
	tSound.delay = delay;
	loopedSound.push_back(tSound);
}
const list<const LoopedSound>::type::iterator MagixSoundManager::stopLoopedSound(const list<const LoopedSound>::type::iterator &it)
{
	if (!mSoundMgr)return loopedSound.end();

	LoopedSound *sound = &*it;
	if (sound->channel != INVALID_SOUND_CHANNEL)mSoundMgr->StopSound(&sound->channel);
	return loopedSound.erase(it);
}
void MagixSoundManager::stopLoopedSoundByNode(SceneNode *node, const char *type)
{
	if (!mSoundMgr)return;
	if (!node)return;

	const int tLoopedSound = (type == "" ? INVALID_SOUND_INDEX : mSoundMgr->CreateLoopedSound(String(type)));
	list<const LoopedSound>::type::iterator it = loopedSound.begin();
	while (it != loopedSound.end())
	{
		LoopedSound *sound = &*it;
		if (sound->mNode == node && (type == "" || sound->sound == tLoopedSound))it = stopLoopedSound(it);
		else it++;
	}
}
bool MagixSoundManager::hasLoopedSound(const char *type, SceneNode *node)
{
	if (!mSoundMgr)return false;
	const int tLoopedSound = mSoundMgr->CreateLoopedSound(String(type));
	list<const LoopedSound>::type::iterator it = loopedSound.begin();
	while (it != loopedSound.end())
	{
		LoopedSound *sound = &*it;
		if ((sound->mNode == node || !node) && sound->sound == tLoopedSound)return true;
		it++;
	}
	return false;
}
void MagixSoundManager::stopAllLoopedSounds()
{
	list<const LoopedSound>::type::iterator it = loopedSound.begin();
	while (it != loopedSound.end())it = stopLoopedSound(it);
}
void MagixSoundManager::playAmbientSound(const char *type, const float &volume)
{
	if (!mSoundMgr)return;

	ambientSound = mSoundMgr->CreateLoopedStream(String(type));
	if (mAmbientChannel)mAmbientChannel->stop();
	mSoundMgr->PlayMusic(ambientSound, &mAmbientChannel);
	mAmbientChannel->setVolume(soundVolume * volume);
}
void MagixSoundManager::setAmbientSoundVolume(const float &volume)
{
	if (!mSoundMgr || !mAmbientChannel)return;
	mAmbientChannel->setVolume(soundVolume * volume);
}
void MagixSoundManager::stopAmbientSound()
{
	if (!mSoundMgr || !mAmbientChannel)return;
	mAmbientChannel->stop();
}
void MagixSoundManager::setMusicVolume(const float &ratio)
{
	if (!mSoundMgr)return;

	if (mMusicChannel)mMusicChannel->setVolume(ratio);
	musicVolume = ratio;
}
float MagixSoundManager::getMusicVolume()
{
	return musicVolume;
}
void MagixSoundManager::setGUISoundVolume(const float &ratio)
{
	if (!mSoundMgr)return;

	if (mGuiChannel)mGuiChannel->setVolume(ratio);
	guiVolume = ratio;
}
float MagixSoundManager::getGUISoundVolume()
{
	return guiVolume;
}
void MagixSoundManager::setSoundVolume(const float &ratio)
{
	if (!mSoundMgr)return;
	soundVolume = ratio;

	list<const LoopedSound>::type::iterator it = loopedSound.begin();
	while (it != loopedSound.end())
	{
		LoopedSound *sound = &*it;
		mSoundMgr->GetSoundChannel(sound->channel)->setVolume(sound->volume * ratio);
		it++;
	}
}
float MagixSoundManager::getSoundVolume()
{
	return soundVolume;
}
void MagixSoundManager::clearChannelsConnectedToSceneNode(SceneNode *node)
{
	if (!mSoundMgr)return;

	stopLoopedSoundByNode(node);
	mSoundMgr->clearChannelsConnectedToSceneNode(node);
}

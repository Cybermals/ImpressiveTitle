#ifdef __MINGW__
    #include <thread>
#endif

#include "MagixSoundManager.h"


//Sound Manager Functions
//==============================================================
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

    if(mSoundMgr)
    {
        delete mSoundMgr;
    }
}


void MagixSoundManager::initialize(MagixCamera *camera)
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


void MagixSoundManager::reset()
{
    stopAllLoopedSounds();
}


void MagixSoundManager::update(const FrameEvent &evt)
{
    if(!mSoundMgr)
    {
        return;
    }

    if(mCamera->getCameraMode() == CAMERA_FREE)
    {
        updateLoopedSounds(mCamera->getCamera()->getPosition(), evt.timeSinceLastFrame);
        mSoundMgr->FrameStarted(mCamera->getCamera(), evt.timeSinceLastFrame);
    }
    else
    {
        updateLoopedSounds(mCamera->getActiveCamNode()->_getDerivedPosition(),
            evt.timeSinceLastFrame);
        mSoundMgr->FrameStarted(mCamera->getActiveCamNode(), evt.timeSinceLastFrame);
    }

    updateMusic(evt);
}


void MagixSoundManager::updateMusic(const FrameEvent &evt)
{
    if(!mMusicChannel)
    {
        return;
    }

    FMOD_BOOL isPlaying = false;
    FMOD_Channel_IsPlaying(mMusicChannel, &isPlaying);

    if(musicFadeTimeout > 0 && isPlaying)
    {
        musicFadeTimeout -= evt.timeSinceLastFrame;

        if(musicFadeTimeout <= 0)
        {
            musicFadeTimeout = 0;
            FMOD_Channel_Stop(mMusicChannel);
        }
        else
        {
            FMOD_Channel_SetVolume(mMusicChannel, musicFadeTimeout / 2 * musicVolume);
        }
    }

    if(loopMusic)
    {
        if(!isPlaying)
        {
            playMusic(music);
        }

        return;
    }

    //Random playlist
    if(isRandomPlaylist && !isPlaying)
    {
        if(musicSilenceTimeout == 0)
        {
            musicSilenceTimeout = Math::RangeRandom(5, 10);
            return;
        }
        if(musicSilenceTimeout > 0)
        {
            musicSilenceTimeout -= evt.timeSinceLastFrame;

            if(musicSilenceTimeout <= 0)
            {
                musicSilenceTimeout = 0;
                playRandomAmbientMusic();
            }
        }
    }
}


void MagixSoundManager::updateLoopedSounds(const Vector3 &camPosition,
    const Real &timeElapsed)
{
    list<LoopedSound>::type::iterator it = loopedSound.begin();

    while(it != loopedSound.end())
    {
        LoopedSound *sound = &*it;
        if(sound->sound != INVALID_SOUND_INDEX)
        {
            if(sound->delay > 0)
            {
                sound->delay -= timeElapsed; if(sound->delay < 0)sound->delay = 0;
            }

            if(sound->delay <= 0)
            {
                const Real distToCameraSquared = sound->mNode->getPosition().squaredDistance(
                    camPosition);

                if(distToCameraSquared < LOOPEDSOUND_RANGE)
                {
                    mSoundMgr->PlaySound(sound->sound, sound->mNode, &sound->channel);
                    FMOD_Channel_SetVolume(mSoundMgr->GetSoundChannel(sound->channel),
                        sound->volume * soundVolume);
                }
                else if(sound->channel != INVALID_SOUND_CHANNEL)
                {
                    mSoundMgr->StopSound(&sound->channel);
                }
            }
        }

        it++;
    }
}


void MagixSoundManager::playMusic(String type, bool loop)
{
    if(!mSoundMgr)
    {
        return;
    }

    //Use a separate thread to prevent lag when the music is loaded.
    #ifdef __MINGW__
    std::thread t(&_playMusic, this, type, loop);
    t.detach();
    #else
	_playMusic(type, loop);
    #endif
}


void MagixSoundManager::_playMusic(String type, bool loop)
{
    music = mSoundMgr->CreateStream(type);
    playMusic(music, loop);
}


void MagixSoundManager::playMusic(int musicID, bool loop)
{
    if(!mSoundMgr)
    {
        return;
    }

    music = musicID;

    if(mMusicChannel)
    {
        FMOD_Channel_Stop(mMusicChannel);
    }

    mSoundMgr->PlayMusic(musicID, &mMusicChannel);
    setMusicVolume(musicVolume);
    loopMusic = loop;
    musicFadeTimeout = 0;
}


void MagixSoundManager::stopMusic(bool fadeOut)
{
    loopMusic = false;

    if(!mMusicChannel)
    {
        return;
    }

    if(fadeOut)
    {
        musicFadeTimeout = 2;
    }
    else
    {
        FMOD_Channel_Stop(mMusicChannel);
    }
}


void MagixSoundManager::setRandomPlaylist(bool flag)
{
    isRandomPlaylist = flag;

    if(flag)
    {
        loopMusic = false;
    }
}


bool MagixSoundManager::getRandomPlaylist()
{
    return isRandomPlaylist;
}


void MagixSoundManager::playRandomAmbientMusic()
{
    if(musicList.size() > 1)
    {
        bool isRepeat = true;

        while(isRepeat)
        {
            unsigned short tID = (unsigned short)Math::RangeRandom(0, (Real)musicList.size());

            if(tID >= (int)musicList.size())
            {
                tID = 0;
            }

            const int tLastMusic = music;
            playMusic(musicList[tID].c_str(), false);
            isRepeat = (tLastMusic == music);
        }
    }
    else if(musicList.size() > 0)
    {
        playMusic(musicList[0].c_str(), true);
    }
}


void MagixSoundManager::pushMusicList(const String &name)
{
    for(int i = 0; i < (int)musicList.size(); i++)
    {
        if(musicList[i] == name)return;
    }

    musicList.push_back(name);
}


void MagixSoundManager::resetMusicList(bool useDefault)
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


void MagixSoundManager::playGUISound(const char *type)
{
    if(!mSoundMgr)
    {
        return;
    }

    String tmp(type);
    guiSound = mSoundMgr->CreateStream(tmp);
    //mSoundMgr->StopSound(&guiChannel);
    mSoundMgr->PlayMusic(guiSound, &mGuiChannel);
    setGUISoundVolume(guiVolume);
}


void MagixSoundManager::playSound(const char *type, SceneNode *node)
{
    if(!mSoundMgr)
    {
        return;
    }

    String tmp(type);
    int tSound = node ? mSoundMgr->CreateSound(tmp) :
        mSoundMgr->CreateStream(tmp);
    int tSoundChannel = INVALID_SOUND_CHANNEL;
    FMOD_CHANNEL *tStreamChannel = 0;

    if(node)
    {
        mSoundMgr->PlaySound(tSound, node, &tSoundChannel);
        FMOD_Channel_SetVolume(mSoundMgr->GetSoundChannel(tSoundChannel), soundVolume);
    }
    else
    {
        mSoundMgr->PlayMusic(tSound, &tStreamChannel);

        if(tStreamChannel)
        {
            FMOD_Channel_SetVolume(tStreamChannel, soundVolume);
        }
    }
}


void MagixSoundManager::playLoopedSound(const char *type, SceneNode *node,
    const float &volume, const Real &delay)
{
    if(!mSoundMgr)
    {
        return;
    }

    LoopedSound tSound;
    String tmp(type);
    tSound.sound = mSoundMgr->CreateLoopedSound(tmp);
    tSound.channel = INVALID_SOUND_CHANNEL;
    tSound.mNode = node;
    tSound.volume = volume;
    tSound.delay = delay;
    loopedSound.push_back(tSound);
}


const list<LoopedSound>::type::iterator MagixSoundManager::stopLoopedSound(
    const list<LoopedSound>::type::iterator &it)
{
    if(!mSoundMgr)
    {
        return loopedSound.end();
    }

    LoopedSound *sound = &*it;

    if(sound->channel != INVALID_SOUND_CHANNEL)
    {
        mSoundMgr->StopSound(&sound->channel);
    }

    return loopedSound.erase(it);
}


void MagixSoundManager::stopLoopedSoundByNode(SceneNode *node, const char *type)
{
    if(!mSoundMgr)
    {
        return;
    }

    if(!node)
    {
        return;
    }

    String tmp(type);
    const int tLoopedSound = (strcmp(type, "") == 0 ? INVALID_SOUND_INDEX :
        mSoundMgr->CreateLoopedSound(tmp));
    list<LoopedSound>::type::iterator it = loopedSound.begin();

    while(it != loopedSound.end())
    {
        LoopedSound *sound = &*it;

        if(sound->mNode == node && (strcmp(type, "") == 0 || sound->sound == tLoopedSound))
        {
            it = stopLoopedSound(it);
        }
        else
        {
            it++;
        }
    }
}


bool MagixSoundManager::hasLoopedSound(const char *type, SceneNode *node)
{
    if(!mSoundMgr)
    {
        return false;
    }

    String tmp(type);
    const int tLoopedSound = mSoundMgr->CreateLoopedSound(tmp);
    list<LoopedSound>::type::iterator it = loopedSound.begin();

    while(it != loopedSound.end())
    {
        LoopedSound *sound = &*it;

        if((sound->mNode == node || !node) && sound->sound == tLoopedSound)
        {
            return true;
        }

        it++;
    }

    return false;
}


void MagixSoundManager::stopAllLoopedSounds()
{
    list<LoopedSound>::type::iterator it = loopedSound.begin();

    while(it != loopedSound.end())
    {
        it = stopLoopedSound(it);
    }
}


void MagixSoundManager::playAmbientSound(const char *type, const float &volume)
{
    if(!mSoundMgr)
    {
        return;
    }

    String tmp(type);
    ambientSound = mSoundMgr->CreateLoopedStream(tmp);

    if(mAmbientChannel)
    {
        FMOD_Channel_Stop(mAmbientChannel);
    }

    mSoundMgr->PlayMusic(ambientSound, &mAmbientChannel);
    FMOD_Channel_SetVolume(mAmbientChannel, soundVolume * volume);
}


void MagixSoundManager::setAmbientSoundVolume(const float &volume)
{
    if(!mSoundMgr || !mAmbientChannel)
    {
        return;
    }

    FMOD_Channel_SetVolume(mAmbientChannel, soundVolume * volume);
}


void MagixSoundManager::stopAmbientSound()
{
    if(!mSoundMgr || !mAmbientChannel)
    {
        return;
    }

    FMOD_Channel_Stop(mAmbientChannel);
}


void MagixSoundManager::setMusicVolume(const float &ratio)
{
    if(!mSoundMgr)
    {
        return;
    }

    if(mMusicChannel)
    {
        FMOD_Channel_SetVolume(mMusicChannel, ratio);
    }

    musicVolume = ratio;
}


float MagixSoundManager::getMusicVolume()
{
    return musicVolume;
}


void MagixSoundManager::setGUISoundVolume(const float &ratio)
{
    if(!mSoundMgr)
    {
        return;
    }

    if(mGuiChannel)
    {
        FMOD_Channel_SetVolume(mGuiChannel, ratio);
    }

    guiVolume = ratio;
}


float MagixSoundManager::getGUISoundVolume()
{
    return guiVolume;
}


void MagixSoundManager::setSoundVolume(const float &ratio)
{
    if(!mSoundMgr)
    {
        return;
    }

    soundVolume = ratio;

    list<LoopedSound>::type::iterator it = loopedSound.begin();

    while(it != loopedSound.end())
    {
        LoopedSound *sound = &*it;
        FMOD_Channel_SetVolume(mSoundMgr->GetSoundChannel(sound->channel), sound->volume * ratio);
        it++;
    }
}


float MagixSoundManager::getSoundVolume()
{
    return soundVolume;
}


void MagixSoundManager::clearChannelsConnectedToSceneNode(SceneNode *node)
{
    if(!mSoundMgr)
    {
        return;
    }

    stopLoopedSoundByNode(node);
    mSoundMgr->clearChannelsConnectedToSceneNode(node);
}

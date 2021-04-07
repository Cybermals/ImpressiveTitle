#include <fmod_errors.h>
#include "SoundManager.h"

#define INITIAL_VECTOR_SIZE   100
#define INCREASE_VECTOR_SIZE  20

#define DOPPLER_SCALE         0.01 //1.0
#define DISTANCE_FACTOR       5    //1.0
#define ROLLOFF_SCALE         0.05 //0.5

#define FMOD_CHANNEL_FREE nullptr


//Sound Instance Functions
//=======================================================================
void SoundInstance::Clear(void)
{
    fileName.clear();
    streamPtr.setNull();
    fileArchive = nullptr;
    fmodSound = nullptr;
    soundType = SOUND_TYPE_INVALID;
}


//Channel Instance Functions
//=======================================================================
void ChannelInstance::Clear(void)
{
    sceneNode = nullptr;
    prevPosition = Ogre::Vector3(0, 0, 0);
}



//File Locator
//=======================================================================
class FileLocator : public ResourceGroupManager
{
public:
    FileLocator() {}
    ~FileLocator() {}

    Archive *Find(String &filename)
    {
        ResourceGroup* grp = getResourceGroup("General");

        if(!grp)
        {
            OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
                "Cannot locate a resource group called 'General'",
                "ResourceGroupManager::openResource");
        }

        OGRE_LOCK_MUTEX(grp->OGRE_AUTO_MUTEX_NAME) // lock group mutex
        ResourceLocationIndex::iterator rit = grp->resourceIndexCaseSensitive.find(filename);

        if(rit != grp->resourceIndexCaseSensitive.end())
        {
            // Found in the index
            Archive *fileArchive = rit->second;
            filename = fileArchive->getName() + "/" + filename;
            return fileArchive;
        }

        return nullptr;
    }
};


//Static Sound Manager Members
//=======================================================================
template<> SoundManager* Singleton<SoundManager>::ms_Singleton = 0;


//Sound Manager Functions
//=======================================================================
SoundManager::SoundManager()
{
    system = nullptr;
    prevListenerPosition = Vector3(0, 0, 0);
    soundInstanceVector = new SoundInstanceVector;

    // Initialized to zero, but pre-incremented in GetNextSoundInstanceIndex(), so vector starts at one.
    nextSoundInstanceIndex = 0;

    // Start off with INITIAL_VECTOR_SIZE soundInstanceVectors.  It can grow from here.
    soundInstanceVector->resize(INITIAL_VECTOR_SIZE);

    for(int vectorIndex = 0; vectorIndex < INITIAL_VECTOR_SIZE; vectorIndex++)
    {
        soundInstanceVector->at(vectorIndex) = new SoundInstance;
        soundInstanceVector->at(vectorIndex)->Clear();
    }

    for(int channelIndex = 0; channelIndex < MAX_SOUND_CHANNELS; channelIndex++)
    {
        channelArray[channelIndex].Clear();
    }
}


SoundManager::~SoundManager()
{
    for(int vectorIndex = 0; vectorIndex < (int)soundInstanceVector->capacity();
        vectorIndex++)
    {
        soundInstanceVector->at(vectorIndex)->fileName.clear();
        //      soundInstanceVector->at(vectorIndex)->streamPtr->close();
        delete soundInstanceVector->at(vectorIndex);
    }

    delete soundInstanceVector;

    if(system)
    {
        FMOD_System_Release(system);
    }
}


bool SoundManager::Initialize(void)
{
    FMOD_RESULT result;

    // Create the main system object.
    /*result = FMOD::System_Create(&system);
    if (result != FMOD_OK)
    OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "FMOD error! (" + StringConverter::toString(result) + "): " + FMOD_ErrorString(result), "SoundManager::Initialize");

    result = system->init(MAX_SOUND_CHANNELS, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
    if (result != FMOD_OK)
    OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "FMOD error! (" + StringConverter::toString(result) + "): " + FMOD_ErrorString(result), "SoundManager::Initialize");

    system->set3DSettings(DOPPLER_SCALE, DISTANCE_FACTOR, ROLLOFF_SCALE);

    result = system->setFileSystem(&fmodFileOpenCallback, &fmodFileCloseCallback, &fmodFileReadCallback, &fmodFileSeekCallback, 2048);
    if (result != FMOD_OK)
    OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "FMOD error! (" + StringConverter::toString(result) + "): " + FMOD_ErrorString(result), "SoundManager::Initialize");
    */

    result = FMOD_System_Create(&system);

    if(result != FMOD_OK)
    {
        Ogre::LogManager::getSingleton().logMessage("FMOD error! (" +
            StringConverter::toString(result) + "): " + FMOD_ErrorString(result));
        Ogre::LogManager::getSingleton().logMessage(
            "SoundManager Failed to initialize, this game will run without sound.");
        return false;
    }

    result = FMOD_System_Init(system, MAX_SOUND_CHANNELS, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.

    if(result != FMOD_OK)
    {
        Ogre::LogManager::getSingleton().logMessage("FMOD error! (" +
            StringConverter::toString(result) + "): " + FMOD_ErrorString(result));
        Ogre::LogManager::getSingleton().logMessage(
            "SoundManager Failed to initialize, this game will run without sound.");
        return false;
    }

    FMOD_System_Set3DSettings(system, DOPPLER_SCALE, DISTANCE_FACTOR, ROLLOFF_SCALE);
    result = FMOD_System_SetFileSystem(system, &fmodFileOpenCallback, &fmodFileCloseCallback,
        &fmodFileReadCallback, &fmodFileSeekCallback, nullptr, nullptr, 2048);

    if(result != FMOD_OK)
    {
        Ogre::LogManager::getSingleton().logMessage("FMOD error! (" +
            StringConverter::toString(result) + "): " + FMOD_ErrorString(result));
        Ogre::LogManager::getSingleton().logMessage(
            "SoundManager Failed to initialize, this game will run without sound.");
        return false;
    }

    Ogre::LogManager::getSingleton().logMessage("SoundManager Initialized");
    return true;
}


SoundManager* SoundManager::getSingletonPtr(void)
{
    return ms_Singleton;
}


SoundManager& SoundManager::getSingleton(void)
{
    assert(ms_Singleton);
    return (*ms_Singleton);
}


void SoundManager::FrameStarted(Ogre::SceneNode *listenerNode,
    Ogre::Real timeElapsed)
{
    int            channelIndex;
    FMOD_CHANNEL *nextChannel;
    FMOD_VECTOR    listenerPosition;
    FMOD_VECTOR    listenerForward;
    FMOD_VECTOR    listenerUp;
    FMOD_VECTOR    listenerVelocity;
    Ogre::Vector3  vectorVelocity;
    Ogre::Vector3  vectorForward;
    Ogre::Vector3  vectorUp;

    if(timeElapsed > 0)
    {
        vectorVelocity = (listenerNode->_getDerivedPosition() -
            prevListenerPosition) / timeElapsed;
    }
    else
    {
        vectorVelocity = Vector3(0, 0, 0);
    }

    vectorForward = listenerNode->_getDerivedOrientation().zAxis();
    vectorForward.normalise();

    vectorUp = listenerNode->_getDerivedOrientation().yAxis();
    vectorUp.normalise();

    listenerPosition.x = listenerNode->_getDerivedPosition().x;
    listenerPosition.y = listenerNode->_getDerivedPosition().y;
    listenerPosition.z = listenerNode->_getDerivedPosition().z;

    listenerForward.x = vectorForward.x;
    listenerForward.y = vectorForward.y;
    listenerForward.z = vectorForward.z;

    listenerUp.x = vectorUp.x;
    listenerUp.y = vectorUp.y;
    listenerUp.z = vectorUp.z;

    listenerVelocity.x = vectorVelocity.x;
    listenerVelocity.y = vectorVelocity.y;
    listenerVelocity.z = vectorVelocity.z;

    // update 'ears'
    FMOD_System_Set3DListenerAttributes(system, 0, &listenerPosition, &listenerVelocity,
        &listenerForward, &listenerUp);
    FMOD_System_Update(system);

    prevListenerPosition = listenerNode->_getDerivedPosition();

    for(channelIndex = 0; channelIndex < MAX_SOUND_CHANNELS; channelIndex++)
    {
        if(channelArray[channelIndex].sceneNode != nullptr)
        {
            FMOD_System_GetChannel(system, channelIndex, &nextChannel);

            if(timeElapsed > 0)
            {
                vectorVelocity = (channelArray[channelIndex].sceneNode->_getDerivedPosition() -
                    channelArray[channelIndex].prevPosition) / timeElapsed;
            }
            else
            {
                vectorVelocity = Vector3(0, 0, 0);
            }

            listenerPosition.x = channelArray[channelIndex].sceneNode->_getDerivedPosition().x;
            listenerPosition.y = channelArray[channelIndex].sceneNode->_getDerivedPosition().y;
            listenerPosition.z = channelArray[channelIndex].sceneNode->_getDerivedPosition().z;

            listenerVelocity.x = vectorVelocity.x;
            listenerVelocity.y = vectorVelocity.y;
            listenerVelocity.z = vectorVelocity.z;

            FMOD_Channel_Set3DAttributes(nextChannel, &listenerPosition, &listenerVelocity, nullptr);
            channelArray[channelIndex].prevPosition = channelArray[channelIndex].sceneNode->_getDerivedPosition();
        }
    }
}

void SoundManager::FrameStarted(Ogre::Camera *listenerNode, Ogre::Real timeElapsed)
{
    int            channelIndex;
    FMOD_CHANNEL *nextChannel;
    FMOD_VECTOR    listenerPosition;
    FMOD_VECTOR    listenerForward;
    FMOD_VECTOR    listenerUp;
    FMOD_VECTOR    listenerVelocity;
    Ogre::Vector3  vectorVelocity;
    Ogre::Vector3  vectorForward;
    Ogre::Vector3  vectorUp;

    if(timeElapsed > 0)
    {
        vectorVelocity = (listenerNode->getPosition() - prevListenerPosition) /
            timeElapsed;
    }
    else
    {
        vectorVelocity = Vector3(0, 0, 0);
    }

    vectorForward = listenerNode->getOrientation().zAxis();
    vectorForward.normalise();

    vectorUp = listenerNode->getOrientation().yAxis();
    vectorUp.normalise();

    listenerPosition.x = listenerNode->getPosition().x;
    listenerPosition.y = listenerNode->getPosition().y;
    listenerPosition.z = listenerNode->getPosition().z;

    listenerForward.x = vectorForward.x;
    listenerForward.y = vectorForward.y;
    listenerForward.z = vectorForward.z;

    listenerUp.x = vectorUp.x;
    listenerUp.y = vectorUp.y;
    listenerUp.z = vectorUp.z;

    listenerVelocity.x = vectorVelocity.x;
    listenerVelocity.y = vectorVelocity.y;
    listenerVelocity.z = vectorVelocity.z;

    // update 'ears'
    FMOD_System_Set3DListenerAttributes(system, 0, &listenerPosition, &listenerVelocity,
        &listenerForward, &listenerUp);
    FMOD_System_Update(system);

    prevListenerPosition = listenerNode->getPosition();

    for(channelIndex = 0; channelIndex < MAX_SOUND_CHANNELS; channelIndex++)
    {
        if(channelArray[channelIndex].sceneNode != nullptr)
        {
            FMOD_System_GetChannel(system, channelIndex, &nextChannel);

            if(timeElapsed > 0)
            {
                vectorVelocity = (channelArray[channelIndex].sceneNode->_getDerivedPosition() -
                    channelArray[channelIndex].prevPosition) / timeElapsed;
            }
            else
            {
                vectorVelocity = Vector3(0, 0, 0);
            }

            listenerPosition.x = channelArray[channelIndex].sceneNode->_getDerivedPosition().x;
            listenerPosition.y = channelArray[channelIndex].sceneNode->_getDerivedPosition().y;
            listenerPosition.z = channelArray[channelIndex].sceneNode->_getDerivedPosition().z;

            listenerVelocity.x = vectorVelocity.x;
            listenerVelocity.y = vectorVelocity.y;
            listenerVelocity.z = vectorVelocity.z;

            FMOD_Channel_Set3DAttributes(nextChannel, &listenerPosition, &listenerVelocity, nullptr);
            channelArray[channelIndex].prevPosition = channelArray[channelIndex].sceneNode->_getDerivedPosition();
        }
    }
}


int SoundManager::CreateStream(String &fileName)
{
    return CreateSound(fileName, SOUND_TYPE_2D_SOUND);
}


int SoundManager::CreateSound(String &fileName)
{
    return CreateSound(fileName, SOUND_TYPE_3D_SOUND);
}


int SoundManager::CreateLoopedSound(String &fileName)
{
    return CreateSound(fileName, SOUND_TYPE_3D_SOUND_LOOPED);
}


int SoundManager::CreateLoopedStream(String &fileName)
{
    return CreateSound(fileName, SOUND_TYPE_2D_SOUND_LOOPED);
}



// fileName is actually a pointer to a SoundInstance, passed in from CreateSound().
FMOD_RESULT SoundManager::fmodFileOpenCallback(const char *fileName,
    unsigned int *filesize, void **handle, void *userdata)
{
    SoundInstance *soundInstance;

    assert(fileName);

    soundInstance = (SoundInstance *)fileName;
    assert(soundInstance->fileArchive);

    *handle = (void *)soundInstance;
    //*userdata = nullptr;

    soundInstance->streamPtr = soundInstance->fileArchive->open(
        soundInstance->fileName);

    if(soundInstance->streamPtr.isNull())
    {
        *filesize = 0;
        return FMOD_ERR_FILE_NOTFOUND;
    }

    *filesize = (unsigned int)soundInstance->streamPtr->size();
    return FMOD_OK;
}


FMOD_RESULT SoundManager::fmodFileCloseCallback(void *handle, void *userdata)
{
    return FMOD_OK;
}


FMOD_RESULT SoundManager::fmodFileReadCallback(void *handle, void *buffer,
    unsigned int sizeBytes, unsigned int *bytesRead, void *userData)
{
    SoundInstance *soundInstance;

    soundInstance = (SoundInstance *)handle;
    *bytesRead = (unsigned int)soundInstance->streamPtr->read(buffer,
        (size_t)sizeBytes);

    if(*bytesRead == 0)
    {
        return FMOD_ERR_FILE_EOF;
    }

    return FMOD_OK;
}


FMOD_RESULT SoundManager::fmodFileSeekCallback(void *handle, unsigned int pos,
    void *userdata)
{
    SoundInstance *soundInstance;
    soundInstance = (SoundInstance *)handle;
    soundInstance->streamPtr->seek((size_t)pos);
    return FMOD_OK;
}



int SoundManager::CreateSound(String &fileName, SOUND_TYPE soundType)
{
    Archive *      fileArchive;
    FMOD_RESULT    result;
    FMOD_SOUND *  sound;
    String         fullPathName;
    SoundInstance *newSoundInstance;

    int soundIndex;
    soundIndex = FindSound(fileName, soundType);

    if(soundIndex != INVALID_SOUND_INDEX)
    {
        return soundIndex;
    }

    fullPathName = fileName;
    FileLocator * fileLocator = (FileLocator *)ResourceGroupManager::getSingletonPtr();
    fileArchive = fileLocator->Find(fullPathName);

    if(!fileArchive)
    {
        Ogre::LogManager::getSingleton().logMessage(
            "SoundManager::CreateSound could not find sound '" + fileName + "'");
        return INVALID_SOUND_INDEX;
    }

    IncrementNextSoundInstanceIndex();
    newSoundInstance = soundInstanceVector->at(nextSoundInstanceIndex);
    newSoundInstance->fileName = fileName;
    newSoundInstance->fileArchive = fileArchive;
    newSoundInstance->soundType = soundType;

    switch(soundType)
    {
        case SOUND_TYPE_3D_SOUND:
            result = FMOD_System_CreateSound(system, (const char *)newSoundInstance,
                FMOD_3D /*| FMOD_HARDWARE*/, 0, &sound);
            break;

        case SOUND_TYPE_3D_SOUND_LOOPED:
            result = FMOD_System_CreateSound(system, (const char *)newSoundInstance,
                FMOD_LOOP_NORMAL | FMOD_3D /*| FMOD_HARDWARE*/, 0, &sound);
            break;

        case SOUND_TYPE_2D_SOUND:
            result = FMOD_System_CreateSound(system, (const char *)newSoundInstance,
                FMOD_DEFAULT, 0, &sound);
            break;

        case SOUND_TYPE_2D_SOUND_LOOPED:
            result = FMOD_System_CreateSound(system, (const char *)newSoundInstance,
                FMOD_LOOP_NORMAL | FMOD_2D /*| FMOD_HARDWARE*/, 0, &sound);
            break;

        default:
            Ogre::LogManager::getSingleton().logMessage(
                "SoundManager::CreateSound could not load sound '" + fileName +
                "' (invalid soundType)");
            return INVALID_SOUND_INDEX;
    }

    if(result != FMOD_OK)
    {
        Ogre::LogManager::getSingleton().logMessage(
            "SoundManager::CreateSound could not load sound '" + fileName +
            "'  FMOD Error:" + FMOD_ErrorString(result));
        return INVALID_SOUND_INDEX;
    }

    newSoundInstance->fmodSound = sound;
    return nextSoundInstanceIndex;
}



void SoundManager::IncrementNextSoundInstanceIndex(void)
{
    int oldVectorCapacity = (int)soundInstanceVector->capacity();
    nextSoundInstanceIndex += 1;

    if(nextSoundInstanceIndex < oldVectorCapacity)
    {
        return;
    }

    int vectorIndex;
    SoundInstanceVector *newSoundInstanceVector;

    // Create a new, larger SoundInstanceVector
    newSoundInstanceVector = new SoundInstanceVector();
    newSoundInstanceVector->resize(oldVectorCapacity + INCREASE_VECTOR_SIZE);

    // Check Ogre.log for these messages, and change INITIAL_VECTOR_SIZE to be a more appropriate value
    Ogre::LogManager::getSingleton().logMessage(
        "SoundManager::IncrementNextSoundInstanceIndex increasing size of soundInstanceVector to " +
        StringConverter::toString(oldVectorCapacity + INCREASE_VECTOR_SIZE));

    // Copy values from old vector to new
    for(vectorIndex = 0; vectorIndex < oldVectorCapacity; vectorIndex++)
    {
        newSoundInstanceVector->at(vectorIndex) = soundInstanceVector->at(vectorIndex);
    }

    int newVectorCapacity = (int)newSoundInstanceVector->capacity();

    // Clear out the rest of the new vector
    while(vectorIndex < newVectorCapacity)
    {
        newSoundInstanceVector->at(vectorIndex) = new SoundInstance();
        newSoundInstanceVector->at(vectorIndex)->Clear();
        vectorIndex++;
    }

    // Clear out the old vector and point to the new one.
    soundInstanceVector->clear();
    delete soundInstanceVector;
    soundInstanceVector = newSoundInstanceVector;
}


void SoundManager::PlaySound(int soundIndex, SceneNode *soundNode,
    int *channelIndex)
{
    int            channelIndexTemp;
    FMOD_RESULT    result;
    FMOD_VECTOR    initialPosition;
    FMOD_CHANNEL *channel;
    SoundInstance *soundInstance;

    if(soundIndex == INVALID_SOUND_INDEX)
    {
        return;
    }

    if(channelIndex)
    {
        channelIndexTemp = *channelIndex;
    }
    else
    {
        channelIndexTemp = INVALID_SOUND_CHANNEL;
    }

    assert((soundIndex > 0) &&
        (soundIndex < (int)soundInstanceVector->capacity()));

    // If the channelIndex already has a sound assigned to it, test if it's the same sceneNode.
    if((channelIndexTemp != INVALID_SOUND_CHANNEL) &&
        (channelArray[channelIndexTemp].sceneNode != nullptr))
    {
        result = FMOD_System_GetChannel(system, channelIndexTemp, &channel);

        if(result == FMOD_OK)
        {
            FMOD_BOOL isPlaying;
            result = FMOD_Channel_IsPlaying(channel, &isPlaying);

            if((result == FMOD_OK) && (isPlaying == true) &&
                (channelArray[channelIndexTemp].sceneNode == soundNode))
            {
                return;  // Already playing this sound attached to this node.
            }
        }
    }

    soundInstance = soundInstanceVector->at(soundIndex);

    // Start the sound paused
    result = FMOD_System_PlaySound(system, soundInstance->fmodSound, FMOD_CHANNEL_FREE, true,
        &channel);

    if(result != FMOD_OK)
    {
        Ogre::LogManager::getSingleton().logMessage(
            String("SoundManager::PlaySound could not play sound  FMOD Error:") +
            FMOD_ErrorString(result));

        if(channelIndex)
        {
            *channelIndex = INVALID_SOUND_CHANNEL;
        }

        return;
    }

    FMOD_Channel_GetIndex(channel, &channelIndexTemp);
    channelArray[channelIndexTemp].sceneNode = soundNode;

    if(soundNode)
    {
        channelArray[channelIndexTemp].prevPosition = soundNode->_getDerivedPosition();

        initialPosition.x = soundNode->_getDerivedPosition().x;
        initialPosition.y = soundNode->_getDerivedPosition().y;
        initialPosition.z = soundNode->_getDerivedPosition().z;
        FMOD_Channel_Set3DAttributes(channel, &initialPosition, nullptr, nullptr);
    }

    result = FMOD_Channel_SetVolume(channel, 1.0);

    // This is where the sound really starts.
    result = FMOD_Channel_SetPaused(channel, false);

    if(channelIndex)
    {
        *channelIndex = channelIndexTemp;
    }
}


void SoundManager::PlayMusic(int soundIndex, FMOD_CHANNEL **channel)
{
    SoundInstance *soundInstance;
    FMOD_RESULT result;

    if(soundIndex == INVALID_SOUND_INDEX)
    {
        return;
    }

    assert((soundIndex > 0) &&
        (soundIndex < (int)soundInstanceVector->capacity()));

    soundInstance = soundInstanceVector->at(soundIndex);
    result = FMOD_System_PlaySound(system, soundInstance->fmodSound, FMOD_CHANNEL_FREE, false,
        channel);

    if(result != FMOD_OK)
    {
        Ogre::LogManager::getSingleton().logMessage(
            String("SoundManager::PlaySound could not play sound  FMOD Error:") +
            FMOD_ErrorString(result));
        return;
    }
}


SoundInstance *SoundManager::GetSoundInstance(int soundIndex)
{
    return soundInstanceVector->at(soundIndex);
}


FMOD_CHANNEL *SoundManager::GetSoundChannel(int channelIndex)
{
    if(channelIndex == INVALID_SOUND_CHANNEL)
    {
        return nullptr;
    }

    FMOD_CHANNEL *soundChannel;

    assert((channelIndex > 0) && (channelIndex < MAX_SOUND_CHANNELS));

    FMOD_System_GetChannel(system, channelIndex, &soundChannel);
    return soundChannel;
}


void SoundManager::Set3DMinMaxDistance(int channelIndex, float minDistance,
    float maxDistance)
{
    FMOD_RESULT    result;
    FMOD_CHANNEL *channel;

    if(channelIndex == INVALID_SOUND_CHANNEL)
    {
        return;
    }

    result = FMOD_System_GetChannel(system, channelIndex, &channel);

    if(result == FMOD_OK)
    {
        FMOD_Channel_Set3DMinMaxDistance(channel, minDistance, maxDistance);
    }
}



void SoundManager::StopAllSounds(void)
{
    int            channelIndex;
    FMOD_RESULT    result;
    FMOD_CHANNEL *nextChannel;

    for(channelIndex = 0; channelIndex < MAX_SOUND_CHANNELS; channelIndex++)
    {
        result = FMOD_System_GetChannel(system, channelIndex, &nextChannel);

        if((result == FMOD_OK) && (nextChannel != nullptr))
        {
            FMOD_Channel_Stop(nextChannel);
        }

        channelArray[channelIndex].Clear();
    }
}


void SoundManager::StopSound(int *channelIndex)
{
    if(*channelIndex == INVALID_SOUND_CHANNEL)
    {
        return;
    }

    FMOD_CHANNEL *soundChannel;

    assert((*channelIndex > 0) && (*channelIndex < MAX_SOUND_CHANNELS));

    FMOD_System_GetChannel(system, *channelIndex, &soundChannel);
    FMOD_Channel_Stop(soundChannel);

    channelArray[*channelIndex].Clear();
    *channelIndex = INVALID_SOUND_CHANNEL;
}



int SoundManager::FindSound(String &fileName, SOUND_TYPE soundType)
{
    int            vectorIndex;
    int            vectorCapacity = (int)soundInstanceVector->capacity();
    SoundInstance *nextSoundInstance;

    for(vectorIndex = 0; vectorIndex < vectorCapacity; vectorIndex++)
    {
        nextSoundInstance = soundInstanceVector->at(vectorIndex);

        if((soundType == nextSoundInstance->soundType) &&
            (fileName == nextSoundInstance->fileName))
        {
            //      if ((soundType == nextSoundInstance->soundType) && (fileName == nextSoundInstance->fileArchive->getName()))
            return vectorIndex;
        }
    }

    return INVALID_SOUND_INDEX;
}



float SoundManager::GetSoundLength(int soundIndex)
{
    if(soundIndex == INVALID_SOUND_INDEX)
    {
        return 0.0;
    }

    assert((soundIndex > 0) &&
        (soundIndex < (int)soundInstanceVector->capacity()));

    unsigned int   soundLength;   // length in milliseconds
    FMOD_RESULT    result;
    SoundInstance *soundInstance;

    soundInstance = soundInstanceVector->at(soundIndex);

    if(soundInstance)
    {
        result = FMOD_Sound_GetLength(soundInstance->fmodSound, &soundLength,
            FMOD_TIMEUNIT_MS);

        if(result != FMOD_OK)
        {
            Ogre::LogManager::getSingleton().logMessage(
                String("SoundManager::GetSoundLength could not get length  FMOD Error:") +
                FMOD_ErrorString(result));
            return 0.0;
        }
    }
    else
    {
        Ogre::LogManager::getSingleton().logMessage(
            String("SoundManager::GetSoundLength could not find soundInstance"));
        return 0.0;
    }

    return (float)soundLength / 1000.0f;
}

void SoundManager::clearChannelsConnectedToSceneNode(SceneNode *node)
{
    for(int i = 0; i < MAX_SOUND_CHANNELS; i++)
    {
        if(channelArray[i].sceneNode == node)
        {
            channelArray[i].Clear();
        }
    }
}

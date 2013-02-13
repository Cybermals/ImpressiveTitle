#ifndef __SOUNDMANAGER_H__
#define __SOUNDMANAGER_H__

#include "Ogre.h"
#include <fmod.hpp>

#define MAX_SOUND_CHANNELS       30

#define INVALID_SOUND_INDEX      0
#define INVALID_SOUND_CHANNEL    -1


using namespace Ogre;


typedef enum
   {
   SOUND_TYPE_INVALID = 0,
   SOUND_TYPE_3D_SOUND,
   SOUND_TYPE_3D_SOUND_LOOPED,
   SOUND_TYPE_2D_SOUND,
   SOUND_TYPE_2D_SOUND_LOOPED,
   } SOUND_TYPE;


class SoundInstance
   {
public:
   void           Clear(void);
   String         fileName;
   Archive *      fileArchive;
   DataStreamPtr  streamPtr;
   SOUND_TYPE     soundType;
   FMOD::Sound *  fmodSound;
   };


class ChannelInstance
   {
public:
   void              Clear(void);
   Ogre::SceneNode * sceneNode;
   Ogre::Vector3     prevPosition;
   };




class SoundManager : public Ogre::Singleton<SoundManager>
   {
public:
                        SoundManager();
   virtual             ~SoundManager();
   bool                 Initialize(void);
   void                 StopAllSounds(void);
   void                 FrameStarted(Ogre::SceneNode *listenerNode, Ogre::Real timeElapsed);
   void                 FrameStarted(Ogre::Camera *listenerNode, Ogre::Real timeElapsed);

   int                  CreateSound(String &fileName);         // single-shot 3D sound.  returns soundIndex
   int                  CreateStream(String &fileName);        // single-shot 2D stream.  returns soundIndex
   int                  CreateLoopedSound(String &fileName);   // looping 3D sound.  returns soundIndex
   int                  CreateLoopedStream(String &fileName);  // looping 2D stream.  returns soundIndex

   int                  CreateSound(String &fileName, SOUND_TYPE soundType);

   void                 PlaySound(int soundIndex, SceneNode *soundNode, int *channelIndex);
   void					PlayMusic(int soundIndex, FMOD::Channel **channel);
   void                 StopSound(int *channelIndex);
   int                  FindSound(String &fileName, SOUND_TYPE soundType);    // returns soundIndex;

   void                 Set3DMinMaxDistance(int channelIndex, float minDistance, float maxDistance);

   float                GetSoundLength(int soundIndex);        // returns seconds
   SoundInstance       *GetSoundInstance(int soundIndex);
   FMOD::Channel       *GetSoundChannel(int channelIndex);

   static SoundManager& getSingleton(void);
   static SoundManager* getSingletonPtr(void);

   void					clearChannelsConnectedToSceneNode(SceneNode *node);

private:
   typedef std::vector<SoundInstance *>   SoundInstanceVector;
   typedef SoundInstanceVector::iterator  SoundInstanceVectorItr;

   int                  nextSoundInstanceIndex;
   FMOD::System *       system;
   Ogre::Vector3        prevListenerPosition;
   SoundInstanceVector *soundInstanceVector;
   ChannelInstance      channelArray[MAX_SOUND_CHANNELS];

   void                 IncrementNextSoundInstanceIndex(void);

   static FMOD_RESULT F_CALLBACK  fmodFileOpenCallback(const char *fileName, int unicode, unsigned int *filesize, void **handle, void **userdata);
   static FMOD_RESULT F_CALLBACK  fmodFileCloseCallback(void *handle, void *userdata);
   static FMOD_RESULT F_CALLBACK  fmodFileReadCallback(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata);
   static FMOD_RESULT F_CALLBACK  fmodFileSeekCallback(void *handle, unsigned int pos, void *userdata);
   };

#endif // __SOUNDMANAGER_H__
#include <fstream>
#include <sstream>

#include "OgreScriptCompiler.h"

#include "MagixApplication.h"

#include "GameConfig.h"

using namespace Ogre;


//Application Functions
//================================================================================
void MagixApplication::go(void)
{
    if(!setup())
    {
        return;
    }

    mRoot->startRendering();

    // clean up
    destroyScene();
}


bool MagixApplication::setup(void)
{

    String pluginsPath;
    // only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB
#ifdef _DEBUG
    pluginsPath = mResourcePath + "plugins_d.cfg";
#else
    pluginsPath = mResourcePath + "plugins.cfg";
#endif
#endif

    mRoot = new Root(pluginsPath,
        mResourcePath + "ogre.cfg", mResourcePath + "Ogre.log");

    setupResources();

    bool carryOn = configure();

    if(!carryOn)
    {
        return false;
    }

    // Set default mipmap level (NB some APIs ignore this)
    TextureManager::getSingleton().setDefaultNumMipmaps(5);
    chooseSceneManager();
    createCamera();
    createViewports();

    // Create any resource listeners (for loading screens)
    createResourceListener();

    // Load resources
    loadResources();

    // Create the scene
    createScene();
    createFrameListener();
    return true;
}


/** Configures the application - returns false if the user chooses to abandon configuration. */
bool MagixApplication::configure(void)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true);
        mWindow->setDeactivateOnFocusChange(false);
        return true;
    }
    else
    {
        return false;
    }
}


void MagixApplication::chooseSceneManager(void)
{
    // Create the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE);
}


void MagixApplication::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");

    // Position it at 500 in Z direction
    mCamera->setPosition(Vector3(0, 0, 500));

    // Look back along -Z
    mCamera->lookAt(Vector3(0, 0, -300));
    mCamera->setNearClipDistance(5);
}


void MagixApplication::createFrameListener(void)
{
    mFrameListener = new MagixFrameListener(mMagixHandler, mWindow, mCamera);
    mFrameListener->showDebugOverlay(false);
    mRoot->addFrameListener(mFrameListener);
}


void MagixApplication::destroyScene(void) {}    // Optional to override this


void MagixApplication::createViewports(void)
{
    // Create one viewport, entire window
    Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(ColourValue(0, 0, 0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
}


/// Method which will define the source of resources (other than current folder)
void MagixApplication::setupResources(void)
{
    // Create the factory for handling encrypted zips
    mMagixEncryptionZipFactory = new MagixEncryptionZipFactory();
    Ogre::ArchiveManager::getSingleton().addArchiveFactory(
        mMagixEncryptionZipFactory);

    // Load resource paths from config file
    ConfigFile cf;
    cf.load(mResourcePath + "resources.cfg");

    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    String secName, typeName, archName;

    while(seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        /*bool res = (seci.begin() == seci.end());
        if (!res)
        continue;*/
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;

        for(i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;

            //Scan for wildcard filenames like data/terrains/(World.txt)
            if(archName.at(archName.length() - 1) == ')')
            {
                const vector<String>::type tFilenameVect = StringUtil::split(
                    archName, "()");

                if(tFilenameVect.size() > 1)
                {
                    const String tFilename = tFilenameVect[0] + tFilenameVect[1];
                    unsigned int tSize = 0;
                    char *tBuffer;
                    String tData = "";

                    std::ifstream inFile;
                    inFile.open(tFilename.c_str(), std::ifstream::in);

                    if(!inFile.good())
                    {
                        continue;
                    }

                    inFile.seekg(0, std::ios::end);
                    tSize = (unsigned int)inFile.tellg();
                    inFile.seekg(0, std::ios::beg);
                    tBuffer = new char[tSize];
                    strcpy(tBuffer, "");
                    inFile.getline(tBuffer, tSize, '#');
                    inFile.close();
                    tData = tBuffer;
                    delete[] tBuffer;

                    const vector<String>::type tLine = StringUtil::split(tData,
                        "\r\n#");

                    for(int j = 0; j < (int)tLine.size(); j++)
                    {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
                        ResourceGroupManager::getSingleton().addResourceLocation(
                            String(macBundlePath() + "/" + tFilenameVect[0] + tLine[j]), typeName, secName);
#else
                        ResourceGroupManager::getSingleton().addResourceLocation(
                            tFilenameVect[0] + tLine[j], typeName, secName);
#endif
                    }
                    continue;
                }
            }

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
            // OS X does not set the working directory relative to the app,
            // In order to make things portable on OS X we need to provide
            // the loading with it's own bundle path location
            ResourceGroupManager::getSingleton().addResourceLocation(
                String(macBundlePath() + "/" + archName), typeName, secName);
#else
            ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
#endif
        }
    }

    for(int j = 0; j < 2; j++)
    {
        for(int i = 1; i <= 200; i++)
        {
            const String archName = "data/materials/Presets/Preset" +
                StringConverter::toString(j * 200 + i) + ".dat";
            std::ifstream inFile(archName.c_str());

            if(!inFile.good())
            {
                Ogre::LogManager::getSingleton().logMessage(
                    "Preset File not found: " + archName);
                inFile.close();
                break;
            }

            inFile.close();
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
            // OS X does not set the working directory relative to the app,
            // In order to make things portable on OS X we need to provide
            // the loading with it's own bundle path location
            ResourceGroupManager::getSingleton().addResourceLocation(
                String(macBundlePath() + "/" + archName), "Zip", "General");
#else
            ResourceGroupManager::getSingleton().addResourceLocation(
                archName, "Zip", "General");
#endif
        }
    }
}


/// Optional override method where you can create resource listeners (e.g. for loading screens)
void MagixApplication::createResourceListener(void)
{
    mLoadingBar = new MagixLoadingBar();
}


/// Optional override method where you can perform resource group loading
/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
void MagixApplication::loadResources(void)
{
    mLoadingBar->start(mWindow, 2, 1, 1);

    // Initialise, parse scripts etc
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    Ogre::ResourceManager::ResourceMapIterator iter = Ogre::FontManager::getSingleton().getResourceIterator();

    while(iter.hasMoreElements())
    {
        iter.getNext()->load();
    }

    //Calculate number of scripts to generate and setup progress bar for script generation phase.
    int scriptCnt = 4; //faster to update progress for each batch rather than each script
    mLoadingBar->matGenerationStarted("Internal", scriptCnt);

    //Generate materials for solid colored body
    using namespace std;

    fstream f("data/materials/scripts/player/BodyColorTemplate.txt", ios::in);

    if(f.bad())
    {
        LogManager::getSingletonPtr()->logMessage("ERROR: Unable to open body color template.");
        throw exception();
    }

    std::stringstream markingSource;
    markingSource << f.rdbuf();
    ScriptCompiler *compiler = new ScriptCompiler();
    bool skip = false;
    mLoadingBar->matGenerateStarted("Body", skip);

    for(int i = 0; i < GMAXBODIES; i++)
    {
        std::stringstream title;
        string source = StringUtil::replaceAll(markingSource.str(), "%body%", StringConverter::toString(i + 1));
        title << "Body" << i + 1 << "ModColor";
        LogManager::getSingletonPtr()->logMessage("Generating material '" + title.str() + "'...");

        if(!compiler->compile(source, title.str(), "General"))
        {
            LogManager::getSingletonPtr()->logMessage("ERROR: Failed to compile script.");
        }
    }

    f.close();

    //Generate materials for body markings
    f.open("data/materials/scripts/player/BodyMarkTemplate.txt", ios::in);

    if(f.bad())
    {
        LogManager::getSingletonPtr()->logMessage("ERROR: Unable to open body marking template.");
        throw exception();
    }

    markingSource.str("");
    markingSource << f.rdbuf();

    for(int i = 0; i < GMAXBODIES; i++)
    {
        for(int j = 0; j < GMAXBODYMARKS; j++)
        {
            std::stringstream title;
            string source = StringUtil::replaceAll(markingSource.str(), "%body%", StringConverter::toString(i + 1));
            source = StringUtil::replaceAll(source, "%mark%", StringConverter::toString(j + 1));
            title << "Body" << i + 1 << "Mark" << j + 1;
            LogManager::getSingletonPtr()->logMessage("Generating material '" + title.str() + "'...");

            if(!compiler->compile(source, title.str(), "General"))
            {
                LogManager::getSingletonPtr()->logMessage("ERROR: Failed to compile script.");
            }
        }
    }

    f.close();
    mLoadingBar->matGenerateEnded("Body", skip);

    //Generate materials for solid colored head
    f.open("data/materials/scripts/player/HeadColorTemplate.txt", ios::in);

    if(f.bad())
    {
        LogManager::getSingletonPtr()->logMessage("ERROR: Unable to open head color template.");
        throw exception();
    }

    markingSource.str("");
    markingSource << f.rdbuf();
    mLoadingBar->matGenerateStarted("Head", skip);

    for(int i = 0; i < GMAXHEADS; i++)
    {
        std::stringstream title;
        string source = StringUtil::replaceAll(markingSource.str(), "%head%", StringConverter::toString(i + 1));
        title << "Head" << i + 1 << "ModColor";
        LogManager::getSingletonPtr()->logMessage("Generating material '" + title.str() + "'...");

        if(!compiler->compile(source, title.str(), "General"))
        {
            LogManager::getSingletonPtr()->logMessage("ERROR: Failed to compile script.");
        }
    }

    f.close();

    //Generate materials for head markings
    f.open("data/materials/scripts/player/HeadMarkTemplate.txt", ios::in);

    if(f.bad())
    {
        LogManager::getSingletonPtr()->logMessage("ERROR: Unable to open head marking template.");
        throw exception();
    }

    markingSource.str("");
    markingSource << f.rdbuf();

    for(int i = 0; i < GMAXHEADS; i++)
    {
        for(int j = 0; j < GMAXHEADMARKS; j++)
        {
            std::stringstream title;
            string source = StringUtil::replaceAll(markingSource.str(), "%head%", StringConverter::toString(i + 1));
            source = StringUtil::replaceAll(source, "%mark%", StringConverter::toString(j + 1));
            title << "Head" << i + 1 << "Mark" << j + 1;
            LogManager::getSingletonPtr()->logMessage("Generating material '" + title.str() + "'...");

            if(!compiler->compile(source, title.str(), "General"))
            {
                LogManager::getSingletonPtr()->logMessage("ERROR: Failed to compile script.");
            }
        }
    }

    f.close();
    mLoadingBar->matGenerateEnded("Head", skip);

    //Generate materials for tail markings
    f.open("data/materials/scripts/player/TailMarkTemplate.txt", ios::in);

    if(f.bad())
    {
        LogManager::getSingletonPtr()->logMessage("ERROR: Unable to open tail marking template.");
        throw exception();
    }

    markingSource.str("");
    markingSource << f.rdbuf();
    mLoadingBar->matGenerateStarted("Tail", skip);

    //for(int i = 0; i < GMAXTAILS; i++)
    {
        for(int j = 0; j < GMAXTAILMARKS; j++)
        {
            std::stringstream title;
            string source = StringUtil::replaceAll(markingSource.str(), "%tail%", ""); //StringConverter::toString(i + 1));
            source = StringUtil::replaceAll(source, "%mark%", StringConverter::toString(j + 1));
            title << "Tail" << "Mark" << j + 1;
            LogManager::getSingletonPtr()->logMessage("Generating material '" + title.str() + "'...");

            if(!compiler->compile(source, title.str(), "General"))
            {
                LogManager::getSingletonPtr()->logMessage("ERROR: Failed to compile script.");
            }
        }
    }

    f.close();
    mLoadingBar->matGenerateEnded("Tail", skip);

    //Generate materials for solid colored wings
    f.open("data/materials/scripts/player/WingColorTemplate.txt", ios::in);

    if(f.bad())
    {
        LogManager::getSingletonPtr()->logMessage("ERROR: Unable to open wing color template.");
        throw exception();
    }

    markingSource.str("");
    markingSource << f.rdbuf();
    mLoadingBar->matGenerateStarted("Wings", skip);

    for(int i = 0; i < GMAXWINGS - 1; i++)
    {
        std::stringstream title;
        string source = StringUtil::replaceAll(markingSource.str(), "%wing%", StringConverter::toString(i + 1));
        title << "Wing" << i + 1 << "ModColor";
        LogManager::getSingletonPtr()->logMessage("Generating material '" + title.str() + "'...");

        if(!compiler->compile(source, title.str(), "General"))
        {
            LogManager::getSingletonPtr()->logMessage("ERROR: Failed to compile script.");
        }
    }

    f.close();
    mLoadingBar->matGenerateEnded("Wings", skip);
    delete compiler;
    mLoadingBar->finish();
}

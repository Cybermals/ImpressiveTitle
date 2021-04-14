#Minimum CMake version and policy settings
cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0076 NEW)

#Project Name
project(Game)

#Add executable
add_executable(Game)

#Add include dirs
if(WIN32)
    if(MINGW)
        target_include_directories(
            Game
            PUBLIC
            include
            ../config
            ../deps/windows/i386/mingw/Ogre/include/OGRE
            ../deps/windows/i386/mingw/Ogre/include/OIS
            ../deps/windows/i386/mingw/PagedGeometry/include/PagedGeometry
            ../deps/windows/i386/all/fmod/include
            ../deps/windows/i386/mingw/zziplib/include
            ../deps/windows/i386/mingw/RakNet/include
        )
    endif(MINGW)
    
    if(MSVC10)
        target_include_directories(
            Game
            PUBLIC
            include
            ../config
            ../deps/windows/i386/vs2010/Ogre/include/OGRE
            ../deps/windows/i386/vs2010/Ogre/include/OIS
            ../deps/windows/i386/vs2010/PagedGeometry/include/PagedGeometry
            ../deps/windows/i386/all/fmod/include
            ../deps/windows/i386/vs2010/zziplib/include
            ../deps/windows/i386/vs2010/RakNet/include
        )
    endif(MSVC10)
endif(WIN32)

if(UNIX)
    target_include_directories(
        Game
        PUBLIC
        include
        ../config
        ../deps/linux/amd64/gcc9/Ogre/include/OGRE
        ../deps/linux/amd64/gcc9/Ogre/include/OIS
        ../deps/linux/amd64/gcc9/PagedGeometry/include
        ../deps/linux/amd64/all/fmod/include
        ../deps/linux/amd64/gcc9/zziplib/include
        ../deps/linux/amd64/gcc9/RakNet/include
        ../Game/include
    )
endif(UNIX)

#Add link dirs
if(WIN32)
    if(MINGW)
        target_link_directories(
            Game
            PUBLIC
            ../deps/windows/i386/mingw/Ogre/bin/release
            ../deps/windows/i386/mingw/PagedGeometry/lib
            ../deps/windows/i386/all/fmod/bin
            ../deps/windows/i386/mingw/zziplib/lib/Release
            ../deps/windows/i386/mingw/RakNet/bin
        )
    endif(MINGW)
    
    if(MSVC10)
        target_link_directories(
            Game
            PUBLIC
            ../deps/windows/i386/vs2010/Ogre/lib/release
            ../deps/windows/i386/vs2010/PagedGeometry/lib
            ../deps/windows/i386/all/fmod/lib
            ../deps/windows/i386/vs2010/zziplib/lib/Release
            ../deps/windows/i386/vs2010/RakNet/lib
        )
    endif(MSVC10)
endif(WIN32)

if(UNIX)
    target_link_directories(
        Game
        PUBLIC
        ../deps/linux/amd64/gcc9/Ogre/lib
        ../deps/linux/amd64/gcc9/PagedGeometry/lib
        ../deps/linux/amd64/all/fmod/lib
        ../deps/linux/amd64/gcc9/zziplib/lib
        ../deps/linux/amd64/gcc9/RakNet/lib
    )
endif(UNIX)

#Add source code
set(
    SOURCES
    src/DebugTrace.cpp
    src/MagixAlertBox.cpp
    src/MagixAnimated.cpp
    src/MagixApplication.cpp
    src/MagixCamera.cpp
    src/MagixCampaignManager.cpp
    src/MagixCharScreenManager.cpp
    src/MagixChatManager.cpp
    src/MagixCollisionManager.cpp
    src/MagixCritter.cpp
    src/MagixCritterManager.cpp
    src/MagixEffectsManager.cpp
    src/MagixEncryption.cpp
    src/MagixExternalDefinitions.cpp
    src/MagixFrameListener.cpp
    src/MagixGameStateManager.cpp
    src/MagixGUI.cpp
    src/MagixHandler.cpp
    src/MagixIndexedUnit.cpp
    src/MagixInputManager.cpp
    src/MagixItem.cpp
    src/MagixItemManager.cpp
    src/MagixLiving.cpp
    src/MagixLoadingBar.cpp
    src/MagixMiniMap.cpp
    src/MagixNetworkManager.cpp
    src/MagixObject.cpp
    src/MagixOwnershipManager.cpp
    src/MagixPlayer.cpp
    src/MagixSkyManager.cpp
    src/MagixSoundManager.cpp
    src/MagixTimer.cpp
    src/MagixUnit.cpp
    src/MagixUnitManager.cpp
    src/MagixWorld.cpp
    src/main.cpp
    src/MovableTextOverlay.cpp
    src/OgreMagixApp.cpp
    src/OgreMagixFrameListener.cpp
    src/SoundManager.cpp
)

if(WIN32)
    set(
        SOURCES
        ${SOURCES}
        res/ogremagix.rc
    )
endif(WIN32)

target_sources(
    Game
    PRIVATE
    ${SOURCES}
)

#Libraries to link against
set(LIBS
    OgreMain
    OIS
    PagedGeometry
    RakNetDLL
    zziplib
    zlib
)

if(WIN32)
    if(MINGW)
        target_link_libraries(
            Game
            dbghelp
            ws2_32
            ${LIBS}
            fmod
        )
    endif(MINGW)
    
    if(MSVC10)
        target_link_libraries(
            Game
            dbghelp
            ws2_32
            ${LIBS}
            fmod_vc
        )
    endif(MSVC10)
endif(WIN32)

if(UNIX)
    target_link_libraries(
        Game
        ${LIBS}
        fmod
    )
endif(UNIX)

#Add compile flags

if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-stdcall-fixup")
endif(MINGW)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
endif(UNIX)

if(MSVC10)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /TP")
endif(MSVC10)

if(UNIX)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=.")
endif(UNIX)

#Copy deps
if(WIN32)
    if(MINGW)
        file(
            GLOB DEPS
            ../deps/windows/i386/mingw/Ogre/bin/release/*
            ../deps/windows/i386/mingw/PagedGeometry/docs/LICENSE*
            ../deps/windows/i386/all/fmod/bin/*
            ../deps/windows/i386/mingw/RakNet/bin/*
        )
        file(COPY ${DEPS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif(MINGW)
    
    if(MSVC10)
        file(
            GLOB DEPS
            ../deps/windows/i386/vs2010/Ogre/bin/release/*
            ../deps/windows/i386/vs2010/PagedGeometry/docs/LICENSE*
            ../deps/windows/i386/all/fmod/bin/*
            ../deps/windows/i386/vs2010/RakNet/bin/*
        )
        file(COPY ${DEPS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif(MSVC10)
endif(WIN32)

if(UNIX)
    file(
        GLOB DEPS
        ../deps/linux/amd64/gcc9/Ogre/lib/*
        ../deps/linux/amd64/gcc9/PagedGeometry/docs/LICENSE*
        ../deps/linux/amd64/all/fmod/lib/*
        ../deps/linux/amd64/gcc9/RakNet/lib/*
    )
    file(COPY ${DEPS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif(UNIX)

#Copy data files
file(COPY ../data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
    file(GLOB CONFIG ../static/client/config/windows/*)
endif(WIN32)

if(UNIX)
    file(GLOB CONFIG ../static/client/config/linux/*)
endif(UNIX)

file(COPY ${CONFIG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB DAT ../static/client/default/*)
file(COPY ${DAT} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#Install
install(TARGETS Game DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/client)
install(FILES ${DEPS} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/client)
install(DIRECTORY ../data DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/client)
install(FILES ${CONFIG} ${DAT} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/client)
install(DIRECTORY ../static/client/screenshots DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/client)
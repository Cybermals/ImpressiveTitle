#Minimum CMake version and policy settings
cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0076 NEW)

#Project Name
project(GameServer)

#Add executable
add_executable(GameServer)

#Add include dirs
if(WIN32)
    if(MINGW)
        target_include_directories(
            GameServer
            PUBLIC
            include
            ../config
            ../deps/windows/i386/mingw/RakNet/include
            ../Game/include
        )
    endif(MINGW)
    
    if(MSVC10)
        target_include_directories(
            GameServer
            PUBLIC
            include
            ../config
            ../deps/windows/i386/vs2010/RakNet/include
            ../Game/include
        )
    endif(MSVC10)
endif(WIN32)

if(UNIX)
    target_include_directories(
        GameServer
        PUBLIC
        include
        ../config
        ../deps/linux/amd64/gcc/RakNet/include
        ../Game/include
    )
endif(UNIX)

#Add link dirs
if(WIN32)
    if(MINGW)
        target_link_directories(
            GameServer
            PUBLIC
            ../deps/windows/i386/mingw/RakNet/bin
        )
    endif(MINGW)
    
    if(MSVC10)
        target_link_directories(
            GameServer
            PUBLIC
            ../deps/windows/i386/vs2010/RakNet/lib
        )
    endif(MSVC10)
endif(WIN32)

if(UNIX)
    target_link_directories(
        GameServer
        PUBLIC
        ../deps/linux/amd64/gcc/RakNet/bin
    )
endif(UNIX)

#Add source code
target_sources(
    GameServer
    PRIVATE
    src/main.cpp
)

#Libraries to link against
set(LIBS
    RakNetDLL
)

if(WIN32)
    target_link_libraries(
        GameServer
        ws2_32
        ${LIBS}
    )
endif(WIN32)

if(UNIX)
    target_link_libraries(
        GameServer
        ${LIBS}
    )
endif(UNIX)

#Add compile flags
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
            ../deps/windows/i386/mingw/RakNet/bin/*
        )
        file(COPY ${DEPS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif(MINGW)
    
    if(MSVC10)
        file(
            GLOB DEPS
            ../deps/windows/i386/vs2010/RakNet/bin/*
        )
        file(COPY ${DEPS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif(MSVC10)
endif(WIN32)

if(UNIX)
    file(
        GLOB DEPS
        ../deps/linux/amd64/gcc/RakNet/bin/*
    )
    file(COPY ${DEPS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif(UNIX)

#Copy data files
# file(COPY ../static/server/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#Install
install(TARGETS GameServer DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/server)
install(FILES ${DEPS} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/server)
# install(DIRECTORY ../static/server/data DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/server)
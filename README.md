# Retro Impressive Title
 A new and improved build of the classic animal MMORPG originally written by KovuLKD.
See the LEGAL file for details.


## Build Instructions
### Windows
![img](https://i.imgur.com/rMLrraJ.png)
![img](https://i.imgur.com/368cJ02.png)
1. download and install MinGW-w64 8.1.0
2. download and install CMake
3. clone this repo
4. download the media files from the latest release
5. unzip the media files into the project folder
6. modify the settings in the "GameConfig.h" header to your liking
7. add any additional media files to "data" and/or modify existing ones
8. modify any config files in "static/client" to your liking
9. use the "IT XOR v5.exe" tool to encrypt the modified config files and store them
in "static/client/default"
10. modify any config files in "static/client/config" to your liking
11. start CMake and set the source dir to this repo
12. set the binary dir to the folder where you want to build the game
13. set the install prefix to the folder where you want to install the built game
14. click "Configure"
15. click "Generate"
16. open a command prompt window to your binary dir
17. run "mingw32-make install" to build and install the game in your install prefix

### Linux
1. install build-essential, gdb, and cmake via your package manager
2. clone this repo
3. download the media files from the latest release
4. unzip the media files into the project folder
5. modify the settings in the "GameConfig.h" header to your liking
6. add any additional media files to "data" and/or modify existing ones
7. modify any config file in "static/client/config" to your liking
8. open a terminal window
9. execute "cmake -S RetroIT -B RetroIT-gcc -DCMAKE_INSTALL_PREFIX=path -DCMAKE_BUILD_TYPE=Release"
where "path" is the full path to the folder where you want to install the game and servers
10. execute "cmake --build RetroIT-gcc" to build the game and servers
11. execute "cmake --install RetroIT-gcc" to install the game and servers into the
folder you gave earlier


## Operating the Game and Servers
1. in your install prefix you will find "client" and "server" folders
2. start "LoginServer.exe"
3. start "GameServer.exe"
4. start "Game.exe"
5. user data will be stored inside the "server/data" folder
6. when finished playing, simply quit the game
7. to shutdown the servers, switch to the GameServer window and press Ctrl+C
8. switch to the LoginServer window and press Ctrl+C

# Retro Impressive Title
 A new and improved build of the classic animal MMORPG originally written by KovuLKD.
See the LEGAL file for details.


## Build Instructions
### Windows
1. download and install MinGW-w64 8.1.0
2. download and install CMake
3. clone this repo
4. modify the settings in the "GameConfig.h" header to your liking
5. add any additional media files to "data" and/or modify existing ones
6. modify any config files in "static/client" to your liking
7. use the "IT XOR v5.exe" tool to encrypt the modified config files and store them
in "static/client/default"
8. modify any config files in "static/client/config" to your liking
9. start CMake and set the source dir to this repo
10. set the binary dir to the folder where you want to build the game
11. set the install prefix to the folder where you want to install the built game
12. click "Configure"
13. click "Generate"
14. open a command prompt window to your binary dir
15. run "mingw32-make install" to build and install the game in your install prefix

### Linux
* coming soon


## Operating the Game and Servers
1. in your install prefix you will find "client" and "server" folders
2. start "LoginServer.exe"
3. start "GameServer.exe"
4. start "Game.exe"
5. user data will be stored inside the "server/data" folder
6. when finished playing, simply quit the game
7. to shutdown the servers, switch to the GameServer window and press Ctrl+C
8. switch to the LoginServer window and press Ctrl+C

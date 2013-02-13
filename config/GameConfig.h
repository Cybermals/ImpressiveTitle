#ifndef __GameDefines_h_
#define __GameDefines_h_

// 1 to enable
// 0 to disable

// BIG FAT WARNING:
// DO NOT CHANGE MAX_EQUIP ON A SERVER THAT IS ALREADY RUNNING
#define MAX_EQUIP 8 // 9 is max recommended
#define MAX_STASH 20
#define MAX_PARTYMEMBERS 6

// BIG FAT WARNING:
// DO NOT DECREASE THESE VALUES ONCE SET. WILL LEAD TO CRASHES
// Read the names of these defines and they'll make sense (Game Max ____)
#define GMAXHEADS 8
#define GMAXMANES 25
#define GMAXTAILS 7
#define GMAXWINGS 4
#define GMAXTUFTS 12
#define GMAXBODYMARKS 21
#define GMAXHEADMARKS 18
#define GMAXTAILMARKS 9

// CHAT COLORS:
#define CHAT_LOCAL_COLOUR_TOP "1 0 0"
#define CHAT_LOCAL_COLOUR_BOTTOM "0 0 0"
#define CHAT_GENERAL_COLOUR_TOP "0 1 0"
#define CHAT_GENERAL_COLOUR_BOTTOM "0 0 0"
#define CHAT_PARTY_COLOUR_TOP "0 0 1"
#define CHAT_PARTY_COLOUR_BOTTOM "0 0 0"

// STANDARD TEXTS
#define USER_ADMIN_TEXT "ADMIN"
#define USER_MOD_TEXT "MOD"

// HOME/DEFAULT SETTINGS
#define MAP_DEFAULT "Default"
#define MAP_SPAWN 2500,2500 // x,z spawn location

// OVERALL LIGHTING
#define AMBIENT_LIGHT_OUTDOOR 0.75, 0.75, 0.75
#define AMBIENT_LIGHT_INDOOR 0.5, 0.5, 0.5

// Uncomment the next line and change the key
#define XORKEY 0 // Change this key like: 3*(i%2)+3*(i%7)+4
#define CUSTOMCRITTERS 1 // allows users custom critters
#define MAXDIMS 100 // Maximum number of dims

#define SERVER_PORT 40000
#define MAIN_SERVER_PORT 40010

// Uncomment next lines and change them
#define MAIN_SERVER_IP "127.0.0.1" // enter your server address
#define SERVER_PASSWORD "KITO1.7.4" // enter a key

#endif
#ifndef __GameDefines_h_
#define __GameDefines_h_

// 1 to enable
// 0 to disable

//App Title
#define APP_TITLE "Impressive Title"
#define DEFAULT_FONT "Oxanium/Regular"

/// New Version 5
// Enable encrypted items.cfg
// 0 = Reads items.cfg
// 1 = Reads items.dat (like ad1.dat)
#define ENCRYPTED_ITEMS 0

// Enable encrypted zips and your custom file extension (instead of zip - Remember the .)
#define ENCRYPTED_PACKAGES 1
#define ENCRYPTION_EXTENSION ".kzip"
#define ENCRYPTION_XORS { 11, 22, 33, 144, 57, 63, 69, 96, 240, 128 }
//Change the above numbers 1   2   3    4   5   6   7   8    9   10

// BIG FAT WARNING:
// DO NOT CHANGE MAX_EQUIP ON A SERVER THAT IS ALREADY RUNNING
#define MAX_EQUIP 8 // 9 is max recommended
#define MAX_STASH 20
#define MAX_PARTYMEMBERS 6

// BIG FAT WARNING:
// DO NOT DECREASE THESE VALUES ONCE SET. WILL LEAD TO CRASHES
// Read the names of these defines and they'll make sense (Game Max ____)
#define GMAXBODIES 1
#define GMAXHEADS 6
#define GMAXMANES 18         //must include Maneless
#define GMAXTAILS 6
#define GMAXWINGS 4          //must include Wingless
#define GMAXTUFTS 10         //must include Tuftless
#define GMAXBODYMARKS 21     //must include No Markings
#define GMAXHEADMARKS 18     //must include No Markings
#define GMAXTAILMARKS 9      //must include No Markings

// CHAT COLORS:
#define CHAT_GENERAL_COLOUR_TOP "1 1 1"
#define CHAT_GENERAL_COLOUR_BOTTOM ".5 .5 .5"
#define CHAT_LOCAL_COLOUR_TOP "1 0 0"
#define CHAT_LOCAL_COLOUR_BOTTOM ".5 0 0"
#define CHAT_PARTY_COLOUR_TOP "0 1 0"
#define CHAT_PARTY_COLOUR_BOTTOM "0 .5 0"
#define CHAT_PRIVATE_COLOUR_TOP "0 1 1"
#define CHAT_PRIVATE_COLOUR_BOTTOM "0 .5 .5"

// STANDARD TEXTS
#define USER_ADMIN_TEXT "ADMIN"
#define USER_MOD_TEXT "MOD"

// HOME/DEFAULT SETTINGS
#define MAP_DEFAULT "Default"
#define MAP_SPAWN 2500, 2500 // x,z spawn location

// OVERALL LIGHTING
#define AMBIENT_LIGHT_OUTDOOR 0.75, 0.75, 0.75
#define AMBIENT_LIGHT_INDOOR 0.5, 0.5, 0.5

// Uncomment the next line and change the key
#define XORKEY 0 // Change this key like: 3*(i%2)+3*(i%7)+4
#define CUSTOMCRITTERS 1 // allows users custom critters
#define MAXDIMS 100 // Maximum number of dims

//Server
#define SERVER_PORT 40000
#define MAIN_SERVER_PORT 40010
#define SERVER_VERSION_STRING "Impressive Title Game Server v4.0.1\n"

// Uncomment next lines and change them
#define MAIN_SERVER_IP "127.0.0.1" // enter your server address
#define SERVER_PASSWORD "ImpressiveTitle4.0.1" // enter a key

#endif

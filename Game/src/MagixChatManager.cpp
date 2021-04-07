/*
Notes:
MagixChatManager::processInput processes commands typed in the chat.
MagixChatManager::prefixChatLine adds the player name/title to the messages sent.
*/

#include "GameConfig.h"
#include "MagixChatManager.h"


//Chat Manager Functions
//================================================================================
MagixChatManager::MagixChatManager()
{
    reset();
    channel = 0;
    pmTarget = "";
    pmEnter = false;
    doLocalUsername = false;
    doGeneralCharname = false;
}


void MagixChatManager::reset(bool clearHistory)
{
    if(clearHistory)
    {
        for(int i = 0; i < MAX_CHANNELS; i++)
        {
            chatString[i].clear();
            chatSayer[i].clear();
            chatType[i].clear();
            hasNewLine[i] = false;
        }
    }
}


void MagixChatManager::push(const String &caption, const String &sayer,
    const unsigned char &type)
{
    unsigned short tChannel;

    switch(type)
    {
    case CHAT_LOCAL:
    case CHAT_LOCALACTION:
    case CHAT_LOCALEVENT:
        tChannel = 0;
        break;

    case CHAT_PARTY:
    case CHAT_PARTYACTION:
    case CHAT_PARTYEVENT:
        tChannel = 2;
        break;

    case CHAT_PRIVATE:
        tChannel = 3;
        break;

    default:
        tChannel = 1;
        break;
    }

    //Enforce max line width
    String text = caption;
    String sender = sayer;
    int prefixLen = (int)sender.size() + 3;

    while((int)text.size() > 35 - prefixLen)
    {
        //Push next part of message
        chatString[tChannel].push_back(text.substr(0, 
            35 - prefixLen));
        chatSayer[tChannel].push_back(sender);
        chatType[tChannel].push_back(type);

        //Remove message frag from buffer
        text.erase(0, 35 - prefixLen);

        if(prefixLen)
        {
            sender = "";
            prefixLen = 0;
        }
    }

    chatString[tChannel].push_back(text);
    chatSayer[tChannel].push_back(sender);
    chatType[tChannel].push_back(type);

    if((int)chatString[tChannel].size() > MAX_LINES)
    {
        chatString[tChannel].erase(chatString[tChannel].begin(),
            chatString[tChannel].begin() + 1);
        chatSayer[tChannel].erase(chatSayer[tChannel].begin(),
            chatSayer[tChannel].begin() + 1);
        chatType[tChannel].erase(chatType[tChannel].begin(),
            chatType[tChannel].begin() + 1);
    }

    hasNewLine[tChannel] = true;
}


const String MagixChatManager::getChatBlock(const unsigned short &lines,
    const Real &boxWidth, const Real &charHeight, const Real &lastOffset)
{
    //unsigned char channel = 0; //always get channel 0 text
    vector<String>::type tChatBlock;
    tChatBlock.clear();
    String tFinalChat = "";
    unsigned short tFinalLines = 0;
    int start = int((chatString[channel].size() - lines) * lastOffset);

    if(start < 0)
    {
        start = 0;
    }

    //Process entire chatblock within estimated range
    for(int i = start; i < int(chatString[channel].size()); i++)
    {
        const String tLineBlock = prefixChatLine(processChatString(
            chatString[channel][i], chatSayer[channel][i], 
            chatType[channel][i], boxWidth, charHeight), 
            chatSayer[channel][i], chatType[channel][i]);
        vector<String>::type tCaption = StringUtil::split(tLineBlock, "\n");

        for(int j = 0; j < int(tCaption.size()); j++)
        {
            tChatBlock.push_back(tCaption[j]);
        }
    }

    //Process actual block to return
    start = 0;

    if(start < 0)
    {
        start = 0;
    }

    for(int i = start; i < int(tChatBlock.size()); i++)
    {
        tFinalChat += (i == start ? "" : "\n");
        tFinalChat += tChatBlock[i];
        tFinalLines += 1;

        if(tFinalLines >= lines)
        {
            return tFinalChat;
        }
    }

    return tFinalChat;
}


void MagixChatManager::processInput(String &caption, unsigned char &type,
    String &param)
{
    param = "";

    switch(channel)
    {
    case 0:
        type = CHAT_LOCAL;
        break;

    case 1:
        type = CHAT_GENERAL;
        break;

    case 2:
        type = CHAT_PARTY;
        break;

    case 3:
        type = CHAT_PRIVATE;
        param = pmTarget;
        break;
    }

    //Set home point
    if(StringUtil::startsWith(caption, COMMAND_SETHOME))
    {
        type = CHAT_COMMAND;
        param = COMMAND_SETHOME;
        return;
    }
    //Go home point
    else if(StringUtil::startsWith(caption, COMMAND_GOHOME))
    {
        type = CHAT_COMMAND;
        param = COMMAND_GOHOME;
        return;
    }
    //Reset home point
    else if(StringUtil::startsWith(caption, COMMAND_RESETHOME))
    {
        type = CHAT_COMMAND;
        param = COMMAND_RESETHOME;
        return;
    }
    //get position
    else if(StringUtil::startsWith(caption, COMMAND_POSITION))
    {
        type = CHAT_COMMAND;
        param = COMMAND_POSITION;
        return;
    }
    //friend
    else if(StringUtil::startsWith(caption, COMMAND_FRIEND))
    {
        caption.erase(0, 8);
        type = CHAT_COMMAND;
        param = COMMAND_FRIEND;
        return;
    }
    //unfriend
    else if(StringUtil::startsWith(caption, COMMAND_UNFRIEND))
    {
        caption.erase(0, 10);
        type = CHAT_COMMAND;
        param = COMMAND_UNFRIEND;
        return;
    }
    //block
    else if(StringUtil::startsWith(caption, COMMAND_BLOCK))
    {
        caption.erase(0, 7);
        type = CHAT_COMMAND;
        param = COMMAND_BLOCK;
        return;
    }
    //unblock
    else if(StringUtil::startsWith(caption, COMMAND_UNBLOCK))
    {
        caption.erase(0, 9);
        type = CHAT_COMMAND;
        param = COMMAND_UNBLOCK;
        return;
    }
    //kick
    else if(StringUtil::startsWith(caption, COMMAND_KICK))
    {
        caption.erase(0, 6);
        type = CHAT_COMMAND;
        param = COMMAND_KICK;
        return;
    }
    //roll
    else if(StringUtil::startsWith(caption, COMMAND_ROLL))
    {
        caption.erase(0, 6);
        type = CHAT_COMMAND;
        param = COMMAND_ROLL;
        return;
    }
    //createitem
    else if(StringUtil::startsWith(caption, COMMAND_CREATEITEM))
    {
        caption.erase(0, 12);
        type = CHAT_COMMAND;
        param = COMMAND_CREATEITEM;
        return;
    }
    //godspeak
    else if(StringUtil::startsWith(caption, COMMAND_GODSPEAK))
    {
        caption.erase(0, 10);
        type = CHAT_COMMAND;
        param = COMMAND_GODSPEAK;
        return;
    }
    //modon/off
    else if(StringUtil::startsWith(caption, COMMAND_MODON))
    {
        type = CHAT_COMMAND;
        param = COMMAND_MODON;
        return;
    }
    else if(StringUtil::startsWith(caption, COMMAND_MODOFF))
    {
        type = CHAT_COMMAND;
        param = COMMAND_MODOFF;
        return;
    }
    //Earthquakeeeeeee
    else if(StringUtil::startsWith(caption, COMMAND_EARTHQUAKE))
    {
        type = CHAT_COMMAND;
        param = COMMAND_EARTHQUAKE;
        return;
    }
    //Clear All history
    else if(StringUtil::startsWith(caption, COMMAND_CLEARALL))
    {
        type = CHAT_COMMAND;
        param = COMMAND_CLEARALL;
        reset(true);
        return;
    }
    //Clear history
    else if(StringUtil::startsWith(caption, COMMAND_CLEAR))
    {
        type = CHAT_COMMAND;
        param = COMMAND_CLEAR;
        chatString[channel].clear();
        chatSayer[channel].clear();
        chatType[channel].clear();
        hasNewLine[channel] = false;
        return;
    }
    //Lipsync :U
    else if(StringUtil::startsWith(caption, COMMAND_LIPSYNC))
    {
        caption.erase(0, 9);
        type = CHAT_LIPSYNC;
        return;
    }
    //Goto
    else if(StringUtil::startsWith(caption, COMMAND_GOTO))
    {
        caption.erase(0, 6);
        type = CHAT_COMMAND;
        param = COMMAND_GOTO;
        return;
    }
    //Spawn
    else if(StringUtil::startsWith(caption, COMMAND_SPAWN))
    {
        caption.erase(0, 7);
        type = CHAT_COMMAND;
        param = COMMAND_SPAWN;
        return;
    }
    //Where
    else if(StringUtil::startsWith(caption, COMMAND_WHERE))
    {
        caption.erase(0, 7);
        type = CHAT_COMMAND;
        param = COMMAND_WHERE;
        return;
    }
    //Whois
    else if(StringUtil::startsWith(caption, COMMAND_WHOIS))
    {
        caption.erase(0, 7);
        type = CHAT_COMMAND;
        param = COMMAND_WHOIS;
        return;
    }
    //Release pet
    else if(StringUtil::startsWith(caption, COMMAND_RELEASEPET))
    {
        type = CHAT_COMMAND;
        param = COMMAND_RELEASEPET;
        return;
    }
    //Pet follow
    else if(StringUtil::startsWith(caption, COMMAND_PETFOLLOW))
    {
        type = CHAT_COMMAND;
        param = COMMAND_PETFOLLOW;
        return;
    }
    //Commands
    else if(StringUtil::startsWith(caption, COMMAND_COMMANDS))
    {
        type = CHAT_COMMAND;
        param = COMMAND_COMMANDS;
        return;
    }
    //Massblock
    else if(StringUtil::startsWith(caption, COMMAND_MASSBLOCK))
    {
        caption.erase(0, 11);
        type = CHAT_COMMAND;
        param = COMMAND_MASSBLOCK;
        return;
    }
    //Ban
    else if(StringUtil::startsWith(caption, COMMAND_BAN))
    {
        caption.erase(0, 5);
        type = CHAT_COMMAND;
        param = COMMAND_BAN;
        return;
    }
    //Party
    else if(StringUtil::startsWith(caption, COMMAND_PARTY))
    {
        caption.erase(0, 7);
        type = CHAT_COMMAND;
        param = COMMAND_PARTY;
        return;
    }
    //Savechat
    else if(StringUtil::startsWith(caption, COMMAND_SAVECHAT))
    {
        if(caption.length() > 10)caption.erase(0, 10);
        else caption = "";
        type = CHAT_COMMAND;
        param = COMMAND_SAVECHAT;
        return;
    }
    //Action
    else if(StringUtil::startsWith(caption, "/me "))
    {
        caption.erase(0, 4);

        switch(type)
        {
        case CHAT_PARTY:
            type = CHAT_PARTYACTION;
            break;

        case CHAT_LOCAL:
            type = CHAT_LOCALACTION;
            break;

        default:
            type = CHAT_ACTION;
            break;
        }

        return;
    }
    //spawnitem
    else if(StringUtil::startsWith(caption, COMMAND_SPAWNITEM))
    {
        caption.erase(0, 11);
        type = CHAT_COMMAND;
        param = COMMAND_SPAWNITEM;
        return;
    }
    //setdim
    else if(StringUtil::startsWith(caption, COMMAND_SETDIM))
    {
        caption.erase(0, 8);
        type = CHAT_COMMAND;
        param = COMMAND_SETDIM;
        return;
    }
    //Private message
    else if(StringUtil::startsWith(caption, "/"))
    {
        const vector<String>::type tSplit = StringUtil::split(caption, ":", 1);

        if(tSplit.size() > 1 && tSplit[0].length() > 1)
        {
            param = tSplit[0];
            param.erase(0, 1);
            pmTarget = param;

            caption = tSplit[1];
            type = CHAT_PRIVATE;
            channel = 3;
            pmEnter = true;
        }
        else
        {
            type = CHAT_INVALID;
        }

        return;
    }
}


void MagixChatManager::say(MagixUnitManager *unitMgr, MagixUnit *target,
    const String &caption, unsigned char &type)
{
    const String tName = (((!doLocalUsername && (type == CHAT_LOCAL ||
        type == CHAT_LOCALACTION || type == CHAT_LOCALEVENT ||
        type == CHAT_PARTY || type == CHAT_PARTYACTION ||
        type == CHAT_PARTYEVENT)) ||
        (doGeneralCharname && (type == CHAT_GENERAL || type == CHAT_ACTION ||
        type == CHAT_EVENT)) ||
        target->getUser() == "") ? target->getName() : target->getUser());

    if(channel == 3)
    {
        type = CHAT_PRIVATE;
    }

    if((type != CHAT_LOCAL && type != CHAT_LOCALACTION) ||
        target->getBodyEnt()->isVisible())
    {
        push(caption, tName, type);
    }

    if((type == CHAT_LOCAL || type == CHAT_PARTY) &&
        target->getBodyEnt()->isVisible())
    {
        unitMgr->createChatBubble(target, caption);
    }
}


void MagixChatManager::message(const String &caption, const unsigned char &type)
{
    if(type == 0)
    {
        unsigned char type;

        switch(channel)
        {
        case 0:
            type = CHAT_LOCAL;
            break;

        case 1:
            type = CHAT_GENERAL;
            break;

        case 2:
            type = CHAT_PARTY;
            break;

        case 3:
            type = CHAT_PRIVATE;
            break;

        default:
            type = CHAT_INVALID;
            break;
        }

        push(caption, "", type);
    }
    else
    {
        push(caption, "", type);
    }
}


bool MagixChatManager::popHasNewLine(const unsigned char &chan)
{
    const bool tNewLine = hasNewLine[chan];
    hasNewLine[chan] = false;
    return tNewLine;
}


bool MagixChatManager::getHasNewLine(const unsigned char &chan)
{
    return hasNewLine[chan];
}


const String MagixChatManager::prefixChatLine(const String &caption,
    const String &sayer, const unsigned char &type, bool isPostfixLength)
{
    //Prefix
    String tLine = "";

    if(sayer != "" && !isPostfixLength)
    {
        if(type == CHAT_ADMIN)
        {
            tLine += "(" USER_ADMIN_TEXT ")";
        }
        else if(type == CHAT_MOD)
        {
            tLine += "(" USER_MOD_TEXT ")";
        }

        if(type == CHAT_ACTION || type == CHAT_PARTYACTION ||
            type == CHAT_LOCALACTION)
        {
            tLine += sayer + " ";
        }
        else if(type == CHAT_PRIVATE)
        {
            tLine += "((" + sayer + ")) ";
        }
        else if(type == CHAT_EVENT || type == CHAT_LOCALEVENT ||
            type == CHAT_PARTYEVENT)
        {
            tLine += "[" + sayer + " ";
        }
        else
        {
            tLine += "<" + sayer + "> ";
        }
    }

    //Caption
    tLine += caption;

    //Postfix
    if(sayer != "" && (caption != "" || isPostfixLength))
    {
        if(type == CHAT_EVENT || type == CHAT_LOCALEVENT ||
            type == CHAT_PARTYEVENT)
        {
            tLine += "]";
        }
    }

    return tLine;
}


const unsigned short MagixChatManager::getPrefixLength(const String &sayer, 
    const unsigned char &type)
{
    return (int)prefixChatLine("", sayer, type, false).length();
}


const unsigned short MagixChatManager::getPostfixLength(const String &sayer, 
    const unsigned char &type)
{
    return (int)prefixChatLine("", sayer, type, true).length();
}


const String MagixChatManager::processChatString(const String &caption, 
    const String &sayer, const unsigned char &type, const Real &boxWidth, 
    const Real &charHeight)
{
    String tCaption = prefixChatLine(caption, sayer, type);
    //String tPrefix = prefixChatLine("",sayer,type);
    const unsigned short tPrefix = getPrefixLength(sayer, type);
    const unsigned short tPostfix = getPostfixLength(sayer, type);

    if(tCaption.length() > 0)
    {
        //size caption
        const Font *pFont = dynamic_cast<Ogre::Font*>(
            Ogre::FontManager::getSingleton().getByName(DEFAULT_FONT).getPointer());
        const Real tHeight = charHeight;

        int tSpacePos = -1;
        Real tTextWidth = 0;
        Real tWidthFromSpace = 0;

        for(int i = 0; i < int(tCaption.length()); i++)
        {
            if(tCaption[i] == 0x0020)
            {
                tTextWidth += pFont->getGlyphAspectRatio(0x0030);
                tWidthFromSpace += pFont->getGlyphAspectRatio(0x0030);
            }
            else
            {
                tTextWidth += pFont->getGlyphAspectRatio(tCaption[i]);
                tWidthFromSpace += pFont->getGlyphAspectRatio(tCaption[i]);
            }

            if(tCaption[i] == ' ' && i > tPrefix)
            {
                tSpacePos = i;
                tWidthFromSpace = pFont->getGlyphAspectRatio(0x0030);
            }

            if(tTextWidth * tHeight >= boxWidth)
            {
                if(tSpacePos != -1)
                {
                    tCaption[tSpacePos] = '\n';
                    tTextWidth = tWidthFromSpace;
                    tSpacePos = -1;
                }
                else
                {
                    tCaption.insert(i + 1, String(1, '\n'));
                    tTextWidth = 0;
                }
            }
        }
    }

    tCaption.erase(0, tPrefix);
    tCaption.erase(tCaption.length() - tPostfix, tPostfix);
    return tCaption;
}


void MagixChatManager::toggleChannel()
{
    channel++;

    if(channel >= MAX_CHANNELS)
    {
        channel = 0;
    }
}


void MagixChatManager::setChannel(const unsigned char &value)
{
    channel = value;

    if(channel >= MAX_CHANNELS)
    {
        channel = MAX_CHANNELS - 1;
    }
}


const unsigned char MagixChatManager::getChannel()
{
    return channel;
}


const vector<unsigned char>::type MagixChatManager::getOtherChannels()
{
    vector<unsigned char>::type tChannels;

    for(int i = 0; i < MAX_CHANNELS; i++)
    {
        if(i != channel)
        {
            tChannels.push_back(i);
        }
    }

    return tChannels;
}


void MagixChatManager::saveChatLog(const String &directory, 
    const String &filename)
{
    std::ofstream outFile;
    const String tFilename = directory + filename;
    outFile.open(tFilename.c_str());

    if(outFile.good())
    {
        for(int i = 0; i < (int)chatString[channel].size(); i++)
        {
            const String tBuffer = prefixChatLine(chatString[channel][i],
                chatSayer[channel][i], chatType[channel][i]) + "\n";
            outFile.write(tBuffer.c_str(), (int)tBuffer.length());
        }
    }

    outFile.close();
    message("Saved chat log as " + filename + ".");
}

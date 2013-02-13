#ifndef __MagixInputManager_h_
#define __MagixInputManager_h_

#define INPUT_CONTROL 1
#define INPUT_TEXT 2

#define MAX_TEMPSTORAGE 5

#include "MagixGUI.h"
#include <OIS.h>

using namespace Ogre;

class MagixInputManager
{
protected:
	OverlayElement *mInputBoxText;
	unsigned short inputMode;
	String inputText;
	OIS::KeyCode heldKey;
	unsigned int heldKeyText;
	Real heldDelay;
	unsigned short inputCursorPos;
	String defaultCaption;
	bool clearAtReturn;
	bool allowTempStorage;
	bool allowNewLine;
	bool hideText;
	unsigned short charLimit;
	bool isShiftDown;
	String tempStorage[MAX_TEMPSTORAGE];
	short tempStoragePos;
	int heldHotkey;
public:
	MagixInputManager()
	{
		mInputBoxText = 0;
		inputMode = INPUT_CONTROL;
		inputText = "";
		heldKey = OIS::KC_RETURN;
		heldKeyText = 0;
		heldDelay = 0;
		inputCursorPos = 0;
		defaultCaption = "";
		clearAtReturn = false;
		allowTempStorage = false;
		allowNewLine = false;
		hideText = false;
		charLimit = 0;
		for(int i=0;i<MAX_TEMPSTORAGE;i++)
		{
			tempStorage[i] = "";
		}
		tempStoragePos = -1;
		heldHotkey = 0;
	}
	~MagixInputManager()
	{
	}
	void update(const FrameEvent evt)
	{
		if(heldDelay>0)
		{
			heldDelay -= evt.timeSinceLastFrame;
			if(heldDelay<=0)
			{
				heldDelay = 0.02;
				inputHeldKey();
			}
		}
	}
	bool toggleInputMode()
	{
		bool sayFlag = false;

		switch(inputMode)
		{
			case INPUT_CONTROL:
				inputMode = INPUT_TEXT;
				inputCursorPos = int(inputText.length());
				inputText = inputText + "|";
				tempStoragePos = -1;
				setInputText(inputText,true);
			break;
			case INPUT_TEXT:
				//Newline
				if(isShiftDown && allowNewLine)
				{
					pushChar('\n');
					return false;
				}
				inputMode = INPUT_CONTROL;
				inputText.erase(inputCursorPos,1);
				if(inputText.length()>0)sayFlag = true;
				if(clearAtReturn)setInputText("",false);
				else setInputText(inputText,true);
				heldDelay = 0;
			break;
		}
		return sayFlag;
	}
	unsigned short getInputMode()
	{
		return inputMode;
	}
	void setString(const String &caption)
	{
		inputText = caption;
		if(inputText.length() > charLimit)inputText.erase(charLimit,charLimit-inputText.length());
		inputText += "|";
		inputCursorPos = int(inputText.length()-1);
		setInputText(inputText,true);
	}
	void pushChar(const unsigned int &a)
	{
		if((inputText.length()-1)>=charLimit)return;
		tempStoragePos = -1;
		String temp = "";
		temp.push_back(a);
		inputText.insert(inputCursorPos,temp);
		inputCursorPos += 1;
		setInputText(inputText,true);
	}
	void popChar(bool del=false)
	{
		//Delete-type backspace
		if(del)
		{
			if(inputText.length()>1 && inputCursorPos<inputText.length())
			{
				inputText.erase(inputCursorPos+1,1);
			}
		}
		//Normal backspace
		else if(inputText.length()>1 && inputCursorPos>0)
		{
			inputText.erase(inputCursorPos-1,1);
			inputCursorPos -= 1;
		}
		setInputText(inputText,true);
	}
	void setInputCursorPos(const unsigned short &pos)
	{
		if(pos>=0&&pos<int(inputText.length()))
		{
			inputText.erase(inputCursorPos,1);
			inputCursorPos = pos;
			inputText.insert(inputCursorPos,"|");
			setInputText(inputText,true);
		}
	}
	void setInputText(const String &text, bool enable)
	{
		String caption = defaultCaption;
		if(enable && mInputBoxText)
		{
			//Normal text
			if(!hideText)
			{
				caption += text;
				normalizeText(caption,mInputBoxText);

				//limit visible lines
				const Real tHeight = StringConverter::parseReal(mInputBoxText->getParameter("char_height"));
				if(tHeight>0)
				{
					vector<String>::type tLine;
					tLine.clear();
					String tCaption = "";
					for(int i=0;i<(int)caption.length();i++)
					{
						if(caption[i]=='\n')
						{
							tLine.push_back(tCaption);
							tCaption = "";
						}
						else tCaption += caption[i];
					}
					if(tCaption!="")tLine.push_back(tCaption);

					const unsigned short maxInputLines = Math::IFloor(mInputBoxText->getHeight()/tHeight);
					if(tLine.size()>maxInputLines)
					{
						unsigned short cursorLine = int(tLine.size())-1;
						//total length = caption - no of "\n"s - last line
						unsigned short tLength = int(caption.length() - (tLine.size()-1) - tLine[cursorLine].length());
						//find which line cursor is at
						while(tLength>(inputCursorPos+int(defaultCaption.length())) && cursorLine>(maxInputLines-1))
						{
							cursorLine -= 1;
							tLength -= int(tLine[cursorLine].length());
						}
						if(cursorLine<(maxInputLines-1))cursorLine = maxInputLines-1;

						caption = "";
						for(int i=maxInputLines-1; i>=0; i--)
						{
							caption += tLine[cursorLine-i] + (i==0?"":"\n");
						}
					}
				}
			}
			//Hide text
			else
			{
				for(int i=0; i<(int)text.length(); i++)
					caption += (i==inputCursorPos?"|":"*");
			}
		}
		if(mInputBoxText)mInputBoxText->setCaption(enable?caption:"");
	}
	void normalizeText(String &caption, OverlayElement *box)
	{
		if(!box)return;
		//size caption
		const Font *pFont = dynamic_cast<Ogre::Font*>(Ogre::FontManager::getSingleton().getByName("Tahoma").getPointer());
		const Real tHeight = StringConverter::parseReal(box->getParameter("char_height"));

		int tSpacePos = -1;
		Real tTextWidth = 0;
		Real tWidthFromSpace = 0;
		for(int i=0; i<int(caption.length());i++)
		{   
			if(caption[i] == '\n')
			{
				tTextWidth = 0;
				tWidthFromSpace = 0;
				tSpacePos = -1;
			}
			else if(caption[i] == ' ')
			{
				tTextWidth += pFont->getGlyphAspectRatio(0x0030);
				tWidthFromSpace += pFont->getGlyphAspectRatio(0x0030);
			}
			else
			{
				tTextWidth += pFont->getGlyphAspectRatio(caption[i]);
				tWidthFromSpace += pFont->getGlyphAspectRatio(caption[i]);
			}

			if(caption[i] == ' ' && i>int(defaultCaption.length()))
			{
				tSpacePos = i;
				tWidthFromSpace = pFont->getGlyphAspectRatio(0x0030);
			}
			if(tTextWidth*tHeight >= box->getWidth())
			{
				if(tSpacePos!=-1)
				{
					caption[tSpacePos] = '\n';
					tTextWidth = tWidthFromSpace;
					tSpacePos = -1;
				}
				else
				{
					if((i+1)<int(caption.length()))
					if(caption[i+1]!='\n')
					{
						caption.insert(i+1,String(1,'\n'));
						tTextWidth = 0;
					}
				}
			}
		}
	}
	const String getInputText()
	{
		return inputText;
	}
	void keyHeld(OIS::Keyboard *keyboard, const OIS::KeyCode &key, const unsigned int &text, bool held)
	{
		heldKey = key;
		heldKeyText = text;
		if(heldKeyText > '~')heldKeyText = '~';
		#pragma warning(push)
		#pragma warning(disable : 4482)
		isShiftDown = keyboard->isModifierDown(OIS::Keyboard::Modifier::Shift);
		if(!held)
		{
			heldDelay = 0;
			return;
		}
		if(keyboard->isModifierDown(OIS::Keyboard::Modifier::Alt))return;
		if(keyboard->isModifierDown(OIS::Keyboard::Modifier::Ctrl))return;
		#pragma warning(pop)
		heldDelay = 0.5;
		inputHeldKey();
	}
	void inputHeldKey()
	{
		using namespace OIS;

		switch(heldKey)
		{
			case KC_BACK: popChar(); break;
			case KC_DELETE: popChar(true); break;
			case KC_HOME: setInputCursorPos(0); break;
			case KC_END: setInputCursorPos(int(inputText.length())-1); break;
			case KC_LEFT: setInputCursorPos(inputCursorPos-1); break;
			case KC_RIGHT: setInputCursorPos(inputCursorPos+1); break;
			case KC_UP: 
				if(allowTempStorage)
				{
					const String tInput = getTempStorage(true,false);
					if(tInput!="")
					{
						//make current input text as last storage
						inputText.erase(inputCursorPos,1);
						if(inputText.length()>0)pushTempStorage(inputText);
						setString(tInput);
					}
					break;
				}
			case KC_DOWN:
				if(allowTempStorage)
				{
					const String tInput = getTempStorage(false,true);
					if(tInput!="")setString(tInput);
					break;
				}
			//null keys
			case KC_LSHIFT:
			case KC_RSHIFT:
			case KC_LCONTROL:
			case KC_RCONTROL:
			case KC_LMENU:
			case KC_RMENU:
			case KC_LWIN:
			case KC_PGUP:
			case KC_PGDOWN:
			case KC_CAPITAL:
			case KC_DIVIDE:
			case KC_DECIMAL:
			case KC_NUMLOCK:
			case KC_SYSRQ:
			case KC_F1:
			case KC_F2:
			case KC_F3:
			case KC_F4:
			case KC_F5:
			case KC_F6:
			case KC_F7:
			case KC_F8:
			case KC_F9:
			case KC_F10:
			case KC_F11:
			case KC_F12:
			break;
			//numpad keys
			case KC_NUMPAD0: pushChar('0'); break;
			case KC_NUMPAD1: pushChar('1'); break;
			case KC_NUMPAD2: pushChar('2'); break;
			case KC_NUMPAD3: pushChar('3'); break;
			case KC_NUMPAD4: pushChar('4'); break;
			case KC_NUMPAD5: pushChar('5'); break;
			case KC_NUMPAD6: pushChar('6'); break;
			case KC_NUMPAD7: pushChar('7'); break;
			case KC_NUMPAD8: pushChar('8'); break;
			case KC_NUMPAD9: pushChar('9'); break;
			case KC_NUMPADCOMMA: pushChar(','); break;
			case KC_NUMPADEQUALS: pushChar('='); break;
			//different keys (special keyboards)
			case KC_KANJI: pushChar( (isShiftDown?'~':'`') ); break;
			case KC_PREVTRACK: pushChar( (isShiftDown?'+':'=') ); break;
			case KC_AT: pushChar( (isShiftDown?'|':'\\') ); break;
			case KC_COLON: pushChar( (isShiftDown?'\'':'\"') ); break;
			default:
				pushChar(heldKeyText);
			break;
		}
	}
	void setInputBox(OverlayElement *inputBox, const String &caption, unsigned short limit, bool clear, const String &startText, bool bTempStorage, bool bNewLine, bool bHideText)
	{
		mInputBoxText = inputBox;
		defaultCaption = caption;
		charLimit = limit;
		clearAtReturn = clear;
		inputText = startText;
		allowTempStorage = bTempStorage;
		allowNewLine = bNewLine;
		hideText = bHideText;
	}
	OverlayElement* getInputBox()
	{
		return mInputBoxText;
	}
	void setClearAtReturn(bool clear)
	{
		clearAtReturn = clear;
	}
	void pushTempStorage(const String &input)
	{
		//Don't repeat previous storage
		if(tempStorage[0]==input)return;

		for(int i=MAX_TEMPSTORAGE-1;i>0;i--)
		{
			tempStorage[i] = tempStorage[i-1];
		}
		tempStorage[0] = input;
	}
	const String getTempStorage(bool increment=false, bool decrement=false)
	{
		if(increment && tempStoragePos<MAX_TEMPSTORAGE-1)tempStoragePos += 1;
		else if(decrement && tempStoragePos>0)tempStoragePos -= 1;
		if(tempStoragePos>=0 && tempStoragePos<MAX_TEMPSTORAGE)return tempStorage[tempStoragePos];
		return "";
	}
	void hotkeyHeld(const int &key)
	{
		heldHotkey = key;
	}
	const int getHeldHotkey()
	{
		return heldHotkey;
	}

};

#endif
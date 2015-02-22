#pragma once

#define INPUT_CONTROL 1
#define INPUT_TEXT 2

#define MAX_TEMPSTORAGE 5

//#include "MagixGUI.h"
#include <OIS.h>
#include "Ogre.h"

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
	inline std::wstring atow(const std::string& str)
	{
		std::wstringstream wstrm;
		wstrm << str.c_str();
		return wstrm.str();
	}
	MagixInputManager();
	~MagixInputManager();
	void update(const FrameEvent evt);
	bool toggleInputMode();
	unsigned short getInputMode();
	void setString(const String &caption);
	void pushChar(const unsigned int &a);
	void popChar(bool del = false);
	void setInputCursorPos(const unsigned short &pos);
	void setInputText(const String &text, bool enable);
	void normalizeText(String &caption, OverlayElement *box);
	const String getInputText();
	void keyHeld(OIS::Keyboard *keyboard, const OIS::KeyCode &key, const unsigned int &text, bool held);
	void inputHeldKey();
	void setInputBox(OverlayElement *inputBox, const String &caption, unsigned short limit, bool clear, const String &startText, bool bTempStorage, bool bNewLine, bool bHideText);
	OverlayElement* getInputBox();
	void setClearAtReturn(bool clear);
	void pushTempStorage(const String &input);
	const String getTempStorage(bool increment = false, bool decrement = false);
	void hotkeyHeld(const int &key);
	const int getHeldHotkey();

};

#pragma once

//#include "Ogre.h"
#include "MagixAnimated.h"

class MagixLiving : public MagixAnimated
{
protected:
	short maxHP;
	short hp;
	unsigned char alliance;
	bool invulnerable;
public:
	bool isHidden;
	MagixLiving();
	~MagixLiving();
	virtual void setHP(const short &iMaxHP, const Real &ratio = 1);
	virtual void setHPRatio(const Real &ratio);
	virtual bool addHP(const short &value);
	const Real getHPRatio();
	const short getHP();
	const short getMaxHP();
	void setAlliance(const unsigned char &ally);
	const unsigned char getAlliance();
	bool matchAlliance(const unsigned char &ally);
	virtual const OBJECT_TYPE getType();
	bool isInvulnerable();
	void setInvulnerable(bool flag);
	virtual const Real getHeight();
};

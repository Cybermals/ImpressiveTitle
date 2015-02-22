#include "MagixLiving.h"

MagixLiving::MagixLiving()
{
	maxHP = 0;
	hp = 0;
	alliance = 0;
	invulnerable = false;
	isHidden = false;
}
MagixLiving::~MagixLiving()
{
}
void MagixLiving::setHP(const short &iMaxHP, const Real &ratio)
{
	maxHP = iMaxHP;
	hp = (short)(ratio*maxHP);
}
void MagixLiving::setHPRatio(const Real &ratio)
{
	hp = (short)(ratio*maxHP);
}
bool MagixLiving::addHP(const short &value)
{
	if (invulnerable && value<0)return false;
	hp += value;
	if (hp>maxHP)hp = maxHP;
	if (hp<0)hp = 0;
	return (hp == 0);
}
const Real MagixLiving::getHPRatio()
{
	if (maxHP <= 0)return 0;
	return Real(hp) / Real(maxHP);
}
const short MagixLiving::getHP()
{
	return hp;
}
const short MagixLiving::getMaxHP()
{
	return maxHP;
}
void MagixLiving::setAlliance(const unsigned char &ally)
{
	alliance = ally;
}
const unsigned char MagixLiving::getAlliance()
{
	return alliance;
}
bool MagixLiving::matchAlliance(const unsigned char &ally)
{
	if (alliance == ally)return true;
	if (alliance == ALLIANCE_PLAYER)
	{
		if (ally == ALLIANCE_FRIEND)return true;
	}
	if (alliance == ALLIANCE_FRIEND)
	{
		if (ally == ALLIANCE_PLAYER)return true;
	}
	return false;
}
const OBJECT_TYPE MagixLiving::getType()
{
	return OBJECT_CRITTER;
}
bool MagixLiving::isInvulnerable()
{
	return invulnerable;
}
void MagixLiving::setInvulnerable(bool flag)
{
	invulnerable = flag;
}
const Real MagixLiving::getHeight()
{
	return 0;
}
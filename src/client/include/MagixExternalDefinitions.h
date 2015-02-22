#pragma once

#include "MagixNetworkDefines.h"
#include "GameConfig.h"
#include "Ogre.h"
#include <OgreStreamSerialiser.h>
#include <OgreException.h>
//using namespace std;
using namespace Ogre;

typedef vector<const std::pair<String,String>>::type CampaignEvent;
typedef vector<CampaignEvent>::type CampaignEventList;

#define MAX_HOTKEYF 8

struct WeatherEvent
{
	Real start;
	Real end;
	ColourValue skyShader;
	ColourValue skyAdder;
	String type;
	Real rate;
	vector<String>::type effect;
	Real effectFrequency;
	bool isConstant;
	WeatherEvent()
	{
		start = 0;
		end = 7000;
		skyShader = ColourValue::White;
		skyAdder = ColourValue::Black;
		type = "";
		rate = 0;
		effect.clear();
		effectFrequency = 0;
		isConstant = false;
	}
};
struct AttackFX
{
	String bone;
	String trailMat;
	Real trailWidth;
	ColourValue colour;
	ColourValue colourChange;
	String trailHeadMat;
	AttackFX()
	{
		bone = "";
		trailMat = "";
		trailWidth = 2;
		colour = ColourValue(1,0.9,0.75);
		colourChange = ColourValue(7,7,7,7);
		trailHeadMat = "";
	}
};
struct Attack
{
	String name;
	String anim;
	vector<AttackFX>::type FX;
	Real range;
	Vector3 hitForce;
	Vector3 offset;
	Real hpMin;
	Real hpMax;
	bool hitAlly;
	bool autoTarget;
	Vector3 moveForce;
	bool singleTarget;
	Real speedMultiplier;
	bool soundRoar;
	unsigned short attackRange;
	Attack()
	{
		name = "";
		anim = "";
		FX.clear();
		range = 0;
		hitForce = Vector3::ZERO;
		offset = Vector3::ZERO;
		hpMin = 0;
		hpMax = 0;
		hitAlly = false;
		autoTarget = true;
		moveForce = Vector3::ZERO;
		singleTarget = false;
		speedMultiplier = 1;
		soundRoar = false;
		attackRange = 0;
	}
};

struct CritterAttack
{
	Real hp;
	Real range;
	Vector3 hitForce;
	bool hitAlly;
	CritterAttack(const Real & h, const Real &r, const Vector3 &hF, bool hA=false)
	{
		hp = h;
		range = r;
		hitForce = hF;
		hitAlly = hA;
	}
};

struct Critter
{
	String type;
	String mesh;
	Real hp;
	bool flying;
	bool friendly;
	bool invulnerable;
	bool isDrawPoint;
	bool isUncustomizable;
	vector<const std::pair<String,Real>>::type dropList;
	std::pair<String,unsigned char> skillDrop;
	Real maxSpeed;
	unsigned char decisionMin;
	unsigned char decisionDeviation;
	Real scale;
	vector<const CritterAttack>::type attackList;
	String sound;
	String material;
	Critter()
	{
		type = "";
		mesh = "";
		hp = 100;
		flying = false;
		friendly = false;
		invulnerable = false;
		isDrawPoint = false;
		isUncustomizable = false;
		dropList.clear();
		skillDrop = std::pair<String,unsigned char>("",0);
		maxSpeed = 100;
		decisionMin = 1;
		decisionDeviation = 9;
		scale = 1;
		attackList.clear();
		sound = "";
		material = "";
	}
};

struct WorldCritter
{
	String type;
	Real spawnRate;
	bool hasRoamArea;
	unsigned short roamAreaID;
	WorldCritter(const String &t="", const Real &sR=0, bool hRA=false, const unsigned short &rAID=0)
	{
		type = t;
		spawnRate = sR;
		hasRoamArea = hRA;
		roamAreaID = rAID;
	}
};

class MagixExternalDefinitions
{
protected:
	String lastBioName;
	String hotkeyF[MAX_HOTKEYF];
public:
	bool isAdmin;
	bool isMod;
	bool isModOn;
	ColourValue adminAuraColour;
	bool hasVertexProgram;
	bool hasFragmentProgram;
	bool pagedGeometryOn;
	bool pagedGeometryForced;
	Real viewDistance;
	unsigned short maxHeads;
	unsigned short maxManes;
	unsigned short maxTails;
	unsigned short maxWings;
	unsigned short maxTufts;
	unsigned short maxBodyMarks;
	unsigned short maxHeadMarks;
	unsigned short maxTailMarks;
	vector<String>::type headMesh;
	vector<String>::type maneMesh;
	vector<String>::type tailMesh;
	vector<String>::type wingMesh;
	vector<String>::type tuftMesh;
	unsigned short maxEmotes;
	vector<String>::type emoteName;
	vector<String>::type emoteAnims;
	vector<Real>::type emoteEyelidState;
	unsigned short maxItems;
	vector<String>::type itemMesh;
	vector<String>::type itemBone;
	vector<String>::type itemName;
	vector<bool>::type itemIsPrivate;
	vector<bool>::type itemHasOffset;
	vector<bool>::type itemHasAltAnim;
	vector<String>::type itemParticle;
	vector<String>::type itemParticleOffset;
	vector<bool>::type itemParticleOnNode;
	vector<const Attack>::type attackList;
	vector<const Critter>::type critterList;

	MagixExternalDefinitions()
	{
		lastBioName = "";
		isAdmin = false;
		isMod = false;
		isModOn = false;
		adminAuraColour = ColourValue(1,1,1);
		pagedGeometryOn = false;
		pagedGeometryForced = false;
		viewDistance = 1;
		hasVertexProgram = false;
		hasFragmentProgram = false;
		maxHeads = 0;
		maxManes = 0;
		maxTails = 0;
		maxWings = 0;
		maxTufts = 0;
		maxBodyMarks = 0;
		maxHeadMarks = 0;
		maxTailMarks = 0;
		maxEmotes = 0;
		maxItems = 0;
	}
	~MagixExternalDefinitions()
	{
	}

	void initialize();

	void initializeCapabilities(const RenderSystemCapabilities *capabilities);

	void loadUnitMeshes(const String &filename);

	void loadUnitEmotes(const String &filename);

	bool isRestricted(const short &headID, const short &maneID);

	bool isRestrictedTuft(const short &tuftID);

	void loadSettings(bool &isTablet, Real &mouseSensitivity, Real &musicVol, Real &guiVol, Real &soundVol, bool &isShadowsOn, bool &isBloom, String &username, bool &generalCharname, bool &localUsername, bool &channelBlink, bool &runMode, bool &doubleJumpOn, bool &autoLipSync, bool &useWindowsCursor);

	void saveSettings(bool isTablet, const Real &mouseSensitivity, const Real &musicVol, const Real &guiVol, const Real &soundVol, const bool &isShadowsOn, bool isBloom, const String &username = "", bool generalCharname = false, bool localUsername = false, bool channelBlink = true, bool runMode = true, bool doubleJumpOn = true, bool autoLipSync = false, bool useWindowsCursor = false);

	void savePassword(bool flag, const String &password = "");

	const String loadPassword();

	void saveBanFile(bool flag, const unsigned short &numDays = 0);

	const bool loadBanFile(unsigned short &numDays);

	vector<const String>::type XORInternal(const String inFile, bool preChecksum = false);

	const String XOR7(const String &input, unsigned long *checksum = 0, bool preChecksum = false);

	bool XOR7FileGen(const String &infile, const String &outfile, bool decrypt, bool checksum = false);

	bool loadWorld(const String &filename, String &terrain, Real &x, Real &z, Vector3 &spawnSquare, Vector2 &worldBounds);

	const String loadWorldObjects(const String &filename);

	void loadCampaignList(vector<String>::type &list, bool customCampaigns = false);

	bool loadCampaign(const String &name, CampaignEventList &data, String &fileName, bool customCampaigns = false);

	void loadCampaignScript(const String &filename, const vector<const String>::type &nextEvent, CampaignEventList &data, const unsigned short &eventCount = 0, bool loadFirstSection = false);

	bool loadCredits(const String &filename, vector<String>::type &credits);

	void decrypt(String &input);

	void encrypt(String &input);

	bool loadSavePoint(String &mapName, Vector2 &point, unsigned char &dimension);

	bool loadHomePoint(String &mapName, Vector2 &point, unsigned char &dimension, bool isSavePoint = false);

	void saveSavePoint(const String &mapName, const Vector2 &point, const unsigned char &dimension);

	void saveHomePoint(const String &mapName, const Vector2 &point, const unsigned char &dimension, bool isSavePoint = false);

	const vector<String>::type loadFriendList(bool isBlockList = false);

	const vector<String>::type loadBlockedList();

	void editFriendList(const String &name, bool add, bool isBlockList = false);

	void editBlockedList(const String &name, bool add);

	bool isInFriendList(String name, bool isBlockList = false);

	bool isInBlockedList(String name);

	void saveBio(const String &name, const String &bio);

	const String loadBio(const String &name = "");

	const String loadUpdateCaption();

	void loadItems(const String &filename, const bool decrypt);

	const String getItemName(const String &meshName);

	const String getItemBone(const String &meshName);

	const bool getItemIsPrivate(const String &meshName);

	const bool getItemHasOffset(const String &meshName);

	const bool getItemHasAltAnim(const String &meshName);

	const String getItemParticle(const String &meshName);

	const std::pair<Vector3, bool> getItemParticleOffset(const String &meshName);

	const bool itemMeshExists(const String &meshName);

	void loadHotkeys(const String &filename);

	void saveHotkeys(const String &filename);

	void setHotkeyF(const int &key, const String &value);

	const String getHotkeyF(const int &key);

	bool hasManeMesh(const String &mesh);

	bool hasTailMesh(const String &mesh);

	bool hasWingMesh(const String &mesh);

	bool hasTuftMesh(const String &mesh);

	bool hasBodyMark(const unsigned char &id);

	bool hasHeadMark(const unsigned char &id);

	bool hasTailMark(const unsigned char &id);

	void loadWeatherCycle(const String &type, vector<const WeatherEvent>::type &list, bool isCustom = false);

	void loadWeather(const String &type, String &particle, Vector3 &offset, String &sound, bool isCustom = false);

	void loadAttacks(const String &filename, bool isCustom);

	const Attack getAttack(const String &name);

	const String getAttackAnim(const String &name);

	bool isSkillTargetsAllies(const String &name);

	bool isSkillTargetsSelf(const String &name);

	void loadCritters(const String &filename, bool isCustom);

	const Critter getCritter(const String &type);

	const vector<const std::pair<String, Real>>::type getCritterDropList(const String &type);

	const std::pair<String, unsigned char> getCritterSkillDrop(const String &type);

	const std::pair<String, unsigned char> getRandomSkillDrop();

	const unsigned char getCritterRandomAttack(const String &type, bool *hitAlly = 0);

	const unsigned char getCritterRandomSpecificAttack(const String &type, bool getNonHeal);

	const std::pair<CritterAttack, String> getCritterAttackAndSound(const String &type, const unsigned char &iID);

	bool loadCritterSpawnList(const String &worldName,
		unsigned short &limit,
		vector<const WorldCritter>::type &list,
		vector<const std::pair<Vector3, Vector3>>::type &roamArea,
		const String &customFilename = "");

	bool loadCritterSpawnListFile(const String &filename, const String &worldName, unsigned short &limit, vector<const WorldCritter>::type &list, vector<const std::pair<Vector3, Vector3>>::type &roamArea);

	bool loadCustomCritterSpawnList(const String &filename, unsigned short &limit, vector<const WorldCritter>::type &list, vector<const std::pair<Vector3, Vector3>>::type &roamArea);

	const String loadLocalIP(const String &filename);

	const String getMapName(const String &name);
};

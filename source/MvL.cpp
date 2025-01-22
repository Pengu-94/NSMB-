#include "nsmb.hpp"

bool replaceLevels = false;

u32* mvlLevel1 = (u32*)0x0203CF14;
u32* bgdat1 = (u32*)0x0203CC30;
u32* mvlLevel2 = (u32*)0x0203CF18;
u32* bgdat2 = (u32*)0x0203CC34;
u32* mvlLevel3 = (u32*)0x0203CF1C;
u32* bgdat3 = (u32*)0x0203CC38;
u32* mvlLevel4 = (u32*)0x0203CF20;
u32* bgdat4 = (u32*)0x0203CC3C;
u32* mvlLevel5 = (u32*)0x0203CF24;
u32* bgdat5 = (u32*)0x0203CC40;

ncp_hook(0x02155270, 0x34)
void ReplaceMvLLevels()
{
	if (Input::getPressedKeys() & Keys::Select)
	{
        replaceLevels = !replaceLevels;	
		SND::playSFX(0x17B);
	}

	if (replaceLevels == true)
	{
		*mvlLevel1 = 0x7AF;
		*bgdat1 = 0x7A6;
		*mvlLevel2 = 0x7A7;
		*bgdat2 = 0x7A8;
		*mvlLevel3 = 0x7A9;
		*bgdat3 = 0x7AA;
		*mvlLevel4 = 0x7AB;
		*bgdat4 = 0x7AC;
		*mvlLevel5 = 0x7AD;
		*bgdat5 = 0x7AE;
	}

	if (replaceLevels == false)
	{
		*mvlLevel1 = 0x41C;
		*bgdat1 = 0x41D;
		*mvlLevel2 = 0x41E;
		*bgdat2 = 0x41F;
		*mvlLevel3 = 0x420;
		*bgdat3 = 0x421;
		*mvlLevel4 = 0x422;
		*bgdat4 = 0x423;
		*mvlLevel5 = 0x424;
		*bgdat5 = 0x425;
	}
}
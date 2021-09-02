/********************************************************************************
****
**      MQ2ItemScore.cpp : Rewrite for 2017
**
****
*********************************************************************************
****
**      This plugin will provide fast item comparisons based on a total score
**      which is adjustable by class. There are 18 attributes that can be used
**      to calculate the total score [ hp, mana, ac, etc ]
**
**      /iScore 			; shows basic status
**      /iScore AC 10 		; scores AC as 10:1
**      /iScore HP 1 		; scores HP as 1:1
**      /iScore hSTA 15     ; scores HeroicSTA at 15:1
**
****
*********************************************************************************
****
**     2019-02-17 - Name change to MQ2ItemScore
**          - Bug fix for ${DisplayItem}
**          - Added evaluation of Luck
**     2019-02-20 - Major bug fixes.
**          - Found how to find the 'luck' from link with out opening it.
**          - Added spam timer to links so we don't re-evaluate the same one over and over
**     2019-03-14 - Added 'click channel' option
**          - Modified 'click' options to include yes,no,1,0
**          - Made signifcant changes to help and reporting
**     2019-08-01 - Fixed problem with Evaluate & debug mode using strcpy_s and overwritting data
**     2019-08-06 - Another fix to Evaluate in debug mode.
**
****
*********************************************************************************/


#include <mq/Plugin.h>

PreSetup("MQ2ItemScore");
PLUGIN_VERSION(3.16);

char  Version[] = "Version 3.16 - Aug 6th 2019";

#define TIME unsigned long long

int DebugMode = 0;

char  ReportChannel[MAX_STRING];
char  ReportBestStr[MAX_STRING];
char  ClickChannel[MAX_STRING];
int   ClickGroup = 0;
int   ClickGuild = 0;
int   ClickRaid = 0;
int   ClickAny = 0;
int   IniLoaded = 0;
int   BestSlot = 0;
float BestScore = 0;
float CurrScore = 0;
float CurrSlot = 0;
int   LinkState = 0;    // 0 = idle, 1 = link requested, 2 = DisplayItem valid - process
TIME  LinkTimer = 0;
int   LinkTimeout = 30000;
int   WearingItem = 0;
int   UpgradeLuckyItem = 0;

#define LINK_LEN 90

#define  AttribMax  18

typedef struct
{
	char* Name;
	float Weight;
} trATR;

trATR    AttribList[] = {
	/* 00 */{ "AC",0 },
	/* 01 */{ "HP",0 },
	/* 02 */{ "HPReg",0 },
	/* 03 */{ "Mana",0 },
	/* 04 */{ "ManaReg",0 },
	/* 05 */{ "hSTR",0 },
	/* 06 */{ "hSTA",0 },
	/* 07 */{ "hAGI",0 },
	/* 08 */{ "hDEX",0 },
	/* 09 */{ "hINT",0 },
	/* 10 */{ "hWIS",0 },
	/* 11 */{ "hCHR",0 },
	/* 12 */{ "Heal",0 },
	/* 13 */{ "Nuke",0 },
	/* 14 */{ "Clrv",0 },
	/* 15 */{ "Attack",0 },
	/* 16 */{ "Haste",0 },
	/* 17 */{ "Ratio",0 }
};

typedef struct
{
	char  Name[ITEM_NAME_LEN];
	char  Class[100];
	char  Slots[250];
	char  Type[25];
	int   AugType;
	int   Lore;
	int   Luck;
	float Score;
	float Attrib[AttribMax];
	void* Next;
	int   FileID;
	//char  Link[200];
	TIME  TM;
} trITEM_ATTRIB;

trITEM_ATTRIB* ItemDB = NULL;
int            ItemDB_Loaded = 0;
char           ItemDB_Header[MAX_STRING] = "";

trITEM_ATTRIB  LinkedItem;
trITEM_ATTRIB  WornItem;
trITEM_ATTRIB  WorstWornItem;

typedef struct
{
	DWORD SlotMask;
	char* SlotName;
	char* ClassShortName;
	char* ClassName;
} tSLOTINFO;

tSLOTINFO SlotInfo[] = {
	/* 00 | 0x000001 */{ 1          ,"Charm"        ,"WAR", "Warrior" },
	/* 01 | 0x000002 */{ 2          ,"Left Ear"     ,"CLR", "Cleric" },
	/* 02 | 0x000004 */{ 4          ,"Head"         ,"PAL", "Paladin" },
	/* 03 | 0x000008 */{ 8          ,"Face"         ,"RNG", "Ranger" },
	/* 04 | 0x000010 */{ 16         ,"Right Ear"    ,"SHD", "Shadow Knight" },
	/* 05 | 0x000020 */{ 32         ,"Neck"         ,"DRU", "Druid" },
	/* 06 | 0x000040 */{ 64         ,"Shoulders"    ,"MNK", "Monk" },
	/* 07 | 0x000080 */{ 128        ,"Arms"         ,"BRD", "Bard" },
	/* 08 | 0x000100 */{ 256        ,"Back"         ,"ROG", "Rogue" },
	/* 09 | 0x000200 */{ 512        ,"Left Wrist"   ,"SHM", "Shaman" },
	/* 10 | 0x000401 */{ 1024       ,"Right Wrist"  ,"NEC", "Necromancer" },
	/* 11 | 0x000800 */{ 2048       ,"Range"        ,"WIZ", "Wizard" },
	/* 12 | 0x001000 */{ 4096       ,"Hands"        ,"MAG", "Magician" },
	/* 13 | 0x002000 */{ 8192       ,"Primary"      ,"ENC", "Enchanter" },
	/* 14 | 0x004000 */{ 16384      ,"Secondary"    ,"BST", "Beastlord" },
	/* 15 | 0x008000 */{ 32768      ,"Left Finger"  ,"BER", "Berserker" },
	/* 16 | 0x010000 */{ 65536      ,"Right Finger" },
	/* 17 | 0x020000 */{ 131072     ,"Chest"        },
	/* 18 | 0x040000 */{ 262144     ,"Legs"         },
	/* 19 | 0x080000 */{ 524288     ,"Feet"         },
	/* 20 | 0x100000 */{ 1048576    ,"Waist"        },
	/* 21 | 0x200000 */{ 2097152    ,"Power Source" },
	/* 22 | 0x400000 */{ 4194304    ,"Ammo"         },
	/* 23 | 0x800000 */{ 0          ,NULL           } };

#define SLOT_MAX 23
#define CLASS_MAX 16

void DoTest()
{
	//EzCommand("/timed 10 /ds loaddb");
	//EzCommand("/timed 20 /ds load");
	//EzCommand("/timed 30 /ds ac 10");
	//EzCommand("/timed 30 /ds hp 1");
	//EzCommand("/timed 40 /ds save");
	EzCommand("/timed 100 /plugin MQ2ItemScore unload");
}

void DoWelcomeText()
{
	char buf[MAX_STRING];
	sprintf_s(buf, "\agMQ2ItemScore\aw by \ayDewey2461\aw %s  Type /iScore help for command list.", Version);
	WriteChatColor(buf);
}



// ***************************************************************************
//  Load external data
// ***************************************************************************
void PrintItem(trITEM_ATTRIB* Src);

void ClearDB()
{
	trITEM_ATTRIB* p, * q;
	if (ItemDB_Loaded) {
		ItemDB_Loaded = 0;
		p = ItemDB;
		while (p)
		{
			q = p;
			p = (trITEM_ATTRIB*)p->Next;
			free(q);
		}
		ItemDB = NULL;
	}
}

// We should not be cashing things becaue we lose the 'luck' and 'lore' stats
void SaveCache()
{
	char  FileName[MAX_STRING];
	sprintf_s(FileName, "%s", INIFileName);
	int l = strlen(FileName);
	if (l > 4) FileName[l - 4] = 0;
	strcat_s(FileName, "Cache.txt");
	FILE* fp;
	fopen_s(&fp, FileName, "w");

	if (!fp) {
		WriteChatf("MQ2ItemScore::SaveCache [ %s ] Failed to write file.", FileName);
		return;
	}

	fprintf(fp, "%s", ItemDB_Header);

	trITEM_ATTRIB* p = ItemDB;
	while (p)
	{
		if (p->FileID)
		{
			fprintf(fp, "%s\t", p->Name);
			fprintf(fp, "%s\t", p->Class);
			fprintf(fp, "%s\t", p->Slots);
			fprintf(fp, "%s\t", p->Type);
			fprintf(fp, "%d\t", p->AugType);
			for (int i = 0; i < AttribMax; i++)
				fprintf(fp, "%0.0f\t", p->Attrib[i]);
			fprintf(fp, "%d\t", p->Lore);
			//fprintf(fp, "%d\t", p->Luck);
			//fprintf(fp, "%s\t", p->Link);
			fprintf(fp, "\n");
		}
		p = (trITEM_ATTRIB*)p->Next;
	}
	fclose(fp);
}


void LoadDB(int FileID, char* PostFix)
{
	trITEM_ATTRIB* p, * q;
	trITEM_ATTRIB   Item;
	char S[MAX_STRING];
	int i = 0;

	FILE* fp;

	// Figure out the DB file name and open it
	char  FileName[MAX_STRING];
	sprintf_s(FileName, "%s", INIFileName);
	int l = strlen(FileName);
	if (l > 4) FileName[l - 4] = 0;
	strcat_s(FileName, PostFix);
	errno_t error = fopen_s(&fp, FileName, "r");

	if (error) {
		WriteChatf("MQ2ItemScore::LoadDB Failed to open file [%s]", FileName);
		return;
	}

	p = ItemDB;
	while (p && p->Next)
		p = (trITEM_ATTRIB*)p->Next;

	fgets(S, MAX_STRING, fp);
	if (FileID == 0) strcpy_s(ItemDB_Header, S);

	while (fgets(S, MAX_STRING, fp))
	{
		//WriteChatf(" > %s ", S);

		memset(&Item, 0, sizeof(trITEM_ATTRIB));
		Item.FileID = FileID;

		char* s, * t;
		s = strtok_s(S, "\t", &t);          if (s) strcpy_s(Item.Name, s);
		s = strtok_s(NULL, "\t", &t);       if (s) strcpy_s(Item.Class, s);
		s = strtok_s(NULL, "\t", &t);       if (s) strcpy_s(Item.Slots, s);
		s = strtok_s(NULL, "\t", &t);       if (s) strcpy_s(Item.Type, s);
		s = strtok_s(NULL, "\t", &t);       if (s) Item.AugType = GetIntFromString(s, 0);

		for (i = 0; i < AttribMax; i++) {
			s = strtok_s(NULL, "\t", &t);   if (s) Item.Attrib[i] = GetFloatFromString(s, 0);
		}

		s = strtok_s(NULL, "\t", &t);       if (s) Item.Lore = GetIntFromString(s, 0);
		//s = strtok_s(NULL, "\t", &t);       if (s) Item.Luck = atoi(s);
		//s = strtok_s(NULL, "\t", &t);       if (s) strcpy_s(Item.Link, s);

		if (_stricmp(Item.Slots, "EAR") == 0) strcpy_s(Item.Slots, "LEFT EAR|RIGHT EAR|");
		if (_stricmp(Item.Slots, "WRIST") == 0) strcpy_s(Item.Slots, "LEFT WRIST|RIGHT WRIST|");
		if (_stricmp(Item.Slots, "FINGER") == 0) strcpy_s(Item.Slots, "LEFT FINGER|RIGHT FINGER|");

		if (Item.Name[0] != 0) {
			q = (trITEM_ATTRIB*)malloc(sizeof(trITEM_ATTRIB));
			memcpy(q, &Item, sizeof(trITEM_ATTRIB));
			if (ItemDB == 0)
				ItemDB = q;
			else
				p->Next = q;
			p = q;
		}
	}
	fclose(fp);
}



void PrintDBSatus()
{
	trITEM_ATTRIB* p;
	char S[MAX_STRING];
	char T[MAX_STRING];
	int i;

	S[0] = 0;
	T[0] = 0;
	p = ItemDB;
	i = 0;
	while (p && _stricmp(p->Type, "Armor"))
	{
		i++;
		if (_stricmp(p->Type, S) != 0) {
			if (S[0] != 0)
				strcat_s(T, " | ");
			strcpy_s(S, p->Type);
			strcat_s(T, S);
		}
		p = (trITEM_ATTRIB*)p->Next;
	}
	while (p)
	{
		i++;
		p = (trITEM_ATTRIB*)p->Next;
	}

	WriteChatf("MQ2ItemScore::LoadDB: %d items, Found templates for [ %s ]", i, T);
}


void LoadDBS()
{
	// Clear DB if it was loaded
	ClearDB();

	ItemDB_Loaded = 1;

	LoadDB(0, "Templates.txt");
	LoadDB(1, "Cache.txt");

	PrintDBSatus();
}

int SearchDB(trITEM_ATTRIB* Item)
{
	trITEM_ATTRIB* p;
	p = ItemDB;
	int NameMatch = 0;
	int LinkMatch = 0;
	while (p)
	{
		NameMatch = _stricmp(p->Name, Item->Name) == 0;
		LinkMatch = 1;
		//if (NameMatch && Item->Link[0] != 0) LinkMatch = _stricmp(p->Link, Item->Link);
		if (NameMatch && LinkMatch)
		{
			memcpy(Item, p, sizeof(trITEM_ATTRIB));
			Item->Score = 0;
			for (int i = 0; i < AttribMax; i++)
				Item->Score = Item->Score + Item->Attrib[i] * AttribList[i].Weight;

			if (DebugMode > 1) WriteChatf("MQ2ItemScore::SearchDB: %s found", Item->Name);
			if (DebugMode > 1) PrintItem(Item);

			return 1;
		}
		p = (trITEM_ATTRIB*)p->Next;
	}
	return 0;
}

int AddItemToCache(trITEM_ATTRIB* Item)
{
	trITEM_ATTRIB* p, * q;
	q = NULL;
	p = ItemDB;
	int NameMatch = 0;
	int LinkMatch = 0;
	while (p)
	{
		NameMatch = _stricmp(p->Name, Item->Name) == 0;
		LinkMatch = 1;
		//if (NameMatch && Item->Link[0] != 0) LinkMatch = _stricmp(p->Link, Item->Link);
		if (NameMatch && LinkMatch)
		{
			if (DebugMode > 1) WriteChatf("MQ2ItemScore::AddItemToCache: %s - Item already cached", Item->Name);
			// Update last time item was accessed
			p->TM = Item->TM;
			return 0;
		}
		q = p;
		p = (trITEM_ATTRIB*)p->Next;
	}
	// Item was not found
	if (DebugMode) WriteChatf("MQ2ItemScore::AddItemToCache: %s", Item->Name);
	p = (trITEM_ATTRIB*)malloc(sizeof(trITEM_ATTRIB));
	memcpy(p, Item, sizeof(trITEM_ATTRIB));
	p->Next = 0;
	p->FileID = 1;
	if (ItemDB == 0)
		ItemDB = p;
	else
		q->Next = p;

	SaveCache();
	return 1;
}

// ***************************************************************************
//  To Evaluate or Not to Evaluate that is the question... I choose to EVAL for easy development
// ***************************************************************************


float Evaluate(char* zOutput, char* zFormat, ...) {
	va_list vaList;
	va_start(vaList, zFormat);
	char szTemp[MAX_STRING];
	vsprintf_s(szTemp, MAX_STRING, zFormat, vaList);
	if (DebugMode > 9) WriteChatf("Eval[%s]", szTemp);
	ParseMacroData(szTemp, MAX_STRING);
	if (DebugMode > 9) WriteChatf("EvalResp[%s]", szTemp);
	if (gszLastNormalError[0] || gszLastSyntaxError[0] || gszLastMQ2DataError[0]) {
		char szBuff[MAX_STRING];
		vsprintf_s(szBuff, MAX_STRING, zFormat, vaList);
		if (gszLastNormalError[0])  WriteChatf("MQ2ItemScore Error:[%s] on Evaluate[%s] = [%s]", gszLastNormalError, szBuff, szTemp);
		if (gszLastSyntaxError[0])  WriteChatf("MQ2ItemScore Error:[%s] on Evaluate[%s] = [%s]", gszLastSyntaxError, szBuff, szTemp);
		if (gszLastMQ2DataError[0]) WriteChatf("MQ2ItemScore Error:[%s] on Evaluate[%s] = [%s]", gszLastMQ2DataError, szBuff, szTemp);
		gszLastNormalError[0] = gszLastSyntaxError[0] = gszLastMQ2DataError[0] = 0;
	}
	if (DebugMode > 29) {
		WriteChatf("Len[%s] = %d", szTemp, strlen(szTemp));
	}
	if (zOutput) {
		char* p = szTemp;
		while (*p) *zOutput++ = *p++;
		*zOutput = 0;
	}
	if (_stricmp(szTemp, "NULL") == 0) return 0;
	if (_stricmp(szTemp, "FALSE") == 0) return 0;
	if (_stricmp(szTemp, "TRUE") == 0) return 1;
	return GetFloatFromString(szTemp, 0);
}

// ***************************************************************************
//  This section is methods to interact with the AttributeList
// ***************************************************************************

int  SetAttribListWeight(char* Key, char* Val)
{
	int i;
	for (i = 0; i < AttribMax; i++)
	{
		if (_stricmp(AttribList[i].Name, Key) == 0)
		{
			WriteChatf("MQ2ItemScore::Setting %s to %s", AttribList[i].Name, Val);
			AttribList[i].Weight = GetFloatFromString(Val, 0);
			return 1;
		}
	}
	return 0;
}

void ClearAttribListWeights()
{
	int i;
	for (i = 0; i < AttribMax; i++)
		AttribList[i].Weight = 0;
}

void PrintItem(trITEM_ATTRIB* Src)
{
	WriteChatf("MQ2ItemScore");
	WriteChatf("   Name =%s", Src->Name);
	WriteChatf("   Class=%s", Src->Class);
	WriteChatf("   Slots=%s", Src->Slots);
	WriteChatf("   Type=%s", Src->Type);
	WriteChatf("   Lore=%d", Src->Lore);
	WriteChatf("   Luck=%d", Src->Luck);
	WriteChatf("   Score=%0.0f", Src->Score);
	if (DebugMode > 2) {
		for (int i = 0; i < AttribMax; i++) WriteChatf("   %s %0.0f", AttribList[i].Name, Src->Attrib[i]);
	}
	//if (DebugMode > 3) 	WriteChatf("   Link=%s", Src->Link);

}

void ClearItem(trITEM_ATTRIB* Dest)
{
	memset(Dest, 0, sizeof(trITEM_ATTRIB));
}



void ToUpper(char* p)
{
	while (*p++ = toupper(*p));
}

void SetItemByTLO(trITEM_ATTRIB* Dest, char szTLO[], char* Class, char* Slot)
{
	char szKey[MAX_STRING];
	char szVal[MAX_STRING];


	Dest->Attrib[0] = Evaluate(szKey, "${%s.AC}", szTLO);
	Dest->Attrib[1] = Evaluate(szKey, "${%s.HP}", szTLO);
	Dest->Attrib[2] = Evaluate(szKey, "${%s.HPRegen}", szTLO);
	Dest->Attrib[3] = Evaluate(szKey, "${%s.Mana}", szTLO);
	Dest->Attrib[4] = Evaluate(szKey, "${%s.ManaRegen}", szTLO);
	Dest->Attrib[5] = Evaluate(szKey, "${%s.HeroicSTR}", szTLO);
	Dest->Attrib[6] = Evaluate(szKey, "${%s.HeroicSTA}", szTLO);
	Dest->Attrib[7] = Evaluate(szKey, "${%s.HeroicAGI}", szTLO);
	Dest->Attrib[8] = Evaluate(szKey, "${%s.HeroicDEX}", szTLO);
	Dest->Attrib[9] = Evaluate(szKey, "${%s.HeroicINT}", szTLO);
	Dest->Attrib[10] = Evaluate(szKey, "${%s.HeroicWIS}", szTLO);
	Dest->Attrib[11] = Evaluate(szKey, "${%s.HeroicCHA}", szTLO);
	Dest->Attrib[12] = Evaluate(szKey, "${%s.HealAmount}", szTLO);
	Dest->Attrib[13] = Evaluate(szKey, "${%s.SpellDamage}", szTLO);
	Dest->Attrib[14] = Evaluate(szKey, "${%s.Clairvoyance}", szTLO);
	Dest->Attrib[15] = Evaluate(szKey, "${%s.Attack}", szTLO);
	Dest->Attrib[16] = Evaluate(szKey, "${%s.Haste}", szTLO);

	if (Evaluate(szKey, "${%s.ItemDelay}", szTLO))
		Dest->Attrib[17] = Evaluate(szKey, "${Math.Calc[100.0*${%s.Damage}/${%s.ItemDelay}]}", szTLO, szTLO);
	else
		Dest->Attrib[17] = Evaluate(szKey, "${%s.Damage}", szTLO);

	Evaluate(Dest->Name, "${%s.Name}", szTLO);
	Evaluate(Dest->Type, "${%s.Type}", szTLO);

	Dest->Lore = 0;
	if (Evaluate(szKey, "${%s.Lore}", szTLO)) Dest->Lore = 1;
	if (Evaluate(szKey, "${%s.LoreEquipped}", szTLO)) Dest->Lore = 1;

	Dest->Luck = static_cast<int>(Evaluate(szKey, "${%s.Luck}", szTLO));

	Dest->Score = 0;
	Dest->Slots[0] = 0;
	Dest->Class[0] = 0;
	Dest->AugType = static_cast<int>(Evaluate(szKey, "${%s.AugType}", szTLO));

	for (int i = 0; i < AttribMax; i++)
	{
		Dest->Score = Dest->Score + Dest->Attrib[i] * AttribList[i].Weight;
	}


	if (Class)
	{
		strcpy_s(Dest->Class, Class);
	}
	else
	{
		int Max = static_cast<int>(Evaluate(szKey, "${%s.Classes}", szTLO));
		int j = 1;
		for (int i = 0; i < Max; i++)
		{
			Evaluate(szKey, "${%s.Class[%d]}", szTLO, i + 1);
			//WriteChatf("%s.Class[%d] = %s", szTLO,i, szKey);
			for (int j = 0; j < CLASS_MAX; j++)
			{
				if (_stricmp(SlotInfo[j].ClassName, szKey) == 0) {
					strcat_s(Dest->Class, SlotInfo[j].ClassShortName);
					strcat_s(Dest->Class, "|");
					j = CLASS_MAX;
				}
			}
		}
	}


	if (Slot)
	{
		strcpy_s(Dest->Slots, Slot);
	}
	else
	{
		int WornSlots = static_cast<int>(Evaluate(szKey, "${%s.WornSlots}", szTLO));
		for (int i = 0; i < WornSlots; i++)
		{
			int j = static_cast<int>(Evaluate(szKey, "${%s.WornSlot[%d]}", szTLO, i + 1));
			strcat_s(Dest->Slots, SlotInfo[j].SlotName);
			strcat_s(Dest->Slots, "|");
		}
	}
	ToUpper(Dest->Slots);

	// Now see if we want to over ride the score
	char  szClass[MAX_STRING];
	Evaluate(szClass, "${Me.Class.ShortName}");
	GetPrivateProfileString(szClass, Dest->Name, "0", szVal, 256, INIFileName);
	if (szVal[0] != '0') Dest->Score = GetFloatFromString(szVal, 0);

	//PrintItem(Dest);
}


// If the "LinkedItem" is one of the templates then replace it with the class item
void DoTemplateMatching(char* ClassName)
{
	trITEM_ATTRIB* p, * q;
	p = ItemDB;

	//WriteChatf("CheckForSubstitions: %s", LinkedItem.Name);
	p = ItemDB;
	q = NULL;
	while (p && _stricmp(p->Type, "Armor"))
	{
		if (_stricmp(p->Name, LinkedItem.Name) == 0) {
			//WriteChatf("CheckForSubstitions: Template %s", p->Type);
			q = p;
		}
		p = (trITEM_ATTRIB*)p->Next;
	}

	if (!q) {
		//WriteChatf("MQ2ItemScore::DoTemplateMatching - Not a template");
		return;
	}

	// we have a match lets go find it.

	char szStr[MAX_STRING];
	char szTmp[MAX_STRING];
	szStr[0] = szTmp[0] = 0;
	int l = strlen(q->Type);

	while (p)
	{
		if (_stricmp(q->Slots, p->Slots) == 0 && strncmp(q->Type, p->Name, l) == 0 && _stricmp(ClassName, p->Class) == 0)
		{
			if (DebugMode) WriteChatf("MATCH: %s -> %s", LinkedItem.Name, p->Name);

			memcpy(&LinkedItem, p, sizeof(trITEM_ATTRIB));
			LinkedItem.Score = 0;
			for (int i = 0; i < AttribMax; i++)
			{
				LinkedItem.Score = LinkedItem.Score + LinkedItem.Attrib[i] * AttribList[i].Weight;
				if (DebugMode)
				{
					sprintf_s(szTmp, " %+0d %s ", (int)LinkedItem.Attrib[i], AttribList[i].Name);
					strcat_s(szStr, szTmp);
				}
			}
			if (DebugMode) WriteChatf("ATTRIB: %s %s", LinkedItem.Name, szStr);
			return;
		}
		p = (trITEM_ATTRIB*)p->Next;
	}
}


// ***************************************************************************
//  This section is to set, save, and report global profile variables.
// ***************************************************************************

void LoadAttribListWeights(int Echo)
{
	char  szVal[MAX_STRING];
	char  szClass[MAX_STRING];
	Evaluate(szClass, "${Me.Class.ShortName}");

	int i;
	for (i = 0; i < AttribMax; i++)
	{
		GetPrivateProfileString(szClass, AttribList[i].Name, "0", szVal, 256, INIFileName);
		AttribList[i].Weight = GetFloatFromString(szVal, 0);
	}
	if (Echo) WriteChatf("MQ2ItemScore::reading attributes for [%s]", szClass);

}

void SaveAttribListWeights(int Echo)
{
	char  szVal[MAX_STRING];
	char  szClass[MAX_STRING];
	Evaluate(szClass, "${Me.Class.ShortName}");

	int i;
	for (i = 0; i < AttribMax; i++)
	{
		sprintf_s(szVal, "%0.2f", AttribList[i].Weight);
		WritePrivateProfileString(szClass, AttribList[i].Name, szVal, INIFileName);
	}
	if (Echo) WriteChatf("MQ2ItemScore::saving settings for [%s]", szClass);

}

void ClearProfile(int Echo)
{
	sprintf_s(ReportChannel, "None");
	ClickChannel[0] = 0;
	ClickGroup = 0;
	ClickGuild = 0;
	ClickRaid = 0;
	ClickAny = 0;
	ClearAttribListWeights();
	if (Echo) WriteChatf("MQ2ItemScore::plugin variables cleared but not saved.");
}

void ReadProfile(int Echo)
{
	char szVal[MAX_STRING];
	GetPrivateProfileString("Global", "Report", "None", ReportChannel, 256, INIFileName);
	GetPrivateProfileString("Global", "ClickChannel", "", ClickChannel, 256, INIFileName);
	GetPrivateProfileString("Global", "ClickGroup", "0", szVal, 256, INIFileName);  ClickGroup = atoi(szVal);
	GetPrivateProfileString("Global", "ClickGuild", "0", szVal, 256, INIFileName);  ClickGuild = atoi(szVal);
	GetPrivateProfileString("Global", "ClickRaid", "0", szVal, 256, INIFileName);   ClickRaid = atoi(szVal);
	GetPrivateProfileString("Global", "ClickAny", "0", szVal, 256, INIFileName);    ClickAny = atoi(szVal);
	LoadAttribListWeights(Echo);
	IniLoaded = 1;
}

void WriteProfile(int Echo)
{
	char szKey[MAX_STRING];

	WritePrivateProfileString("Global", "Report", ReportChannel, INIFileName);
	WritePrivateProfileString("Global", "ClickChannel", ClickChannel, INIFileName);
	sprintf_s(szKey, "%d", ClickGroup); WritePrivateProfileString("Global", "ClickGroup", szKey, INIFileName);
	sprintf_s(szKey, "%d", ClickGuild); WritePrivateProfileString("Global", "ClickGuild", szKey, INIFileName);
	sprintf_s(szKey, "%d", ClickRaid);  WritePrivateProfileString("Global", "ClickRaid", szKey, INIFileName);
	sprintf_s(szKey, "%d", ClickAny);   WritePrivateProfileString("Global", "ClickAny", szKey, INIFileName);
	SaveAttribListWeights(Echo);
}

void EchoProfile(int reporting, int clicking, int weights)
{
	if (reporting)
	{
		WriteChatf("MQ2ItemScore::Report to [ %s ]", ReportChannel);
	}

	if (clicking)
	{
		WriteChatf("MQ2ItemScore::ClickChannel [%s]", ClickChannel);
		WriteChatf("MQ2ItemScore::ClickGroup = %d", ClickGroup);
		WriteChatf("MQ2ItemScore::ClickGuild = %d", ClickGuild);
		WriteChatf("MQ2ItemScore::ClickRaid  = %d", ClickRaid);
		WriteChatf("MQ2ItemScore::ClickAny   = %d", ClickAny);
	}
	if (weights)
	{
		for (int i = AttribMax - 1; i >= 0; i--)
		{
			WriteChatf("MQ2ItemScore::%s = %0.0f", AttribList[i].Name, AttribList[i].Weight);
		}
	}
}

void EchoHelp(int Echo)
{
	WriteChatf("MQ2ItemScore::Commands :");
	WriteChatf("  /iScore clear -- resets all variables");
	WriteChatf("  /iScore load -- load settings from disk");
	WriteChatf("  /iScore save -- save current settings to disk");
	WriteChatf("  /iScore click -- show current click options");
	WriteChatf("  /iScore click [Group|Raid|Guild|Any] [Value] -- toggles or sets auto click option");
	WriteChatf("  /iScore click Channel [Value] -- will auto click links in this channel");
	WriteChatf("  /iScore report [Channel] -- send upgrade using this command ex: /bc ");
	WriteChatf("  /iScore [attribute] [weight] -- sets an item weight");
	WriteChatf("  /iScore best [value] -- set item on cursor to this value");
	WriteChatf("  /iScore -- evaluate item on cursor or shows current values");
}


void EchoCommands(int Echo)
{
	EchoHelp(TRUE);
	EchoProfile(FALSE, TRUE, FALSE);
	EchoProfile(TRUE, FALSE, TRUE);
}

// ***************************************************************************
//  This section is for each of the user commands.
// ***************************************************************************

void DoScoreForCursor();

void SetReportChannel(char* Val)
{
	strcpy_s(ReportChannel, Val);
	EchoProfile(TRUE, FALSE, FALSE);
}

int SetClickMode(int Val, char* Opt)
{
	if (!Opt) return !Val;
	if (Opt[0]) return !Val;
	if (_stricmp(Opt, "YES") == 0) return 1;
	if (_stricmp(Opt, "NO") == 0) return 0;
	if (_stricmp(Opt, "1") == 0) return 1;
	if (_stricmp(Opt, "0") == 0) return 0;
	return Val;
}

void SetClickMode(char* Val, char* Opt)
{
	if (_stricmp(Val, "Channel") == 0) strcpy_s(ClickChannel, Opt);
	if (_stricmp(Val, "Group") == 0) ClickGroup = !ClickGroup;
	if (_stricmp(Val, "Guild") == 0) ClickGuild = !ClickGuild;
	if (_stricmp(Val, "Raid") == 0) ClickRaid = !ClickRaid;
	if (_stricmp(Val, "Any") == 0) ClickAny = !ClickAny;
	EchoProfile(FALSE, TRUE, FALSE);
	WriteProfile(FALSE);
}

void SetDebugMode(char* Val)
{
	DebugMode = (int)atoi(Val);
	WriteChatf("MQ2ItemScore::DebugMode=%d", DebugMode);
}

void SetAsBestItem()
{
	char szStr[MAX_STRING];
	char szClass[MAX_STRING];

	if (Evaluate(szStr, "${Cursor.ID}") > 0)
	{
		Evaluate(szStr, "${Cursor.Name}");
		Evaluate(szClass, "${Me.Class.ShortName}");

		WritePrivateProfileString(szClass, szStr, "999999", INIFileName);
		WriteChatf("MQ2ItemScore::Set [%s] score to 999999 for all [%s]", szStr, szClass);
	}
}




// ***************************************************************************
//  This is the actual entry point for user commands. Parse and divy up the work.
// ***************************************************************************

void DoItemScoreUserCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	CHAR Key[MAX_STRING] = { 0 };
	CHAR Val[MAX_STRING] = { 0 };
	CHAR Opt[MAX_STRING] = { 0 };
	char* pName = GetCharInfo()->Name;
	if (!pName) return;

	GetArg(Key, szLine, 1);
	GetArg(Val, szLine, 2);
	GetArg(Opt, szLine, 3);

	if (_stricmp(Key, "save") == 0) { WriteProfile(TRUE);           return; }
	if (_stricmp(Key, "load") == 0) { ReadProfile(TRUE);            return; }
	if (_stricmp(Key, "loaddb") == 0) { LoadDBS();                  return; }
	if (_stricmp(Key, "report") == 0) { SetReportChannel(Val);      return; }
	if (_stricmp(Key, "click") == 0) { SetClickMode(Val, Opt);      return; }
	if (_stricmp(Key, "clear") == 0) { ClearProfile(TRUE);          return; }
	if (_stricmp(Key, "help") == 0) { EchoHelp(TRUE);               return; }
	if (_stricmp(Key, "debug") == 0) { SetDebugMode(Val);           return; }
	if (_stricmp(Key, "best") == 0) { SetAsBestItem();              return; }
	if (Key[0] == 0) { DoScoreForCursor();                          return; }
	SetAttribListWeight(Key, Val);
}


void CompareLinkVsWornItem()
{
	// Input is a valid LinkedItem vs WornItem
	// Skip checks if we have "Nothing" in slot

	if (DebugMode > 2) WriteChatf("Eval: Name %s ", WornItem.Name);

	if (strstr(LinkedItem.Slots, WornItem.Slots) == 0) {
		if (DebugMode > 2) WriteChatf("Eval: Slot '%s' vs item '%s' - don't match", WornItem.Slots, LinkedItem.Slots);
		return;
	}

	if (_stricmp(WornItem.Name, "NULL") == 0) {
		strcpy_s(WornItem.Name, "Nothing");
		WornItem.Score = 1;
	}
	else {
		if (LinkedItem.AugType != WornItem.AugType) return;    // Filter out aug vs armor
	}

	if (_stricmp(LinkedItem.Name, WornItem.Name) == 0) {
		WearingItem = 1;
		if (WornItem.Lore) {
			// If we are wearing a lore item only choice is to see if its luckier
			memcpy_s(&WorstWornItem, sizeof(trITEM_ATTRIB), &WornItem, sizeof(trITEM_ATTRIB));
			if (LinkedItem.Luck > WornItem.Luck) {
				UpgradeLuckyItem = 1;
			}
		}
	}

	if (DebugMode > 2) WriteChatf("WornItem: %s : %0.0f : Luck %+0d ", WornItem.Name, WornItem.Score, WornItem.Luck);
	if (DebugMode > 2) WriteChatf("LinkItem: %s : %0.0f : Luck %+0d ", LinkedItem.Name, LinkedItem.Score, LinkedItem.Luck);

	if (LinkedItem.Score > WornItem.Score && !UpgradeLuckyItem) {
		if (WorstWornItem.Score == 0 || WornItem.Score < WorstWornItem.Score) {
			memcpy_s(&WorstWornItem, sizeof(trITEM_ATTRIB), &WornItem, sizeof(trITEM_ATTRIB));
		}
	}
}

void EvaluateWornItems(char* Class)
{
	int i;
	char szStr[MAX_STRING];
	char szTmp[MAX_STRING];

	WearingItem = 0;
	UpgradeLuckyItem = 0;

	ReportBestStr[0] = 0;
	int Primary = static_cast<int>(Evaluate(szTmp, "${Me.Inventory[13].Type}"));

	// We're wearing everything here so cheat and pass in the Class and Slot info to save CPU cycles.
	for (i = 0; i < BAG_SLOT_START - 1; i++)
	{
		if (LinkedItem.AugType == 0) {
			if (!(i == 14 && Primary == 2)) {        // Skip shield upgrade if I'm using a 2h
				sprintf_s(szStr, "Me.Inventory[%d]", i);
				SetItemByTLO(&WornItem, szStr, Class, SlotInfo[i].SlotName);
				CompareLinkVsWornItem();

			}
		}
		else {
			for (int j = 1; j < 6; j++)
			{
				int AugType = static_cast<int>(Evaluate(szTmp, "${Me.Inventory[%d].AugSlot%d}", i, j));
				//WriteChatf("Item.AugType = %d vs AugType %d", AugType, LinkedItem.AugType);
				if (AugType > 0 && (SlotInfo[AugType - 1].SlotMask & LinkedItem.AugType)) {
					sprintf_s(szStr, "Me.Inventory[%d].Item[%d]", i, j);
					SetItemByTLO(&WornItem, szStr, Class, SlotInfo[i].SlotName);
					CompareLinkVsWornItem();
				}
			}
		}
	}
}

void ReportUpgrade(int FromLink)
{
	if (WorstWornItem.Name[0] == 0) {
		WriteChatf("MQ2ItemScore: \ay%s\aw Not an upgrade", LinkedItem.Name);
		return;
	}

	if (WearingItem && LinkedItem.Lore && !UpgradeLuckyItem) {
		WriteChatf("MQ2ItemScore: \ay%s\aw is lore and not luckiere", LinkedItem.Name);
		return;
	}

	if (WorstWornItem.Score == LinkedItem.Score && LinkedItem.Luck <= WorstWornItem.Luck) {
		WriteChatf("MQ2ItemScore: \ay%s\aw Not an upgrade", LinkedItem.Name);
		return;
	}


	int i;
	int d;
	float v;
	char szTmp[MAX_STRING];
	char szStr[MAX_STRING];
	char szVerb[MAX_STRING];

	if (FromLink)
		sprintf_s(szVerb, "LOOT ");
	else
		sprintf_s(szVerb, "MOVE ");

	// -- Same item but better luck
	if (UpgradeLuckyItem) {
		sprintf_s(szStr, "MQ2ItemScore: \ay%s\aw has better luck.", LinkedItem.Name);
		if (ReportChannel[0] == '/')
		{
			char szCmd[MAX_STRING];
			sprintf_s(szCmd, "%s %s %s for +%d LUCK", ReportChannel, szVerb, LinkedItem.Name, LinkedItem.Luck - WorstWornItem.Luck);
			EzCommand(szCmd);
		}
		return;
	}

	// -- Different item so show stats
	szStr[0] = szTmp[0] = 0;

	for (i = 0; i < AttribMax; i++)
	{
		d = static_cast<int>(LinkedItem.Attrib[i] - WorstWornItem.Attrib[i]);
		if (AttribList[i].Weight != 0 && d != 0)
		{
			sprintf_s(szTmp, " %+0d %s ", d, AttribList[i].Name);
			strcat_s(szStr, szTmp);
		}
	}

	if (WorstWornItem.Score > 1)
		v = 100.0f * (LinkedItem.Score / WorstWornItem.Score) - 100.0f;
	else
		v = 100.0f;


	sprintf_s(ReportBestStr, "%s %s for %s : %+0.1f%%  %s", szVerb, LinkedItem.Name, WorstWornItem.Slots, v, szStr);

	if (ReportChannel[0] == '/')
	{
		char szCmd[MAX_STRING];
		sprintf_s(szCmd, "%s %s", ReportChannel, ReportBestStr);
		EzCommand(szCmd);
	}
	else {
		WriteChatf("MQ2ItemScore: %s \ay%s\aw for \ag%s\aw : %+0.1f%%  %s", szVerb, LinkedItem.Name, WorstWornItem.Slots, v, szStr);
	}

}


void EvaluateLuckLore()
{
	char szStr[MAX_STRING];
	if (WearingItem && LinkedItem.Lore) {
		memcpy(&WorstWornItem, &LinkedItem, sizeof(trITEM_ATTRIB));
		WorstWornItem.Luck = static_cast<int>(Evaluate(szStr, "${FindItem[=%s].Luck}", LinkedItem.Name));
		if (DebugMode > 2) WriteChatf("FindItemCheck:Found Item: WornLuck = %d   LinkLuck = %d", WorstWornItem.Luck, LinkedItem.Luck);
	}
}

void DoItemScore(int FromLink)
{
	// At this point LinkedItem is the item we are going to compare
	// PrintItem(&LinkedItem);
	//WriteChatColor("DoItemScore()");

	AddItemToCache(&LinkedItem);

	char ClassName[MAX_STRING];
	Evaluate(ClassName, "${Me.Class.ShortName}");

	DoTemplateMatching(ClassName);

	if (DebugMode > 1) PrintItem(&LinkedItem);

	// Check to see if we are the right class.
	if (strstr(LinkedItem.Class, ClassName) == 0 && _stricmp(LinkedItem.Class, "ALL") != 0) {
		if (DebugMode > 1) WriteChatColor("Item Unusable due to class restriction");
		return;
	}

	ClearItem(&WorstWornItem);
	EvaluateWornItems(ClassName);
	//EvaluateLuckLore();
	ReportUpgrade(FromLink);
}

void DoScoreForCursor()
{
	char szStr[MAX_STRING];

	if (Evaluate(szStr, "${Cursor.ID}") > 0)
	{
		SetItemByTLO(&LinkedItem, "Cursor", NULL, NULL);
		DoItemScore(FALSE);
	}
	else {
		DoWelcomeText();
		EchoProfile(TRUE, TRUE, TRUE);
	}
}




/******************************************************************
****
**     Initialize / Shudown plugin moved.
****
******************************************************************/

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	DoWelcomeText();

	// Remove the stuff in core

	RemoveCommand("/iScore");
	RemoveCommand("/ItemScore");

	// Add our version back in
	AddCommand("/iscore", DoItemScoreUserCommand);

	if (gGameState == GAMESTATE_INGAME) ReadProfile(FALSE);
	LoadDBS();

	//DoTest();

}

PLUGIN_API VOID SetGameState(DWORD GameState)
{
	if (GameState == GAMESTATE_INGAME && IniLoaded == 0)    ReadProfile(FALSE);
	if (GameState != GAMESTATE_INGAME && IniLoaded == 1)    IniLoaded = 0;
}


// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	ClearDB();
	RemoveCommand("/iScore");
}



PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
	if (ClickGroup || ClickRaid || ClickGuild || ClickAny || ClickChannel[0] != 0)
	{
		char szText[MAX_STRING];
		char szStart[MAX_STRING];
		char szCommand[MAX_STRING];
		char* p, * q, * v;
		int  doLink = 0;

		sprintf_s(szStart, "%c%c", 0x12, 0x30);
		p = strstr(Line, szStart);
		if (!p) return 0;

		if (ClickAny) doLink = 1;
		if (!doLink && ClickGroup && (strstr(Line, "tells the group") || strstr(Line, "tell your party"))) doLink = 1;
		if (!doLink && ClickRaid && (strstr(Line, "tells the raid") || strstr(Line, "tell your raid"))) doLink = 1;
		if (!doLink && ClickGuild && (strstr(Line, "tells the guild") || strstr(Line, "say to your guild"))) doLink = 1;
		if (!doLink && ClickChannel[0] != 0 && strstr(Line, ClickChannel)) doLink = 1;

		if (doLink && p && strlen(p) > LINK_LEN + 2)
		{
			ClearItem(&LinkedItem);

			// Get the name
			q = p + 2 + LINK_LEN;
			v = LinkedItem.Name;
			while (*q && *q != 0x12)
				*v++ = *q++;
			*v = 0;

#ifdef USE_LINK
			// Convert link to hex string
			sprintf_s(LinkedItem.Link, "0x");

			q = p + 2;
			for (int i = 0; i < LINK_LEN; i++)
			{
				sprintf_s(szText, "%02X", *q);
				strcat_s(LinkedItem.Link, szText);
				q++;
			}

			WriteChatf("MQ2ItemScore::Link[80] = %c", p[80 + 2]);
			WriteChatf("MQ2ItemScore::Link[81] = %c", p[81 + 2]);  // This is correct
			WriteChatf("MQ2ItemScore::Link[82] = %c", p[82 + 2]);

#endif

			//WriteChatf("MQ2ItemScore::LinkItemName = [%s]", LinkedItem.Name);

			// Check to see if we have the item - if so we're good to evaluate it
			// 2019-02-17 - Skip the DB because we want to open links to get 'luck'

			//WriteChatf("MQ2ItemScore::LinkItemName = [%s]", LinkedItem.Name);
			if (SearchDB(&LinkedItem)) {
				//WriteChatf("MQ2ItemScore::SearchDB Found Item - Checking TM");
				if (GetTickCount64() > LinkedItem.TM || 1)
				{
					LinkedItem.Luck = p[83] - '0';

					//WriteChatf("MQ2ItemScore::SearchDB Found Item - Link is old - report it again TM=%d",LinkedItem.TM);
					//WriteChatf("MQ2ItemScore::SearchDB Found Item - Link luck = %d", LinkedItem.Luck);
					DoItemScore(TRUE);
					return 0;
				}
				//WriteChatf("MQ2ItemScore::SearchDB Found Item - Link is new enough - skip it ");
				return 0;
			}


			// Check to see if DisplayItem has the data.
			Evaluate(szText, "${DisplayItem}");
			if (_stricmp(szText, LinkedItem.Name) == 0) {
				//WriteChatf(" -- TLO:DisplayItme has data, don't need to request link. ");
				SetItemByTLO(&LinkedItem, "DisplayItem", NULL, NULL);
				LinkedItem.Luck = p[83] - '0';
				DoItemScore(TRUE);
				return 0;
			}
		}


		if (doLink && p && strlen(p) > LINK_LEN + 2)
		{
			LinkedItem.TM = GetTickCount64() + LinkTimeout;

			if (DebugMode) WriteChatf("MQ2ItemScore::Requesting link from client");
			memset(szText, 0, 100);
			strncpy_s(szText, p + 2, LINK_LEN);
			sprintf_s(szCommand, "/notify ChatWindow CW_ChatOutput link %s", szText);
			DoCommand(((PSPAWNINFO)pCharSpawn), szCommand);

			sprintf_s(szCommand, "/timed 10 /keypress ESC");
			DoCommand(((PSPAWNINFO)pCharSpawn), szCommand);

			LinkState = 1;
			LinkTimer = GetTickCount64() + 200;

			//WriteChatf("OnIncomingChat::Cmd = %s",Line);
			//for (i=0; Line[i]; i++)   WriteChatf("Line[%d] = 0x%02X = %c ",i,Line[i],Line[i]);

			//WriteChatf("OnIncomingChat::Cmd = %s",szCommand);
			//for (i=0; szText[i]; i++) WriteChatf("szText[%d] = 0x%02X = %c ",i,szText[i],szText[i]);


		}
	}
	return 0;
}

// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
	if (gGameState != GAMESTATE_INGAME) return;

	// We have requested an item link.
	if (LinkState == 1 && LinkTimer < GetTickCount64())
	{
		char szKey[MAX_STRING];

		Evaluate(szKey, "${DisplayItem}");
		if (_stricmp(szKey, LinkedItem.Name) == 0)
		{
			SetItemByTLO(&LinkedItem, "DisplayItem", NULL, NULL);
			DoItemScore(TRUE);
			LinkState = 0;
			LinkTimer = 0;
		}
		else {
			LinkTimer = GetTickCount64() + 200;
		}
	}
}


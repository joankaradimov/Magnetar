#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <stdio.h>
#include <Windows.h>
#include <ddraw.h>
#include <Mmreg.h>
#include <DSound.h>

#define __unaligned
#define __hidden
#define __far
#define __noreturn

// Some convenience macros to make partial accesses nicer
// first unsigned macros:
#undef LOBYTE
#define LOBYTE(x)   (*((_BYTE*)&(x)))   // low byte
#undef LOWORD
#define LOWORD(x)   (*((_WORD*)&(x)))   // low word
#define LODWORD(x)  (*((_DWORD*)&(x)))  // low dword
#undef HIBYTE
#define HIBYTE(x)   (*((_BYTE*)&(x)+sizeof(x)-1))
#undef HIWORD
#define HIWORD(x)   (*((_WORD*)&(x)+sizeof(x)/2-1))
#define HIDWORD(x)  (*((_DWORD*)&(x)+1))
#define BYTEn(x, n)   (*((_BYTE*)&(x)+n))
#define WORDn(x, n)   (*((_WORD*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)         // byte 1 (counting from 0)
#define BYTE2(x)   BYTEn(x,  2)
#define BYTE3(x)   BYTEn(x,  3)
#define BYTE4(x)   BYTEn(x,  4)
#define BYTE5(x)   BYTEn(x,  5)
#define BYTE6(x)   BYTEn(x,  6)
#define BYTE7(x)   BYTEn(x,  7)
#define BYTE8(x)   BYTEn(x,  8)
#define BYTE9(x)   BYTEn(x,  9)
#define BYTE10(x)  BYTEn(x, 10)
#define BYTE11(x)  BYTEn(x, 11)
#define BYTE12(x)  BYTEn(x, 12)
#define BYTE13(x)  BYTEn(x, 13)
#define BYTE14(x)  BYTEn(x, 14)
#define BYTE15(x)  BYTEn(x, 15)
#define WORD1(x)   WORDn(x,  1)
#define WORD2(x)   WORDn(x,  2)         // third word of the object, unsigned
#define WORD3(x)   WORDn(x,  3)
#define WORD4(x)   WORDn(x,  4)
#define WORD5(x)   WORDn(x,  5)
#define WORD6(x)   WORDn(x,  6)
#define WORD7(x)   WORDn(x,  7)

// now signed macros (the same but with sign extension)
#define SLOBYTE(x)   (*((__int8*)&(x)))
#define SLOWORD(x)   (*((__int16*)&(x)))
#define SLODWORD(x)  (*((__int32*)&(x)))
#define SHIBYTE(x)   (*((__int8*)&(x)+1))
#define SHIWORD(x)   (*((__int16*)&(x)+1))
#define SHIDWORD(x)  (*((__int32*)&(x)+1))
#define SBYTEn(x, n)   (*((__int8*)&(x)+n))
#define SWORDn(x, n)   (*((__int16*)&(x)+n))
#define SBYTE1(x)   SBYTEn(x,  1)
#define SBYTE2(x)   SBYTEn(x,  2)
#define SBYTE3(x)   SBYTEn(x,  3)
#define SBYTE4(x)   SBYTEn(x,  4)
#define SBYTE5(x)   SBYTEn(x,  5)
#define SBYTE6(x)   SBYTEn(x,  6)
#define SBYTE7(x)   SBYTEn(x,  7)
#define SBYTE8(x)   SBYTEn(x,  8)
#define SBYTE9(x)   SBYTEn(x,  9)
#define SBYTE10(x)  SBYTEn(x, 10)
#define SBYTE11(x)  SBYTEn(x, 11)
#define SBYTE12(x)  SBYTEn(x, 12)
#define SBYTE13(x)  SBYTEn(x, 13)
#define SBYTE14(x)  SBYTEn(x, 14)
#define SBYTE15(x)  SBYTEn(x, 15)
#define SWORD1(x)   SWORDn(x,  1)
#define SWORD2(x)   SWORDn(x,  2)
#define SWORD3(x)   SWORDn(x,  3)
#define SWORD4(x)   SWORDn(x,  4)
#define SWORD5(x)   SWORDn(x,  5)
#define SWORD6(x)   SWORDn(x,  6)
#define SWORD7(x)   SWORDn(x,  7)

namespace game::starcraft
{
typedef __int8 _BYTE;
typedef __int16 _WORD;
typedef __int32 _DWORD;
typedef __int64 _QWORD;
typedef __int8 _TBYTE;

struct PMD;
struct RTTIClassHierarchyDescriptor;
struct RTTICompleteObjectLocator;
struct RTCI;
struct _SCOPETABLE_ENTRY;
enum DialogFlags;
enum DialogType : unsigned __int16;
struct dialog_ctrl;
struct struct_1;
struct struct_2;
struct __declspec(align(2)) DatLoad;
struct ForceName;
enum PlayerType : __int8;
enum RaceId : __int8;
struct PlayerResources;
struct PlayerAlliance;
struct __declspec(align(1)) swishTimer;
struct GameSpeeds;
enum EventNo : unsigned __int16;
enum EventUser;
struct bounds;
enum CSpriteFlags : unsigned __int8;
enum Anims : __int8;
enum ImageFlags : unsigned __int16;
enum RLEType : unsigned __int8;
struct points;
struct __declspec(align(4)) point;
enum Order : unsigned __int8;
enum UnitMovementState : __int8;
struct CUnitFighter;
enum Tech : __int8;
enum UnitType : unsigned __int16;
struct CUnitNydus;
struct CUnitGhost;
struct CUnitPylon;
struct CUnitSilo;
struct CUnitGatherer;
struct CUnitPsiProvider;
enum StatusFlags : unsigned __int32;
struct TransVectorEntry;
struct vx4entry;
struct Chat_TextLine;
enum GamePosition : __int16;
enum MenuPosition : __int32;
enum BulletState : unsigned __int8;
enum WeaponType : unsigned __int8;
struct CThingy;
enum UnitPrototypeFlags;
enum Tileset : unsigned __int16;
struct TileID;
enum MegatileFlags;
struct SpriteTileData;
struct AI_Flags;
struct ChkLoader;
struct __declspec(align(2)) ChkSectionLoader;
struct UnitStat;
struct ButtonSet;
enum PlayerGroups;
struct TriggerList;
enum FlingyID : unsigned __int8;
enum DamageType : __int8;
enum WeaponBehavior : __int8;
enum MapData : __int16;
struct activation_delays;
enum CheatFlags : unsigned __int32;
struct CndSignature;
struct ImagesDatExtraOverlayLO_Files;
struct LO_Overlays;
enum WeaponTargetFlags : __int16;
struct UnitDimentions;
enum MapData4 : __int32;
enum Tech2 : __int16;
enum SfxData;
enum FogOfWar : unsigned __int8;
enum GameType : __int8;
enum PlayerTypes : unsigned __int8;
enum ResourceType : unsigned __int8;
enum StartingPositions : unsigned __int8;
enum StartingUnits : unsigned __int8;
enum UnitStats : unsigned __int8;
enum VictoryConditions : unsigned __int8;
struct struct_5;
struct __declspec(align(2)) AudioVideoInitializationError;
struct __declspec(align(4)) CharacterData;
enum ForceFlags : unsigned __int8;
struct ChunkListItem;
struct Char4_characters;
enum SaiAccessabilityFlags : __int16;
union SaiRegionUser;
struct __declspec(align(4)) EstablishingShotPosition;
enum GluAllTblEntry : __int16;
enum MapDirEntryFlags : unsigned __int8;
struct Timer;
struct __declspec(align(2)) struc_66FE20;
struct MapDownload;
struct __declspec(align(4)) GameActionDataBlock;
struct _RTTIClassHierarchyDescriptor;
struct _RTTICompleteObjectLocator;
struct struct_v6;
struct ListNode;
struct STREAMED;
enum Cinematic : __int8;
struct struct_0;
struct __declspec(align(4)) ChunkNode;
enum UnitGroupFlags : unsigned __int16;
struct __declspec(align(4)) UnitRelated20;
struct __declspec(align(4)) struct_v4;
enum EndgameState : unsigned __int8;
enum MusicTrack;
enum MusicTrackType : unsigned __int8;
struct __declspec(align(4)) TriggerOrderRelated;
enum RegistryOptionsFlags;
struct __declspec(align(4)) BNetGateways;
struct __declspec(align(8)) FontColorRelated;
enum Color : unsigned __int8;
struct DlgGrp;
struct SNETVERSIONDATA;
enum ButtonState;
enum Icon : unsigned __int16;
struct BltMask;
struct RegistryOption;
enum CursorType;
struct struc_659B10;
struct struct_a1_2;
enum PrintFlags : __int8;
struct __declspec(align(4)) struct_a3;
struct __declspec(align(4)) SNETPROGRAMDATA;
enum CommandId : unsigned __int8;
struct struc_64DEC8;
struct SNETCAPS;
struct __declspec(align(4)) SNETUIDATA;
struct SNETPLAYERDATA;
struct struc_51B1E8;
struct TemplateList;
struct struct_a4;
struct __declspec(align(1)) struct_v2;
enum SnetInfoType;
enum IScriptOpcodes : unsigned __int8;
enum ImageOrder;
enum OverlayType : unsigned __int8;
struct __declspec(align(4)) ListNodeUnknown;
struct __declspec(align(4)) struc_59A0F0;
struct ScoreFormatRelated;
struct __unaligned __declspec(align(1)) struct_v0;
struct ShaState;
struct __unaligned __declspec(align(1)) LoadGameCommand;
struct __declspec(align(4)) GluRelated;
struct struct_v3;
enum StormVideoFlags;
struct DownloadListMaybe;
struct struct_v4_1;
struct struct_a2_1;
struct struc_500F60;
struct struc_640898;
enum StatDataDescriptorType : unsigned __int16;
enum ExplosionType : unsigned __int8;
struct RTTIBaseClassDescriptor;
struct _EH3_EXCEPTION_REGISTRATION;
struct Bitmap;
struct __declspec(align(2)) rect;
struct dialog_dlg;
struct dialog_edit;
struct dialog_optn;
struct dialog_scroll;
struct PlayerInfo;
struct __declspec(align(1)) Position;
struct pt;
struct FontChar;
struct Font;
struct layer;
struct UpgradesSC;
struct UpgradesBW;
struct TechSC;
struct TechBW;
struct __declspec(align(2)) IScriptProgramState;
struct grpFrame;
struct __declspec(align(4)) Target_;
struct UnitFinderData;
struct CUnitVulture;
struct CUnitCarrier;
struct CUnitBeacon;
struct __declspec(align(2)) CUnitBuilding;
struct CUnitWorker;
struct CUnitResource;
struct CUnitPowerup;
struct CUnitRally;
struct CUnitFinder;
struct CUnitStatus;
struct SuppliesPerRace;
struct Counts;
struct vr4entry;
struct UnitAvail;
struct ScrollSpeeds;
struct __declspec(align(2)) SightStruct;
struct fontMemStruct;
struct TileTypeFlags;
struct __declspec(align(4)) struct_a1;
struct MiniTileFlagArray;
struct Action;
struct StringTbl;
struct MapSize;
struct __declspec(align(4)) UpdateFunction;
struct __declspec(align(4)) RenderFunction;
struct Box32;
struct Condition;
struct EstablishingShot;
struct s_evt;
struct LO_Header;
struct __declspec(align(2)) ColorShiftData;
struct __declspec(align(2)) AiCaptain;
struct __declspec(align(1)) GotFileValues;
struct struct_6;
struct __declspec(align(2)) LobbySlot;
struct __declspec(align(4)) MapChunks;
union Char4;
struct Box16;
struct SaiContour;
struct __declspec(align(4)) SaiContourHub;
struct SaiSplit;
struct UnknownTilesetRelated2;
struct MinimapSurfaceInfoRelated;
struct CycleStruct;
struct __declspec(align(4)) SFX_related;
struct __declspec(align(2)) CampaignMenuEntry;
struct CinematicIntro;
struct __declspec(align(4)) MissionCheatRelated;
struct __declspec(align(2)) CheatHashMaybe;
struct __declspec(align(4)) GlueRelated;
struct UnitProperties;
struct __declspec(align(2)) MusicTrackDescription;
struct __declspec(align(2)) RegistryOptions;
struct ReplayPlayer;
struct __declspec(align(2)) TypeDropdownSelect;
struct RaceDropdownSelect;
struct __declspec(align(4)) ButtonOrder;
struct ID;
struct struc_581D76;
struct __declspec(align(1)) ReplaySpeedCommand;
struct __declspec(align(1)) SaveGameCommand;
struct CancelNukeCommand;
struct MergeDarkArchonCommand;
struct MergeArchonCommand;
struct DecloakCommand;
struct UnburrowCommand;
struct BurrowCommand;
struct CancelAddonCommand;
struct CancelUpgradeCommand;
struct UpgradeCommand;
struct CancelResearchCommand;
struct ResearchCommand;
struct HoldPositionCommand;
struct TrainFighterCommand;
struct CarrierStopCommand;
struct ReaverStopCommand;
struct StopCommand;
struct CancelUnitMorphCommand;
struct CancelConstructionCommand;
struct SiegeCommand;
struct UnsiegeCommand;
struct __declspec(align(1)) CancelTrainCommand;
struct __declspec(align(1)) TrainCommand;
struct StimpackCommand;
struct CloakCommand;
struct ReturnCargoCommand;
struct __declspec(align(1)) MorphCommand;
struct __declspec(align(1)) BuildingMorphCommand;
struct UnloadAllCommand;
struct RightClickCommand;
struct SetLatencyCommand;
struct RestartGameCommand;
struct TimerRelated;
struct UnknownPathRelated;
struct BytePair;
struct IScript;
struct IScriptProgram;
struct __declspec(align(1)) MinimapPingCommand;
struct __declspec(align(1)) GroupUnitsCommand;
struct UnitGroupRelatedInner;
struct TSAVEGAME;
struct GlueEffectBuffer;
struct StatDataDescriptor;
struct ParallaxData;
struct ParallaxLayerData;
struct ParallaxItem;
struct CPPEH_RECORD;
struct dialog_btn;
struct __declspec(align(1)) dialog_list;
struct dlgEvent;
struct __declspec(align(4)) CSprite;
struct __declspec(align(4)) CImage;
struct __declspec(align(2)) grpHead;
struct __declspec(align(4)) COrder;
struct CUnitHatchery;
union CUnitFields1;
union CUnitFields3;
struct AllScoresStruct;
struct __declspec(align(4)) baseLocation;
struct CBullet;
struct TileType;
struct MiniTileMaps_type;
struct __declspec(align(2)) AI_Main;
struct Trigger;
struct Location;
struct GotFile;
struct __declspec(align(1)) GameData;
struct __declspec(align(4)) Map;
struct __declspec(align(4)) SectionData;
struct SaiRegion;
struct __declspec(align(2)) PathCreateRelated;
struct UnknownTilesetRelated1;
struct __declspec(align(4)) CFlingy;
struct __declspec(align(4)) TPROVIDER;
struct CheatHashRelated;
struct Chunk;
struct ChunkUnitEntry;
struct BriefingEntry;
struct __declspec(align(4)) struct_a1_1;
struct __declspec(align(1)) struct_path_related;
struct StatFlufDialog;
struct TemplateListEntry;
struct __unaligned __declspec(align(1)) LiftCommand;
struct __declspec(align(2)) PlaceBuildingCommand;
struct __declspec(align(1)) Portrait;
struct UnitGroupRelated;
union dialog_fields;
union CUnitFields2;
struct TriggerListEntry;
struct __declspec(align(2)) ChunkData;
struct SAI_Paths;
struct __declspec(align(4)) MapDirEntry;
struct __unaligned __declspec(align(1)) ReplayHeader;
struct __declspec(align(4)) struct_a2;
struct __declspec(align(2)) dialog;
struct __declspec(align(4)) CUnit;



typedef struct _GUID GUID;

#pragma pack(push, 1)
struct PMD
{
  int mdisp;
  int pdisp;
  int vdisp;
};
#pragma pack(pop)
static_assert(sizeof(PMD) == 12, "Incorrect size for type `PMD`. Expected: 12");

#pragma pack(push, 1)
struct RTTIClassHierarchyDescriptor
{
  int signature;
  int attributes;
  int numBaseClasses;
  void *pBaseClassArray;
};
#pragma pack(pop)
static_assert(sizeof(RTTIClassHierarchyDescriptor) == 16, "Incorrect size for type `RTTIClassHierarchyDescriptor`. Expected: 16");

#pragma pack(push, 1)
struct RTTICompleteObjectLocator
{
  int signature;
  int offset;
  int cdOffset;
  void *pTypeDescriptor;
  void *pClassDescriptor;
};
#pragma pack(pop)
static_assert(sizeof(RTTICompleteObjectLocator) == 20, "Incorrect size for type `RTTICompleteObjectLocator`. Expected: 20");

#pragma pack(push, 1)
struct RTCI
{
  void *m_lpszClassName;
  int m_nObjectSize;
  int m_wSchema;
  void *m_pfnCreateObject;
  void *m_pfnGetBaseClass;
  int m_pNextClass;
  int m_pClassInit;
};
#pragma pack(pop)
static_assert(sizeof(RTCI) == 28, "Incorrect size for type `RTCI`. Expected: 28");

typedef struct _SCOPETABLE_ENTRY *PSCOPETABLE_ENTRY;

typedef struct _EH3_EXCEPTION_REGISTRATION *PEH3_EXCEPTION_REGISTRATION;

struct _SCOPETABLE_ENTRY
{
  int EnclosingLevel;
  void *FilterFunc;
  void *HandlerFunc;
};
static_assert(sizeof(_SCOPETABLE_ENTRY) == 12, "Incorrect size for type `_SCOPETABLE_ENTRY`. Expected: 12");

typedef unsigned __int16 u16;

typedef unsigned __int8 u8;

enum DialogFlags
{
  CTRL_UPDATE = 0x1,
  CTRL_DISABLED = 0x2,
  CTRL_ACTIVE = 0x4,
  CTRL_VISIBLE = 0x8,
  CTRL_EVENTS = 0x10,
  CTRL_UNKOWN1 = 0x20,
  CTRL_BTN_CANCEL = 0x40,
  CTRL_BTN_NO_SOUND = 0x80,
  CTRL_BTN_VIRT_HOTKEY = 0x100,
  CTRL_BTN_HOTKEY = 0x200,
  CTRL_FONT_SMALLEST = 0x400,
  CTRL_FONT_LARGE = 0x800,
  CTRL_UNKOWN2 = 0x1000,
  CTRL_TRANSPARENT = 0x2000,
  CTRL_FONT_LARGEST = 0x4000,
  CTRL_UNKOWN3 = 0x8000,
  CTRL_FONT_SMALL = 0x10000,
  CTRL_PLAIN = 0x20000,
  CTRL_TRANSLUCENT = 0x40000,
  CTRL_BTN_DEFAULT = 0x80000,
  CTRL_TOP = 0x100000,
  CTRL_HALIGN_LEFT = 0x200000,
  CTRL_HALIGN_RIGHT = 0x400000,
  CTRL_HALIGN_CENTER = 0x800000,
  CTRL_VALIGN_TOP = 0x1000000,
  CTRL_VALIGN_MIDDLE = 0x2000000,
  CTRL_VALIGN_BOTTOM = 0x4000000,
  CTRL_UNKOWN4 = 0x8000000,
  CTRL_DLG_NOREDRAW = 0x10000000,
  CTRL_REVERSE = 0x10000000,
  CTRL_USELOCALGRAPHIC = 0x20000000,
  CTRL_DLG_ACTIVE = 0x40000000,
  CTRL_LBOX_NORECALC = 0x80000000,
};

enum DialogType : unsigned __int16
{
  cDLG = 0x0,
  cDFLTBTN = 0x1,
  cBUTTON = 0x2,
  cOPTION = 0x3,
  cCHKBOX = 0x4,
  cIMAGE = 0x5,
  cHSCROLL = 0x6,
  cVSCROLL = 0x7,
  cEDIT = 0x8,
  cLSTATIC = 0x9,
  cRSTATIC = 0xA,
  cCSTATIC = 0xB,
  cLIST = 0xC,
  cCOMBO = 0xD,
  cFLCBTN = 0xE,
};

typedef int (__fastcall *FnInteract)(dialog *dlg, dlgEvent *evt);

typedef void (__fastcall *FnUpdate)(dialog *dlg, int x, int y, rect *dst);

typedef __int16 s16;

struct dialog_ctrl
{
  dialog *pDlg;
};
static_assert(sizeof(dialog_ctrl) == 4, "Incorrect size for type `dialog_ctrl`. Expected: 4");

typedef unsigned __int32 u32;

#pragma pack(push, 1)
struct struct_1
{
  struct_1 *field_0;
  char gap_4[28];
  __int16 field_4;
  char gap_22[36];
  char field_5;
  char gap_47[1];
  char field_6;
};
#pragma pack(pop)
static_assert(sizeof(struct_1) == 73, "Incorrect size for type `struct_1`. Expected: 73");

#pragma pack(push, 1)
struct struct_2
{
  int field_0;
};
#pragma pack(pop)
static_assert(sizeof(struct_2) == 4, "Incorrect size for type `struct_2`. Expected: 4");

#pragma pack(push, 1)
struct __declspec(align(2)) DatLoad
{
  void *address;
  int length;
  int entries;
};
#pragma pack(pop)
static_assert(sizeof(DatLoad) == 12, "Incorrect size for type `DatLoad`. Expected: 12");

struct ForceName
{
  char name[30];
};
static_assert(sizeof(ForceName) == 30, "Incorrect size for type `ForceName`. Expected: 30");

enum PlayerType : __int8
{
  PT_NotUsed = 0x0,
  PT_Computer = 0x1,
  PT_Human = 0x2,
  PT_Rescuable = 0x3,
  PT_Unknown0 = 0x4,
  PT_ComputerSlot = 0x5,
  PT_OpenSlot = 0x6,
  PT_Neutral = 0x7,
  PT_ClosedSlot = 0x8,
  PT_Unknown1 = 0x9,
  PT_HumanDefeated = 0xA,
  PT_ComputerDefeated = 0xB,
};

enum RaceId : __int8
{
  RACE_Zerg = 0x0,
  RACE_Terran = 0x1,
  RACE_Protoss = 0x2,
  RACE_Other = 0x3,
  RACE_Unused = 0x4,
  RACE_Select = 0x5,
  RACE_Random = 0x6,
  RACE_None = 0x7,
  RACE_Unknown = 0x8,
};

struct PlayerResources
{
  int minerals[12];
  int gas[12];
  int cumulativeGas[12];
  int cumulativeMinerals[12];
};
static_assert(sizeof(PlayerResources) == 192, "Incorrect size for type `PlayerResources`. Expected: 192");

struct PlayerAlliance
{
  unsigned __int8 player[12];
};
static_assert(sizeof(PlayerAlliance) == 12, "Incorrect size for type `PlayerAlliance`. Expected: 12");

#pragma pack(push, 1)
struct __declspec(align(1)) swishTimer
{
  unsigned __int16 wIndex;
  unsigned __int16 wType;
};
#pragma pack(pop)
static_assert(sizeof(swishTimer) == 4, "Incorrect size for type `swishTimer`. Expected: 4");

struct GameSpeeds
{
  unsigned __int32 gameSpeedModifiers[7];
  unsigned __int32 altSpeedModifiers[7];
};
static_assert(sizeof(GameSpeeds) == 56, "Incorrect size for type `GameSpeeds`. Expected: 56");

typedef __int32 s32;

typedef __int8 s8;

enum EventNo : unsigned __int16
{
  EVN_KEYFIRST = 0x0,
  EVN_KEYDOWN = 0x0,
  EVN_KEYRPT = 0x1,
  EVN_KEYREPEAT = 0x1,
  EVN_KEYUP = 0x2,
  EVN_MOUSEMOVE = 0x3,
  EVN_MOUSEFIRST = 0x3,
  EVN_LBUTTONDOWN = 0x4,
  EVN_LBUTTONUP = 0x5,
  EVN_LBUTTONDBLCLK = 0x6,
  EVN_RBUTTONDOWN = 0x7,
  EVN_RBUTTONUP = 0x8,
  EVN_RBUTTONDBLCLK = 0x9,
  EVN_MBUTTONDOWN = 0xA,
  EVN_MBUTTONUP = 0xB,
  EVN_MBUTTONDBLCLK = 0xC,
  EVN_IDLE = 0xD,
  EVN_USER = 0xE,
  EVN_CHAR = 0xF,
  EVN_SYSCHAR = 0x10,
  EVN_WHEELUP = 0x11,
  EVN_WHEELDWN = 0x12,
};

enum EventUser
{
  USER_CREATE = 0x0,
  USER_DESTROY = 0x1,
  USER_ACTIVATE = 0x2,
  USER_HOTKEY = 0x3,
  USER_MOUSEMOVE = 0x4,
  USER_PREV = 0x5,
  USER_NEXT = 0x6,
  USER_UNK_7 = 0x7,
  USER_UNK_8 = 0x8,
  USER_MOUSEENTER = 0x9,
  USER_INIT = 0xA,
  USER_SELECT = 0xB,
  USER_UNK_12 = 0xC,
  USER_SHOW = 0xD,
  USER_HIDE = 0xE,
  USER_SCROLLUP = 0xF,
  USER_SCROLLDOWN = 0x10,
};

struct bounds
{
  WORD left;
  WORD top;
  WORD right;
  WORD bottom;
  WORD width;
  WORD height;
};
static_assert(sizeof(bounds) == 12, "Incorrect size for type `bounds`. Expected: 12");

typedef unsigned __int8 BYTE;

enum CSpriteFlags : unsigned __int8
{
  CSF_DrawCircle = 0x1,
  CSF_TeamCircle = 0x6,
  CSF_Selected = 0x8,
  CSF_Prop_10 = 0x10,
  CSF_Hidden = 0x20,
  CSF_Burrowed = 0x40,
  CSF_DontIntSeq = 0x80,
};

enum Anims : __int8
{
  AE_Init = 0x0,
  AE_Death = 0x1,
  AE_GndAttkInit = 0x2,
  AE_AirAttkInit = 0x3,
  AE_Unused1 = 0x4,
  AE_GndAttkRpt = 0x5,
  AE_AirAttkRpt = 0x6,
  AE_CastSpell = 0x7,
  AE_GndAttkToIdle = 0x8,
  AE_AirAttkToIdle = 0x9,
  AE_Unused2 = 0xA,
  AE_Walking = 0xB,
  AE_WalkingToIdle = 0xC,
  AE_SpecialState1 = 0xD,
  AE_SpecialState2 = 0xE,
  AE_AlmostBuilt = 0xF,
  AE_Built = 0x10,
  AE_Landing = 0x11,
  AE_LiftOff = 0x12,
  AE_IsWorking = 0x13,
  AE_WorkingToIdle = 0x14,
  AE_WarpIn = 0x15,
  AE_Unused3 = 0x16,
  AE_StarEditInit = 0x17,
  AE_Disable = 0x18,
  AE_Burrow = 0x19,
  AE_UnBurrow = 0x1A,
  AE_Enable = 0x1B,
  AE_COUNT = 0x1C,
};

enum ImageFlags : unsigned __int16
{
  IF_REDRAW = 0x1,
  IF_HORIZONTALLY_FLIPPED = 0x2,
  IF_Y_FROZEN = 0x4,
  IF_HAS_DIRECTIONAL_FRAMES = 0x8,
  IF_HAS_ISCRIPT_ANIMATIONS = 0x10,
  IF_CLICKABLE = 0x20,
  IF_HIDDEN = 0x40,
  IF_USES_SPECIAL_OFFSET = 0x80,
};

enum RLEType : unsigned __int8
{
  RLE_NORMAL = 0x0,
  RLE_NORMAL2 = 0x1,
  RLE_CLOAK_INVIS = 0x2,
  RLE_CLOAKED_INVIS = 0x3,
  RLE_DECLOAK_INVIS = 0x4,
  RLE_CLOAK = 0x5,
  RLE_CLOAKED = 0x6,
  RLE_DECLOAK = 0x7,
  RLE_8 = 0x8,
  RLE_EFFECT = 0x9,
  RLE_SHADOW = 0xA,
  RLE_HPFLOATDRAW = 0xB,
  RLE_TEXTURE = 0xC,
  RLE_SELECTDRAW = 0xD,
  RLE_PLAYER_SIDE = 0xE,
  RLE_GREENBOX = 0xF,
  RLE_SHIFT = 0x10,
  RLE_FADE = 0x11,
};

struct points
{
  __int16 x;
  __int16 y;
};
static_assert(sizeof(points) == 4, "Incorrect size for type `points`. Expected: 4");

struct __declspec(align(4)) point
{
  __int32 x;
  __int32 y;
};
static_assert(sizeof(point) == 8, "Incorrect size for type `point`. Expected: 8");

enum Order : unsigned __int8
{
  ORD_DIE = 0x0,
  ORD_STOP = 0x1,
  ORD_GUARD = 0x2,
  ORD_GUARD_PLAY = 0x3,
  ORD_TURRET_GUARD = 0x4,
  ORD_GUARD_BUNKER = 0x5,
  ORD_IGNORE = 0x6,
  ORD_TRILOBYTE_STOP = 0x7,
  ORD_ATTACK = 0x8,
  ORD_MOVE_ATTACK_UNIT = 0x9,
  ORD_ATTACK_UNIT = 0xA,
  ORD_ATTACK_FIXED_RNGE = 0xB,
  ORD_ATTACK_TILE = 0xC,
  ORD_ATTACK_HOVER = 0xD,
  ORD_ATTACK_MOVE = 0xE,
  ORD_DRONE_INFEST0 = 0xF,
  ORD_NOTHING0 = 0x10,
  ORD_POWERUP0 = 0x11,
  ORD_TOWER_GUARD = 0x12,
  ORD_TOWER_ATTACK = 0x13,
  ORD_VULTURE_MINE = 0x14,
  ORD_STAY_IN_RANGE = 0x15,
  ORD_TURRET_ATTACK = 0x16,
  ORD_NOTHING = 0x17,
  ORD_NOTHING2 = 0x18,
  ORD_DRONE_START_BUILD = 0x19,
  ORD_DRONE_BUILD = 0x1A,
  ORD_DRONE_INFEST = 0x1B,
  ORD_DRONE_INFEST2 = 0x1C,
  ORD_QUEEN_INFEST = 0x1D,
  ORD_BUILD = 0x1E,
  ORD_BUILDPROTOSS = 0x1F,
  ORD_PYLON_BUILD = 0x20,
  ORD_CONSTRUCT_BLDG = 0x21,
  ORD_REPAIR = 0x22,
  ORD_REPAIR_MOVE = 0x23,
  ORD_PLACE_ADDON = 0x24,
  ORD_BUILD_ADDON = 0x25,
  ORD_TRAIN = 0x26,
  ORD_RALLY_POINT = 0x27,
  ORD_RALLY_POINT_TILE = 0x28,
  ORD_ZERG_BIRTH = 0x29,
  ORD_MORPH = 0x2A,
  ORD_ZERG_BLDG_MORPH = 0x2B,
  ORD_BUILDSELF = 0x2C,
  ORD_BUILDSELFZERG = 0x2D,
  ORD_BUILDNYDUS = 0x2E,
  ORD_ENTER_NYDUS = 0x2F,
  ORD_BUILDSELFPROTOSS = 0x30,
  ORD_FOLLOW = 0x31,
  ORD_CARRIER = 0x32,
  ORD_CARRIER_IGNORE = 0x33,
  ORD_CARRIER_STOP = 0x34,
  ORD_CARRIER_ATTACK = 0x35,
  ORD_CARRIER_ATTACK_MOVE = 0x36,
  ORD_CARRIER_IGNORE2 = 0x37,
  ORD_CARRIER_FIGHT = 0x38,
  ORD_CARRIER_HOLDPOS = 0x39,
  ORD_REAVER = 0x3A,
  ORD_REAVER_ATTACK = 0x3B,
  ORD_REAVER_ATTACK_MOVE = 0x3C,
  ORD_REAVER_FIGHT = 0x3D,
  ORD_REAVER_HOLD = 0x3E,
  ORD_TRAIN_FIGHTER = 0x3F,
  ORD_STRAFE_UNIT = 0x40,
  ORD_STRAFE_UNIT_SCARAB = 0x41,
  ORD_RECHARGE_UNIT = 0x42,
  ORD_RECHARGE = 0x43,
  ORD_SHIELD_BATTERY = 0x44,
  ORD_RETURN = 0x45,
  ORD_DRONE_LAND = 0x46,
  ORD_BLDG_LAND = 0x47,
  ORD_BLDG_LIFT_OFF = 0x48,
  ORD_DRONE_LIFT_OFF = 0x49,
  ORD_LIFT_OFF = 0x4A,
  ORD_RESEARCH_TECH = 0x4B,
  ORD_UPGRADE = 0x4C,
  ORD_LARVA = 0x4D,
  ORD_SPAWN_LARVA = 0x4E,
  ORD_HARVEST = 0x4F,
  ORD_HARVEST_MOVE = 0x50,
  ORD_HARVEST_GAS = 0x51,
  ORD_ENTER_GAS = 0x52,
  ORD_IN_GAS = 0x53,
  ORD_RETURN_GAS = 0x54,
  ORD_HARVEST_MINERALS = 0x55,
  ORD_HARVEST_MINERALS_WAIT = 0x56,
  ORD_HARVESTING_MINERALS = 0x57,
  ORD_HARVEST_INTERRUPT = 0x58,
  ORD_HARVEST4 = 0x59,
  ORD_RETURN_MINERALS = 0x5A,
  ORD_HARVEST_INTERRUPT2 = 0x5B,
  ORD_ENTER_TRANSPORT = 0x5C,
  ORD_PICK_UP = 0x5D,
  ORD_PICK_UP_MOBILE = 0x5E,
  ORD_PICK_UP_STATIONARY = 0x5F,
  ORD_PICK_UP_UNK = 0x60,
  ORD_POWERUP = 0x61,
  ORD_SIEGEMODE = 0x62,
  ORD_TANKMODE = 0x63,
  ORD_WATCHTARGET = 0x64,
  ORD_INIT_SPREAD_CREEP = 0x65,
  ORD_SPREAD_CREEP = 0x66,
  ORD_STOP_SPREAD_CREEP = 0x67,
  ORD_GUARDIANASPECT = 0x68,
  ORD_SUMMON_ARCHON = 0x69,
  ORD_SUMMON_COMPLETE = 0x6A,
  ORD_HOLDPOS = 0x6B,
  ORD_HOLDPOS_QUEEN = 0x6C,
  ORD_CLOAK = 0x6D,
  ORD_DECLOAK = 0x6E,
  ORD_UNLOAD = 0x6F,
  ORD_MOVE_UNLOAD = 0x70,
  ORD_YAMATO = 0x71,
  ORD_YAMATO_MOVE = 0x72,
  ORD_MAGNAPULSE = 0x73,
  ORD_BURROW = 0x74,
  ORD_BURROWED = 0x75,
  ORD_DEBURROW = 0x76,
  ORD_DARK_SWARM = 0x77,
  ORD_CAST_PARASITE = 0x78,
  ORD_SUMMON_BROODLINGS = 0x79,
  ORD_EMP_SHOCKWAVE = 0x7A,
  ORD_NUKE_WAIT = 0x7B,
  ORD_NUKE_TRAIN = 0x7C,
  ORD_NUKE_LAUNCH = 0x7D,
  ORD_NUKE_PAINT = 0x7E,
  ORD_NUKE_UNIT = 0x7F,
  ORD_NUKE_GROUND = 0x80,
  ORD_NUKE_TRACK = 0x81,
  ORD_ARBITER = 0x82,
  ORD_CLOAK_NEIGHBORS = 0x83,
  ORD_PLACE_MINE = 0x84,
  ORD_RCLICK = 0x85,
  ORD_SAP_UNIT = 0x86,
  ORD_SAP_LOC = 0x87,
  ORD_SAP_HOLDPOS = 0x88,
  ORD_TELEPORT = 0x89,
  ORD_TELEPORT_LOC = 0x8A,
  ORD_PLACE_SCANNER = 0x8B,
  ORD_SCANNER = 0x8C,
  ORD_DEF_MAT = 0x8D,
  ORD_PSI_STORM = 0x8E,
  ORD_IRRADIATE = 0x8F,
  ORD_PLAGUE = 0x90,
  ORD_CONSUME = 0x91,
  ORD_ENSNARE = 0x92,
  ORD_STASIS = 0x93,
  ORD_HALLUCINATE = 0x94,
  ORD_HALLUCINATION = 0x95,
  ORD_UNDO_FLYER = 0x96,
  ORD_UNDO_TRANSPARENT_WORKER = 0x97,
  ORD_PATROL = 0x98,
  ORD_CTF_COP_INIT = 0x99,
  ORD_CTF_COP = 0x9A,
  ORD_CTF_COP_2 = 0x9B,
  ORD_COMP_AI = 0x9C,
  ORD_ATTACK_MOVE_EP = 0x9D,
  ORD_HARASS_MOVE_EP = 0x9E,
  ORD_AI_PATROL = 0x9F,
  ORD_GUARD_POST = 0xA0,
  ORD_RESCUE_PASSIVE = 0xA1,
  ORD_NEUTRAL = 0xA2,
  ORD_COMP_RETURN = 0xA3,
  ORD_JUICE_INIT = 0xA4,
  ORD_SELF_DESTRUCT = 0xA5,
  ORD_CRITTER = 0xA6,
  ORD_HIDDEN_GUN = 0xA7,
  ORD_OPEN_DOOR = 0xA8,
  ORD_CLOSE_DOOR = 0xA9,
  ORD_HIDE_TRAP = 0xAA,
  ORD_REVEAL_TRAP = 0xAB,
  ORD_ENABLE_DOODAD = 0xAC,
  ORD_DISABLE_DOODAD = 0xAD,
  ORD_PROTOSS_WARP_IN = 0xAE,
  ORD_MEDIC = 0xAF,
  ORD_MEDIC_HEAL = 0xB0,
  ORD_MEDIC_HEALMOVE = 0xB1,
  ORD_MEDIC_HOLDPOS = 0xB2,
  ORD_MEDIC_RETURN_TO_IDLE = 0xB3,
  ORD_MEDIC_CURE = 0xB4,
  ORD_CAST_DISRUPTOR = 0xB5,
  ORD_MINDCONTROL = 0xB6,
  ORD_SUMMON_DARCHON = 0xB7,
  ORD_PSYFEEDBACK = 0xB8,
  ORD_MYOPIA = 0xB9,
  ORD_PARALIZE = 0xBA,
  ORD_JUNK_YARD_DOG = 0xBB,
  ORD_FATAL = 0xBC,
  ORD_None = 0xBD,
  ORD_Unknown = 0xBE,
  ORD_MAX = 0xBF,
};

enum UnitMovementState : __int8
{
  UM_Init = 0x0,
  UM_InitSeq = 0x1,
  UM_Lump = 0x2,
  UM_Turret = 0x3,
  UM_Bunker = 0x4,
  UM_BldgTurret = 0x5,
  UM_Hidden = 0x6,
  UM_Flyer = 0x7,
  UM_FakeFlyer = 0x8,
  UM_AtRest = 0x9,
  UM_Dormant = 0xA,
  UM_AtMoveTarget = 0xB,
  UM_CheckIllegal = 0xC,
  UM_MoveToLegal = 0xD,
  UM_LumpWannabe = 0xE,
  UM_FailedPath = 0xF,
  UM_RetryPath = 0x10,
  UM_StartPath = 0x11,
  UM_UIOrderDelay = 0x12,
  UM_TurnAndStart = 0x13,
  UM_FaceTarget = 0x14,
  UM_NewMoveTarget = 0x15,
  UM_AnotherPath = 0x16,
  UM_Repath = 0x17,
  UM_RepathMovers = 0x18,
  UM_FollowPath = 0x19,
  UM_ScoutPath = 0x1A,
  UM_ScoutFree = 0x1B,
  UM_FixCollision = 0x1C,
  UM_WaitFree = 0x1D,
  UM_GetFree = 0x1E,
  UM_SlidePrep = 0x1F,
  UM_SlideFree = 0x20,
  UM_ForceMoveFree = 0x21,
  UM_FixTerrain = 0x22,
  UM_TerrainSlide = 0x23,
};

struct CUnitFighter
{
  CUnit *parent;
  CUnit *prev;
  CUnit *next;
  bool inHanger;
};
static_assert(sizeof(CUnitFighter) == 16, "Incorrect size for type `CUnitFighter`. Expected: 16");

enum Tech : __int8
{
  TECH_stim_packs = 0x0,
  TECH_lockdown = 0x1,
  TECH_emp_shockwave = 0x2,
  TECH_spider_mines = 0x3,
  TECH_scanner_sweep = 0x4,
  TECH_tank_siege_mode = 0x5,
  TECH_defensive_matrix = 0x6,
  TECH_irradiate = 0x7,
  TECH_yamato_gun = 0x8,
  TECH_cloaking_field = 0x9,
  TECH_personnel_cloaking = 0xA,
  TECH_burrowing = 0xB,
  TECH_infestation = 0xC,
  TECH_spawn_broodlings = 0xD,
  TECH_dark_swarm = 0xE,
  TECH_plague = 0xF,
  TECH_consume = 0x10,
  TECH_ensnare = 0x11,
  TECH_parasite = 0x12,
  TECH_psionic_storm = 0x13,
  TECH_hallucination = 0x14,
  TECH_recall = 0x15,
  TECH_stasis_field = 0x16,
  TECH_archon_warp = 0x17,
  TECH_restoration = 0x18,
  TECH_disruption_web = 0x19,
  TECH_unknown_tech26 = 0x1A,
  TECH_mind_control = 0x1B,
  TECH_dark_archon_meld = 0x1C,
  TECH_feedback = 0x1D,
  TECH_optical_flare = 0x1E,
  TECH_maelstorm = 0x1F,
  TECH_lurker_aspect = 0x20,
  TECH_unknown_tech33 = 0x21,
  TECH_healing = 0x22,
  TECH_unknown_tech35 = 0x23,
  TECH_unknown_tech36 = 0x24,
  TECH_unknown_tech37 = 0x25,
  TECH_unknown_tech38 = 0x26,
  TECH_unknown_tech39 = 0x27,
  TECH_unknown_tech40 = 0x28,
  TECH_unknown_tech41 = 0x29,
  TECH_unknown_tech42 = 0x2A,
  TECH_unknown_tech43 = 0x2B,
  TECH_none = 0x2C,
};

enum UnitType : unsigned __int16
{
  Terran_Marine = 0x0,
  Terran_Ghost = 0x1,
  Terran_Vulture = 0x2,
  Terran_Goliath = 0x3,
  Terran_Goliath_Turret = 0x4,
  Terran_Siege_Tank_Tank_Mode = 0x5,
  Terran_Siege_Tank_Tank_Mode_Turret = 0x6,
  Terran_SCV = 0x7,
  Terran_Wraith = 0x8,
  Terran_Science_Vessel = 0x9,
  Hero_Gui_Montag = 0xA,
  Terran_Dropship = 0xB,
  Terran_Battlecruiser = 0xC,
  Terran_Vulture_Spider_Mine = 0xD,
  Terran_Nuclear_Missile = 0xE,
  Terran_Civilian = 0xF,
  Hero_Sarah_Kerrigan = 0x10,
  Hero_Alan_Schezar = 0x11,
  Hero_Alan_Schezar_Turret = 0x12,
  Hero_Jim_Raynor_Vulture = 0x13,
  Hero_Jim_Raynor_Marine = 0x14,
  Hero_Tom_Kazansky = 0x15,
  Hero_Magellan = 0x16,
  Hero_Edmund_Duke_Tank_Mode = 0x17,
  Hero_Edmund_Duke_Tank_Mode_Turret = 0x18,
  Hero_Edmund_Duke_Siege_Mode = 0x19,
  Hero_Edmund_Duke_Siege_Mode_Turret = 0x1A,
  Hero_Arcturus_Mengsk = 0x1B,
  Hero_Hyperion = 0x1C,
  Hero_Norad_II = 0x1D,
  Terran_Siege_Tank_Siege_Mode = 0x1E,
  Terran_Siege_Tank_Siege_Mode_Turret = 0x1F,
  Terran_Firebat = 0x20,
  Spell_Scanner_Sweep = 0x21,
  Terran_Medic = 0x22,
  Zerg_Larva = 0x23,
  Zerg_Egg = 0x24,
  Zerg_Zergling = 0x25,
  Zerg_Hydralisk = 0x26,
  Zerg_Ultralisk = 0x27,
  Zerg_Broodling = 0x28,
  Zerg_Drone = 0x29,
  Zerg_Overlord = 0x2A,
  Zerg_Mutalisk = 0x2B,
  Zerg_Guardian = 0x2C,
  Zerg_Queen = 0x2D,
  Zerg_Defiler = 0x2E,
  Zerg_Scourge = 0x2F,
  Hero_Torrasque = 0x30,
  Hero_Matriarch = 0x31,
  Zerg_Infested_Terran = 0x32,
  Hero_Infested_Kerrigan = 0x33,
  Hero_Unclean_One = 0x34,
  Hero_Hunter_Killer = 0x35,
  Hero_Devouring_One = 0x36,
  Hero_Kukulza_Mutalisk = 0x37,
  Hero_Kukulza_Guardian = 0x38,
  Hero_Yggdrasill = 0x39,
  Terran_Valkyrie = 0x3A,
  Zerg_Cocoon = 0x3B,
  Protoss_Corsair = 0x3C,
  Protoss_Dark_Templar = 0x3D,
  Zerg_Devourer = 0x3E,
  Protoss_Dark_Archon = 0x3F,
  Protoss_Probe = 0x40,
  Protoss_Zealot = 0x41,
  Protoss_Dragoon = 0x42,
  Protoss_High_Templar = 0x43,
  Protoss_Archon = 0x44,
  Protoss_Shuttle = 0x45,
  Protoss_Scout = 0x46,
  Protoss_Arbiter = 0x47,
  Protoss_Carrier = 0x48,
  Protoss_Interceptor = 0x49,
  Hero_Dark_Templar = 0x4A,
  Hero_Zeratul = 0x4B,
  Hero_Tassadar_Zeratul_Archon = 0x4C,
  Hero_Fenix_Zealot = 0x4D,
  Hero_Fenix_Dragoon = 0x4E,
  Hero_Tassadar = 0x4F,
  Hero_Mojo = 0x50,
  Hero_Warbringer = 0x51,
  Hero_Gantrithor = 0x52,
  Protoss_Reaver = 0x53,
  Protoss_Observer = 0x54,
  Protoss_Scarab = 0x55,
  Hero_Danimoth = 0x56,
  Hero_Aldaris = 0x57,
  Hero_Artanis = 0x58,
  Critter_Rhynadon = 0x59,
  Critter_Bengalaas = 0x5A,
  Special_Cargo_Ship = 0x5B,
  Special_Mercenary_Gunship = 0x5C,
  Critter_Scantid = 0x5D,
  Critter_Kakaru = 0x5E,
  Critter_Ragnasaur = 0x5F,
  Critter_Ursadon = 0x60,
  Zerg_Lurker_Egg = 0x61,
  Hero_Raszagal = 0x62,
  Hero_Samir_Duran = 0x63,
  Hero_Alexei_Stukov = 0x64,
  Special_Map_Revealer = 0x65,
  Hero_Gerard_DuGalle = 0x66,
  Zerg_Lurker = 0x67,
  Hero_Infested_Duran = 0x68,
  Spell_Disruption_Web = 0x69,
  Terran_Command_Center = 0x6A,
  Terran_Comsat_Station = 0x6B,
  Terran_Nuclear_Silo = 0x6C,
  Terran_Supply_Depot = 0x6D,
  Terran_Refinery = 0x6E,
  Terran_Barracks = 0x6F,
  Terran_Academy = 0x70,
  Terran_Factory = 0x71,
  Terran_Starport = 0x72,
  Terran_Control_Tower = 0x73,
  Terran_Science_Facility = 0x74,
  Terran_Covert_Ops = 0x75,
  Terran_Physics_Lab = 0x76,
  Unused_Terran1 = 0x77,
  Terran_Machine_Shop = 0x78,
  Unused_Terran2 = 0x79,
  Terran_Engineering_Bay = 0x7A,
  Terran_Armory = 0x7B,
  Terran_Missile_Turret = 0x7C,
  Terran_Bunker = 0x7D,
  Special_Crashed_Norad_II = 0x7E,
  Special_Ion_Cannon = 0x7F,
  Powerup_Uraj_Crystal = 0x80,
  Powerup_Khalis_Crystal = 0x81,
  Zerg_Infested_Command_Center = 0x82,
  Zerg_Hatchery = 0x83,
  Zerg_Lair = 0x84,
  Zerg_Hive = 0x85,
  Zerg_Nydus_Canal = 0x86,
  Zerg_Hydralisk_Den = 0x87,
  Zerg_Defiler_Mound = 0x88,
  Zerg_Greater_Spire = 0x89,
  Zerg_Queens_Nest = 0x8A,
  Zerg_Evolution_Chamber = 0x8B,
  Zerg_Ultralisk_Cavern = 0x8C,
  Zerg_Spire = 0x8D,
  Zerg_Spawning_Pool = 0x8E,
  Zerg_Creep_Colony = 0x8F,
  Zerg_Spore_Colony = 0x90,
  Unused_Zerg1 = 0x91,
  Zerg_Sunken_Colony = 0x92,
  Special_Overmind_With_Shell = 0x93,
  Special_Overmind = 0x94,
  Zerg_Extractor = 0x95,
  Special_Mature_Chrysalis = 0x96,
  Special_Cerebrate = 0x97,
  Special_Cerebrate_Daggoth = 0x98,
  Unused_Zerg2 = 0x99,
  Protoss_Nexus = 0x9A,
  Protoss_Robotics_Facility = 0x9B,
  Protoss_Pylon = 0x9C,
  Protoss_Assimilator = 0x9D,
  Unused_Protoss1 = 0x9E,
  Protoss_Observatory = 0x9F,
  Protoss_Gateway = 0xA0,
  Unused_Protoss2 = 0xA1,
  Protoss_Photon_Cannon = 0xA2,
  Protoss_Citadel_of_Adun = 0xA3,
  Protoss_Cybernetics_Core = 0xA4,
  Protoss_Templar_Archives = 0xA5,
  Protoss_Forge = 0xA6,
  Protoss_Stargate = 0xA7,
  Special_Stasis_Cell_Prison = 0xA8,
  Protoss_Fleet_Beacon = 0xA9,
  Protoss_Arbiter_Tribunal = 0xAA,
  Protoss_Robotics_Support_Bay = 0xAB,
  Protoss_Shield_Battery = 0xAC,
  Special_Khaydarin_Crystal_Form = 0xAD,
  Special_Protoss_Temple = 0xAE,
  Special_XelNaga_Temple = 0xAF,
  Resource_Mineral_Field = 0xB0,
  Resource_Mineral_Field_Type_2 = 0xB1,
  Resource_Mineral_Field_Type_3 = 0xB2,
  Unused_Cave = 0xB3,
  Unused_Cave_In = 0xB4,
  Unused_Cantina = 0xB5,
  Unused_Mining_Platform = 0xB6,
  Unused_Independant_Command_Center = 0xB7,
  Special_Independant_Starport = 0xB8,
  Unused_Independant_Jump_Gate = 0xB9,
  Unused_Ruins = 0xBA,
  Unused_Khaydarin_Crystal_Formation = 0xBB,
  Resource_Vespene_Geyser = 0xBC,
  Special_Warp_Gate = 0xBD,
  Special_Psi_Disrupter = 0xBE,
  Unused_Zerg_Marker = 0xBF,
  Unused_Terran_Marker = 0xC0,
  Unused_Protoss_Marker = 0xC1,
  Special_Zerg_Beacon = 0xC2,
  Special_Terran_Beacon = 0xC3,
  Special_Protoss_Beacon = 0xC4,
  Special_Zerg_Flag_Beacon = 0xC5,
  Special_Terran_Flag_Beacon = 0xC6,
  Special_Protoss_Flag_Beacon = 0xC7,
  Special_Power_Generator = 0xC8,
  Special_Overmind_Cocoon = 0xC9,
  Spell_Dark_Swarm = 0xCA,
  Special_Floor_Missile_Trap = 0xCB,
  Special_Floor_Hatch = 0xCC,
  Special_Upper_Level_Door = 0xCD,
  Special_Right_Upper_Level_Door = 0xCE,
  Special_Pit_Door = 0xCF,
  Special_Right_Pit_Door = 0xD0,
  Special_Floor_Gun_Trap = 0xD1,
  Special_Wall_Missile_Trap = 0xD2,
  Special_Wall_Flame_Trap = 0xD3,
  Special_Right_Wall_Missile_Trap = 0xD4,
  Special_Right_Wall_Flame_Trap = 0xD5,
  Special_Start_Location = 0xD6,
  Powerup_Flag = 0xD7,
  Powerup_Young_Chrysalis = 0xD8,
  Powerup_Psi_Emitter = 0xD9,
  Powerup_Data_Disk = 0xDA,
  Powerup_Khaydarin_Crystal = 0xDB,
  Powerup_Mineral_Cluster_Type_1 = 0xDC,
  Powerup_Mineral_Cluster_Type_2 = 0xDD,
  Powerup_Protoss_Gas_Orb_Type_1 = 0xDE,
  Powerup_Protoss_Gas_Orb_Type_2 = 0xDF,
  Powerup_Zerg_Gas_Sac_Type_1 = 0xE0,
  Powerup_Zerg_Gas_Sac_Type_2 = 0xE1,
  Powerup_Terran_Gas_Tank_Type_1 = 0xE2,
  Powerup_Terran_Gas_Tank_Type_2 = 0xE3,
  UT_MAX = 0xE4,
};

struct CUnitNydus
{
  CUnit *exit;
};
static_assert(sizeof(CUnitNydus) == 4, "Incorrect size for type `CUnitNydus`. Expected: 4");

struct CUnitGhost
{
  CSprite *nukeDot;
};
static_assert(sizeof(CUnitGhost) == 4, "Incorrect size for type `CUnitGhost`. Expected: 4");

struct CUnitPylon
{
  CSprite *pPowerTemplate;
};
static_assert(sizeof(CUnitPylon) == 4, "Incorrect size for type `CUnitPylon`. Expected: 4");

struct CUnitSilo
{
  CUnit *pNuke;
  bool bReady;
};
static_assert(sizeof(CUnitSilo) == 8, "Incorrect size for type `CUnitSilo`. Expected: 8");

struct CUnitGatherer
{
  CUnit *harvestTarget;
  CUnit *prevHarvestUnit;
  CUnit *nextHarvestUnit;
};
static_assert(sizeof(CUnitGatherer) == 12, "Incorrect size for type `CUnitGatherer`. Expected: 12");

struct CUnitPsiProvider
{
  CUnit *prevPsiProvider;
  CUnit *nextPsiProvider;
};
static_assert(sizeof(CUnitPsiProvider) == 8, "Incorrect size for type `CUnitPsiProvider`. Expected: 8");

enum StatusFlags : unsigned __int32
{
  Completed = 0x1,
  GroundedBuilding = 0x2,
  InAir = 0x4,
  Disabled = 0x8,
  Burrowed = 0x10,
  InBuilding = 0x20,
  InTransport = 0x40,
  UNKNOWN1 = 0x80,
  RequiresDetection = 0x100,
  Cloaked = 0x200,
  DoodadStatesThing = 0x400,
  CloakingForFree = 0x800,
  CanNotReceiveOrders = 0x1000,
  NoBrkCodeStart = 0x2000,
  UNKNOWN2 = 0x4000,
  CanNotAttack = 0x8000,
  IsAUnit = 0x10000,
  IsABuilding = 0x20000,
  IgnoreTileCollision = 0x40000,
  UNKNOWN4 = 0x80000,
  IsNormal = 0x100000,
  NoCollide = 0x200000,
  UNKNOWN5 = 0x400000,
  IsGathering = 0x800000,
  UNKNOWN6 = 0x1000000,
  UNKNOWN7 = 0x2000000,
  Invincible = 0x4000000,
  HoldingPosition = 0x8000000,
  SpeedUpgrade = 0x10000000,
  CooldownUpgrade = 0x20000000,
  IsHallucination = 0x40000000,
  IsSelfDestructing = 0x80000000,
};

struct TransVectorEntry
{
  HANDLE hTrans;
  RECT info;
  DWORD dwReserved;
};
static_assert(sizeof(TransVectorEntry) == 24, "Incorrect size for type `TransVectorEntry`. Expected: 24");

struct vx4entry
{
  WORD wImageRef[4][4];
};
static_assert(sizeof(vx4entry) == 32, "Incorrect size for type `vx4entry`. Expected: 32");

struct Chat_TextLine
{
  char chars[218];
};
static_assert(sizeof(Chat_TextLine) == 218, "Incorrect size for type `Chat_TextLine`. Expected: 218");

enum GamePosition : __int16
{
  GAME_INTRO = 0x0,
  GAME_RUNINIT = 0x1,
  GAME_EXIT = 0x2,
  GAME_RUN = 0x3,
  GAME_GLUES = 0x4,
  GAME_RESTART = 0x5,
  GAME_WIN = 0x6,
  GAME_LOSE = 0x7,
  GAME_CREDITS = 0x8,
  GAME_EPILOG = 0x9,
  GAME_CINEMATIC = 0xA,
  GAME_MAX = 0xB,
};

enum MenuPosition : __int32
{
  GLUE_MAIN_MENU = 0x0,
  GLUE_SIMULATE = 0x1,
  GLUE_CONNECT = 0x2,
  GLUE_CHAT = 0x3,
  GLUE_BATTLE = 0x4,
  GLUE_LOGIN = 0x5,
  GLUE_CAMPAIGN = 0x6,
  GLUE_READY_T = 0x7,
  GLUE_READY_Z = 0x8,
  GLUE_READY_P = 0x9,
  GLUE_GAME_SELECT = 0xA,
  GLUE_CREATE = 0xB,
  GLUE_CREATE_MULTI = 0xC,
  GLUE_LOAD = 0xD,
  GLUE_SCORE_Z_DEFEAT = 0xE,
  GLUE_SCORE_Z_VICTORY = 0xF,
  GLUE_SCORE_T_DEFEAT = 0x10,
  GLUE_SCORE_T_VICTORY = 0x11,
  GLUE_SCORE_P_DEFEAT = 0x12,
  GLUE_SCORE_P_VICTORY = 0x13,
  GLUE_MODEM = 0x14,
  GLUE_DIRECT = 0x15,
  GLUE_EX_CAMPAIGN = 0x16,
  GLUE_GAME_MODE = 0x17,
  GLUE_EX_GAME_MODE = 0x18,
  GLUE_GENERIC = 0x19,
  GLUE_MAX = 0x1A,
};

enum BulletState : unsigned __int8
{
  Init = 0x0,
  MovingToPosition = 0x1,
  MovingToUnit = 0x2,
  Bounce = 0x3,
  TargetGround = 0x4,
  ReachedDestination = 0x5,
  MovingNearUnit = 0x6,
};

enum WeaponType : unsigned __int8
{
  Gauss_Rifle = 0x0,
  Gauss_Rifle_Jim_Raynor = 0x1,
  C_10_Canister_Rifle = 0x2,
  C_10_Canister_Rifle_Sarah_Kerrigan = 0x3,
  Fragmentation_Grenade = 0x4,
  Fragmentation_Grenade_Jim_Raynor = 0x5,
  Spider_Mines = 0x6,
  Twin_Autocannons = 0x7,
  Hellfire_Missile_Pack = 0x8,
  Twin_Autocannons_Alan_Schezar = 0x9,
  Hellfire_Missile_Pack_Alan_Schezar = 0xA,
  Arclite_Cannon = 0xB,
  Arclite_Cannon_Edmund_Duke = 0xC,
  Fusion_Cutter = 0xD,
  Gemini_Missiles = 0xF,
  Burst_Lasers = 0x10,
  Gemini_Missiles_Tom_Kazansky = 0x11,
  Burst_Lasers_Tom_Kazansky = 0x12,
  ATS_Laser_Battery = 0x13,
  ATA_Laser_Battery = 0x14,
  ATS_Laser_Battery_Hero = 0x15,
  ATA_Laser_Battery_Hero = 0x16,
  ATS_Laser_Battery_Hyperion = 0x17,
  ATA_Laser_Battery_Hyperion = 0x18,
  Flame_Thrower = 0x19,
  Flame_Thrower_Gui_Montag = 0x1A,
  Arclite_Shock_Cannon = 0x1B,
  Arclite_Shock_Cannon_Edmund_Duke = 0x1C,
  Longbolt_Missile = 0x1D,
  Yamato_Gun = 0x1E,
  Nuclear_Strike = 0x1F,
  Lockdown = 0x20,
  EMP_Shockwave = 0x21,
  Irradiate = 0x22,
  Claws = 0x23,
  Claws_Devouring_One = 0x24,
  Claws_Infested_Kerrigan = 0x25,
  Needle_Spines = 0x26,
  Needle_Spines_Hunter_Killer = 0x27,
  Kaiser_Blades = 0x28,
  Kaiser_Blades_Torrasque = 0x29,
  Toxic_Spores = 0x2A,
  Spines = 0x2B,
  Acid_Spore = 0x2E,
  Acid_Spore_Kukulza = 0x2F,
  Glave_Wurm = 0x30,
  Glave_Wurm_Kukulza = 0x31,
  Seeker_Spores = 0x34,
  Subterranean_Tentacle = 0x35,
  Suicide_Infested_Terran = 0x36,
  Suicide_Scourge = 0x37,
  Parasite = 0x38,
  Spawn_Broodlings = 0x39,
  Ensnare = 0x3A,
  Dark_Swarm = 0x3B,
  Plague = 0x3C,
  Consume = 0x3D,
  Particle_Beam = 0x3E,
  Psi_Blades = 0x40,
  Psi_Blades_Fenix = 0x41,
  Phase_Disruptor = 0x42,
  Phase_Disruptor_Fenix = 0x43,
  Psi_Assault = 0x45,
  Psionic_Shockwave = 0x46,
  Psionic_Shockwave_TZ_Archon = 0x47,
  Dual_Photon_Blasters = 0x49,
  Anti_Matter_Missiles = 0x4A,
  Dual_Photon_Blasters_Mojo = 0x4B,
  Anti_Matter_Missiles_Mojo = 0x4C,
  Phase_Disruptor_Cannon = 0x4D,
  Phase_Disruptor_Cannon_Danimoth = 0x4E,
  Pulse_Cannon = 0x4F,
  STS_Photon_Cannon = 0x50,
  STA_Photon_Cannon = 0x51,
  Scarab = 0x52,
  Stasis_Field = 0x53,
  Psionic_Storm = 0x54,
  Warp_Blades_Zeratul = 0x55,
  Warp_Blades_Hero = 0x56,
  Platform_Laser_Battery = 0x5C,
  Independant_Laser_Battery = 0x5D,
  Twin_Autocannons_Floor_Trap = 0x60,
  Hellfire_Missile_Pack_Wall_Trap = 0x61,
  Flame_Thrower_Wall_Trap = 0x62,
  Hellfire_Missile_Pack_Floor_Trap = 0x63,
  Neutron_Flare = 0x64,
  Disruption_Web = 0x65,
  Restoration = 0x66,
  Halo_Rockets = 0x67,
  Corrosive_Acid = 0x68,
  Mind_Control = 0x69,
  Feedback = 0x6A,
  Optical_Flare = 0x6B,
  Maelstrom = 0x6C,
  Subterranean_Spines = 0x6D,
  Warp_Blades = 0x6F,
  C_10_Canister_Rifle_Samir_Duran = 0x70,
  C_10_Canister_Rifle_Infested_Duran = 0x71,
  Dual_Photon_Blasters_Artanis = 0x72,
  Anti_Matter_Missiles_Artanis = 0x73,
  C_10_Canister_Rifle_Alexei_Stukov = 0x74,
  WT_None = 0x82,
  WT_Unknown = 0x83,
  WT_MAX = 0x84,
};

struct CThingy
{
  CThingy *prev;
  CThingy *next;
  int hitPoints;
  CSprite *sprite;
};
static_assert(sizeof(CThingy) == 16, "Incorrect size for type `CThingy`. Expected: 16");

enum UnitPrototypeFlags
{
  Building = 0x1,
  Addon = 0x2,
  Flyer = 0x4,
  Worker = 0x8,
  Subunit = 0x10,
  FlyingBuilding = 0x20,
  Hero = 0x40,
  RegeneratesHP = 0x80,
  AnimatedIdle = 0x100,
  Cloakable = 0x200,
  TwoUnitsIn1Egg = 0x400,
  NeutralAccessories = 0x800,
  ResourceDepot = 0x1000,
  ResourceContainer = 0x2000,
  RoboticUnit = 0x4000,
  Detector = 0x8000,
  OrganicUnit = 0x10000,
  CreepBuilding = 0x20000,
  Unused = 0x40000,
  RequiresPsi = 0x80000,
  Burrowable = 0x100000,
  Spellcaster = 0x200000,
  PermanentCloak = 0x400000,
  NPCOrAccessories = 0x800000,
  MorphFromOtherUnit = 0x1000000,
  LargeUnit = 0x2000000,
  HugeUnit = 0x4000000,
  AutoAttackAndMove = 0x8000000,
  Attack = 0x10000000,
  Invincible_ = 0x20000000,
  Mechanical = 0x40000000,
  ProducesUnits = 0x80000000,
};

enum Tileset : unsigned __int16
{
  Badlands = 0x0,
  Platform = 0x1,
  Install = 0x2,
  Ashworld = 0x3,
  Jungle = 0x4,
  Desert = 0x5,
  Ice = 0x6,
  Twilight = 0x7,
};

struct TileID
{
  unsigned __int16 tile : 4;
  unsigned __int16 group : 12;
};
static_assert(sizeof(TileID) == 2, "Incorrect size for type `TileID`. Expected: 2");

enum MegatileFlags
{
  VISIBLE_PLAYER_1 = 0x1,
  VISIBLE_PLAYER_2 = 0x2,
  VISIBLE_PLAYER_3 = 0x4,
  VISIBLE_PLAYER_4 = 0x8,
  VISIBLE_PLAYER_5 = 0x10,
  VISIBLE_PLAYER_6 = 0x20,
  VISIBLE_PLAYER_7 = 0x40,
  VISIBLE_PLAYER_8 = 0x80,
  EXPLORED_PLAYER_1 = 0x100,
  EXPLORED_PLAYER_2 = 0x200,
  EXPLORED_PLAYER_3 = 0x400,
  EXPLORED_PLAYER_4 = 0x800,
  EXPLORED_PLAYER_5 = 0x1000,
  EXPLORED_PLAYER_6 = 0x2000,
  EXPLORED_PLAYER_7 = 0x4000,
  EXPLORED_PLAYER_8 = 0x8000,
  MORE_THAN_12_WALKABLE = 0x10000,
  LESS_THAN_13_WALKABLE = 0x40000,
  CAN_MISS = 0x100000,
  REAL_CREEP = 0x400000,
  ALWAYS_UNBUILDABLE = 0x800000,
  HAS_RAMP = 0x1000000,
  MORE_THAN_12_MEDIUM_HEIGHT = 0x2000000,
  MORE_THAN_12_HIGH_HEIGHT = 0x4000000,
  CURRENTLY_OCCUPIED = 0x8000000,
  CREEP_RECEEDING = 0x10000000,
  CLIFF_EDGE = 0x20000000,
  LOCAL_CREEP = 0x40000000,
};

typedef int (__fastcall *ActionPointer)(Action *);

struct SpriteTileData
{
  CSprite *tails[256];
  CSprite *heads[256];
};
static_assert(sizeof(SpriteTileData) == 2048, "Incorrect size for type `SpriteTileData`. Expected: 2048");

struct AI_Flags
{
  unsigned __int16 isSecureFinished : 1;
  unsigned __int16 isTownStarted : 1;
  unsigned __int16 isDefaultBuildOff : 1;
  unsigned __int16 isTransportsOff : 1;
  unsigned __int16 isFarmsNotimingOn : 1;
  unsigned __int16 isUseMapSettings : 1;
  unsigned __int16 flag_0x40 : 1;
  unsigned __int16 spreadCreep : 1;
  unsigned __int16 flag_0x100 : 1;
  unsigned __int16 hasStrongestGndArmy : 1;
  unsigned __int16 bUpgradesFinished : 1;
  unsigned __int16 bTargetExpansion : 1;
};
static_assert(sizeof(AI_Flags) == 2, "Incorrect size for type `AI_Flags`. Expected: 2");

struct ChkLoader
{
  int version;
  ChkSectionLoader *lobby_loaders;
  int lobby_loader_count;
  ChkSectionLoader *briefing_loaders;
  int briefing_loader_count;
  ChkSectionLoader *melee_loaders;
  int melee_loader_count;
  ChkSectionLoader *ums_loaders;
  int ums_loader_count;
  int requires_expansion;
};
static_assert(sizeof(ChkLoader) == 40, "Incorrect size for type `ChkLoader`. Expected: 40");

struct __declspec(align(2)) ChkSectionLoader
{
  const char name[4];
  bool (__stdcall *func)(SectionData *, int, MapChunks *);
  int flags;
};
static_assert(sizeof(ChkSectionLoader) == 12, "Incorrect size for type `ChkSectionLoader`. Expected: 12");

struct UnitStat
{
  int anonymous_0;
  bool (__cdecl *condition)();
  void (__fastcall *action)(dialog *);
};
static_assert(sizeof(UnitStat) == 12, "Incorrect size for type `UnitStat`. Expected: 12");

struct ButtonSet
{
  int button_count;
  ButtonOrder *button_order;
  __int16 f3;
  __int16 f4;
};
static_assert(sizeof(ButtonSet) == 12, "Incorrect size for type `ButtonSet`. Expected: 12");

enum PlayerGroups
{
  Player1 = 0x0,
  Player2 = 0x1,
  Player3 = 0x2,
  Player4 = 0x3,
  Player5 = 0x4,
  Player6 = 0x5,
  Player7 = 0x6,
  Player8 = 0x7,
  Player9 = 0x8,
  Player10 = 0x9,
  Player11 = 0xA,
  Player12 = 0xB,
  PG_None = 0xC,
  PG_CurrentPlayer = 0xD,
  PG_Foes = 0xE,
  PG_Allies = 0xF,
  PG_NeutralPlayers = 0x10,
  PG_AllPlayers = 0x11,
  PG_Force1 = 0x12,
  PG_Force2 = 0x13,
  PG_Force3 = 0x14,
  PG_Force4 = 0x15,
  PG_Unused1 = 0x16,
  PG_Unused2 = 0x17,
  PG_Unused3 = 0x18,
  PG_Unused4 = 0x19,
  PG_NonAlliedVictoryPlayers = 0x1A,
  PG_Max = 0x1B,
};

struct TriggerList
{
  int field_0;
  TriggerListEntry *end;
  TriggerListEntry *begin;
};
static_assert(sizeof(TriggerList) == 12, "Incorrect size for type `TriggerList`. Expected: 12");

enum FlingyID : unsigned __int8
{
  FGY_Scourge = 0x0,
  FGY_Broodling = 0x1,
  FGY_Infested_Terran = 0x2,
  FGY_Guardian_Cocoon = 0x3,
  FGY_Defiler = 0x4,
  FGY_Drone = 0x5,
  FGY_Egg = 0x6,
  FGY_Guardian = 0x7,
  FGY_Hydralisk = 0x8,
  FGY_Infested_Kerrigan = 0x9,
  FGY_Larva = 0xA,
  FGY_Mutalisk = 0xB,
  FGY_Overlord = 0xC,
  FGY_Queen = 0xD,
  FGY_Ultralisk = 0xE,
  FGY_Zergling = 0xF,
  FGY_Cerebrate = 0x10,
  FGY_Infested_Command_Center = 0x11,
  FGY_Spawning_Pool = 0x12,
  FGY_Mature_Chrysalis = 0x13,
  FGY_Evolution_Chamber = 0x14,
  FGY_Creep_Colony = 0x15,
  FGY_Hatchery = 0x16,
  FGY_Hive = 0x17,
  FGY_Lair = 0x18,
  FGY_Sunken_Colony = 0x19,
  FGY_Greater_Spire = 0x1A,
  FGY_Defiler_Mound = 0x1B,
  FGY_Queens_Nest = 0x1C,
  FGY_Nydus_Canal = 0x1D,
  FGY_Overmind_With_Shell = 0x1E,
  FGY_Overmind_Without_Shell = 0x1F,
  FGY_Ultralisk_Cavern = 0x20,
  FGY_Extractor = 0x21,
  FGY_Hydralisk_Den = 0x22,
  FGY_Spire = 0x23,
  FGY_Spore_Colony = 0x24,
  FGY_Arbiter = 0x25,
  FGY_Archon_Energy = 0x26,
  FGY_Carrier = 0x27,
  FGY_Dragoon = 0x28,
  FGY_Interceptor = 0x29,
  FGY_Probe = 0x2A,
  FGY_Scout = 0x2B,
  FGY_Shuttle = 0x2C,
  FGY_High_Templar = 0x2D,
  FGY_Dark_Templar_Hero = 0x2E,
  FGY_Reaver = 0x2F,
  FGY_Scarab = 0x30,
  FGY_Zealot = 0x31,
  FGY_Observer = 0x32,
  FGY_Templar_Archives = 0x33,
  FGY_Assimilator = 0x34,
  FGY_Observatory = 0x35,
  FGY_Citadel_of_Adun = 0x36,
  FGY_Forge = 0x37,
  FGY_Gateway = 0x38,
  FGY_Cybernetics_Core = 0x39,
  FGY_Khaydarin_Crystal_Formation = 0x3A,
  FGY_Nexus = 0x3B,
  FGY_Photon_Cannon = 0x3C,
  FGY_Arbiter_Tribunal = 0x3D,
  FGY_Pylon = 0x3E,
  FGY_Robotics_Facility = 0x3F,
  FGY_Shield_Battery = 0x40,
  FGY_Stargate = 0x41,
  FGY_Stasis_Cell_Prison = 0x42,
  FGY_Robotics_Support_Bay = 0x43,
  FGY_Protoss_Temple = 0x44,
  FGY_Fleet_Beacon = 0x45,
  FGY_Battlecruiser = 0x46,
  FGY_Civilian = 0x47,
  FGY_Dropship = 0x48,
  FGY_Firebat = 0x49,
  FGY_Ghost = 0x4A,
  FGY_Goliath_Base = 0x4B,
  FGY_Goliath_Turret = 0x4C,
  FGY_Sarah_Kerrigan = 0x4D,
  FGY_Marine = 0x4E,
  FGY_Scanner_Sweep = 0x4F,
  FGY_Wraith = 0x50,
  FGY_SCV = 0x51,
  FGY_Siege_Tank_Tank_Base = 0x52,
  FGY_Siege_Tank_Tank_Turret = 0x53,
  FGY_Siege_Tank_Siege_Base = 0x54,
  FGY_Siege_Tank_Siege_Turret = 0x55,
  FGY_Science_Vessel_Base = 0x56,
  FGY_Science_Vessel_Turret = 0x57,
  FGY_Vulture = 0x58,
  FGY_Spider_Mine = 0x59,
  FGY_Terran_Academy = 0x5A,
  FGY_Barracks = 0x5B,
  FGY_Armory = 0x5C,
  FGY_Comsat_Station = 0x5D,
  FGY_Command_Center = 0x5E,
  FGY_Supply_Depot = 0x5F,
  FGY_Control_Tower = 0x60,
  FGY_Factory = 0x61,
  FGY_Covert_Ops = 0x62,
  FGY_Ion_Cannon = 0x63,
  FGY_Machine_Shop = 0x64,
  FGY_Missile_Turret_Base = 0x65,
  FGY_Crashed_Battlecruiser = 0x66,
  FGY_Physics_Lab = 0x67,
  FGY_Bunker = 0x68,
  FGY_Refinery = 0x69,
  FGY_Immobile_Barracks = 0x6A,
  FGY_Science_Facility = 0x6B,
  FGY_Nuke_Silo = 0x6C,
  FGY_Nuclear_Missile = 0x6D,
  FGY_Starport = 0x6E,
  FGY_Engineering_Bay = 0x6F,
  FGY_Terran_Construction_Large = 0x70,
  FGY_Terran_Construction_Small = 0x71,
  FGY_Ragnasaur_Ashworld = 0x72,
  FGY_Rhynadon_Badlands = 0x73,
  FGY_Bengalaas_Jungle = 0x74,
  FGY_Vespene_Geyser = 0x75,
  FGY_Mineral_Field_Type1 = 0x76,
  FGY_Mineral_Field_Type2 = 0x77,
  FGY_Mineral_Field_Type3 = 0x78,
  FGY_Independent_Starport_Unused = 0x79,
  FGY_Zerg_Beacon = 0x7A,
  FGY_Terran_Beacon = 0x7B,
  FGY_Protoss_Beacon = 0x7C,
  FGY_Dark_Swarm = 0x7D,
  FGY_Flag = 0x7E,
  FGY_Young_Chrysalis = 0x7F,
  FGY_Psi_Emitter = 0x80,
  FGY_Data_Disc = 0x81,
  FGY_Khaydarin_Crystal = 0x82,
  FGY_Mineral_Chunk_Type1 = 0x83,
  FGY_Mineral_Chunk_Type2 = 0x84,
  FGY_Protoss_Gas_Orb_Type1 = 0x85,
  FGY_Protoss_Gas_Orb_Type2 = 0x86,
  FGY_Zerg_Gas_Sac_Type1 = 0x87,
  FGY_Zerg_Gas_Sac_Type2 = 0x88,
  FGY_Terran_Gas_Tank_Type1 = 0x89,
  FGY_Terran_Gas_Tank_Type2 = 0x8A,
  FGY_Map_Revealer = 0x8B,
  FGY_Start_Location = 0x8C,
  FGY_Fusion_Cutter_Hit = 0x8D,
  FGY_Gauss_Rifle_Hit = 0x8E,
  FGY_C_10_Canister_Rifle_Hit = 0x8F,
  FGY_Gemini_Missiles = 0x90,
  FGY_Fragmentation_Grenade = 0x91,
  FGY_Hellfire_Missile = 0x92,
  FGY_Unknown147 = 0x93,
  FGY_ATS_ATA_Laser_Battery = 0x94,
  FGY_Burst_Lasers = 0x95,
  FGY_Arclite_Shock_Cannon_Hit = 0x96,
  FGY_EMP_Missile = 0x97,
  FGY_Dual_Photon_Blasters_Hit = 0x98,
  FGY_Particle_Beam_Hit = 0x99,
  FGY_Anti_Matter_Missile = 0x9A,
  FGY_Pulse_Cannon = 0x9B,
  FGY_Psionic_Shockwave_Hit = 0x9C,
  FGY_Psionic_Storm = 0x9D,
  FGY_Yamato_Gun = 0x9E,
  FGY_Phase_Disruptor = 0x9F,
  FGY_STA_STS_Cannon_Overlay = 0xA0,
  FGY_Sunken_Colony_Tentacle = 0xA1,
  FGY_Venom_Unused_Zerg_Weapon = 0xA2,
  FGY_Acid_Spore = 0xA3,
  FGY_Unknown164 = 0xA4,
  FGY_Glave_Wurm = 0xA5,
  FGY_Seeker_Spores = 0xA6,
  FGY_Queen_Spell_Carrier = 0xA7,
  FGY_Plague_Cloud = 0xA8,
  FGY_Consume = 0xA9,
  FGY_Ensnare = 0xAA,
  FGY_Needle_Spine_Hit = 0xAB,
  FGY_White_Circle_Invisible = 0xAC,
  FGY_Left_Upper_Level_Door = 0xAD,
  FGY_Right_Upper_Level_Door = 0xAE,
  FGY_Substructure_Left_Door = 0xAF,
  FGY_Substructure_Right_Door = 0xB0,
  FGY_Substructure_Opening_Hole = 0xB1,
  FGY_Floor_Gun_Trap = 0xB2,
  FGY_Floor_Missile_Trap = 0xB3,
  FGY_Wall_Missile_Trap = 0xB4,
  FGY_Wall_Missile_Trap2 = 0xB5,
  FGY_Wall_Flame_Trap = 0xB6,
  FGY_Wall_Flame_Trap2 = 0xB7,
  FGY_Lurker_Egg = 0xB8,
  FGY_Devourer = 0xB9,
  FGY_Lurker = 0xBA,
  FGY_Dark_Archon_Energy = 0xBB,
  FGY_Dark_Templar_Unit = 0xBC,
  FGY_Medic = 0xBD,
  FGY_Valkyrie = 0xBE,
  FGY_Corsair = 0xBF,
  FGY_Disruption_Web = 0xC0,
  FGY_Overmind_Cocoon = 0xC1,
  FGY_Psi_Disrupter = 0xC2,
  FGY_Warp_Gate = 0xC3,
  FGY_Power_Generator = 0xC4,
  FGY_XelNaga_Temple = 0xC5,
  FGY_Scantid_Desert = 0xC6,
  FGY_Kakaru_Twilight = 0xC7,
  FGY_Ursadon_Ice = 0xC8,
  FGY_Optical_Flare_Grenade = 0xC9,
  FGY_Halo_Rockets = 0xCA,
  FGY_Subterranean_Spines = 0xCB,
  FGY_Corrosive_Acid_Shot = 0xCC,
  FGY_Corrosive_Acid_Hit = 0xCD,
  FGY_Neutron_Flare = 0xCE,
  FGY_Uraj = 0xCF,
  FGY_Khalis = 0xD0,
  FGY_None = 0xD1,
};

enum DamageType : __int8
{
  Independent = 0x0,
  Explosive = 0x1,
  Concussive = 0x2,
  Normal = 0x3,
  Ignore_Armor = 0x4,
  DT_None = 0x5,
  DT_Unknown = 0x6,
  DT_MAX = 0x7,
};

enum WeaponBehavior : __int8
{
  WB_Fly_DoNotFollowTarget = 0x0,
  WB_Fly_FollowTarget = 0x1,
  WB_AppearOnTargetUnit = 0x2,
  WB_PersistOnTargetSite = 0x3,
  WB_AppearOnTargetSite = 0x4,
  WB_AppearOnAttacker = 0x5,
  WB_AttackAndSelfDestruct = 0x6,
  WB_Bounce = 0x7,
  WB_AttackNearbyArea = 0x8,
  WB_GoToMaxRange = 0x9,
};

enum MapData : __int16
{
  MD_none = 0x0,
  MD_tutorial = 0x1,
  MD_terran01 = 0x2,
  MD_terran02 = 0x3,
  MD_terran03 = 0x4,
  MD_terran04 = 0x5,
  MD_terran05 = 0x6,
  MD_terran06 = 0x7,
  MD_terran08 = 0x8,
  MD_terran09 = 0x9,
  MD_terran11 = 0xA,
  MD_terran12 = 0xB,
  MD_zerg01 = 0xC,
  MD_zerg02 = 0xD,
  MD_zerg03 = 0xE,
  MD_zerg04 = 0xF,
  MD_zerg05 = 0x10,
  MD_zerg06 = 0x11,
  MD_zerg07 = 0x12,
  MD_zerg08 = 0x13,
  MD_zerg09 = 0x14,
  MD_zerg10 = 0x15,
  MD_protoss01 = 0x16,
  MD_protoss02 = 0x17,
  MD_protoss03 = 0x18,
  MD_protoss04 = 0x19,
  MD_protoss05 = 0x1A,
  MD_protoss06 = 0x1B,
  MD_protoss07 = 0x1C,
  MD_protoss08 = 0x1D,
  MD_protoss09 = 0x1E,
  MD_protoss10 = 0x1F,
  MD_xprotoss01 = 0x20,
  MD_xprotoss02 = 0x21,
  MD_xprotoss03 = 0x22,
  MD_xprotoss04 = 0x23,
  MD_xprotoss05 = 0x24,
  MD_xprotoss06 = 0x25,
  MD_xprotoss07 = 0x26,
  MD_xprotoss08 = 0x27,
  MD_xterran01 = 0x28,
  MD_xterran02 = 0x29,
  MD_xterran03 = 0x2A,
  MD_xterran04 = 0x2B,
  MD_xterran05a = 0x2C,
  MD_xterran05b = 0x2D,
  MD_xterran06 = 0x2E,
  MD_xterran07 = 0x2F,
  MD_xterran08 = 0x30,
  MD_xzerg01 = 0x31,
  MD_xzerg02 = 0x32,
  MD_xzerg03 = 0x33,
  MD_xzerg04a = 0x34,
  MD_xzerg04b = 0x35,
  MD_xzerg04c = 0x36,
  MD_xzerg04d = 0x37,
  MD_xzerg04e = 0x38,
  MD_xzerg04f = 0x39,
  MD_xzerg05 = 0x3A,
  MD_xzerg06 = 0x3B,
  MD_xzerg07 = 0x3C,
  MD_xzerg08 = 0x3D,
  MD_xzerg09 = 0x3E,
  MD_xbonus = 0x3F,
  MD_xzerg10 = 0x40,
  MD_Unknown = 0x41,
};

#pragma pack(push, 1)
struct activation_delays
{
  int anonymous_0;
  dialog *dlg;
};
#pragma pack(pop)
static_assert(sizeof(activation_delays) == 8, "Incorrect size for type `activation_delays`. Expected: 8");

enum CheatFlags : unsigned __int32
{
  CHEAT_None = 0x0,
  CHEAT_BlackSheepWall = 0x1,
  CHEAT_OperationCwal = 0x2,
  CHEAT_PowerOverwelming = 0x4,
  CHEAT_SomethingForNothing = 0x8,
  CHEAT_ShowMeTheMoney = 0x10,
  CHEAT_GameOverMan = 0x40,
  CHEAT_ThereIsNoCowLevel = 0x80,
  CHEAT_StayingAlive = 0x100,
  CHEAT_Ophelia = 0x200,
  CHEAT_TheGathering = 0x800,
  CHEAT_MedievalMan = 0x1000,
  CHEAT_ModifyThePhaseVariance = 0x2000,
  CHEAT_WarAintWhatItUsedToBe = 0x4000,
  CHEAT_FoodForThought = 0x20000,
  CHEAT_WhatsMineIsMine = 0x40000,
  CHEAT_BreatheDeep = 0x80000,
  CHEAT_NoGlues = 0x20000000,
};

#pragma pack(push, 1)
struct CndSignature
{
  int field_0;
  char filename[20];
};
#pragma pack(pop)
static_assert(sizeof(CndSignature) == 24, "Incorrect size for type `CndSignature`. Expected: 24");

struct ImagesDatExtraOverlayLO_Files
{
  LO_Header *overlays[5][999];
};
static_assert(sizeof(ImagesDatExtraOverlayLO_Files) == 19980, "Incorrect size for type `ImagesDatExtraOverlayLO_Files`. Expected: 19980");

struct LO_Overlays
{
  LO_Header *overlays[999];
};
static_assert(sizeof(LO_Overlays) == 3996, "Incorrect size for type `LO_Overlays`. Expected: 3996");

enum WeaponTargetFlags : __int16
{
  WTF_Air = 0x1,
  WTF_Ground = 0x2,
  WTF_Mechanical = 0x4,
  WTF_Organic = 0x8,
  WTF_NonBuilding = 0x10,
  WTF_NonRobotic = 0x20,
  WTF_Terrain = 0x40,
  WTF_OrgOrMech = 0x80,
  WTF_Own = 0x100,
};

struct UnitDimentions
{
  __int16 a;
  __int16 b;
  __int16 c;
  __int16 d;
};
static_assert(sizeof(UnitDimentions) == 8, "Incorrect size for type `UnitDimentions`. Expected: 8");

enum MapData4 : __int32
{
  MD4_none = 0x0,
  MD4_tutorial = 0x1,
  MD4_terran01 = 0x2,
  MD4_terran02 = 0x3,
  MD4_terran03 = 0x4,
  MD4_terran04 = 0x5,
  MD4_terran05 = 0x6,
  MD4_terran06 = 0x7,
  MD4_terran08 = 0x8,
  MD4_terran09 = 0x9,
  MD4_terran11 = 0xA,
  MD4_terran12 = 0xB,
  MD4_zerg01 = 0xC,
  MD4_zerg02 = 0xD,
  MD4_zerg03 = 0xE,
  MD4_zerg04 = 0xF,
  MD4_zerg05 = 0x10,
  MD4_zerg06 = 0x11,
  MD4_zerg07 = 0x12,
  MD4_zerg08 = 0x13,
  MD4_zerg09 = 0x14,
  MD4_zerg10 = 0x15,
  MD4_protoss01 = 0x16,
  MD4_protoss02 = 0x17,
  MD4_protoss03 = 0x18,
  MD4_protoss04 = 0x19,
  MD4_protoss05 = 0x1A,
  MD4_protoss06 = 0x1B,
  MD4_protoss07 = 0x1C,
  MD4_protoss08 = 0x1D,
  MD4_protoss09 = 0x1E,
  MD4_protoss10 = 0x1F,
  MD4_xprotoss01 = 0x20,
  MD4_xprotoss02 = 0x21,
  MD4_xprotoss03 = 0x22,
  MD4_xprotoss04 = 0x23,
  MD4_xprotoss05 = 0x24,
  MD4_xprotoss06 = 0x25,
  MD4_xprotoss07 = 0x26,
  MD4_xprotoss08 = 0x27,
  MD4_xterran01 = 0x28,
  MD4_xterran02 = 0x29,
  MD4_xterran03 = 0x2A,
  MD4_xterran04 = 0x2B,
  MD4_xterran05a = 0x2C,
  MD4_xterran05b = 0x2D,
  MD4_xterran06 = 0x2E,
  MD4_xterran07 = 0x2F,
  MD4_xterran08 = 0x30,
  MD4_xzerg01 = 0x31,
  MD4_xzerg02 = 0x32,
  MD4_xzerg03 = 0x33,
  MD4_xzerg04a = 0x34,
  MD4_xzerg04b = 0x35,
  MD4_xzerg04c = 0x36,
  MD4_xzerg04d = 0x37,
  MD4_xzerg04e = 0x38,
  MD4_xzerg04f = 0x39,
  MD4_xzerg05 = 0x3A,
  MD4_xzerg06 = 0x3B,
  MD4_xzerg07 = 0x3C,
  MD4_xzerg08 = 0x3D,
  MD4_xzerg09 = 0x3E,
  MD4_xbonus = 0x3F,
  MD4_xzerg10 = 0x40,
  MD4_Unknown = 0x41,
};

enum Tech2 : __int16
{
  TECH2_stim_packs = 0x0,
  TECH2_lockdown = 0x1,
  TECH2_emp_shockwave = 0x2,
  TECH2_spider_mines = 0x3,
  TECH2_scanner_sweep = 0x4,
  TECH2_tank_siege_mode = 0x5,
  TECH2_defensive_matrix = 0x6,
  TECH2_irradiate = 0x7,
  TECH2_yamato_gun = 0x8,
  TECH2_cloaking_field = 0x9,
  TECH2_personnel_cloaking = 0xA,
  TECH2_burrowing = 0xB,
  TECH2_infestation = 0xC,
  TECH2_spawn_broodlings = 0xD,
  TECH2_dark_swarm = 0xE,
  TECH2_plague = 0xF,
  TECH2_consume = 0x10,
  TECH2_ensnare = 0x11,
  TECH2_parasite = 0x12,
  TECH2_psionic_storm = 0x13,
  TECH2_hallucination = 0x14,
  TECH2_recall = 0x15,
  TECH2_stasis_field = 0x16,
  TECH2_archon_warp = 0x17,
  TECH2_restoration = 0x18,
  TECH2_disruption_web = 0x19,
  TECH2_unknown_tech26 = 0x1A,
  TECH2_mind_control = 0x1B,
  TECH2_dark_archon_meld = 0x1C,
  TECH2_feedback = 0x1D,
  TECH2_optical_flare = 0x1E,
  TECH2_maelstorm = 0x1F,
  TECH2_lurker_aspect = 0x20,
  TECH2_unknown_tech33 = 0x21,
  TECH2_healing = 0x22,
  TECH2_unknown_tech35 = 0x23,
  TECH2_unknown_tech36 = 0x24,
  TECH2_unknown_tech37 = 0x25,
  TECH2_unknown_tech38 = 0x26,
  TECH2_unknown_tech39 = 0x27,
  TECH2_unknown_tech40 = 0x28,
  TECH2_unknown_tech41 = 0x29,
  TECH2_unknown_tech42 = 0x2A,
  TECH2_unknown_tech43 = 0x2B,
  TECH2_none = 0x2C,
};

typedef void (__fastcall *AppExitHandle)(bool exit_code);

enum SfxData
{
  SFX_Unknown = 0x0,
  SFX_Zerg_Drone_ZDrErr00 = 0x1,
  SFX_Misc_Buzz_1 = 0x2,
  SFX_Misc_PError_1 = 0x3,
  SFX_Misc_ZBldgPlc_1 = 0x4,
  SFX_Misc_TBldgPlc = 0x5,
  SFX_Misc_PBldgPlc = 0x6,
  SFX_Misc_ExploLrg = 0x7,
  SFX_Misc_ExploMed = 0x8,
  SFX_Misc_ExploSm = 0x9,
  SFX_Misc_Explo1 = 0xA,
  SFX_Misc_Explo2 = 0xB,
  SFX_Misc_Explo3 = 0xC,
  SFX_Misc_Explo4 = 0xD,
  SFX_Misc_Explo5 = 0xE,
  SFX_Misc_Button_1 = 0xF,
  SFX_Misc_BurrowDn = 0x10,
  SFX_Misc_BurrowUp = 0x11,
  SFX_Misc_OnFirSml = 0x12,
  SFX_Misc_IntoNydus = 0x13,
  SFX_Misc_OutOfGas = 0x14,
  SFX_Misc_youwin = 0x15,
  SFX_Misc_youlose = 0x16,
  SFX_Misc_Transmission = 0x17,
  SFX_Misc_Door_Door1Opn = 0x18,
  SFX_Misc_Door_Door1Cls = 0x19,
  SFX_Misc_Door_Door2Opn = 0x1A,
  SFX_Misc_Door_Door2Cls = 0x1B,
  SFX_Misc_Door_Door3Opn = 0x1C,
  SFX_Misc_Door_Door3Cls = 0x1D,
  SFX_Misc_Door_Door4Opn = 0x1E,
  SFX_Misc_Door_Door4Cls = 0x1F,
  SFX_Misc_ZRescue = 0x20,
  SFX_Misc_TRescue = 0x21,
  SFX_Misc_PRescue = 0x22,
  SFX_terran_SCV_EDrRep00 = 0x23,
  SFX_terran_SCV_EDrRep01 = 0x24,
  SFX_terran_SCV_EDrRep02 = 0x25,
  SFX_terran_SCV_EDrRep03 = 0x26,
  SFX_terran_SCV_EDrRep04 = 0x27,
  SFX_Misc_ZOvTra00 = 0x28,
  SFX_Misc_TDrTra00 = 0x29,
  SFX_Misc_PShTra00 = 0x2A,
  SFX_Misc_ZOvTra01 = 0x2B,
  SFX_Misc_TDrTra01 = 0x2C,
  SFX_Misc_PShTra01 = 0x2D,
  SFX_Misc_CRITTERS_JCrWht00 = 0x2E,
  SFX_Misc_CRITTERS_JCrWht01 = 0x2F,
  SFX_Misc_CRITTERS_JCrWht02 = 0x30,
  SFX_Misc_CRITTERS_JCrDth00 = 0x31,
  SFX_Misc_CRITTERS_LCrWht00 = 0x32,
  SFX_Misc_CRITTERS_LCrWht01 = 0x33,
  SFX_Misc_CRITTERS_LCrWht02 = 0x34,
  SFX_Misc_CRITTERS_LCrDth00 = 0x35,
  SFX_Misc_CRITTERS_BCrWht00 = 0x36,
  SFX_Misc_CRITTERS_BCrWht01 = 0x37,
  SFX_Misc_CRITTERS_BCrWht02 = 0x38,
  SFX_Misc_CRITTERS_BCrDth00 = 0x39,
  SFX_Bullet_PArFir00 = 0x3A,
  SFX_Bullet_ZHyFir00 = 0x3B,
  SFX_Bullet_ZHyHit00 = 0x3C,
  SFX_Bullet_ZDrHit00 = 0x3D,
  SFX_Bullet_LaserHit = 0x3E,
  SFX_Bullet_pshield = 0x3F,
  SFX_Bullet_SpoogHit = 0x40,
  SFX_Bullet_BlastCan = 0x41,
  SFX_Bullet_BlastGn2 = 0x42,
  SFX_Bullet_ZBGHit00 = 0x43,
  SFX_Bullet_TTaFir00 = 0x44,
  SFX_Bullet_TMaFir00 = 0x45,
  SFX_Bullet_TGoFir00 = 0x46,
  SFX_Bullet_ZGuFir00 = 0x47,
  SFX_Bullet_ZGuHit00 = 0x48,
  SFX_Bullet_LASERB = 0x49,
  SFX_Bullet_TPhFi100 = 0x4A,
  SFX_Bullet_LASRHIT1 = 0x4B,
  SFX_Bullet_LASRHIT2 = 0x4C,
  SFX_Bullet_LASRHIT3 = 0x4D,
  SFX_Bullet_ZLuFir00 = 0x4E,
  SFX_Bullet_ZLuHit00 = 0x4F,
  SFX_Bullet_HKMISSLE = 0x50,
  SFX_Bullet_TGoFi200 = 0x51,
  SFX_Bullet_TPhFi200 = 0x52,
  SFX_Bullet_TNsFir00_1 = 0x53,
  SFX_Bullet_TNsFir00_2 = 0x54,
  SFX_Bullet_TNsHit00 = 0x55,
  SFX_Bullet_PhoAtt00 = 0x56,
  SFX_Bullet_PhoHit00 = 0x57,
  SFX_Bullet_PSIBLADE = 0x58,
  SFX_Bullet_PSIBOLT = 0x59,
  SFX_Bullet_ZQuFir00 = 0x5A,
  SFX_Bullet_ZQuHit00 = 0x5B,
  SFX_Bullet_ZQuHit01 = 0x5C,
  SFX_Bullet_ZQuHit02 = 0x5D,
  SFX_Bullet_tscFir00 = 0x5E,
  SFX_Bullet_SHOCKBMB = 0x5F,
  SFX_Bullet_TTaFi200 = 0x60,
  SFX_Bullet_SHCKLNCH = 0x61,
  SFX_Bullet_TGhFir00 = 0x62,
  SFX_Bullet_DragBull = 0x63,
  SFX_Bullet_psaHit00 = 0x64,
  SFX_Bullet_LaserB = 0x65,
  SFX_Bullet_pTrFir00 = 0x66,
  SFX_Bullet_pTrFir01 = 0x67,
  SFX_Bullet_pzeFir00 = 0x68,
  SFX_Bullet_tbaFir00 = 0x69,
  SFX_Bullet_tvuFir00 = 0x6A,
  SFX_Bullet_tvuHit00 = 0x6B,
  SFX_Bullet_tvuHit01 = 0x6C,
  SFX_Bullet_tvuHit02 = 0x6D,
  SFX_Bullet_zdeAtt00 = 0x6E,
  SFX_Bullet_UZeFir00 = 0x6F,
  SFX_Bullet_UKiFir00 = 0x70,
  SFX_Bullet_zmuFir00 = 0x71,
  SFX_Misc_ZPwrDown = 0x72,
  SFX_Misc_TPwrDown = 0x73,
  SFX_Misc_PPwrDown = 0x74,
  SFX_Zerg_Advisor_ZAdUpd00 = 0x75,
  SFX_Terran_Advisor_TAdUpd00 = 0x76,
  SFX_Protoss_Advisor_PAdUpd00 = 0x77,
  SFX_Zerg_Advisor_ZAdUpd01 = 0x78,
  SFX_Terran_Advisor_TAdUpd01 = 0x79,
  SFX_Protoss_Advisor_PAdUpd01 = 0x7A,
  SFX_Zerg_Advisor_ZAdUpd02 = 0x7B,
  SFX_Terran_Advisor_TAdUpd02_1 = 0x7C,
  SFX_Protoss_Advisor_PAdUpd02 = 0x7D,
  SFX_Terran_Advisor_TAdUpd03 = 0x7E,
  SFX_Zerg_Advisor_ZAdUpd04 = 0x7F,
  SFX_Terran_Advisor_TAdUpd04 = 0x80,
  SFX_Protoss_Advisor_PAdUpd04 = 0x81,
  SFX_Terran_Advisor_TAdUpd05 = 0x82,
  SFX_Terran_Advisor_TAdUpd06 = 0x83,
  SFX_Terran_Advisor_TAdUpd02_2 = 0x84,
  SFX_Protoss_Advisor_PAdUpd06 = 0x85,
  SFX_Terran_Advisor_TAdUpd07 = 0x86,
  SFX_Zerg_Bldg_ZChRdy00 = 0x87,
  SFX_Terran_SCV_TSCUpd00 = 0x88,
  SFX_Zerg_DRONE_ZDrErr00_1 = 0x89,
  SFX_Zerg_DRONE_ZDrErr00_2 = 0x8A,
  SFX_Misc_Buzz_2 = 0x8B,
  SFX_Misc_PError_2 = 0x8C,
  SFX_Misc_ZBldgPlc_2 = 0x8D,
  SFX_Terran_Advisor_tAdErr04_1 = 0x8E,
  SFX_Terran_Advisor_tAdErr03_1 = 0x8F,
  SFX_Terran_Advisor_tAdErr03_2 = 0x90,
  SFX_Terran_Advisor_tAdErr04_2 = 0x91,
  SFX_Zerg_Advisor_ZAdErr00_1 = 0x92,
  SFX_Zerg_Advisor_ZAdErr00_2 = 0x93,
  SFX_Terran_Advisor_tAdErr00 = 0x94,
  SFX_Protoss_Advisor_PAdErr00 = 0x95,
  SFX_Zerg_Advisor_ZAdErr01 = 0x96,
  SFX_Terran_Advisor_tAdErr01 = 0x97,
  SFX_Protoss_Advisor_PAdErr01 = 0x98,
  SFX_Zerg_Advisor_ZAdErr02 = 0x99,
  SFX_Terran_Advisor_tAdErr02 = 0x9A,
  SFX_Protoss_Advisor_PAdErr02 = 0x9B,
  SFX_Zerg_Advisor_ZAdErr06 = 0x9C,
  SFX_Terran_Advisor_tAdErr06 = 0x9D,
  SFX_Protoss_Advisor_PAdErr06 = 0x9E,
  SFX_Terran_SCV_TSCErr01 = 0x9F,
  SFX_Terran_Advisor_tAdErr04_3 = 0xA0,
  SFX_Protoss_PROBE_PPrErr00 = 0xA1,
  SFX_Terran_SCV_TSCErr00 = 0xA2,
  SFX_Terran_Advisor_tAdErr04_4 = 0xA3,
  SFX_Protoss_PROBE_PPrErr01 = 0xA4,
  SFX_glue_mousedown2 = 0xA5,
  SFX_glue_mouseover = 0xA6,
  SFX_glue_bnetclick = 0xA7,
  SFX_glue_scorefill = 0xA8,
  SFX_glue_scorefillend = 0xA9,
  SFX_glue_countdown = 0xAA,
  SFX_glue_swishlock = 0xAB,
  SFX_glue_swishin = 0xAC,
  SFX_glue_swishout = 0xAD,
  SFX_Terran_SCV_TSCMin00 = 0xAE,
  SFX_Terran_SCV_TSCMin01 = 0xAF,
  SFX_Terran_BATTLE_tbardy00 = 0xB0,
  SFX_Terran_BATTLE_tbaDth00 = 0xB1,
  SFX_Terran_BATTLE_tBaYam01 = 0xB2,
  SFX_Terran_BATTLE_tBaYam02 = 0xB3,
  SFX_Terran_BATTLE_tbapss00 = 0xB4,
  SFX_Terran_BATTLE_tbapss01 = 0xB5,
  SFX_Terran_BATTLE_tbapss02 = 0xB6,
  SFX_Terran_BATTLE_tbapss03 = 0xB7,
  SFX_Terran_BATTLE_tbapss04 = 0xB8,
  SFX_Terran_BATTLE_tbawht00 = 0xB9,
  SFX_Terran_BATTLE_tbawht01 = 0xBA,
  SFX_Terran_BATTLE_tbawht02 = 0xBB,
  SFX_Terran_BATTLE_tbawht03 = 0xBC,
  SFX_Terran_BATTLE_tbayes00 = 0xBD,
  SFX_Terran_BATTLE_tbayes01 = 0xBE,
  SFX_Terran_BATTLE_tbayes02 = 0xBF,
  SFX_Terran_BATTLE_tbayes03 = 0xC0,
  SFX_Terran_CIVILIAN_TCvPss00 = 0xC1,
  SFX_Terran_CIVILIAN_TCvPss01 = 0xC2,
  SFX_Terran_CIVILIAN_TCvPss02 = 0xC3,
  SFX_Terran_CIVILIAN_TCvPss03 = 0xC4,
  SFX_Terran_CIVILIAN_TCvPss04 = 0xC5,
  SFX_Terran_CIVILIAN_TCvWht00 = 0xC6,
  SFX_Terran_CIVILIAN_TCvWht01 = 0xC7,
  SFX_Terran_CIVILIAN_TCvWht02 = 0xC8,
  SFX_Terran_CIVILIAN_TCvWht03 = 0xC9,
  SFX_Terran_CIVILIAN_TCvYes00 = 0xCA,
  SFX_Terran_CIVILIAN_TCvYes01 = 0xCB,
  SFX_Terran_CIVILIAN_TCvYes02 = 0xCC,
  SFX_Terran_CIVILIAN_TCvYes03 = 0xCD,
  SFX_Terran_CIVILIAN_TCvYes04 = 0xCE,
  SFX_Terran_CIVILIAN_TCvDth00 = 0xCF,
  SFX_Terran_CIVILIAN_TCvRdy00 = 0xD0,
  SFX_Terran_DROPSHIP_TDrRdy00 = 0xD1,
  SFX_Terran_DROPSHIP_TDrDth00 = 0xD2,
  SFX_Terran_DROPSHIP_TDrPss00 = 0xD3,
  SFX_Terran_DROPSHIP_TDrPss01 = 0xD4,
  SFX_Terran_DROPSHIP_TDrPss02 = 0xD5,
  SFX_Terran_DROPSHIP_TDrPss03 = 0xD6,
  SFX_Terran_DROPSHIP_TDrWht00 = 0xD7,
  SFX_Terran_DROPSHIP_TDrWht01 = 0xD8,
  SFX_Terran_DROPSHIP_TDrWht02 = 0xD9,
  SFX_Terran_DROPSHIP_TDrWht03 = 0xDA,
  SFX_Terran_DROPSHIP_TDrYes00 = 0xDB,
  SFX_Terran_DROPSHIP_TDrYes01 = 0xDC,
  SFX_Terran_DROPSHIP_TDrYes02 = 0xDD,
  SFX_Terran_DROPSHIP_TDrYes03 = 0xDE,
  SFX_Terran_DROPSHIP_TDrYes04 = 0xDF,
  SFX_Terran_DROPSHIP_TDrYes05 = 0xE0,
  SFX_Terran_GHOST_TGhRdy00 = 0xE1,
  SFX_Terran_GHOST_TGhPss00 = 0xE2,
  SFX_Terran_GHOST_TGhPss01 = 0xE3,
  SFX_Terran_GHOST_TGhPss02 = 0xE4,
  SFX_Terran_GHOST_TGhPss03 = 0xE5,
  SFX_Terran_GHOST_TGhWht00 = 0xE6,
  SFX_Terran_GHOST_TGhWht01 = 0xE7,
  SFX_Terran_GHOST_TGhWht02 = 0xE8,
  SFX_Terran_GHOST_TGhWht03 = 0xE9,
  SFX_Terran_GHOST_TGhYes00 = 0xEA,
  SFX_Terran_GHOST_TGhYes01 = 0xEB,
  SFX_Terran_GHOST_TGhYes02 = 0xEC,
  SFX_Terran_GHOST_TGhYes03 = 0xED,
  SFX_Terran_GHOST_TGhDth00 = 0xEE,
  SFX_Terran_GHOST_TGhLas00 = 0xEF,
  SFX_Terran_GHOST_TGhLkd00 = 0xF0,
  SFX_Terran_GOLIATH_TGoRdy00 = 0xF1,
  SFX_Terran_GOLIATH_TGoPss00 = 0xF2,
  SFX_Terran_GOLIATH_TGoPss01 = 0xF3,
  SFX_Terran_GOLIATH_TGoPss02 = 0xF4,
  SFX_Terran_GOLIATH_TGoPss03 = 0xF5,
  SFX_Terran_GOLIATH_TGoPss04 = 0xF6,
  SFX_Terran_GOLIATH_TGoPss05 = 0xF7,
  SFX_Terran_GOLIATH_TGoWht00 = 0xF8,
  SFX_Terran_GOLIATH_TGoWht01 = 0xF9,
  SFX_Terran_GOLIATH_TGoWht02 = 0xFA,
  SFX_Terran_GOLIATH_TGoWht03 = 0xFB,
  SFX_Terran_GOLIATH_TGoYes00 = 0xFC,
  SFX_Terran_GOLIATH_TGoYes01 = 0xFD,
  SFX_Terran_GOLIATH_TGoYes02 = 0xFE,
  SFX_Terran_GOLIATH_TGoYes03 = 0xFF,
  SFX_Terran_PHOENIX_TPhRdy00 = 0x100,
  SFX_Terran_PHOENIX_TPhDth00 = 0x101,
  SFX_Terran_PHOENIX_TPhPss00 = 0x102,
  SFX_Terran_PHOENIX_TPhPss01 = 0x103,
  SFX_Terran_PHOENIX_TPhPss02 = 0x104,
  SFX_Terran_PHOENIX_TPhPss03 = 0x105,
  SFX_Terran_PHOENIX_TPhPss04 = 0x106,
  SFX_Terran_PHOENIX_TPhPss05 = 0x107,
  SFX_Terran_PHOENIX_TPhPss06 = 0x108,
  SFX_Terran_PHOENIX_TPhWht00 = 0x109,
  SFX_Terran_PHOENIX_TPhWht01 = 0x10A,
  SFX_Terran_PHOENIX_TPhWht02 = 0x10B,
  SFX_Terran_PHOENIX_TPhWht03 = 0x10C,
  SFX_Terran_PHOENIX_TPhYes00 = 0x10D,
  SFX_Terran_PHOENIX_TPhYes01 = 0x10E,
  SFX_Terran_PHOENIX_TPhYes02 = 0x10F,
  SFX_Terran_PHOENIX_TPhYes03 = 0x110,
  SFX_Terran_PHOENIX_TPhClo00 = 0x111,
  SFX_Terran_PHOENIX_TPhClo01 = 0x112,
  SFX_Terran_MARINE_TMaRdy00 = 0x113,
  SFX_Terran_MARINE_TMaDth00 = 0x114,
  SFX_Terran_MARINE_TMaDth01 = 0x115,
  SFX_Terran_MARINE_TMaSti00 = 0x116,
  SFX_Terran_MARINE_TMaSti01 = 0x117,
  SFX_Terran_MARINE_TMaPss00 = 0x118,
  SFX_Terran_MARINE_TMaPss01 = 0x119,
  SFX_Terran_MARINE_TMaPss02 = 0x11A,
  SFX_Terran_MARINE_TMaPss03 = 0x11B,
  SFX_Terran_MARINE_TMaPss04 = 0x11C,
  SFX_Terran_MARINE_TMaPss05 = 0x11D,
  SFX_Terran_MARINE_TMaPss06 = 0x11E,
  SFX_Terran_MARINE_TMaWht00 = 0x11F,
  SFX_Terran_MARINE_TMaWht01 = 0x120,
  SFX_Terran_MARINE_TMaWht02 = 0x121,
  SFX_Terran_MARINE_TMaWht03 = 0x122,
  SFX_Terran_MARINE_TMaYes00 = 0x123,
  SFX_Terran_MARINE_TMaYes01 = 0x124,
  SFX_Terran_MARINE_TMaYes02 = 0x125,
  SFX_Terran_MARINE_TMaYes03 = 0x126,
  SFX_Terran_Firebat_TFBRdy00 = 0x127,
  SFX_Terran_Firebat_TFBDth00 = 0x128,
  SFX_Terran_Firebat_TFBDth01 = 0x129,
  SFX_Terran_Firebat_TFBDth02 = 0x12A,
  SFX_Terran_Firebat_TFBWht00 = 0x12B,
  SFX_Terran_Firebat_TFBWht01 = 0x12C,
  SFX_Terran_Firebat_TFBWht02 = 0x12D,
  SFX_Terran_Firebat_TFBWht03 = 0x12E,
  SFX_Terran_Firebat_TFBPss00 = 0x12F,
  SFX_Terran_Firebat_TFBPss01 = 0x130,
  SFX_Terran_Firebat_TFBPss02 = 0x131,
  SFX_Terran_Firebat_TFBPss03 = 0x132,
  SFX_Terran_Firebat_TFBPss04 = 0x133,
  SFX_Terran_Firebat_TFBPss05 = 0x134,
  SFX_Terran_Firebat_TFBPss06 = 0x135,
  SFX_Terran_Firebat_TFBYes00 = 0x136,
  SFX_Terran_Firebat_TFBYes01 = 0x137,
  SFX_Terran_Firebat_TFBYes02 = 0x138,
  SFX_Terran_Firebat_TFBYes03 = 0x139,
  SFX_Terran_Firebat_TFBFir00 = 0x13A,
  SFX_Terran_Firebat_TFBFir01 = 0x13B,
  SFX_Terran_TANK_TTaRdy00 = 0x13C,
  SFX_Terran_TANK_TTaDth00 = 0x13D,
  SFX_Terran_TANK_TTaTra00 = 0x13E,
  SFX_Terran_TANK_TTaTra01 = 0x13F,
  SFX_Terran_TANK_TTaPss00 = 0x140,
  SFX_Terran_TANK_TTaPss01 = 0x141,
  SFX_Terran_TANK_TTaPss02 = 0x142,
  SFX_Terran_TANK_TTaPss03 = 0x143,
  SFX_Terran_TANK_TTaWht00 = 0x144,
  SFX_Terran_TANK_TTaWht01 = 0x145,
  SFX_Terran_TANK_TTaWht02 = 0x146,
  SFX_Terran_TANK_TTaWht03 = 0x147,
  SFX_Terran_TANK_TTaYes00 = 0x148,
  SFX_Terran_TANK_TTaYes01 = 0x149,
  SFX_Terran_TANK_TTaYes02 = 0x14A,
  SFX_Terran_TANK_TTaYes03 = 0x14B,
  SFX_Terran_VESSEL_TVeRdy00 = 0x14C,
  SFX_Terran_VESSEL_TVePss00 = 0x14D,
  SFX_Terran_VESSEL_TVePss01 = 0x14E,
  SFX_Terran_VESSEL_TVePss02 = 0x14F,
  SFX_Terran_VESSEL_TVePss03 = 0x150,
  SFX_Terran_VESSEL_TVePss04 = 0x151,
  SFX_Terran_VESSEL_TVePss05 = 0x152,
  SFX_Terran_VESSEL_TVePss06 = 0x153,
  SFX_Terran_VESSEL_TVeWht00 = 0x154,
  SFX_Terran_VESSEL_TVeWht01 = 0x155,
  SFX_Terran_VESSEL_TVeWht02 = 0x156,
  SFX_Terran_VESSEL_TVeWht03 = 0x157,
  SFX_Terran_VESSEL_TVeYes00 = 0x158,
  SFX_Terran_VESSEL_TVeYes01 = 0x159,
  SFX_Terran_VESSEL_TVeYes02 = 0x15A,
  SFX_Terran_VESSEL_TVeYes03 = 0x15B,
  SFX_Terran_VESSEL_TVeDth00 = 0x15C,
  SFX_Terran_VESSEL_TVeDef00 = 0x15D,
  SFX_Terran_VESSEL_TVeEmp00 = 0x15E,
  SFX_Terran_VESSEL_TVeIrr00 = 0x15F,
  SFX_Terran_VULTURE_TVuRdy00 = 0x160,
  SFX_Terran_VULTURE_TVuDth00 = 0x161,
  SFX_Terran_VULTURE_TVuMin01 = 0x162,
  SFX_Terran_VULTURE_TVuMin00 = 0x163,
  SFX_Terran_VULTURE_TVuPss00 = 0x164,
  SFX_Terran_VULTURE_TVuPss01 = 0x165,
  SFX_Terran_VULTURE_TVuPss02 = 0x166,
  SFX_Terran_VULTURE_TVuPss03 = 0x167,
  SFX_Terran_VULTURE_TVuWht00 = 0x168,
  SFX_Terran_VULTURE_TVuWht01 = 0x169,
  SFX_Terran_VULTURE_TVuWht02 = 0x16A,
  SFX_Terran_VULTURE_TVuWht03 = 0x16B,
  SFX_Terran_VULTURE_TVuYes00 = 0x16C,
  SFX_Terran_VULTURE_TVuYes01 = 0x16D,
  SFX_Terran_VULTURE_TVuYes02 = 0x16E,
  SFX_Terran_VULTURE_TVuYes03 = 0x16F,
  SFX_Terran_SCV_TSCRdy00 = 0x170,
  SFX_Terran_SCV_TSCDth00 = 0x171,
  SFX_Terran_SCV_TSCPss00 = 0x172,
  SFX_Terran_SCV_TSCPss01 = 0x173,
  SFX_Terran_SCV_TSCPss02 = 0x174,
  SFX_Terran_SCV_TSCPss03 = 0x175,
  SFX_Terran_SCV_TSCPss04 = 0x176,
  SFX_Terran_SCV_TSCPss05 = 0x177,
  SFX_Terran_SCV_TSCPss06 = 0x178,
  SFX_Terran_SCV_TSCWht00 = 0x179,
  SFX_Terran_SCV_TSCWht01 = 0x17A,
  SFX_Terran_SCV_TSCWht02 = 0x17B,
  SFX_Terran_SCV_TSCWht03 = 0x17C,
  SFX_Terran_SCV_TSCYes00 = 0x17D,
  SFX_Terran_SCV_TSCYes01 = 0x17E,
  SFX_Terran_SCV_TSCYes02 = 0x17F,
  SFX_Terran_SCV_TSCYes03 = 0x180,
  SFX_Terran_bldg_tacwht00 = 0x181,
  SFX_Terran_bldg_tclwht00 = 0x182,
  SFX_Terran_bldg_tcswht00 = 0x183,
  SFX_Terran_bldg_tcsSca00 = 0x184,
  SFX_Terran_bldg_tddwht00 = 0x185,
  SFX_Terran_bldg_tglwht00 = 0x186,
  SFX_Misc_UICWht00 = 0x187,
  SFX_Terran_bldg_tmswht00 = 0x188,
  SFX_Terran_bldg_tmtwht00 = 0x189,
  SFX_Misc_UNrWht00 = 0x18A,
  SFX_Terran_bldg_tRlWht00 = 0x18B,
  SFX_Terran_bldg_tnswht00 = 0x18C,
  SFX_Terran_bldg_tpgwht00 = 0x18D,
  SFX_Terran_bldg_tplwht00 = 0x18E,
  SFX_Misc_Button_2 = 0x18F,
  SFX_Terran_bldg_trewht00 = 0x190,
  SFX_Terran_bldg_trfwht00 = 0x191,
  SFX_Terran_bldg_twpwht00 = 0x192,
  SFX_Terran_bldg_OnFirLrg = 0x193,
  SFX_Terran_SCV_TScTra00 = 0x194,
  SFX_Terran_RAYNORM_URaDth00 = 0x195,
  SFX_Terran_RAYNORM_URaDth01 = 0x196,
  SFX_Terran_RAYNORM_URaPss00 = 0x197,
  SFX_Terran_RAYNORM_URaPss01 = 0x198,
  SFX_Terran_RAYNORM_URaPss02 = 0x199,
  SFX_Terran_RAYNORM_URaPss03 = 0x19A,
  SFX_Terran_RAYNORM_URaWht00 = 0x19B,
  SFX_Terran_RAYNORM_URaWht01 = 0x19C,
  SFX_Terran_RAYNORM_URaWht02 = 0x19D,
  SFX_Terran_RAYNORM_URaWht03 = 0x19E,
  SFX_Terran_RAYNORM_URaYes00 = 0x19F,
  SFX_Terran_RAYNORM_URaYes01 = 0x1A0,
  SFX_Terran_RAYNORM_URaYes02 = 0x1A1,
  SFX_Terran_RAYNORM_URaYes03 = 0x1A2,
  SFX_Terran_RAYNORV_URVPss00 = 0x1A3,
  SFX_Terran_RAYNORV_URVPss01 = 0x1A4,
  SFX_Terran_RAYNORV_URVPss02 = 0x1A5,
  SFX_Terran_RAYNORV_URVPss03 = 0x1A6,
  SFX_Terran_RAYNORV_URVWht00 = 0x1A7,
  SFX_Terran_RAYNORV_URVWht01 = 0x1A8,
  SFX_Terran_RAYNORV_URVWht02 = 0x1A9,
  SFX_Terran_RAYNORV_URVWht03 = 0x1AA,
  SFX_Terran_RAYNORV_URVYes00 = 0x1AB,
  SFX_Terran_RAYNORV_URVYes01 = 0x1AC,
  SFX_Terran_RAYNORV_URVYes02 = 0x1AD,
  SFX_Terran_RAYNORV_URVYes03 = 0x1AE,
  SFX_Terran_DUKET_UDTPss00 = 0x1AF,
  SFX_Terran_DUKET_UDTPss01 = 0x1B0,
  SFX_Terran_DUKET_UDTPss02 = 0x1B1,
  SFX_Terran_DUKET_UDTPss03 = 0x1B2,
  SFX_Terran_DUKET_UDTPss04 = 0x1B3,
  SFX_Terran_DUKET_UDTWht00 = 0x1B4,
  SFX_Terran_DUKET_UDTWht01 = 0x1B5,
  SFX_Terran_DUKET_UDTWht02 = 0x1B6,
  SFX_Terran_DUKET_UDTWht03 = 0x1B7,
  SFX_Terran_DUKET_UDTYes00 = 0x1B8,
  SFX_Terran_DUKET_UDTYes01 = 0x1B9,
  SFX_Terran_DUKET_UDTYes02 = 0x1BA,
  SFX_Terran_DUKET_UDTYes03 = 0x1BB,
  SFX_Terran_DUKEB_UDupss00 = 0x1BC,
  SFX_Terran_DUKEB_UDupss01 = 0x1BD,
  SFX_Terran_DUKEB_UDupss02 = 0x1BE,
  SFX_Terran_DUKEB_UDupss03 = 0x1BF,
  SFX_Terran_DUKEB_UDupss04 = 0x1C0,
  SFX_Terran_DUKEB_UDuwht00 = 0x1C1,
  SFX_Terran_DUKEB_UDuwht01 = 0x1C2,
  SFX_Terran_DUKEB_UDuwht02 = 0x1C3,
  SFX_Terran_DUKEB_UDuwht03 = 0x1C4,
  SFX_Terran_DUKEB_UDuyes00 = 0x1C5,
  SFX_Terran_DUKEB_UDuyes01 = 0x1C6,
  SFX_Terran_DUKEB_UDuyes02 = 0x1C7,
  SFX_Terran_DUKEB_UDuyes03 = 0x1C8,
  SFX_Terran_KERRIGAN_UKePss00 = 0x1C9,
  SFX_Terran_KERRIGAN_UKePss01 = 0x1CA,
  SFX_Terran_KERRIGAN_UKePss02 = 0x1CB,
  SFX_Terran_KERRIGAN_UKePss03 = 0x1CC,
  SFX_Terran_KERRIGAN_UKePss04 = 0x1CD,
  SFX_Terran_KERRIGAN_UKeWht00 = 0x1CE,
  SFX_Terran_KERRIGAN_UKeWht01 = 0x1CF,
  SFX_Terran_KERRIGAN_UKeWht02 = 0x1D0,
  SFX_Terran_KERRIGAN_UKeWht03 = 0x1D1,
  SFX_Terran_KERRIGAN_UKeYes00 = 0x1D2,
  SFX_Terran_KERRIGAN_UKeYes01 = 0x1D3,
  SFX_Terran_KERRIGAN_UKeYes02 = 0x1D4,
  SFX_Terran_KERRIGAN_UKeYes03 = 0x1D5,
  SFX_Terran_KERRIGAN_UKeDth00 = 0x1D6,
  SFX_Misc_LiftOff = 0x1D7,
  SFX_Misc_Land = 0x1D8,
  SFX_Protoss_Bldg_pAcWht00 = 0x1D9,
  SFX_Protoss_Bldg_pAsWht00 = 0x1DA,
  SFX_Protoss_Bldg_pBaWht00 = 0x1DB,
  SFX_Protoss_Bldg_pBeWht00 = 0x1DC,
  SFX_Protoss_Bldg_pCiWht00 = 0x1DD,
  SFX_Protoss_Bldg_pFoWht00 = 0x1DE,
  SFX_Protoss_Bldg_pGaWht00 = 0x1DF,
  SFX_Protoss_Bldg_pGcWht00 = 0x1E0,
  SFX_Protoss_Bldg_PbaAct00 = 0x1E1,
  SFX_Misc_Button_3 = 0x1E2,
  SFX_Protoss_Bldg_pNaWht00_1 = 0x1E3,
  SFX_Protoss_Bldg_pNeWht00 = 0x1E4,
  SFX_Protoss_Bldg_pPBWht00 = 0x1E5,
  SFX_Misc_UTmWht00 = 0x1E6,
  SFX_Protoss_Bldg_pPyWht00 = 0x1E7,
  SFX_Protoss_Bldg_pRoWht00 = 0x1E8,
  SFX_Misc_UPiWht00 = 0x1E9,
  SFX_Protoss_Bldg_PTrWht00 = 0x1EA,
  SFX_Protoss_Bldg_pWaWht00 = 0x1EB,
  SFX_Protoss_DRAGOON_PDrRdy00 = 0x1EC,
  SFX_Protoss_DRAGOON_PDrDth00 = 0x1ED,
  SFX_Protoss_DRAGOON_PDrPss00 = 0x1EE,
  SFX_Protoss_DRAGOON_PDrPss01 = 0x1EF,
  SFX_Protoss_DRAGOON_PDrPss02 = 0x1F0,
  SFX_Protoss_DRAGOON_PDrPss03 = 0x1F1,
  SFX_Protoss_DRAGOON_PDrWht00 = 0x1F2,
  SFX_Protoss_DRAGOON_PDrWht01 = 0x1F3,
  SFX_Protoss_DRAGOON_PDrWht02 = 0x1F4,
  SFX_Protoss_DRAGOON_PDrWht03 = 0x1F5,
  SFX_Protoss_DRAGOON_PDrWht04 = 0x1F6,
  SFX_Protoss_DRAGOON_PDrWht05 = 0x1F7,
  SFX_Protoss_DRAGOON_PDrWht06 = 0x1F8,
  SFX_Protoss_DRAGOON_PDrWht07 = 0x1F9,
  SFX_Protoss_DRAGOON_PDrYes00 = 0x1FA,
  SFX_Protoss_DRAGOON_PDrYes01 = 0x1FB,
  SFX_Protoss_DRAGOON_PDrYes02 = 0x1FC,
  SFX_Protoss_DRAGOON_PDrYes03 = 0x1FD,
  SFX_Protoss_DRAGOON_PDrYes04 = 0x1FE,
  SFX_Protoss_DRAGOON_PDrYes05 = 0x1FF,
  SFX_Protoss_DRAGOON_PDrYes06 = 0x200,
  SFX_Protoss_SHUTTLE_PShRdy00 = 0x201,
  SFX_Protoss_SHUTTLE_PShDth00 = 0x202,
  SFX_Protoss_SHUTTLE_PShPss00 = 0x203,
  SFX_Protoss_SHUTTLE_PShPss01 = 0x204,
  SFX_Protoss_SHUTTLE_PShPss02 = 0x205,
  SFX_Protoss_SHUTTLE_PShPss03 = 0x206,
  SFX_Protoss_SHUTTLE_PShPss04 = 0x207,
  SFX_Protoss_SHUTTLE_PShWht00 = 0x208,
  SFX_Protoss_SHUTTLE_PShWht01 = 0x209,
  SFX_Protoss_SHUTTLE_PShWht02 = 0x20A,
  SFX_Protoss_SHUTTLE_PShWht03 = 0x20B,
  SFX_Protoss_SHUTTLE_PShYes00 = 0x20C,
  SFX_Protoss_SHUTTLE_PShYes01 = 0x20D,
  SFX_Protoss_SHUTTLE_PShYes02 = 0x20E,
  SFX_Protoss_SHUTTLE_PShYes03 = 0x20F,
  SFX_Protoss_SHUTTLE_pshBld00 = 0x210,
  SFX_Protoss_SHUTTLE_pshBld03 = 0x211,
  SFX_Protoss_SHUTTLE_pshBld01 = 0x212,
  SFX_Protoss_SHUTTLE_pshBld02 = 0x213,
  SFX_Protoss_SHUTTLE_pshBld04 = 0x214,
  SFX_Protoss_Scout_PScDth00 = 0x215,
  SFX_Protoss_SCOUT_PScRdy00 = 0x216,
  SFX_Protoss_Scout_PScPss00 = 0x217,
  SFX_Protoss_Scout_PScPss01 = 0x218,
  SFX_Protoss_Scout_PScPss02 = 0x219,
  SFX_Protoss_Scout_PScPss03 = 0x21A,
  SFX_Protoss_Scout_PScPss04 = 0x21B,
  SFX_Protoss_Scout_PScWht00 = 0x21C,
  SFX_Protoss_Scout_PScWht01 = 0x21D,
  SFX_Protoss_Scout_PScWht02 = 0x21E,
  SFX_Protoss_Scout_PScWht03 = 0x21F,
  SFX_Protoss_Scout_PScYes00 = 0x220,
  SFX_Protoss_Scout_PScYes01 = 0x221,
  SFX_Protoss_Scout_PScYes02 = 0x222,
  SFX_Protoss_Scout_PScYes03 = 0x223,
  SFX_Protoss_ARBITER_PAbCag00 = 0x224,
  SFX_Protoss_ARBITER_PAbRdy00 = 0x225,
  SFX_Protoss_ARBITER_PAbFol00 = 0x226,
  SFX_Protoss_ARBITER_PAbFol01 = 0x227,
  SFX_Protoss_ARBITER_PAbFol02 = 0x228,
  SFX_Protoss_ARBITER_PAbFol03 = 0x229,
  SFX_Protoss_ARBITER_PAbPss00 = 0x22A,
  SFX_Protoss_ARBITER_PAbPss01 = 0x22B,
  SFX_Protoss_ARBITER_PAbPss02 = 0x22C,
  SFX_Protoss_ARBITER_PAbPss03 = 0x22D,
  SFX_Protoss_ARBITER_PAbPss04 = 0x22E,
  SFX_Protoss_ARBITER_PAbWht00 = 0x22F,
  SFX_Protoss_ARBITER_PAbWht01 = 0x230,
  SFX_Protoss_ARBITER_PAbWht02 = 0x231,
  SFX_Protoss_ARBITER_PAbWht03 = 0x232,
  SFX_Protoss_ARBITER_PAbYes00 = 0x233,
  SFX_Protoss_ARBITER_PAbYes01 = 0x234,
  SFX_Protoss_ARBITER_PAbYes02 = 0x235,
  SFX_Protoss_ARBITER_PAbDth00 = 0x236,
  SFX_Protoss_ARCHON_PArRdy00 = 0x237,
  SFX_Protoss_ARCHON_PArMin00 = 0x238,
  SFX_Protoss_ARCHON_PArPss00 = 0x239,
  SFX_Protoss_ARCHON_PArPss01 = 0x23A,
  SFX_Protoss_ARCHON_PArPss02 = 0x23B,
  SFX_Protoss_ARCHON_PArPss03 = 0x23C,
  SFX_Protoss_ARCHON_PArWht00 = 0x23D,
  SFX_Protoss_ARCHON_PArWht01 = 0x23E,
  SFX_Protoss_ARCHON_PArWht02 = 0x23F,
  SFX_Protoss_ARCHON_PArWht03 = 0x240,
  SFX_Protoss_ARCHON_PArYes00 = 0x241,
  SFX_Protoss_ARCHON_PArYes01 = 0x242,
  SFX_Protoss_ARCHON_PArYes02 = 0x243,
  SFX_Protoss_ARCHON_PArYes03 = 0x244,
  SFX_Protoss_ARCHON_PArDth00 = 0x245,
  SFX_Protoss_CARRIER_PCaRdy00 = 0x246,
  SFX_Protoss_Carrier_PCaPss00 = 0x247,
  SFX_Protoss_Carrier_PCaPss01 = 0x248,
  SFX_Protoss_Carrier_PCaPss02 = 0x249,
  SFX_Protoss_Carrier_PCaPss03 = 0x24A,
  SFX_Protoss_Carrier_PCaWht00 = 0x24B,
  SFX_Protoss_Carrier_PCaWht01 = 0x24C,
  SFX_Protoss_Carrier_PCaWht02 = 0x24D,
  SFX_Protoss_Carrier_PCaWht03 = 0x24E,
  SFX_Protoss_Carrier_PCaYes00 = 0x24F,
  SFX_Protoss_Carrier_PCaYes01 = 0x250,
  SFX_Protoss_Carrier_PCaYes02 = 0x251,
  SFX_Protoss_Carrier_PCaYes03 = 0x252,
  SFX_Protoss_Carrier_PCaDth00 = 0x253,
  SFX_Protoss_Carrier_PCaDth01 = 0x254,
  SFX_Protoss_PROBE_PPrRdy00 = 0x255,
  SFX_Protoss_PROBE_PPrDth00 = 0x256,
  SFX_Protoss_PROBE_PPrAtt00 = 0x257,
  SFX_Protoss_PROBE_PPrAtt01 = 0x258,
  SFX_Protoss_PROBE_PPrMin00 = 0x259,
  SFX_Protoss_PROBE_PPrPss00_1 = 0x25A,
  SFX_Protoss_PROBE_PPrPss01 = 0x25B,
  SFX_Protoss_PROBE_PPrPss02 = 0x25C,
  SFX_Protoss_PROBE_PPrPss03 = 0x25D,
  SFX_Protoss_PROBE_PPrWht00 = 0x25E,
  SFX_Protoss_PROBE_PPrWht01 = 0x25F,
  SFX_Protoss_PROBE_PPrWht02 = 0x260,
  SFX_Protoss_PROBE_PPrWht03 = 0x261,
  SFX_Protoss_PROBE_PPrYes00 = 0x262,
  SFX_Protoss_PROBE_PPrYes01 = 0x263,
  SFX_Protoss_PROBE_PPrYes02 = 0x264,
  SFX_Protoss_PROBE_PPrYes03 = 0x265,
  SFX_Protoss_INTERCEP_PInLau00 = 0x266,
  SFX_Protoss_PROBE_PPrPss00_2 = 0x267,
  SFX_Protoss_TEMPLAR_PTeSum00_1 = 0x268,
  SFX_Protoss_TEMPLAR_PTeSum00_2 = 0x269,
  SFX_Protoss_TEMPLAR_PTeHal00 = 0x26A,
  SFX_Protoss_TEMPLAR_PTeHal01 = 0x26B,
  SFX_Protoss_TEMPLAR_PTeSto00 = 0x26C,
  SFX_Protoss_TEMPLAR_PTeSto01 = 0x26D,
  SFX_Protoss_TEMPLAR_PTeRdy00 = 0x26E,
  SFX_Protoss_TEMPLAR_PTePss00 = 0x26F,
  SFX_Protoss_TEMPLAR_PTePss01 = 0x270,
  SFX_Protoss_TEMPLAR_PTePss02 = 0x271,
  SFX_Protoss_TEMPLAR_PTePss03 = 0x272,
  SFX_Protoss_TEMPLAR_PTeWht00 = 0x273,
  SFX_Protoss_TEMPLAR_PTeWht01 = 0x274,
  SFX_Protoss_TEMPLAR_PTeWht02 = 0x275,
  SFX_Protoss_TEMPLAR_PTeWht03 = 0x276,
  SFX_Protoss_TEMPLAR_PTeYes00 = 0x277,
  SFX_Protoss_TEMPLAR_PTeYes01 = 0x278,
  SFX_Protoss_TEMPLAR_PTeYes02 = 0x279,
  SFX_Protoss_TEMPLAR_PTeYes03 = 0x27A,
  SFX_Protoss_TEMPLAR_PTeDth00 = 0x27B,
  SFX_Protoss_TEMPLAR_PTeMov00 = 0x27C,
  SFX_Protoss_TRILOBYTE_PTrRdy00 = 0x27D,
  SFX_Protoss_TRILOBYTE_PTrDth00 = 0x27E,
  SFX_Protoss_TRILOBYTE_PTrPss00 = 0x27F,
  SFX_Protoss_TRILOBYTE_PTrPss01 = 0x280,
  SFX_Protoss_TRILOBYTE_PTrPss02 = 0x281,
  SFX_Protoss_TRILOBYTE_PTrWht00 = 0x282,
  SFX_Protoss_TRILOBYTE_PTrWht01 = 0x283,
  SFX_Protoss_TRILOBYTE_PTrWht02 = 0x284,
  SFX_Protoss_TRILOBYTE_PTrWht03 = 0x285,
  SFX_Protoss_TRILOBYTE_PTrYes00 = 0x286,
  SFX_Protoss_TRILOBYTE_PTrYes01 = 0x287,
  SFX_Protoss_TRILOBYTE_PTrYes02 = 0x288,
  SFX_Protoss_TRILOBYTE_PTrYes03 = 0x289,
  SFX_Protoss_Witness_PWiRdy00 = 0x28A,
  SFX_Protoss_Witness_PWiDth00 = 0x28B,
  SFX_Protoss_Witness_PWiDth01 = 0x28C,
  SFX_Protoss_Witness_PWiPss00 = 0x28D,
  SFX_Protoss_Witness_PWiPss01 = 0x28E,
  SFX_Protoss_Witness_PWiPss02 = 0x28F,
  SFX_Protoss_Witness_PWiPss03 = 0x290,
  SFX_Protoss_Witness_PWiPss04 = 0x291,
  SFX_Protoss_Witness_PWiWht00 = 0x292,
  SFX_Protoss_Witness_PWiWht01 = 0x293,
  SFX_Protoss_Witness_PWiYes00 = 0x294,
  SFX_Protoss_Witness_PWiYes01 = 0x295,
  SFX_Protoss_ZEALOT_pzeAtt00 = 0x296,
  SFX_Protoss_ZEALOT_pzeAtt01 = 0x297,
  SFX_Protoss_ZEALOT_pzeHit00 = 0x298,
  SFX_Protoss_ZEALOT_PZeRag00 = 0x299,
  SFX_Protoss_ZEALOT_PZeRdy00 = 0x29A,
  SFX_Protoss_ZEALOT_PZePss00 = 0x29B,
  SFX_Protoss_ZEALOT_PZePss01 = 0x29C,
  SFX_Protoss_ZEALOT_PZePss02 = 0x29D,
  SFX_Protoss_ZEALOT_PZeWht00 = 0x29E,
  SFX_Protoss_ZEALOT_PZeWht01 = 0x29F,
  SFX_Protoss_ZEALOT_PZeWht02 = 0x2A0,
  SFX_Protoss_ZEALOT_PZeWht03 = 0x2A1,
  SFX_Protoss_ZEALOT_PZeYes00 = 0x2A2,
  SFX_Protoss_ZEALOT_PZeYes01 = 0x2A3,
  SFX_Protoss_ZEALOT_PZeYes02 = 0x2A4,
  SFX_Protoss_ZEALOT_PZeYes03 = 0x2A5,
  SFX_Protoss_ZEALOT_PZeDth00 = 0x2A6,
  SFX_Protoss_FENIXD_UFdPss00 = 0x2A7,
  SFX_Protoss_FENIXD_UFdPss01 = 0x2A8,
  SFX_Protoss_FENIXD_UFdPss02 = 0x2A9,
  SFX_Protoss_FENIXD_UFdPss03 = 0x2AA,
  SFX_Protoss_FENIXD_UFdWht00 = 0x2AB,
  SFX_Protoss_FENIXD_UFdWht01 = 0x2AC,
  SFX_Protoss_FENIXD_UFdWht02 = 0x2AD,
  SFX_Protoss_FENIXD_UFdWht03 = 0x2AE,
  SFX_Protoss_FENIXD_UFdYes00 = 0x2AF,
  SFX_Protoss_FENIXD_UFdYes01 = 0x2B0,
  SFX_Protoss_FENIXD_UFdYes02 = 0x2B1,
  SFX_Protoss_FENIXD_UFdYes03 = 0x2B2,
  SFX_Protoss_FENIXZ_UFePss00 = 0x2B3,
  SFX_Protoss_FENIXZ_UFePss01 = 0x2B4,
  SFX_Protoss_FENIXZ_UFePss02 = 0x2B5,
  SFX_Protoss_FENIXZ_UFePss03 = 0x2B6,
  SFX_Protoss_FENIXZ_UFeWht00 = 0x2B7,
  SFX_Protoss_FENIXZ_UFeWht01 = 0x2B8,
  SFX_Protoss_FENIXZ_UFeWht02 = 0x2B9,
  SFX_Protoss_FENIXZ_UFeWht03 = 0x2BA,
  SFX_Protoss_FENIXZ_UFeYes00 = 0x2BB,
  SFX_Protoss_FENIXZ_UFeYes01 = 0x2BC,
  SFX_Protoss_FENIXZ_UFeYes02 = 0x2BD,
  SFX_Protoss_FENIXZ_UFeYes03 = 0x2BE,
  SFX_Protoss_GANTRITHOR_UTCPss00 = 0x2BF,
  SFX_Protoss_GANTRITHOR_UTCPss01 = 0x2C0,
  SFX_Protoss_GANTRITHOR_UTCPss02 = 0x2C1,
  SFX_Protoss_GANTRITHOR_UTCPss03 = 0x2C2,
  SFX_Protoss_GANTRITHOR_UTCWht00 = 0x2C3,
  SFX_Protoss_GANTRITHOR_UTCWht01 = 0x2C4,
  SFX_Protoss_GANTRITHOR_UTCWht02 = 0x2C5,
  SFX_Protoss_GANTRITHOR_UTCWht03 = 0x2C6,
  SFX_Protoss_GANTRITHOR_UTCYes00 = 0x2C7,
  SFX_Protoss_GANTRITHOR_UTCYes01 = 0x2C8,
  SFX_Protoss_GANTRITHOR_UTCYes02 = 0x2C9,
  SFX_Protoss_GANTRITHOR_UTCYes03 = 0x2CA,
  SFX_Protoss_TASSADAR_UTaPss00 = 0x2CB,
  SFX_Protoss_TASSADAR_UTaPss01 = 0x2CC,
  SFX_Protoss_TASSADAR_UTaPss02 = 0x2CD,
  SFX_Protoss_TASSADAR_UTaPss03 = 0x2CE,
  SFX_Protoss_TASSADAR_UTaWht00 = 0x2CF,
  SFX_Protoss_TASSADAR_UTaWht01 = 0x2D0,
  SFX_Protoss_TASSADAR_UTaWht02 = 0x2D1,
  SFX_Protoss_TASSADAR_UTaWht03 = 0x2D2,
  SFX_Protoss_TASSADAR_UTaYes00 = 0x2D3,
  SFX_Protoss_TASSADAR_UTaYes01 = 0x2D4,
  SFX_Protoss_TASSADAR_UTaYes02 = 0x2D5,
  SFX_Protoss_TASSADAR_UTaYes03 = 0x2D6,
  SFX_Protoss_TASSADAR_UTaDth00 = 0x2D7,
  SFX_Protoss_DARKTEMPLAR_PDTRdy00 = 0x2D8,
  SFX_Protoss_DARKTEMPLAR_PDTPss00 = 0x2D9,
  SFX_Protoss_DARKTEMPLAR_PDTPss01 = 0x2DA,
  SFX_Protoss_DARKTEMPLAR_PDTPss02 = 0x2DB,
  SFX_Protoss_DARKTEMPLAR_PDTPss03 = 0x2DC,
  SFX_Protoss_DARKTEMPLAR_PDTWht00 = 0x2DD,
  SFX_Protoss_DARKTEMPLAR_PDTWht01 = 0x2DE,
  SFX_Protoss_DARKTEMPLAR_PDTWht02 = 0x2DF,
  SFX_Protoss_DARKTEMPLAR_PDTWht03 = 0x2E0,
  SFX_Protoss_DARKTEMPLAR_PDTYes00 = 0x2E1,
  SFX_Protoss_DARKTEMPLAR_PDTYes01 = 0x2E2,
  SFX_Protoss_DARKTEMPLAR_PDTYes02 = 0x2E3,
  SFX_Protoss_DARKTEMPLAR_PDTYes03 = 0x2E4,
  SFX_Protoss_DARKTEMPLAR_PDTDth00 = 0x2E5,
  SFX_Protoss_ZERATUL_UZePss00 = 0x2E6,
  SFX_Protoss_ZERATUL_UZePss01 = 0x2E7,
  SFX_Protoss_ZERATUL_UZePss02 = 0x2E8,
  SFX_Protoss_ZERATUL_UZePss03 = 0x2E9,
  SFX_Protoss_ZERATUL_UZeWht00 = 0x2EA,
  SFX_Protoss_ZERATUL_UZeWht01 = 0x2EB,
  SFX_Protoss_ZERATUL_UZeWht02 = 0x2EC,
  SFX_Protoss_ZERATUL_UZeWht03 = 0x2ED,
  SFX_Protoss_ZERATUL_UZeYes00 = 0x2EE,
  SFX_Protoss_ZERATUL_UZeYes01 = 0x2EF,
  SFX_Protoss_ZERATUL_UZeYes02 = 0x2F0,
  SFX_Protoss_ZERATUL_UZeYes03 = 0x2F1,
  SFX_Protoss_ZERATUL_UZeDth00 = 0x2F2,
  SFX_Zerg_Bldg_ZChWht00 = 0x2F3,
  SFX_Zerg_Bldg_ZCbWht00 = 0x2F4,
  SFX_Zerg_Bldg_ZEvWht00 = 0x2F5,
  SFX_Zerg_Bldg_ZFcWht00 = 0x2F6,
  SFX_Zerg_Bldg_ZHaWht00 = 0x2F7,
  SFX_Zerg_Bldg_ZHiWht00 = 0x2F8,
  SFX_Zerg_Bldg_ZIgWht00 = 0x2F9,
  SFX_Zerg_Bldg_ZLrWht00 = 0x2FA,
  SFX_Zerg_Bldg_ZLuWht00 = 0x2FB,
  SFX_Zerg_Bldg_ZMcWht00 = 0x2FC,
  SFX_Protoss_Bldg_pNaWht00_2 = 0x2FD,
  SFX_Zerg_Bldg_ZMhWht00 = 0x2FE,
  SFX_Zerg_Bldg_ZNeWht00 = 0x2FF,
  SFX_Zerg_Bldg_ZNyWht00 = 0x300,
  SFX_Zerg_Bldg_ZO1Wht00 = 0x301,
  SFX_Zerg_Bldg_ZRcWht00 = 0x302,
  SFX_Zerg_Bldg_ZSbWht00 = 0x303,
  SFX_Zerg_Bldg_ZScWht00 = 0x304,
  SFX_Zerg_Bldg_ZSpWht00 = 0x305,
  SFX_Zerg_Bldg_ZBldgDth = 0x306,
  SFX_Zerg_AVENGER_ZAvRdy00 = 0x307,
  SFX_Zerg_AVENGER_ZAvDth00 = 0x308,
  SFX_Zerg_AVENGER_ZAvAtt00 = 0x309,
  SFX_Zerg_AVENGER_ZAvHit00 = 0x30A,
  SFX_Zerg_AVENGER_ZAvPss00 = 0x30B,
  SFX_Zerg_AVENGER_ZAvPss01 = 0x30C,
  SFX_Zerg_AVENGER_ZAvWht00 = 0x30D,
  SFX_Zerg_AVENGER_ZAvWht01 = 0x30E,
  SFX_Zerg_AVENGER_ZAvYes00 = 0x30F,
  SFX_Zerg_AVENGER_ZAvYes01 = 0x310,
  SFX_Zerg_BROODLING_ZBrDth00 = 0x311,
  SFX_Zerg_BROODLING_ZBrAtt00 = 0x312,
  SFX_Zerg_BROODLING_ZBrRdy00 = 0x313,
  SFX_Zerg_BROODLING_ZBrPss00 = 0x314,
  SFX_Zerg_BROODLING_ZBrPss01 = 0x315,
  SFX_Zerg_BROODLING_ZBrPss02 = 0x316,
  SFX_Zerg_BROODLING_ZBrPss03 = 0x317,
  SFX_Zerg_BROODLING_ZBrWht00 = 0x318,
  SFX_Zerg_BROODLING_ZBrWht01 = 0x319,
  SFX_Zerg_BROODLING_ZBrWht02 = 0x31A,
  SFX_Zerg_BROODLING_ZBrWht03 = 0x31B,
  SFX_Zerg_BROODLING_ZBrYes00 = 0x31C,
  SFX_Zerg_BROODLING_ZBrYes01 = 0x31D,
  SFX_Zerg_BROODLING_ZBrYes02 = 0x31E,
  SFX_Zerg_BUGGUY_ZBGRdy00_1 = 0x31F,
  SFX_Zerg_BUGGUY_ZBGRdy00_2 = 0x320,
  SFX_Zerg_BUGGUY_ZBGPss00 = 0x321,
  SFX_Zerg_BUGGUY_ZBGPss01 = 0x322,
  SFX_Zerg_BUGGUY_ZBGPss02 = 0x323,
  SFX_Zerg_BUGGUY_ZBGPss03 = 0x324,
  SFX_Zerg_BUGGUY_ZBGWht00 = 0x325,
  SFX_Zerg_BUGGUY_ZBGWht01 = 0x326,
  SFX_Zerg_BUGGUY_ZBGWht02 = 0x327,
  SFX_Zerg_BUGGUY_ZBGWht03 = 0x328,
  SFX_Zerg_BUGGUY_ZBGYes00 = 0x329,
  SFX_Zerg_BUGGUY_ZBGYes01 = 0x32A,
  SFX_Zerg_BUGGUY_ZBGYes02 = 0x32B,
  SFX_Zerg_BUGGUY_ZBGYes03 = 0x32C,
  SFX_Zerg_DEFILER_ZDeCon00 = 0x32D,
  SFX_Zerg_DEFILER_ZDeRdy00 = 0x32E,
  SFX_Zerg_DEFILER_ZDeDth00 = 0x32F,
  SFX_Zerg_DEFILER_ZDeBlo00 = 0x330,
  SFX_Zerg_DEFILER_ZDeBlo01 = 0x331,
  SFX_Zerg_DEFILER_ZDePss00 = 0x332,
  SFX_Zerg_DEFILER_ZDePss01 = 0x333,
  SFX_Zerg_DEFILER_ZDePss02 = 0x334,
  SFX_Zerg_DEFILER_ZDeWht00 = 0x335,
  SFX_Zerg_DEFILER_ZDeWht01 = 0x336,
  SFX_Zerg_DEFILER_ZDeWht02 = 0x337,
  SFX_Zerg_DEFILER_ZDeYes00 = 0x338,
  SFX_Zerg_DEFILER_ZDeYes01 = 0x339,
  SFX_Zerg_DEFILER_ZDeYes02 = 0x33A,
  SFX_Zerg_Egg_ZEgRdy00 = 0x33B,
  SFX_Zerg_Egg_ZEgWht00 = 0x33C,
  SFX_Zerg_Egg_ZEgWht01 = 0x33D,
  SFX_Zerg_Egg_ZEgDth00 = 0x33E,
  SFX_Zerg_Egg_ZEgPss00 = 0x33F,
  SFX_Zerg_DRONE_ZDrRdy00 = 0x340,
  SFX_Zerg_DRONE_ZDrDth00 = 0x341,
  SFX_Zerg_DRONE_ZDrPss00 = 0x342,
  SFX_Zerg_DRONE_ZDrPss01 = 0x343,
  SFX_Zerg_DRONE_ZDrPss02 = 0x344,
  SFX_Zerg_DRONE_ZDrWht00 = 0x345,
  SFX_Zerg_DRONE_ZDrWht01 = 0x346,
  SFX_Zerg_DRONE_ZDrWht02 = 0x347,
  SFX_Zerg_DRONE_ZDrWht03 = 0x348,
  SFX_Zerg_DRONE_ZDrWht04 = 0x349,
  SFX_Zerg_DRONE_ZDrYes00 = 0x34A,
  SFX_Zerg_DRONE_ZDrYes01 = 0x34B,
  SFX_Zerg_DRONE_ZDrYes02 = 0x34C,
  SFX_Zerg_DRONE_ZDrYes03 = 0x34D,
  SFX_Zerg_DRONE_ZDrYes04 = 0x34E,
  SFX_Zerg_DRONE_ZDrMin00 = 0x34F,
  SFX_Zerg_Larva_ZLaDth00 = 0x350,
  SFX_Terran_Advisor_tAdErr04_5 = 0x351,
  SFX_Zerg_Larva_ZLaPss00 = 0x352,
  SFX_Zerg_Larva_ZLaWht00 = 0x353,
  SFX_Zerg_Guardian_ZGuDth00 = 0x354,
  SFX_Zerg_Guardian_ZGuPss00 = 0x355,
  SFX_Zerg_Guardian_ZGuPss01 = 0x356,
  SFX_Zerg_Guardian_ZGuPss02 = 0x357,
  SFX_Zerg_Guardian_ZGuPss03 = 0x358,
  SFX_Zerg_Guardian_ZGuRdy00 = 0x359,
  SFX_Zerg_Guardian_ZGuWht00 = 0x35A,
  SFX_Zerg_Guardian_ZGuWht01 = 0x35B,
  SFX_Zerg_Guardian_ZGuWht02 = 0x35C,
  SFX_Zerg_Guardian_ZGuWht03 = 0x35D,
  SFX_Zerg_Guardian_ZGuYes00 = 0x35E,
  SFX_Zerg_Guardian_ZGuYes01 = 0x35F,
  SFX_Zerg_Guardian_ZGuYes02 = 0x360,
  SFX_Zerg_Guardian_ZGuYes03 = 0x361,
  SFX_Zerg_Hydra_ZHyRdy00 = 0x362,
  SFX_Zerg_Hydra_ZHyDth00 = 0x363,
  SFX_Zerg_Hydra_ZHyPss00 = 0x364,
  SFX_Zerg_Hydra_ZHyPss01 = 0x365,
  SFX_Zerg_Hydra_ZHyWht00 = 0x366,
  SFX_Zerg_Hydra_ZHyWht01 = 0x367,
  SFX_Zerg_Hydra_ZHyWht02 = 0x368,
  SFX_Zerg_Hydra_ZHyYes00 = 0x369,
  SFX_Zerg_Hydra_ZHyYes01 = 0x36A,
  SFX_Zerg_Hydra_ZHyYes02 = 0x36B,
  SFX_Zerg_Hydra_ZHyYes03 = 0x36C,
  SFX_Zerg_Ultra_ZUlRor00 = 0x36D,
  SFX_Zerg_Ultra_ZUlDth00 = 0x36E,
  SFX_Zerg_Ultra_ZUlPss00 = 0x36F,
  SFX_Zerg_Ultra_ZUlPss01 = 0x370,
  SFX_Zerg_Ultra_ZUlPss02 = 0x371,
  SFX_Zerg_Ultra_ZUlRdy00 = 0x372,
  SFX_Zerg_Ultra_ZUlWht00 = 0x373,
  SFX_Zerg_Ultra_ZUlWht01 = 0x374,
  SFX_Zerg_Ultra_ZUlWht02 = 0x375,
  SFX_Zerg_Ultra_ZUlWht03 = 0x376,
  SFX_Zerg_Ultra_ZUlYes00 = 0x377,
  SFX_Zerg_Ultra_ZUlYes01 = 0x378,
  SFX_Zerg_Ultra_ZUlYes02 = 0x379,
  SFX_Zerg_Ultra_ZUlYes03 = 0x37A,
  SFX_Zerg_Ultra_zulAtt00 = 0x37B,
  SFX_Zerg_Ultra_zulAtt01 = 0x37C,
  SFX_Zerg_Ultra_zulAtt02 = 0x37D,
  SFX_Zerg_Ultra_zulHit00 = 0x37E,
  SFX_Zerg_Ultra_zulHit01 = 0x37F,
  SFX_Zerg_Zergling_ZZeDth00 = 0x380,
  SFX_Zerg_Zergling_ZZePss00 = 0x381,
  SFX_Zerg_Zergling_ZZePss01 = 0x382,
  SFX_Zerg_Zergling_ZZePss02 = 0x383,
  SFX_Zerg_Zergling_ZZeRdy00 = 0x384,
  SFX_Zerg_Zergling_ZZeWht00 = 0x385,
  SFX_Zerg_Zergling_ZZeWht01 = 0x386,
  SFX_Zerg_Zergling_ZZeWht02 = 0x387,
  SFX_Zerg_Zergling_ZZeWht03 = 0x388,
  SFX_Zerg_Zergling_ZZeYes00 = 0x389,
  SFX_Zerg_Zergling_ZZeYes01 = 0x38A,
  SFX_Zerg_Zergling_ZZeYes02 = 0x38B,
  SFX_Zerg_Zergling_ZZeYes03 = 0x38C,
  SFX_Zerg_OVERLORD_ZOvRdy00 = 0x38D,
  SFX_Zerg_OVERLORD_ZOvDth00 = 0x38E,
  SFX_Zerg_OVERLORD_ZOvPss00 = 0x38F,
  SFX_Zerg_OVERLORD_ZOvWht00 = 0x390,
  SFX_Zerg_OVERLORD_ZOvWht01 = 0x391,
  SFX_Zerg_OVERLORD_ZOvWht02 = 0x392,
  SFX_Zerg_OVERLORD_ZOvWht03 = 0x393,
  SFX_Zerg_OVERLORD_ZOvYes00 = 0x394,
  SFX_Zerg_OVERLORD_ZOvYes01 = 0x395,
  SFX_Zerg_OVERLORD_ZOvYes02 = 0x396,
  SFX_Zerg_OVERLORD_ZOvYes03 = 0x397,
  SFX_Zerg_Queen_ZQuTag00 = 0x398,
  SFX_Zerg_Queen_ZQuTag01 = 0x399,
  SFX_Zerg_Queen_ZQuSwm00 = 0x39A,
  SFX_Zerg_Queen_ZQuSwm01 = 0x39B,
  SFX_Zerg_Queen_ZQuEns00 = 0x39C,
  SFX_Zerg_Queen_ZQuDth00 = 0x39D,
  SFX_Zerg_Queen_ZQuDth01 = 0x39E,
  SFX_Zerg_Queen_ZQuDth02 = 0x39F,
  SFX_Zerg_Queen_ZQuRdy00 = 0x3A0,
  SFX_Zerg_Queen_ZQuPss00 = 0x3A1,
  SFX_Zerg_Queen_ZQuPss01 = 0x3A2,
  SFX_Zerg_Queen_ZQuPss02 = 0x3A3,
  SFX_Zerg_Queen_ZQuPss03 = 0x3A4,
  SFX_Zerg_Queen_ZQuWht00 = 0x3A5,
  SFX_Zerg_Queen_ZQuWht01 = 0x3A6,
  SFX_Zerg_Queen_ZQuWht02 = 0x3A7,
  SFX_Zerg_Queen_ZQuWht03 = 0x3A8,
  SFX_Zerg_Queen_ZQuYes00 = 0x3A9,
  SFX_Zerg_Queen_ZQuYes01 = 0x3AA,
  SFX_Zerg_Queen_ZQuYes02 = 0x3AB,
  SFX_Zerg_Queen_ZQuYes03 = 0x3AC,
  SFX_Zerg_Mutalid_ZMuRdy00 = 0x3AD,
  SFX_Zerg_Mutalid_ZMuDth00 = 0x3AE,
  SFX_Zerg_Mutalid_ZMuPss00 = 0x3AF,
  SFX_Zerg_Mutalid_ZMuPss01 = 0x3B0,
  SFX_Zerg_Mutalid_ZMuPss02 = 0x3B1,
  SFX_Zerg_Mutalid_ZMuPss03 = 0x3B2,
  SFX_Zerg_Mutalid_ZMuWht00 = 0x3B3,
  SFX_Zerg_Mutalid_ZMuWht01 = 0x3B4,
  SFX_Zerg_Mutalid_ZMuWht02 = 0x3B5,
  SFX_Zerg_Mutalid_ZMuWht03 = 0x3B6,
  SFX_Zerg_Mutalid_ZMuYes00 = 0x3B7,
  SFX_Zerg_Mutalid_ZMuYes01 = 0x3B8,
  SFX_Zerg_Mutalid_ZMuYes02 = 0x3B9,
  SFX_Zerg_Mutalid_ZMuYes03 = 0x3BA,
  SFX_Zerg_ZERGKERRI_UKiPss00 = 0x3BB,
  SFX_Zerg_ZERGKERRI_UKiPss01 = 0x3BC,
  SFX_Zerg_ZERGKERRI_UKiPss02 = 0x3BD,
  SFX_Zerg_ZERGKERRI_UKiPss03 = 0x3BE,
  SFX_Zerg_ZERGKERRI_UKiWht00 = 0x3BF,
  SFX_Zerg_ZERGKERRI_UKiWht01 = 0x3C0,
  SFX_Zerg_ZERGKERRI_UKiWht02 = 0x3C1,
  SFX_Zerg_ZERGKERRI_UKiWht03 = 0x3C2,
  SFX_Zerg_ZERGKERRI_UKiYes00 = 0x3C3,
  SFX_Zerg_ZERGKERRI_UKiYes01 = 0x3C4,
  SFX_Zerg_ZERGKERRI_UKiYes02 = 0x3C5,
  SFX_Zerg_ZERGKERRI_UKiYes03 = 0x3C6,
  SFX_Zerg_ZERGKERRI_UKiDth00 = 0x3C7,
  SFX_Misc_CRITTERS_PbWht01 = 0x3C8,
  SFX_Misc_CRITTERS_PbWht02 = 0x3C9,
  SFX_Misc_CRITTERS_PbWht03 = 0x3CA,
  SFX_Misc_CRITTERS_PBDeath01 = 0x3CB,
  SFX_Misc_CRITTERS_ScWht01 = 0x3CC,
  SFX_Misc_CRITTERS_ScWht02 = 0x3CD,
  SFX_Misc_CRITTERS_ScWht03 = 0x3CE,
  SFX_Misc_CRITTERS_ScDeath01 = 0x3CF,
  SFX_Misc_CRITTERS_TerWht01 = 0x3D0,
  SFX_Misc_CRITTERS_TerWht02 = 0x3D1,
  SFX_Misc_CRITTERS_TerWht03 = 0x3D2,
  SFX_Misc_CRITTERS_TerDeath01 = 0x3D3,
  SFX_Terran_Duran_TDnPss00 = 0x3D4,
  SFX_Terran_Duran_TDnPss01 = 0x3D5,
  SFX_Terran_Duran_TDnPss02 = 0x3D6,
  SFX_Terran_Duran_TDnPss03 = 0x3D7,
  SFX_Terran_Duran_TDnPss04 = 0x3D8,
  SFX_Terran_Duran_TDnPss05 = 0x3D9,
  SFX_Terran_Duran_TDnPss06 = 0x3DA,
  SFX_Terran_Duran_TDnPss07 = 0x3DB,
  SFX_Terran_Duran_TDnPss08 = 0x3DC,
  SFX_Terran_Duran_TDnWht00 = 0x3DD,
  SFX_Terran_Duran_TDnWht01 = 0x3DE,
  SFX_Terran_Duran_TDnWht02 = 0x3DF,
  SFX_Terran_Duran_TDnWht03 = 0x3E0,
  SFX_Terran_Duran_TDnYes00 = 0x3E1,
  SFX_Terran_Duran_TDnYes01 = 0x3E2,
  SFX_Terran_Duran_TDnYes02 = 0x3E3,
  SFX_Terran_Duran_TDnYes03 = 0x3E4,
  SFX_Terran_Duran_TDnDth00 = 0x3E5,
  SFX_Terran_Medic_TmedCure = 0x3E6,
  SFX_Terran_Medic_TMdRdy00 = 0x3E7,
  SFX_Terran_Medic_TMdDth00 = 0x3E8,
  SFX_Terran_Medic_TMdWht00 = 0x3E9,
  SFX_Terran_Medic_TMdWht01 = 0x3EA,
  SFX_Terran_Medic_TMdWht02 = 0x3EB,
  SFX_Terran_Medic_TMdWht03 = 0x3EC,
  SFX_Terran_Medic_TMdYes00 = 0x3ED,
  SFX_Terran_Medic_TMdYes01 = 0x3EE,
  SFX_Terran_Medic_TMdYes02 = 0x3EF,
  SFX_Terran_Medic_TMdYes03 = 0x3F0,
  SFX_Terran_Medic_TMdPss00 = 0x3F1,
  SFX_Terran_Medic_TMdPss01 = 0x3F2,
  SFX_Terran_Medic_TMdPss02 = 0x3F3,
  SFX_Terran_Medic_TMdPss03 = 0x3F4,
  SFX_Terran_Medic_TMdPss04 = 0x3F5,
  SFX_Terran_Medic_TMdPss05 = 0x3F6,
  SFX_Terran_Medic_TMdPss06 = 0x3F7,
  SFX_Terran_Medic_TMedflsh = 0x3F8,
  SFX_Terran_Medic_TMedRest = 0x3F9,
  SFX_Terran_Medic_TMedHeal = 0x3FA,
  SFX_Terran_Medic_Miopia1 = 0x3FB,
  SFX_Terran_Frigate_AfterOn = 0x3FC,
  SFX_Terran_Frigate_AfterOff = 0x3FD,
  SFX_Bullet_Tfrhit = 0x3FE,
  SFX_Bullet_Tfrshoot = 0x3FF,
  SFX_Terran_FRIGATE_TVkRdy00 = 0x400,
  SFX_Terran_FRIGATE_TVkPss00 = 0x401,
  SFX_Terran_FRIGATE_TVkPss01 = 0x402,
  SFX_Terran_FRIGATE_TVkPss02 = 0x403,
  SFX_Terran_FRIGATE_TVkPss03 = 0x404,
  SFX_Terran_FRIGATE_TVkPss04 = 0x405,
  SFX_Terran_FRIGATE_TVkPss05 = 0x406,
  SFX_Terran_FRIGATE_TVkWht00 = 0x407,
  SFX_Terran_FRIGATE_TVkWht01 = 0x408,
  SFX_Terran_FRIGATE_TVkWht02 = 0x409,
  SFX_Terran_FRIGATE_TVkWht03 = 0x40A,
  SFX_Terran_FRIGATE_TVkYes00 = 0x40B,
  SFX_Terran_FRIGATE_TVkYes01 = 0x40C,
  SFX_Terran_FRIGATE_TVkYes02 = 0x40D,
  SFX_Terran_FRIGATE_TVkYes03 = 0x40E,
  SFX_Terran_FRIGATE_TVkYes04 = 0x40F,
  SFX_Terran_FRIGATE_TVkDth00 = 0x410,
  SFX_Protoss_Corsair_PCoRdy00 = 0x411,
  SFX_Protoss_Corsair_PCorhit1 = 0x412,
  SFX_Protoss_Corsair_PCorhit2 = 0x413,
  SFX_Protoss_Corsair_PCoWht00 = 0x414,
  SFX_Protoss_Corsair_PCoWht01 = 0x415,
  SFX_Protoss_Corsair_PCoWht02 = 0x416,
  SFX_Protoss_Corsair_PCoWht03 = 0x417,
  SFX_Protoss_Corsair_PCoYes00 = 0x418,
  SFX_Protoss_Corsair_PCoYes01 = 0x419,
  SFX_Protoss_Corsair_PCoYes02 = 0x41A,
  SFX_Protoss_Corsair_PCoYes03 = 0x41B,
  SFX_Protoss_Corsair_PCoPss00 = 0x41C,
  SFX_Protoss_Corsair_PCoPss01 = 0x41D,
  SFX_Protoss_Corsair_PCoPss02 = 0x41E,
  SFX_Protoss_Corsair_PCoPss03 = 0x41F,
  SFX_Protoss_Corsair_PCoPss04 = 0x420,
  SFX_Protoss_Corsair_PCoPss05 = 0x421,
  SFX_Protoss_Corsair_PCoPss06 = 0x422,
  SFX_Protoss_Corsair_PCorWeb1 = 0x423,
  SFX_Protoss_Corsair_PCorlasr2 = 0x424,
  SFX_Protoss_Darchon_feedback = 0x425,
  SFX_Protoss_Darchon_mind = 0x426,
  SFX_Protoss_Darchon_ParaAttk = 0x427,
  SFX_Protoss_Darchon_Parahit = 0x428,
  SFX_Protoss_DARCHON_PDaRdy00 = 0x429,
  SFX_Protoss_DARCHON_PDrDth00 = 0x42A,
  SFX_Protoss_DARCHON_PDaPss00 = 0x42B,
  SFX_Protoss_DARCHON_PDaPss01 = 0x42C,
  SFX_Protoss_DARCHON_PDaPss02 = 0x42D,
  SFX_Protoss_DARCHON_PDaPss03 = 0x42E,
  SFX_Protoss_DARCHON_PDaWht00 = 0x42F,
  SFX_Protoss_DARCHON_PDaWht01 = 0x430,
  SFX_Protoss_DARCHON_PDaWht02 = 0x431,
  SFX_Protoss_DARCHON_PDaWht03 = 0x432,
  SFX_Protoss_DARCHON_PDaYes00 = 0x433,
  SFX_Protoss_DARCHON_PDaYes01 = 0x434,
  SFX_Protoss_DARCHON_PDaYes02 = 0x435,
  SFX_Protoss_DARCHON_PDaYes03 = 0x436,
  SFX_Zerg_LURKER_ZLuRdy00 = 0x437,
  SFX_Zerg_LURKER_ZLuDth00 = 0x438,
  SFX_Zerg_LURKER_ZLuPss00 = 0x439,
  SFX_Zerg_Lurker_ZLuPss01 = 0x43A,
  SFX_Zerg_Lurker_ZLuPss02 = 0x43B,
  SFX_Zerg_LURKER_ZLuPss03 = 0x43C,
  SFX_Zerg_LURKER_ZLuWht00 = 0x43D,
  SFX_Zerg_LURKER_ZLuWht01 = 0x43E,
  SFX_Zerg_LURKER_ZLuWht02 = 0x43F,
  SFX_Zerg_LURKER_ZLuWht03 = 0x440,
  SFX_Zerg_LURKER_ZLuYes00 = 0x441,
  SFX_Zerg_LURKER_ZLuYes01 = 0x442,
  SFX_Zerg_LURKER_ZLuYes02 = 0x443,
  SFX_Zerg_LURKER_ZLuYes03 = 0x444,
  SFX_Zerg_LURKER_ZLuBurrw = 0x445,
  SFX_Zerg_Devourer_firesuck = 0x446,
  SFX_Zerg_Devourer_goophit = 0x447,
  SFX_Zerg_Devourer_ZDvRdy00 = 0x448,
  SFX_Zerg_Devourer_ZDvDth00 = 0x449,
  SFX_Zerg_Devourer_ZDvPss00 = 0x44A,
  SFX_Zerg_Devourer_ZDvPss01 = 0x44B,
  SFX_Zerg_Devourer_ZDvPss02 = 0x44C,
  SFX_Zerg_Devourer_ZDvWht00 = 0x44D,
  SFX_Zerg_Devourer_ZDvWht01 = 0x44E,
  SFX_Zerg_Devourer_ZDvWht02 = 0x44F,
  SFX_Zerg_Devourer_ZDvYes00 = 0x450,
  SFX_Zerg_Devourer_ZDvYes01 = 0x451,
  SFX_Zerg_Devourer_ZDvYes02 = 0x452,
  SFX_Zerg_Devourer_ZDvYes03 = 0x453,
  SFX_Bullet_ZLrkFir1 = 0x454,
  SFX_Bullet_ZLrkFir2 = 0x455,
  SFX_Bullet_ZLrkHit1 = 0x456,
  SFX_Bullet_ZLrkHit2 = 0x457,
  SFX_Zerg_ZergDuran_ZDnPss00 = 0x458,
  SFX_Zerg_ZergDuran_ZDnPss01 = 0x459,
  SFX_Zerg_ZergDuran_ZDnPss02 = 0x45A,
  SFX_Zerg_ZergDuran_ZDnPss03 = 0x45B,
  SFX_Zerg_ZergDuran_ZDnPss04 = 0x45C,
  SFX_Zerg_ZergDuran_ZDnPss05 = 0x45D,
  SFX_Zerg_ZergDuran_ZDnPss06 = 0x45E,
  SFX_Zerg_ZergDuran_ZDnPss07 = 0x45F,
  SFX_Zerg_ZergDuran_ZDnPss08 = 0x460,
  SFX_Zerg_ZergDuran_ZDnWht00 = 0x461,
  SFX_Zerg_ZergDuran_ZDnWht01 = 0x462,
  SFX_Zerg_ZergDuran_ZDnWht02 = 0x463,
  SFX_Zerg_ZergDuran_ZDnWht03 = 0x464,
  SFX_Zerg_ZergDuran_ZDnYes00 = 0x465,
  SFX_Zerg_ZergDuran_ZDnYes01 = 0x466,
  SFX_Zerg_ZergDuran_ZDnYes02 = 0x467,
  SFX_Zerg_ZergDuran_ZDnYes03 = 0x468,
  SFX_Zerg_ZergDuran_ZDnDth00 = 0x469,
  SFX_Protoss_Artanis_PAtPss00 = 0x46A,
  SFX_Protoss_Artanis_PAtPss01 = 0x46B,
  SFX_Protoss_Artanis_PAtPss02 = 0x46C,
  SFX_Protoss_Artanis_PAtPss03 = 0x46D,
  SFX_Protoss_Artanis_PAtPss04 = 0x46E,
  SFX_Protoss_Artanis_PAtPss05 = 0x46F,
  SFX_Protoss_Artanis_PAtWht00 = 0x470,
  SFX_Protoss_Artanis_PAtWht01 = 0x471,
  SFX_Protoss_Artanis_PAtWht02 = 0x472,
  SFX_Protoss_Artanis_PAtWht03 = 0x473,
  SFX_Protoss_Artanis_PAtYes00 = 0x474,
  SFX_Protoss_Artanis_PAtYes01 = 0x475,
  SFX_Protoss_Artanis_PAtYes02 = 0x476,
  SFX_Protoss_Artanis_PAtYes03 = 0x477,
  SFX_MAX = 0x478,
};

enum FogOfWar : unsigned __int8
{
  FOW_OFF = 0x0,
  FOW_WARCRAFT1 = 0x1,
  FOW_ON = 0x2,
};

enum GameType : __int8
{
  GT_Melee = 0x2,
  GT_FreeForAll = 0x3,
  GT_OneOnOne = 0x4,
  GT_CaptureTheFlag = 0x5,
  GT_Greed = 0x6,
  GT_Slaughter = 0x7,
  GT_SuddenDeath = 0x8,
  GT_Ladder = 0x9,
  GT_UseMapSettings = 0xA,
  GT_TeamMelee = 0xB,
  GT_TeamFreeForAll = 0xC,
  GT_TeamCTF = 0xD,
  GT_TopVsBottom = 0xF,
};

enum PlayerTypes : unsigned __int8
{
  PT_NO_SINGLE = 0x0,
  PT_NO_COMPUTERS = 0x1,
  PT_NO_SINGLE_WITH_COMPUTERS = 0x2,
  PT_SINGLE_NO_COMPUTERS = 0x3,
  PT_SINGLE_WITH_COMPUTERS = 0x4,
};

enum ResourceType : unsigned __int8
{
  RT_MAP_DEFALUT = 0x0,
  RT_FIXED_VALUE = 0x1,
  RT_LOW = 0x2,
  RT_MEDIUM = 0x3,
  RT_HIGH = 0x4,
  RT_INCOME = 0x5,
};

enum StartingPositions : unsigned __int8
{
  SP_RANDOM = 0x0,
  SP_FIXED = 0x1,
};

enum StartingUnits : unsigned __int8
{
  SU_MAP_DEFAULT = 0x0,
  SU_WORKER_ONLY = 0x1,
  SU_WORKER_AND_CENTER = 0x2,
};

enum UnitStats : unsigned __int8
{
  US_MAP_DEFALUT = 0x0,
  US_STANDARD = 0x1,
};

enum VictoryConditions : unsigned __int8
{
  VC_MAP_DEFAULT = 0x0,
  VC_MELEE = 0x1,
  VC_HIGH_SCORE = 0x2,
  VC_RESOURCES = 0x3,
  VC_CAPTURE_THE_FLAG = 0x4,
  VC_SUSDDEN_DEATH = 0x5,
  VC_SLAUGHTER = 0x6,
  VC_ONE_ON_ONE = 0x7,
};

#pragma pack(push, 1)
struct struct_5
{
  int sound_buffer_bytes;
  int anonymous_1;
  int anonymous_2;
  IDirectSoundBuffer *sound_buffer;
};
#pragma pack(pop)
static_assert(sizeof(struct_5) == 16, "Incorrect size for type `struct_5`. Expected: 16");

typedef void (__fastcall *InputProcedure)(dlgEvent *);

typedef void *(__fastcall *FastFileRead)(char *filename, int default_value, int *bytes_read, char *, int);

typedef void (__stdcall *TemplateConstructor)(char *, char *, GotFileValues *, struct_a4 *);

#pragma pack(push, 1)
struct __declspec(align(2)) AudioVideoInitializationError
{
  _DWORD error_code;
  _DWORD dword4;
  const char *function_name;
};
#pragma pack(pop)
static_assert(sizeof(AudioVideoInitializationError) == 12, "Incorrect size for type `AudioVideoInitializationError`. Expected: 12");

typedef tagPALETTEENTRY PALETTEENTRY;

struct __declspec(align(4)) CharacterData
{
  int char0;
  _BYTE has_ophelia_cheat;
  _BYTE gap5[3];
  _DWORD last_access_time;
  char player_name[24];
  int gap0;
  int unlocked_campaign_mission[3];
  int unlocked_expcampaign_mission[3];
  _BYTE more_data[48];
};
static_assert(sizeof(CharacterData) == 112, "Incorrect size for type `CharacterData`. Expected: 112");

enum ForceFlags : unsigned __int8
{
  RANDOM_START_LOCATION = 0x1,
  ALLIES = 0x2,
  ALLIED_VICTORY = 0x4,
  SHARED_VISION = 0x8,
};

struct ChunkListItem
{
  ChunkData *next;
  ChunkData *previous;
};
static_assert(sizeof(ChunkListItem) == 8, "Incorrect size for type `ChunkListItem`. Expected: 8");

struct Char4_characters
{
  char _0;
  char _1;
  char _2;
  char _3;
};
static_assert(sizeof(Char4_characters) == 4, "Incorrect size for type `Char4_characters`. Expected: 4");

enum SaiAccessabilityFlags : __int16
{
  SAF_UNK = 0x2,
  SAF_HighGround = 0x1FF9,
  SAF_LowGround = 0x1FFB,
  SAF_Inaccessible = 0x1FFD,
};

union SaiRegionUser
{
  void *node;
  int relation;
};
static_assert(sizeof(SaiRegionUser) == 4, "Incorrect size for type `SaiRegionUser`. Expected: 4");

struct __declspec(align(4)) EstablishingShotPosition
{
  int index;
  const char *tag;
};
static_assert(sizeof(EstablishingShotPosition) == 8, "Incorrect size for type `EstablishingShotPosition`. Expected: 8");

enum GluAllTblEntry : __int16
{
  STAR_EDIT_NOT_FOUND = 0x0,
  SPAWNED_BY = 0x1,
  PRINT_FORMAT_S = 0x2,
  UNITS = 0x3,
  STRUCTURES = 0x4,
  RESOURCES = 0x5,
  PRODUCED = 0x6,
  KILLED = 0x7,
  LOST = 0x8,
  CONSTRUCTED = 0x9,
  RAZED = 0xA,
  LOST2 = 0xB,
  GAS_MINED = 0xC,
  MINTERALS_MINED = 0xD,
  TOTAL_SPENT = 0xE,
  ELAPSED_TIME = 0xF,
  DISCONNECTED = 0x13,
  UNDECIDED = 0x14,
  DELETE_SAVE_GAME = 0x15,
  ENTER_CHARACTER_NAME = 0x16,
  DELETE_CHARACTER = 0x17,
  NAME_EXISTS = 0x18,
  PLEASE_DELETE_CHARACTER = 0x19,
  CHARACTER_CREATION_ERROR = 0x1A,
  UP_ONE_LEVEL = 0x1B,
  SAVE = 0x1C,
  PRINT_FORMAT_U = 0x1D,
  NUMBER_OF_PLAYERS = 0x1E,
  PRINT_FORMAT_UX_U = 0x1F,
  MAP_SIZE = 0x20,
  TILESET = 0x21,
  PRINT_FORMAT_U2 = 0x22,
  COMPUTER_SLOTS = 0x23,
  PRINT_FORMAT_U3 = 0x24,
  HUMAN_SLOTS = 0x25,
  BADLANDS = 0x26,
  SPACE = 0x27,
  INSTALLATION = 0x28,
  ASHWORLD = 0x29,
  JUNGLE = 0x2A,
  DESERT = 0x2B,
  ICE = 0x2C,
  TWILIGHT = 0x2D,
  SAVED_GAME = 0x2E,
  MAP_NAME = 0x2F,
  UNABLE_TO_LOAD_MAP = 0x30,
  SELECT_LADDER_MAP = 0x31,
  SELECTED_SCENARIO_NOT_VALID = 0x32,
  ERROR_CREATING_GAME = 0x33,
  NETWORK_GAME_EXISTS = 0x34,
  UNSUPPORTED_GAME_TYPE_OPTIONS = 0x35,
  TOO_MANY_GAMES_ON_NETWORK = 0x36,
  GAME_NETWORK_INITIALIZATION_ERROR = 0x37,
  OPPONENT_PLAYER_REQUIRED = 0x38,
  HUMAN_PLAYER_REQUIRED = 0x39,
  UMS_ONLY = 0x3A,
  EXPANSION_ONLY = 0x3B,
  INVALID_SCENARIO = 0x3C,
  SCENARIO_FILENAME_TOO_LONG = 0x3D,
  SCENARIO_INVALID_OR_CORRUPTED = 0x3E,
  INVALID_SAVE_GAME = 0x3F,
};

enum MapDirEntryFlags : unsigned __int8
{
  MDEF_SAVEGAME = 0x1,
  MDEF_2 = 0x2,
  MDEF_SCENARIO = 0x4,
  MDEF_DIRECTORY = 0x8,
  MDEF_10 = 0x10,
  MDEF_20 = 0x20,
  MDEF_40 = 0x40,
  MDEF_REPLAY = 0x80,
};

struct Timer
{
  struct Timer *next;
  dialog *parent;
  void (__fastcall *callback)(dialog *dlg, __int16 timer_id);
  int start_tick;
  int finish_tick;
  __int16 id;
  int unknown;
};
static_assert(sizeof(Timer) == 28, "Incorrect size for type `Timer`. Expected: 28");

struct __declspec(align(2)) struc_66FE20
{
  char field_0;
  char field_1;
  __int16 field_2;
  __int16 field_3;
  __int16 field_4;
  char player_name[26];
};
static_assert(sizeof(struc_66FE20) == 34, "Incorrect size for type `struc_66FE20`. Expected: 34");

struct MapDownload
{
  int u1;
  int u2;
  int net_players;
  int map_size;
  int checksum;
  int u3;
  char map_path[260];
  char unknown[4];
  int u4;
  int u5;
};
static_assert(sizeof(MapDownload) == 296, "Incorrect size for type `MapDownload`. Expected: 296");

struct __declspec(align(4)) GameActionDataBlock
{
  int field1;
  int field2;
  void *net_record_buffer;
  int field4;
  int net_record_size;
  int field6;
  int field7;
  void *field8;
};
static_assert(sizeof(GameActionDataBlock) == 32, "Incorrect size for type `GameActionDataBlock`. Expected: 32");

struct _RTTIClassHierarchyDescriptor
{
  int signature;
  int attributes;
  int numBaseClasses;
  void *baseClassArray;
};
static_assert(sizeof(_RTTIClassHierarchyDescriptor) == 16, "Incorrect size for type `_RTTIClassHierarchyDescriptor`. Expected: 16");

struct _RTTICompleteObjectLocator
{
  int signature;
  int offset;
  int cdOffset;
  void *typeDescriptor;
  void *classDescriptor;
};
static_assert(sizeof(_RTTICompleteObjectLocator) == 20, "Incorrect size for type `_RTTICompleteObjectLocator`. Expected: 20");

struct struct_v6
{
  _DWORD dword0;
  _DWORD dword4;
  _BYTE byte8;
  _BYTE byte9;
  _BYTE byteA;
  _BYTE byteB;
  _DWORD dwordC;
};
static_assert(sizeof(struct_v6) == 16, "Incorrect size for type `struct_v6`. Expected: 16");

struct ListNode
{
  ListNode *previous;
  ListNode *next;
};
static_assert(sizeof(ListNode) == 8, "Incorrect size for type `ListNode`. Expected: 8");

struct STREAMED
{
  STREAMED *previous_maybe;
  STREAMED *next_maybe;
  HANDLE pvoid8;
};
static_assert(sizeof(STREAMED) == 12, "Incorrect size for type `STREAMED`. Expected: 12");

enum Cinematic : __int8
{
  C_BLIZZARD_LOGO = 0x0,
  C_INTRO = 0x1,
  C_WASTELAND_PATROL_INTRO = 0x2,
  C_THE_DOWNING_OF_NORAD_II_INTRO = 0x3,
  C_OPEN_REBELION_INTRO = 0x4,
  C_THE_INAUGURATION_INTRO = 0x5,
  C_WASTELAND_PATROL = 0x6,
  C_THE_DOWNING_OF_NORAD_II = 0x7,
  C_OPEN_REBELION = 0x8,
  C_THE_INAUGURATION = 0x9,
  C_BATTLE_ON_THE_AMERIGO_INTRO = 0xA,
  C_THE_WARP_INTRO = 0xB,
  C_THE_INVASION_OF_AIUR_INTRO = 0xC,
  C_THE_DREAM = 0xD,
  C_BATTLE_ON_THE_AMERIGO = 0xE,
  C_THE_WARP = 0xF,
  C_THE_INVASION_OF_AIUR = 0x10,
  C_THE_FALL_OF_FENIX_INTRO = 0x11,
  C_THE_AMBUSH_INTRO = 0x12,
  C_THE_RETURN_TO_AIUR_INTRO = 0x13,
  C_THE_FALL_OF_FENIX = 0x14,
  C_THE_AMBUSH = 0x15,
  C_THE_RETURN_TO_AIUR = 0x16,
  C_THE_DEATH_OF_THE_OVERMIND = 0x17,
  C_EXPANSION_INTRO = 0x18,
  C_FURY_OF_THE_XEL_NAGA = 0x19,
  C_UED_VICTORY_REPORT = 0x1A,
  C_THE_ASCENTION = 0x1B,
  C_NONE = 0x19,
};

struct struct_0
{
  void *field_0;
  int field_4;
};
static_assert(sizeof(struct_0) == 8, "Incorrect size for type `struct_0`. Expected: 8");

struct __declspec(align(4)) ChunkNode
{
  ChunkNode *count;
  ChunkData *f1;
  ChunkData *f2;
};
static_assert(sizeof(ChunkNode) == 12, "Incorrect size for type `ChunkNode`. Expected: 12");

enum UnitGroupFlags : unsigned __int16
{
  CLOAK = 0x1,
  BURROW = 0x2,
  IN_TRANSIT = 0x4,
  HALLUCIONATED = 0x8,
  INVINCIBLE = 0x10,
};

struct __declspec(align(4)) UnitRelated20
{
  _DWORD unit_id;
  _DWORD dword4;
  CUnit *unit;
  UnitRelated20 *dwordC;
  UnitRelated20 *dword10;
};
static_assert(sizeof(UnitRelated20) == 20, "Incorrect size for type `UnitRelated20`. Expected: 20");

struct __declspec(align(4)) struct_v4
{
  int int0;
  _DWORD dword4;
  _DWORD dword8;
  _DWORD dwordC;
  _DWORD dword10;
  _BYTE gap14[16];
  _DWORD dword24;
  _DWORD dword28;
  _DWORD dword2C;
  _DWORD dword30;
};
static_assert(sizeof(struct_v4) == 52, "Incorrect size for type `struct_v4`. Expected: 52");

enum EndgameState : unsigned __int8
{
  INITIAL = 0x0,
  DEFEAT = 0x2,
  VICTORY = 0x3,
  UNKNOWN = 0x4,
  DRAW = 0x5,
};

enum MusicTrack
{
  MT_NONE = 0x0,
  MT_ZERG1 = 0x1,
  MT_ZERG2 = 0x2,
  MT_ZERG3 = 0x3,
  MT_TERRAN1 = 0x4,
  MT_TERRAN2 = 0x5,
  MT_TERRAN3 = 0x6,
  MT_PROTOSS1 = 0x7,
  MT_PROTOSS2 = 0x8,
  MT_PROTOSS3 = 0x9,
  MT_ZERG_READY = 0xA,
  MT_TERRAN_READY = 0xB,
  MT_PROTOSS_READY = 0xC,
  MT_ZERG_DEFEAT = 0xD,
  MT_TERRAN_DEFEAT = 0xE,
  MT_PROTOSS_DEFEAT = 0xF,
  MT_ZERG_VICTORY = 0x10,
  MT_TERRAN_VICTORY = 0x11,
  MT_PROTOSS_VICTORY = 0x12,
  MT_ZERG_VICTORY2 = 0x13,
  MT_TERRAN_VICTORY2 = 0x14,
  MT_PROTOSS_VICTORY2 = 0x15,
  MT_TITLE = 0x16,
  MT_TERRAN2_DUPLICATE = 0x17,
  MT_RADIO_FREE_ZERG = 0x18,
};

enum MusicTrackType : unsigned __int8
{
  NONE = 0x0,
  MENU_MUSIC = 0x1,
  IN_GAME_MUSIC = 0x2,
};

struct __declspec(align(4)) TriggerOrderRelated
{
  int player;
  _DWORD unit_type;
  _WORD location_flags;
  _DWORD x;
  _DWORD y;
  _DWORD dword14;
};
static_assert(sizeof(TriggerOrderRelated) == 24, "Incorrect size for type `TriggerOrderRelated`. Expected: 24");

enum RegistryOptionsFlags
{
  F_UNIT_SPEECH_ON = 0x1,
  F_UNIT_NOISE_ON = 0x2,
  F_BLDG_NOISE_ON = 0x4,
  F_SFX_FLAGS = 0x7,
  F_TIPS_ON = 0x100,
  F_PLAY_INTRO = 0x200,
  F_TRIG_TEXT = 0x400,
  F_PLAY_XINTRO = 0x800,
  F_CHAT_COLORS_ON = 0x1000,
};

typedef int (__fastcall *BriefingAction)(Action *action, int arg2);

struct __declspec(align(4)) BNetGateways
{
  _BYTE byte0;
  _DWORD dword4;
  _DWORD gateway_count;
  _DWORD dwordC;
  const char *configuration;
  _DWORD configuration_length;
  _DWORD dword18;
  _DWORD dword1C;
};
static_assert(sizeof(BNetGateways) == 32, "Incorrect size for type `BNetGateways`. Expected: 32");

struct __declspec(align(8)) FontColorRelated
{
  int cyan;
  int field_4;
  int yellow;
  int field_C;
  char white[1];
  char field_11[1];
  char field_12[1];
  char field_13[1];
  char field_14[4];
  int grey;
  int field_1C;
  float red;
  int field_24;
  double green;
  __int64 field_30;
  __int64 field_38;
  __int64 red2;
  __int64 blue;
  __int64 teal;
  __int64 purple;
  __int64 orange;
  __int64 brown;
  __int64 white2;
  __int64 yellow2;
  __int64 green2;
  __int64 bright_yellow;
  __int64 pinkish;
  __int64 dark_cyan;
  __int64 gray_green;
  __int64 bluegray;
  __int64 field_B0;
  __int64 turquiose;
};
static_assert(sizeof(FontColorRelated) == 192, "Incorrect size for type `FontColorRelated`. Expected: 192");

enum Color : unsigned __int8
{
  COLOR_CYAN1 = 0x1,
  COLOR_CYAN2 = 0x2,
  COLOR_YELLOW = 0x3,
  COLOR_WHITE = 0x4,
  COLOR_GREY = 0x5,
  COLOR_RED = 0x6,
  COLOR_GREEN = 0x7,
  COLOR_PLAYER1 = 0x8,
  COLOR_UNK1 = 0x9,
  COLOR_UNK2 = 0xA,
  COLOR_INVISIBLE1 = 0xB,
  COLOR_TRUNCATE = 0xC,
  COLOR_UNK3 = 0xD,
  COLOR_PLAYER2 = 0xE,
  COLOR_PLAYER3 = 0xF,
  COLOR_PLAYER4 = 0x10,
  COLOR_PLAYER5 = 0x11,
  COLOR_UNK4 = 0x12,
  COLOR_UNK5 = 0x13,
  COLOR_INVISIBLE2 = 0x14,
  COLOR_PLAYER6 = 0x15,
  COLOR_PLAYER7 = 0x16,
  COLOR_PLAYER8 = 0x17,
  COLOR_PLAYER9 = 0x18,
  COLOR_PLAYER10 = 0x19,
  COLOR_CYAN = 0x1A,
  COLOR_PLAYER11 = 0x1B,
  COLOR_PLAYER12 = 0x1C,
  COLOR_GREYGREEN = 0x1D,
  COLOR_BLUEGREY = 0x1E,
  COLOR_TURQUIOSE = 0x1F,
  COLOR_UNK6 = 0x20,
};

struct DlgGrp
{
  _BYTE gap0[268];
  _DWORD dword10C;
  _WORD word110;
  _WORD word112;
  _WORD word114;
  _WORD word116;
  _WORD word118;
  _WORD word11A;
  _WORD word11C;
  _WORD word11E;
  _WORD word120;
  _WORD word122;
  _WORD word124;
  _WORD word126;
  _WORD word128;
  _WORD word12A;
  _WORD word12C;
  _WORD word12E;
  _WORD word130;
  _WORD word132;
  _WORD word134;
  _WORD word136;
  _WORD word138;
  _WORD word13A;
  _WORD word13C;
  _WORD word13E;
  _WORD word140;
  _WORD word142;
  _WORD word144;
  _WORD word146;
  _WORD word148;
  _WORD word14A;
};
static_assert(sizeof(DlgGrp) == 332, "Incorrect size for type `DlgGrp`. Expected: 332");

struct SNETVERSIONDATA
{
  unsigned int size;
  char *versionstring;
  char *executablefile;
  char *originalarchivefile;
  char *patcharchivefile;
};
static_assert(sizeof(SNETVERSIONDATA) == 20, "Incorrect size for type `SNETVERSIONDATA`. Expected: 20");

enum ButtonState
{
  BTNST_DISABLED = 0xFFFFFFFF,
  BTNST_HIDDEN = 0x0,
  BTNST_ENABLED = 0x1,
};

enum Icon : unsigned __int16
{
  ICON_MARINE = 0x0,
  ICON_GHOST = 0x1,
  ICON_VULTURE = 0x2,
  ICON_GOLIATH = 0x3,
  ICON_BLANK_GOLIATH_TURRET = 0x4,
  ICON_SIEGE_TANK_TANK_MODE = 0x5,
  ICON_BLANK_TANK_TURRET = 0x6,
  ICON_SCV = 0x7,
  ICON_WRAITH = 0x8,
  ICON_SCIENCE_VESSEL = 0x9,
  ICON_GUI_MONTAG_FIREBAT = 0xA,
  ICON_DROPSHIP = 0xB,
  ICON_BATTLECRUISER = 0xC,
  ICON_VULTURE_SPIDER_MINE = 0xD,
  ICON_NUCLEAR_MISSILE = 0xE,
  ICON_CIVILIAN = 0xF,
  ICON_SARAH_KERRIGAN_GHOST = 0x10,
  ICON_ALAN_SCHEZAR_GOLIATH = 0x11,
  ICON_BLANK_ALAN_TURRET = 0x12,
  ICON_JIM_RAYNOR_VULTURE = 0x13,
  ICON_JIM_RAYNOR_MARINE = 0x14,
  ICON_TOM_KAZANSKY_WRAITH = 0x15,
  ICON_MAGELLAN_SCIENCE_VESSEL = 0x16,
  ICON_EDMUND_DUKE_SIEGE_TANK = 0x17,
  ICON_BLANK_DUKE_TURRET_TANK = 0x18,
  ICON_EDMUND_DUKE_SIEGE_MODE = 0x19,
  ICON_BLANK_DUKE_TURRET_SIEGE = 0x1A,
  ICON_BLANK_ARCTURUS_MENGSK = 0x1B,
  ICON_HYPERION_BATTLECRUISER = 0x1C,
  ICON_NORAD_II_BATTLECRUISER = 0x1D,
  ICON_TERRAN_SIEGE_TANK_SIEGE_MODE = 0x1E,
  ICON_BLANK_SIEGE_TANK_TURRET = 0x1F,
  ICON_FIREBAT = 0x20,
  ICON_MARINE_SCANNER_SWEEP = 0x21,
  ICON_MEDIC = 0x22,
  ICON_LARVA = 0x23,
  ICON_RADIOACTIVE_ZERG_EGG = 0x24,
  ICON_ZERGLING = 0x25,
  ICON_HYDRALISK = 0x26,
  ICON_ULTRALISK = 0x27,
  ICON_BROODLING = 0x28,
  ICON_DRONE = 0x29,
  ICON_OVERLORD = 0x2A,
  ICON_MUTALISK = 0x2B,
  ICON_GUARDIAN = 0x2C,
  ICON_QUEEN = 0x2D,
  ICON_DEFILER = 0x2E,
  ICON_SCOURGE = 0x2F,
  ICON_TORRARSQUE_ULTRALISK = 0x30,
  ICON_MATRIARCH_QUEEN = 0x31,
  ICON_INFESTED_TERRAN = 0x32,
  ICON_INFESTED_KERRIGAN_INFESTED_TERRAN = 0x33,
  ICON_UNCLEAN_ONE_DEFILER = 0x34,
  ICON_HUNTER_KILLER_HYDRALISK = 0x35,
  ICON_DEVOURING_ONE_ZERGLING = 0x36,
  ICON_KUKULZA_MUTALISK = 0x37,
  ICON_KUKULZA_GUARDIAN = 0x38,
  ICON_YGGDRASILL_OVERLORD = 0x39,
  ICON_VALKYRIE = 0x3A,
  ICON_MUTALISK_GUARDIAN_COCOON = 0x3B,
  ICON_CORSAIR = 0x3C,
  ICON_DARK_TEMPLAR_UNIT = 0x3D,
  ICON_DEVOURER = 0x3E,
  ICON_DARK_ARCHON = 0x3F,
  ICON_PROBE = 0x40,
  ICON_ZEALOT = 0x41,
  ICON_DRAGOON = 0x42,
  ICON_HIGH_TEMPLAR = 0x43,
  ICON_ARCHON = 0x44,
  ICON_SHUTTLE = 0x45,
  ICON_SCOUT = 0x46,
  ICON_ARBITER = 0x47,
  ICON_CARRIER = 0x48,
  ICON_INTERCEPTOR = 0x49,
  ICON_DARK_TEMPLAR_HERO = 0x4A,
  ICON_ZERATUL_DARK_TEMPLAR = 0x4B,
  ICON_TASSADAR_ZERATUL_ARCHON = 0x4C,
  ICON_FENIX_ZEALOT = 0x4D,
  ICON_FENIX_DRAGOON = 0x4E,
  ICON_TASSADAR_TEMPLAR = 0x4F,
  ICON_MOJO_SCOUT = 0x50,
  ICON_WARBRINGER_REAVER = 0x51,
  ICON_GANTRITHOR_CARRIER = 0x52,
  ICON_REAVER = 0x53,
  ICON_OBSERVER = 0x54,
  ICON_SCARAB = 0x55,
  ICON_DANIMOTH_ARBITER = 0x56,
  ICON_BLANK_ALDARIS = 0x57,
  ICON_ARTANIS_SCOUT = 0x58,
  ICON_RHYNADON_BADLANDS_CRITTER = 0x59,
  ICON_BENGALAAS_JUNGLE_CRITTER = 0x5A,
  ICON_LURKER_CARGO_SHIP_UNUSED = 0x5B,
  ICON_MERCENARY_GUNSHIP_UNUSED = 0x5C,
  ICON_SCANTID_DESERT_CRITTER = 0x5D,
  ICON_KAKARU_TWILIGHT_CRITTER = 0x5E,
  ICON_RAGNASAUR_ASHWORLD_CRITTER = 0x5F,
  ICON_URSADON_ICE_CRITTER = 0x60,
  ICON_BLANK_ZERG_LURKER_EGG = 0x61,
  ICON_BLANK_RASZAGAL = 0x62,
  ICON_SAMIR_DURAN_GHOST = 0x63,
  ICON_ALEXEI_STUKOV_GHOST = 0x64,
  ICON_MAP_REVEALER = 0x65,
  ICON_BLANK_GERARD_DUGALLE = 0x66,
  ICON_LURKER = 0x67,
  ICON_INFESTED_DURAN_INFESTED_TERRAN = 0x68,
  ICON_BLANK_DISRUPTION_FIELD = 0x69,
  ICON_COMMAND_CENTER = 0x6A,
  ICON_COMSAT_STATION = 0x6B,
  ICON_NUCLEAR_SILO = 0x6C,
  ICON_SUPPLY_DEPOT = 0x6D,
  ICON_REFINERY = 0x6E,
  ICON_BARRACKS = 0x6F,
  ICON_ACADEMY = 0x70,
  ICON_FACTORY = 0x71,
  ICON_STARPORT = 0x72,
  ICON_CONTROL_TOWER = 0x73,
  ICON_SCIENCE_FACILITY = 0x74,
  ICON_COVERT_OPS = 0x75,
  ICON_PHYSICS_LAB = 0x76,
  ICON_BLANK_STARBASE_UNUSED = 0x77,
  ICON_MACHINE_SHOP = 0x78,
  ICON_REPAIR_BAY_UNUSED = 0x79,
  ICON_ENGINEERING_BAY = 0x7A,
  ICON_ARMORY = 0x7B,
  ICON_MISSILE_TOWER = 0x7C,
  ICON_BUNKER = 0x7D,
  ICON_CRASHED_NORAD_II = 0x7E,
  ICON_ION_CANNON = 0x7F,
  ICON_URAJ = 0x80,
  ICON_KHALIS = 0x81,
  ICON_INFESTED_COMMAND_CENTER = 0x82,
  ICON_HATCHERY = 0x83,
  ICON_LAIR = 0x84,
  ICON_HIVE = 0x85,
  ICON_NYDUS_CANAL = 0x86,
  ICON_HYDRALISK_DEN = 0x87,
  ICON_DEFILER_MOUND = 0x88,
  ICON_GREATER_SPIRE = 0x89,
  ICON_QUEENS_NEST = 0x8A,
  ICON_EVOLUTION_CHAMBER = 0x8B,
  ICON_ULTRALISK_CAVERN = 0x8C,
  ICON_SPIRE = 0x8D,
  ICON_SPAWNING_POOL = 0x8E,
  ICON_CREEP_COLONY = 0x8F,
  ICON_SPORE_COLONY = 0x90,
  ICON_RADIOACTIVE_ZERG_BLDG1_UNUSED = 0x91,
  ICON_SUNKEN_COLONY = 0x92,
  ICON_OVERMIND_WITHOUT_SHELL = 0x93,
  ICON_OVERMIND_WITH_SHELL = 0x94,
  ICON_EXTRACTOR = 0x95,
  ICON_MATURE_CHRYSALIS = 0x96,
  ICON_CEREBRATE = 0x97,
  ICON_CEREBRATE_DAGGOTH = 0x98,
  ICON_BLANK_ZERG_BLDG2_UNUSED = 0x99,
  ICON_NEXUS = 0x9A,
  ICON_ROBOTICS_FACILITY = 0x9B,
  ICON_PYLON = 0x9C,
  ICON_ASSIMILATOR = 0x9D,
  ICON_BLANK_PROTOSS_BLDG1_UNUSED = 0x9E,
  ICON_OBSERVATORY = 0x9F,
  ICON_GATEWAY = 0xA0,
  ICON_BLANK_PROTOSS_BLDG2_UNUSED = 0xA1,
  ICON_PHOTON_CANNON2 = 0xA2,
  ICON_CITADEL_OF_ADUN = 0xA3,
  ICON_CYBERNETICS_CORE = 0xA4,
  ICON_TEMPLAR_ARCHIVES = 0xA5,
  ICON_FORGE = 0xA6,
  ICON_STARGATE = 0xA7,
  ICON_STASIS_CELL_PRISON = 0xA8,
  ICON_FLEET_BEACON = 0xA9,
  ICON_ARBITER_TRIBUNAL = 0xAA,
  ICON_ROBOTICS_SUPPORT_BAY = 0xAB,
  ICON_SHIELD_BATTERY = 0xAC,
  ICON_KHAYDARIN_CRYSTAL_FORMATION = 0xAD,
  ICON_PROTOSS_TEMPLE = 0xAE,
  ICON_XELNAGA_TEMPLE = 0xAF,
  ICON_MINERAL_CLUSTER_TYPE_1 = 0xB0,
  ICON_MINERAL_CLUSTER_TYPE_2 = 0xB1,
  ICON_MINERAL_CLUSTER_TYPE_3 = 0xB2,
  ICON_BLANK_CAVE_UNUSED = 0xB3,
  ICON_BLANK_CAVEIN_UNUSED = 0xB4,
  ICON_BLANK_CANTINA_UNUSED = 0xB5,
  ICON_BLANK_MINING_PLATFORM_UNUSED = 0xB6,
  ICON_BLANK_INDEPENDENT_CC_UNUSED = 0xB7,
  ICON_BLANK_INDEPENDENT_STARPORT_UNUSED = 0xB8,
  ICON_BLANK_JUMP_GATE_UNUSED = 0xB9,
  ICON_BLANK_RUINS_UNUSED = 0xBA,
  ICON_BLANK_KHAYD_CRYSTAL_FORM_UNUSED = 0xBB,
  ICON_VESPENE_GEYSER = 0xBC,
  ICON_WARP_GATE = 0xBD,
  ICON_PSI_DISRUPTER = 0xBE,
  ICON_BLANK_ZERG_MARKER = 0xBF,
  ICON_BLANK_TERRAN_MARKER = 0xC0,
  ICON_BLANK_PROTOSS_MARKER = 0xC1,
  ICON_ZERG_BEACON = 0xC2,
  ICON_TERRAN_BEACON = 0xC3,
  ICON_PROTOSS_BEACON = 0xC4,
  ICON_ZERG_FLAG_BEACON = 0xC5,
  ICON_TERRAN_FLAG_BEACON = 0xC6,
  ICON_PROTOSS_FLAG_BEACON = 0xC7,
  ICON_POWER_GENERATOR = 0xC8,
  ICON_OVERMIND_COCOON = 0xC9,
  ICON_BLANK_DARK_SWARM = 0xCA,
  ICON_BLANK_FLOOR_MISSILE_TRAP = 0xCB,
  ICON_BLANK_FLOOR_HATCH_UNUSED = 0xCC,
  ICON_BLANK_LEFT_UPPER_LEVEL_DOOR = 0xCD,
  ICON_BLANK_RIGHT_UPPER_LEVEL_DOOR = 0xCE,
  ICON_BLANK_LEFT_PIT_DOOR = 0xCF,
  ICON_BLANK_RIGHT_PIT_DOOR = 0xD0,
  ICON_BLANK_FLOOR_GUN_TRAP = 0xD1,
  ICON_BLANK_LEFT_WALL_MISSILE_TRAP = 0xD2,
  ICON_BLANK_LEFT_WALL_FLAME_TRAP = 0xD3,
  ICON_INFESTED_MINE_UNUSED = 0xD4,
  ICON_BLANK_RIGHT_WALL_FLAME_TRAP = 0xD5,
  ICON_START_LOCATION = 0xD6,
  ICON_FLAG = 0xD7,
  ICON_YOUNG_CHRYSALIS = 0xD8,
  ICON_PSI_EMITTER = 0xD9,
  ICON_DATA_DISC = 0xDA,
  ICON_KHAYDARIN_CRYSTAL = 0xDB,
  ICON_BLANK_MINERAL_CHUNK_TYPE_1 = 0xDC,
  ICON_BLANK_MINERAL_CHUNK_TYPE_2 = 0xDD,
  ICON_BLANK_PROTOSS_VESPENE_ORB_TYPE_1 = 0xDE,
  ICON_BLANK_PROTOSS_VESPENE_ORB_TYPE_2 = 0xDF,
  ICON_BLANK_ZERG_VESPENE_SAC_TYPE_1 = 0xE0,
  ICON_BLANK_ZERG_VESPENE_SAC_TYPE_2 = 0xE1,
  ICON_BLANK_TERRAN_VESPENE_TANK_TYPE_1 = 0xE2,
  ICON_BLANK_TERRAN_VESPENE_TANK_TYPE_2 = 0xE3,
  ICON_MOVE = 0xE4,
  ICON_STOP = 0xE5,
  ICON_ATTACK = 0xE6,
  ICON_GATHER = 0xE7,
  ICON_REPAIR = 0xE8,
  ICON_RETURN_RESOURCES = 0xE9,
  ICON_TERRAN_BASIC_BUILDINGS = 0xEA,
  ICON_TERRAN_ADVANCED_BUILDINGS = 0xEB,
  ICON_CANCEL = 0xEC,
  ICON_USE_STIMPACK = 0xED,
  ICON_U238_SHELLS = 0xEE,
  ICON_BURST_LASERS_UNUSED = 0xEF,
  ICON_LOCKDOWN = 0xF0,
  ICON_EMP_SHOCKWAVE = 0xF1,
  ICON_IRRADIATE = 0xF2,
  ICON_USE_SPIDER_MINES = 0xF3,
  ICON_AFTERBURNERS_UNUSED_TERRAN_UPGRADE = 0xF4,
  ICON_SEIGE_MODE = 0xF5,
  ICON_TANK_MODE = 0xF6,
  ICON_DEFENSIVE_MATRIX = 0xF7,
  ICON_TITAN_REACTOR = 0xF8,
  ICON_OCULAR_IMPLANTS = 0xF9,
  ICON_SCANNER_SWEEP = 0xFA,
  ICON_YAMATO_GUN = 0xFB,
  ICON_CLOAK = 0xFC,
  ICON_DECLOAK = 0xFD,
  ICON_PATROL = 0xFE,
  ICON_HOLD_POSITION = 0xFF,
  ICON_MOEBIUS_REACTOR = 0x100,
  ICON_ZERG_BASIC_BUILDINGS = 0x101,
  ICON_ZERG_ADVANCED_BUILDINGS = 0x102,
  ICON_BURROW = 0x103,
  ICON_UNBURROW = 0x104,
  ICON_VENTRAL_SACS = 0x105,
  ICON_ANTENNAE = 0x106,
  ICON_METABOLIC_BOOST = 0x107,
  ICON_ADRENAL_GLANDS = 0x108,
  ICON_PLAGUE = 0x109,
  ICON_MUSCULAR_AUGMENTS = 0x10A,
  ICON_ENSNARE = 0x10B,
  ICON_GROOVED_SPINES = 0x10C,
  ICON_ROAR_UNUSED_ZERG_UPGRADE = 0x10D,
  ICON_DARK_SWARM = 0x10E,
  ICON_PARASITE = 0x10F,
  ICON_PROTOSS_BASIC_BUILDINGS = 0x110,
  ICON_PROTOSS_ADVANCED_BUILDINGS = 0x111,
  ICON_MIND_CONTROL_SC_BETA_UNUSED = 0x112,
  ICON_PSIONIC_STORM = 0x113,
  ICON_GRAVITIC_BOOSTERS = 0x114,
  ICON_HALLUCINATION = 0x115,
  ICON_STASIS_FIELD = 0x116,
  ICON_BLANK = 0x117,
  ICON_RECALL = 0x118,
  ICON_SINGULARITY_CHARGE = 0x119,
  ICON_LIFT_OFF = 0x11A,
  ICON_LAND = 0x11B,
  ICON_APOLLO_REACTOR = 0x11C,
  ICON_COLOSSUS_REACTOR = 0x11D,
  ICON_SET_RALLY_POINT = 0x11E,
  ICON_ION_THRUSTERS = 0x11F,
  ICON_INFANTRY_WEAPONS = 0x120,
  ICON_VEHICLE_WEAPONS = 0x121,
  ICON_SHIP_WEAPONS = 0x122,
  ICON_SHIP_PLATING = 0x123,
  ICON_INFANTRY_ARMOR = 0x124,
  ICON_VEHICLE_ARMOR = 0x125,
  ICON_GAMETE_MEIOSIS = 0x126,
  ICON_METASYNAPTIC_NODE = 0x127,
  ICON_PNEUMATIZED_CAPARACE = 0x128,
  ICON_ZERG_CAPARACE = 0x129,
  ICON_FLYER_CAPARACE = 0x12A,
  ICON_MELEE_ATTACKS = 0x12B,
  ICON_MISSILE_ATTACKS = 0x12C,
  ICON_FLYER_ATTACKS = 0x12D,
  ICON_CONSUME = 0x12E,
  ICON_GROUND_ARMOR = 0x12F,
  ICON_AIR_PLATING = 0x130,
  ICON_GROUND_WEAPONS = 0x131,
  ICON_AIR_WEAPONS = 0x132,
  ICON_LEG_ENHANCEMENTS = 0x133,
  ICON_RECHARGE_SHIELDS = 0x134,
  ICON_LOAD_INTO_TRANSPORT = 0x135,
  ICON_PLASMA_SHIELDS = 0x136,
  ICON_NUCLEAR_STRIKE = 0x137,
  ICON_UNLOAD_ALL_FROM_TRANSPORT_BUNKER = 0x138,
  ICON_INFEST_COMMAND_CENTER = 0x139,
  ICON_SCARAB_DAMAGE = 0x13A,
  ICON_REAVER_CAPACITY = 0x13B,
  ICON_GRAVITIC_DRIVE = 0x13C,
  ICON_SENSOR_ARRAY = 0x13D,
  ICON_KHAYDARIN_AMULET = 0x13E,
  ICON_APIAL_SENSORS = 0x13F,
  ICON_GRAVITIC_THRUSTERS = 0x140,
  ICON_CARRIER_CAPACITY = 0x141,
  ICON_KHAYDARIN_CORE = 0x142,
  ICON_GAUSS_RIFLE = 0x143,
  ICON_C10_CANISTER_RIFLE = 0x144,
  ICON_FRAGMENTATION_GRENADE = 0x145,
  ICON_TWIN_AUTOCANNONS = 0x146,
  ICON_HELLFIRE_MISSILE_PACK = 0x147,
  ICON_ARCLITE_CANNON = 0x148,
  ICON_FUSION_CUTTER = 0x149,
  ICON_FUSION_CUTTER_HARVEST = 0x14A,
  ICON_GEMINI_MISSILES = 0x14B,
  ICON_BURST_LASERS = 0x14C,
  ICON_ATS_LASER_BATTERY = 0x14D,
  ICON_ATA_LASER_BATTERY = 0x14E,
  ICON_FLAME_THROWER = 0x14F,
  ICON_ARCLITE_SHOCK_CANNON = 0x150,
  ICON_LONGBOLT_MISSILE = 0x151,
  ICON_CLAWS = 0x152,
  ICON_NEEDLE_SPINES = 0x153,
  ICON_KAISER_BLADES = 0x154,
  ICON_TOXIC_SPORES = 0x155,
  ICON_SPINES = 0x156,
  ICON_FLYER_ATTACK = 0x157,
  ICON_ACID_SPORE = 0x158,
  ICON_GLAVE_WURM = 0x159,
  ICON_VENOM_UNUSED_ZERG_WEAPON = 0x15A,
  ICON_SEEKER_SPORES = 0x15B,
  ICON_SUBTERRANEAN_TENTACLE = 0x15C,
  ICON_SUICIDE_INFESTED_TERRAN = 0x15D,
  ICON_SUICIDE_SCOURGE = 0x15E,
  ICON_PARTICLE_BEAM = 0x15F,
  ICON_PARTICLE_BEAM_HARVEST = 0x160,
  ICON_PSI_WARP_BLADES = 0x161,
  ICON_PHASE_DISRUPTOR = 0x162,
  ICON_PSI_ASSAULT = 0x163,
  ICON_PSIONIC_SHOCKWAVE = 0x164,
  ICON_RADIOACTIVE_UNUSED = 0x165,
  ICON_DUAL_PHOTON_BLASTERS = 0x166,
  ICON_ANTIMATTER_MISSILES = 0x167,
  ICON_PHASE_DISRUPTOR_CANNON = 0x168,
  ICON_PULSE_CANNON = 0x169,
  ICON_PHOTON_CANNON = 0x16A,
  ICON_RADIOACTIVE_UNUSED2 = 0x16B,
  ICON_SPIDER_MINE = 0x16C,
  ICON_HEAL = 0x16D,
  ICON_RESTORATION = 0x16E,
  ICON_RESTORATION2 = 0x16F,
  ICON_DISRUPTION_WEB = 0x170,
  ICON_DISRUPTION_WEB2 = 0x171,
  ICON_UNKNOWN371 = 0x172,
  ICON_MIND_CONTROL = 0x173,
  ICON_FEEDBACK = 0x174,
  ICON_OPTICAL_FLARE = 0x175,
  ICON_AFTERBURNERS_ON_UNUSED = 0x176,
  ICON_AFTERBURNERS_OFF_UNUSED = 0x177,
  ICON_LURKER_ASPECT = 0x178,
  ICON_UNKNOWN378 = 0x179,
  ICON_ANABOLIC_SYNTHESIS = 0x17A,
  ICON_CHITINOUS_PLATING = 0x17B,
  ICON_CHARON_BOOSTERS = 0x17C,
  ICON_MAELSTROM = 0x17D,
  ICON_SUBTERRANEAN_SPINES = 0x17E,
  ICON_ARGUS_JEWEL = 0x17F,
  ICON_CADUCEUS_REACTOR = 0x180,
  ICON_ARGUS_TALISMAN = 0x181,
  ICON_PLAY_REPLAY = 0x182,
  ICON_PAUSE_REPLAY = 0x183,
  ICON_SPEED_UP_REPLAY = 0x184,
  ICON_SLOW_DOWN_REPLAY = 0x185,
};

struct BltMask
{
  void *pvoid0;
  _BYTE gap4[4];
  _DWORD dword8;
  _DWORD dwordC;
  _DWORD dword10;
  _DWORD dword14;
  _DWORD dword18;
};
static_assert(sizeof(BltMask) == 28, "Incorrect size for type `BltMask`. Expected: 28");

struct RegistryOption
{
  int field_0;
  void *aSpeed;
  int field_8;
  int field_C;
  int field_10;
  RegistryOptions *field_14;
};
static_assert(sizeof(RegistryOption) == 24, "Incorrect size for type `RegistryOption`. Expected: 24");

enum CursorType
{
  CUR_ARROW = 0x0,
  CUR_ILLEGAL = 0x1,
  CUR_TARG_Y = 0x2,
  CUR_TARG_R = 0x3,
  CUR_TARG_G = 0x4,
  CUR_TARG_N = 0x5,
  CUR_MAG_G = 0x6,
  CUR_MAG_R = 0x7,
  CUR_MAG_Y = 0x8,
  CUR_DRAG = 0x9,
  CUR_TIME = 0xA,
  CUR_SCROLL_U = 0xB,
  CUR_SCROLL_UR = 0xC,
  CUR_SCROLL_R = 0xD,
  CUR_SCROLL_DR = 0xE,
  CUR_SCROLL_D = 0xF,
  CUR_SCROLL_DL = 0x10,
  CUR_SCROLL_L = 0x11,
  CUR_SCROLL_UL = 0x12,
  CUR_MAX = 0x13,
};

struct struc_659B10
{
  int field_0;
  int field_4;
  int field_8;
  int field_C;
  __int16 field_10;
  __int16 field_12;
};
static_assert(sizeof(struc_659B10) == 20, "Incorrect size for type `struc_659B10`. Expected: 20");

struct struct_a1_2
{
  _BYTE gap0[16];
  HANDLE pvoid10;
};
static_assert(sizeof(struct_a1_2) == 20, "Incorrect size for type `struct_a1_2`. Expected: 20");

enum PrintFlags : __int8
{
  PF_HALIGN_LEFT = 0x1,
  PF_HALIGN_CENTER = 0x2,
  PF_HALIGN_RIGHT = 0x4,
  PF_UNK = 0x8,
  PF_VALIGN_TOP = 0x10,
  PF_VALIGN_CENTER = 0x20,
  PF_VALIGN_BOTTOM = 0x40,
};

struct __declspec(align(4)) struct_a3
{
  _DWORD unit_property_index;
  _DWORD location_index;
};
static_assert(sizeof(struct_a3) == 8, "Incorrect size for type `struct_a3`. Expected: 8");

struct __declspec(align(4)) SNETPROGRAMDATA
{
  DWORD size;
  LPCSTR programname;
  LPCSTR programdescription;
  DWORD programid;
  DWORD versionid;
  DWORD reserved1;
  DWORD maxplayers;
  LPVOID initdata;
  DWORD initdatabytes;
  LPVOID reserved2;
  DWORD optcategorybits;
  DWORD key_owner;
  DWORD key;
  DWORD is_spawn;
  DWORD lang;
};
static_assert(sizeof(SNETPROGRAMDATA) == 60, "Incorrect size for type `SNETPROGRAMDATA`. Expected: 60");

enum CommandId : unsigned __int8
{
  CMD_PublicChat = 0x0,
  CMD_KeepAlive = 0x5,
  CMD_SaveGame = 0x6,
  CMD_LoadGame = 0x7,
  CMD_RestarGame = 0x8,
  CMD_SelectUnits = 0x9,
  CMD_SelectDeltaAdd = 0xA,
  CMD_SelectDeltaDel = 0xB,
  CMD_PlaceBuilding = 0xC,
  CMD_SetFog = 0xD,
  CMD_SetAllies = 0xE,
  CMD_SetSpeed = 0xF,
  CMD_Pause = 0x10,
  CMD_Resume = 0x11,
  CMD_Cheat = 0x12,
  CMD_GroupUnits = 0x13,
  CMD_RightClick = 0x14,
  CMD_Action = 0x15,
  CMD_CancelConstruction = 0x18,
  CMD_CancelUnitMorph = 0x19,
  CMD_Stop = 0x1A,
  CMD_CarrierStop = 0x1B,
  CMD_ReaverStop = 0x1C,
  CMD_Nothing = 0x1D,
  CMD_ReturnCargo = 0x1E,
  CMD_Train = 0x1F,
  CMD_CancelTrain = 0x20,
  CMD_Cloak = 0x21,
  CMD_Decloak = 0x22,
  CMD_Morph = 0x23,
  CMD_Unsiege = 0x25,
  CMD_Siege = 0x26,
  CMD_TrainFighter = 0x27,
  CMD_UnloadAll = 0x28,
  CMD_ExitTransport = 0x29,
  CMD_MergeArchon = 0x2A,
  CMD_HoldPosition = 0x2B,
  CMD_Burrow = 0x2C,
  CMD_Unburrow = 0x2D,
  CMD_CancelNuke = 0x2E,
  CMD_Lift = 0x2F,
  CMD_Research = 0x30,
  CMD_CancelResearch = 0x31,
  CMD_Upgrade = 0x32,
  CMD_CancelUpgrade = 0x33,
  CMD_CancelAddon = 0x34,
  CMD_BuildingMorph = 0x35,
  CMD_Stimpack = 0x36,
  CMD_Sync = 0x37,
  CMD_VoiceUnused1 = 0x38,
  CMD_VoiceUnused2 = 0x39,
  CMD_VoiceUnused3 = 0x3A,
  CMD_VoiceUnused4 = 0x3B,
  CMD_StartGame = 0x3C,
  CMD_MapDownload = 0x3D,
  CMD_GameSlotModification = 0x3E,
  CMD_VersionVerification = 0x3F,
  CMD_VersionConfirmation = 0x40,
  CMD_RaceChange = 0x41,
  CMD_ForceChangeMelee = 0x42,
  CMD_ForceChangeUMS = 0x43,
  CMD_SlotChange = 0x44,
  CMD_SwapPlayers = 0x45,
  CMD_GameInit = 0x48,
  CMD_InformationRequest = 0x49,
  CMD_ForceDataTransfer = 0x4A,
  CMD_ForceNameTransfer = 0x4B,
  CMD_LobbyChat = 0x4C,
  CMD_LobbyChatUnused = 0x4D,
  CMD_BootPlayer = 0x4E,
  CMD_MapDataTransfer = 0x4F,
  CMD_NetworkError = 0x51,
  CMD_MissionBriefingStart = 0x54,
  CMD_SetLatency = 0x55,
  CMD_ReplaySpeed = 0x56,
  CMD_LeaveGame = 0x57,
  CMD_MinimapPing = 0x58,
  CMD_MergeDarkArchon = 0x5A,
  CMD_MakeGamePublic = 0x5B,
  CMD_ReplayChat = 0x5C,
};

struct struc_64DEC8
{
  CUnit *field_0;
  CUnit *field_4;
  int field_8;
  int field_C;
  int field_10;
  int field_14;
  int field_18;
  int field_1C;
  int field_20;
  int field_24;
  int field_28;
  int field_2C;
  int field_30;
  int field_34;
  int field_38;
  int field_3C;
  int field_40;
  int field_44;
  int field_48;
  int field_4C;
  int field_50;
  int field_54;
  int field_58;
  int field_5C;
  int field_60;
  int field_64;
  int field_68;
  int field_6C;
  int field_70;
  int field_74;
  int field_78;
  int field_7C;
};
static_assert(sizeof(struc_64DEC8) == 128, "Incorrect size for type `struc_64DEC8`. Expected: 128");

struct SNETCAPS
{
  DWORD size;
  DWORD flags;
  DWORD maxmessagesize;
  DWORD maxqueuesize;
  DWORD maxplayers;
  DWORD bytessec;
  DWORD latencyms;
  DWORD defaultturnssec;
  DWORD defaultturnsintransit;
};
static_assert(sizeof(SNETCAPS) == 36, "Incorrect size for type `SNETCAPS`. Expected: 36");

struct __declspec(align(4)) SNETUIDATA
{
  DWORD dwSize;
  DWORD dwUnkType;
  HWND hFrameWnd;
  void *pfnBattleGetResource;
  void *pfnBattleGetErrorString;
  void *pfnBattleMakeCreateGameDialog;
  void *pfnBattleUpdateIcons;
  DWORD dwUnk_07;
  void *pfnBattleErrorDialog;
  void *pfnBattlePlaySound;
  DWORD dwUnk_10;
  void *pfnBattleGetCursorLink;
  DWORD dwUnk_12;
  void *pfnUnk_13;
  DWORD dwUnk_14;
  void *pfnBattleMakeProfileDialog;
  char *pszProfileStrings;
  void *pfnBattleDrawProfileInfo;
  void *pfnUnk_18;
  DWORD dwUnk_19;
  void *pfnUnk_20;
  void *pfnUnk_21;
  void *pfnBattleSetLeagueName;
};
static_assert(sizeof(SNETUIDATA) == 92, "Incorrect size for type `SNETUIDATA`. Expected: 92");

struct SNETPLAYERDATA
{
  DWORD dwSize;
  char *pszPlayerName;
  char *pszUnknown;
  DWORD dwUnknown;
};
static_assert(sizeof(SNETPLAYERDATA) == 16, "Incorrect size for type `SNETPLAYERDATA`. Expected: 16");

struct struc_51B1E8
{
  int field_0[1];
  int field_4;
  int field_8;
  int field_C;
  int field_10;
  int field_14;
  int field_18;
  int field_1C;
  int field_20;
  int field_24;
};
static_assert(sizeof(struc_51B1E8) == 40, "Incorrect size for type `struc_51B1E8`. Expected: 40");

struct TemplateList
{
  int field_0;
  TemplateListEntry *end;
  TemplateListEntry *begin;
};
static_assert(sizeof(TemplateList) == 12, "Incorrect size for type `TemplateList`. Expected: 12");

struct struct_a4
{
  char char0[260];
  char char104[32];
  _DWORD dword124;
};
static_assert(sizeof(struct_a4) == 296, "Incorrect size for type `struct_a4`. Expected: 296");

#pragma pack(push, 1)
struct __declspec(align(1)) struct_v2
{
  char unknown;
  int initial_seed;
  char player_bytes[8];
};
#pragma pack(pop)
static_assert(sizeof(struct_v2) == 13, "Incorrect size for type `struct_v2`. Expected: 13");

enum SnetInfoType
{
  SNET_INFO_GAMENAME = 0x1,
  SNET_INFO_GAMEPASSWORD = 0x2,
  SNET_INFO_GAMEDESCRIPTION = 0x3,
  SNET_INFO_GAMEMODE = 0x4,
  SNET_INFO_INITDATA = 0x5,
  SNET_INFO_MAXPLAYERS = 0x6,
};

enum IScriptOpcodes : unsigned __int8
{
  opc_playfram = 0x0,
  opc_playframtile = 0x1,
  opc_sethorpos = 0x2,
  opc_setvertpos = 0x3,
  opc_setpos = 0x4,
  opc_wait = 0x5,
  opc_waitrand = 0x6,
  opc_goto = 0x7,
  opc_imgol = 0x8,
  opc_imgul = 0x9,
  opc_imgolorig = 0xA,
  opc_switchul = 0xB,
  opc___0c = 0xC,
  opc_imgoluselo = 0xD,
  opc_imguluselo = 0xE,
  opc_sprol = 0xF,
  opc_highsprol = 0x10,
  opc_lowsprul = 0x11,
  opc_uflunstable = 0x12,
  opc_spruluselo = 0x13,
  opc_sprul = 0x14,
  opc_sproluselo = 0x15,
  opc_end = 0x16,
  opc_setflipstate = 0x17,
  opc_playsnd = 0x18,
  opc_playsndrand = 0x19,
  opc_playsndbtwn = 0x1A,
  opc_domissiledmg = 0x1B,
  opc_attackmelee = 0x1C,
  opc_followmaingraphic = 0x1D,
  opc_randcondjmp = 0x1E,
  opc_turnccwise = 0x1F,
  opc_turncwise = 0x20,
  opc_turn1cwise = 0x21,
  opc_turnrand = 0x22,
  opc_setspawnframe = 0x23,
  opc_sigorder = 0x24,
  opc_attackwith = 0x25,
  opc_attack = 0x26,
  opc_castspell = 0x27,
  opc_useweapon = 0x28,
  opc_move = 0x29,
  opc_gotorepeatattk = 0x2A,
  opc_engframe = 0x2B,
  opc_engset = 0x2C,
  opc___2d = 0x2D,
  opc_nobrkcodestart = 0x2E,
  opc_nobrkcodeend = 0x2F,
  opc_ignorerest = 0x30,
  opc_attkshiftproj = 0x31,
  opc_tmprmgraphicstart = 0x32,
  opc_tmprmgraphicend = 0x33,
  opc_setfldirect = 0x34,
  opc_call = 0x35,
  opc_return = 0x36,
  opc_setflspeed = 0x37,
  opc_creategasoverlays = 0x38,
  opc_pwrupcondjmp = 0x39,
  opc_trgtrangecondjmp = 0x3A,
  opc_trgtarccondjmp = 0x3B,
  opc_curdirectcondjmp = 0x3C,
  opc_imgulnextid = 0x3D,
  opc___3e = 0x3E,
  opc_liftoffcondjmp = 0x3F,
  opc_warpoverlay = 0x40,
  opc_orderdone = 0x41,
  opc_grdsprol = 0x42,
  opc___43 = 0x43,
  opc_dogrddamage = 0x44,
};

enum ImageOrder
{
  IMGORD_BELOW = 0x0,
  IMGORD_ABOVE = 0x1,
};

enum OverlayType : unsigned __int8
{
  OT_ATTACK = 0x0,
  OT_DAMAGE = 0x1,
  OT_SPECIAL = 0x2,
  OT_LANDING = 0x3,
  OT_LIFTOFF = 0x4,
};

struct __declspec(align(4)) ListNodeUnknown
{
  ListNodeUnknown *previous;
  ListNodeUnknown *next;
  ListNodeUnknown *unknown;
};
static_assert(sizeof(ListNodeUnknown) == 12, "Incorrect size for type `ListNodeUnknown`. Expected: 12");

struct __declspec(align(4)) struc_59A0F0
{
  char field_0[12];
  int field_C;
  dialog *field_10;
  int field_14;
  int field_18;
  char field_1C[384];
  int score_field_0;
  int score_field_1;
  int score_field_2;
  int field_1A8;
  int field_1AC;
  int field_1B0;
  int field_1B4;
  int field_1B8;
  int field_1BC;
  int field_1C0;
  int field_1C4;
  int field_1C8;
  int gap1B4[32];
  int total_score_field;
  int unk0;
  int unk1;
  int unk2;
};
static_assert(sizeof(struc_59A0F0) == 604, "Incorrect size for type `struc_59A0F0`. Expected: 604");

struct ScoreFormatRelated
{
  void (__fastcall *score_calc)(struc_59A0F0 *a1, int player_id);
  char *format_string;
  int x;
  int y;
  int z;
};
static_assert(sizeof(ScoreFormatRelated) == 20, "Incorrect size for type `ScoreFormatRelated`. Expected: 20");

struct __unaligned __declspec(align(1)) struct_v0
{
  char char0[25];
  char char19[30];
  _BYTE gap1A;
  _DWORD dword38;
  _DWORD dword3C;
  _DWORD dword40;
  _BYTE byte44;
  _BYTE byte45[3];
};
static_assert(sizeof(struct_v0) == 72, "Incorrect size for type `struct_v0`. Expected: 72");

struct ShaState
{
  int h[5];
  int bit_count_low_maybe;
  int bit_count_high_maybe;
  int unk;
};
static_assert(sizeof(ShaState) == 32, "Incorrect size for type `ShaState`. Expected: 32");

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) LoadGameCommand
{
  char buffer;
  int var_23;
  char dest[28];
};
#pragma pack(pop)
static_assert(sizeof(LoadGameCommand) == 33, "Incorrect size for type `LoadGameCommand`. Expected: 33");

struct __declspec(align(4)) GluRelated
{
  const char glue_path[260];
  grpHead *grp_head;
  char field_108[1];
  char field_109;
  WORD field_10A;
  _BYTE gap10C[2];
  __int16 field_10E;
  int field_110;
  char unknwon[1024];
  int index;
};
static_assert(sizeof(GluRelated) == 1304, "Incorrect size for type `GluRelated`. Expected: 1304");

struct struct_v3
{
  HANDLE handle;
  _DWORD dword4;
};
static_assert(sizeof(struct_v3) == 8, "Incorrect size for type `struct_v3`. Expected: 8");

enum StormVideoFlags
{
  SVID_FLAG_DOUBLESCANS = 0x1,
  SVID_FLAG_INTERPOLATE = 0x2,
  SVID_FLAG_INTERLACE = 0x4,
  SVID_FLAG_AUTOQUALITY = 0x8,
  SVID_FLAG_1XSIZE = 0x100,
  SVID_FLAG_2XSIZE = 0x200,
  SVID_FLAG_AUTOSIZE = 0x800,
  SVID_FLAG_FILEHANDLE = 0x10000,
  SVID_FLAG_PRELOAD = 0x20000,
  SVID_FLAG_LOOP = 0x40000,
  SVID_FLAG_FULLSCREEN = 0x80000,
  SVID_FLAG_USECURRENTPALETTE = 0x100000,
  SVID_FLAG_CLEARSCREEN = 0x200000,
  SVID_FLAG_NOSKIP = 0x400000,
  SVID_FLAG_UNK = 0x800000,
  SVID_FLAG_UNK2 = 0x1000000,
  SVID_FLAG_NEEDPAN = 0x2000000,
  SVID_FLAG_NEEDVOLUME = 0x4000000,
  SVID_FLAG_TOSCREEN = 0x10000000,
  SVID_FLAG_TOBUFFER = 0x20000000,
};

struct DownloadListMaybe
{
  DownloadListMaybe *previous;
  DownloadListMaybe *next;
  _BYTE gap0[4];
  _DWORD dwordC;
  _DWORD dword10;
  _BYTE gap14[32];
  _BYTE flags;
  _BYTE gap35[3];
  DWORD *gap38;
  _BYTE gap3c[4];
  _DWORD dword40;
  char *pvoid44;
};
static_assert(sizeof(DownloadListMaybe) == 72, "Incorrect size for type `DownloadListMaybe`. Expected: 72");

struct struct_v4_1
{
  int (__stdcall *fn1)(int, HWND, char *);
  BOOL (__stdcall *fn2)(int, HWND hWnd, int);
  HWND hwnd;
};
static_assert(sizeof(struct_v4_1) == 12, "Incorrect size for type `struct_v4_1`. Expected: 12");

struct struct_a2_1
{
  int *gap0[126];
  int dword1F8;
};
static_assert(sizeof(struct_a2_1) == 508, "Incorrect size for type `struct_a2_1`. Expected: 508");

struct struc_500F60
{
  char *xpath_maybe;
  int flags;
  int nIDDlgItem;
  int field_C;
};
static_assert(sizeof(struc_500F60) == 16, "Incorrect size for type `struc_500F60`. Expected: 16");

struct struc_640898
{
  char field_0[48];
};
static_assert(sizeof(struc_640898) == 48, "Incorrect size for type `struc_640898`. Expected: 48");

enum StatDataDescriptorType : unsigned __int16
{
  SDDT_WEAPON = 0x0,
  SDDT_ARMOR = 0x1,
  SDDT_UPGRADE = 0x2,
  SDDT_UNIT = 0x3,
  SDDT_TECH = 0x4,
};

enum ExplosionType : unsigned __int8
{
  ET_NONE = 0x0,
  ET_NORMAL = 0x1,
  ET_RADIAL_SPLASH = 0x2,
  ET_ENEMY_SPLASH = 0x3,
  ET_LOCKDOWN = 0x4,
  ET_NUCLEAR_MISSILE = 0x5,
  ET_PARASITE = 0x6,
  ET_BROODLINGS = 0x7,
  ET_EMP_SHOCKWAVE = 0x8,
  ET_IRRADIATE = 0x9,
  ET_ENSNARE = 0xA,
  ET_PLAGUE = 0xB,
  ET_STASIS_FIELD = 0xC,
  ET_DARK_SWARM = 0xD,
  ET_CONSUME = 0xE,
  ET_YAMATO_GUN = 0xF,
  ET_RESTORATION = 0x10,
  ET_DISRUPTION_WEB = 0x11,
  ET_CORROSIVE_ACID = 0x12,
  ET_MIND_CONTROL = 0x13,
  ET_FEEDBACK = 0x14,
  ET_OPTICAL_FLARE = 0x15,
  ET_MAELSTROM = 0x16,
  ET_23 = 0x17,
  ET_AIR_SPLASH = 0x18,
};

#pragma pack(push, 1)
struct RTTIBaseClassDescriptor
{
  void *pTypeDescriptor;
  int numContainedBases;
  PMD pmd;
  int attributes;
};
#pragma pack(pop)
static_assert(sizeof(RTTIBaseClassDescriptor) == 24, "Incorrect size for type `RTTIBaseClassDescriptor`. Expected: 24");

struct _EH3_EXCEPTION_REGISTRATION
{
  struct _EH3_EXCEPTION_REGISTRATION *Next;
  PVOID ExceptionHandler;
  PSCOPETABLE_ENTRY ScopeTable;
  DWORD TryLevel;
};
static_assert(sizeof(_EH3_EXCEPTION_REGISTRATION) == 16, "Incorrect size for type `_EH3_EXCEPTION_REGISTRATION`. Expected: 16");

struct Bitmap
{
  u16 wid;
  u16 ht;
  u8 *data;
};
static_assert(sizeof(Bitmap) == 8, "Incorrect size for type `Bitmap`. Expected: 8");

struct __declspec(align(2)) rect
{
  s16 left;
  s16 top;
  s16 right;
  s16 bottom;
};
static_assert(sizeof(rect) == 8, "Incorrect size for type `rect`. Expected: 8");

struct dialog_dlg
{
  u32 dwUnk_0x32;
  u16 dstBits_wid;
  u16 dstBits_ht;
  u8 *dstBits_data;
  dialog *pActiveElement;
  dialog *pFirstChild;
  dialog *pMouseElement;
  bool (__fastcall *pModalFcn)(dialog *);
};
static_assert(sizeof(dialog_dlg) == 28, "Incorrect size for type `dialog_dlg`. Expected: 28");

struct dialog_edit
{
  dialog *pDlg;
  u8 bColor;
  u8 bScrollPosition;
  u8 bLeftMargin;
  u8 bTopMargin;
  u16 wUnk_0x3A;
  u16 wUnk_0x3C;
  u8 bCursorPos;
};
static_assert(sizeof(dialog_edit) == 16, "Incorrect size for type `dialog_edit`. Expected: 16");

typedef void (__fastcall *FnDrawItem)(struct dialog *dlg, u8 selectedIndex, rect *dstRect, int x, int y);

struct dialog_optn
{
  dialog *pDlg;
  u8 bEnabled;
};
static_assert(sizeof(dialog_optn) == 8, "Incorrect size for type `dialog_optn`. Expected: 8");

struct dialog_scroll
{
  struct dialog *pDlg;
  void *pfcnScrollerUpdate;
  u16 nCurPos;
  u16 nMin;
  u16 nMax;
  u16 wUnk_0x40;
  u32 dwScrollFlags;
  u8 bSliderSkip;
  u8 bUpdateCounter;
  u8 bSliderGraphic;
  u8 bSliderSpacing;
  u16 wUnk_0x4A;
  u32 dwUnk_0x4C;
};
static_assert(sizeof(dialog_scroll) == 32, "Incorrect size for type `dialog_scroll`. Expected: 32");

struct PlayerInfo
{
  int dwPlayerID;
  int dwStormId;
  PlayerType nType;
  RaceId nRace;
  unsigned __int8 nTeam;
  char szName[25];
};
static_assert(sizeof(PlayerInfo) == 36, "Incorrect size for type `PlayerInfo`. Expected: 36");

#pragma pack(push, 1)
struct __declspec(align(1)) Position
{
  u16 x;
  u16 y;
};
#pragma pack(pop)
static_assert(sizeof(Position) == 4, "Incorrect size for type `Position`. Expected: 4");

struct pt
{
  u16 x;
  u16 y;
};
static_assert(sizeof(pt) == 4, "Incorrect size for type `pt`. Expected: 4");

struct FontChar
{
  u8 w;
  u8 h;
  u8 _x;
  u8 _y;
  u8 data[1];
};
static_assert(sizeof(FontChar) == 5, "Incorrect size for type `FontChar`. Expected: 5");

struct Font
{
  u32 magic;
  u8 low;
  u8 high;
  u8 Xmax;
  u8 Ymax;
  FontChar *chrs[1];
};
static_assert(sizeof(Font) == 12, "Incorrect size for type `Font`. Expected: 12");

struct layer
{
  BYTE buffers;
  BYTE bits;
  WORD left;
  WORD top;
  WORD width;
  WORD height;
  WORD alignment;
  Bitmap *pSurface;
  void (__fastcall *pUpdate)(int a1, int a2, Bitmap *pSurface, bounds *pBounds);
};
static_assert(sizeof(layer) == 20, "Incorrect size for type `layer`. Expected: 20");

struct UpgradesSC
{
  u8 items[46];
};
static_assert(sizeof(UpgradesSC) == 46, "Incorrect size for type `UpgradesSC`. Expected: 46");

struct UpgradesBW
{
  u8 items[15];
};
static_assert(sizeof(UpgradesBW) == 15, "Incorrect size for type `UpgradesBW`. Expected: 15");

struct TechSC
{
  u8 items[24];
};
static_assert(sizeof(TechSC) == 24, "Incorrect size for type `TechSC`. Expected: 24");

struct TechBW
{
  u8 items[20];
};
static_assert(sizeof(TechBW) == 20, "Incorrect size for type `TechBW`. Expected: 20");

struct __declspec(align(2)) IScriptProgramState
{
  u16 iscript_header;
  u16 program_counter;
  u16 return_address;
  Anims anim;
  u8 wait;
};
static_assert(sizeof(IScriptProgramState) == 8, "Incorrect size for type `IScriptProgramState`. Expected: 8");

struct grpFrame
{
  u8 x;
  u8 y;
  u8 wid;
  u8 hgt;
  u32 dataOffset;
};
static_assert(sizeof(grpFrame) == 8, "Incorrect size for type `grpFrame`. Expected: 8");

struct __declspec(align(4)) Target_
{
  points pt;
  struct CUnit *pUnit;
};
static_assert(sizeof(Target_) == 8, "Incorrect size for type `Target_`. Expected: 8");

struct UnitFinderData
{
  s32 unitIndex;
  s32 position;
};
static_assert(sizeof(UnitFinderData) == 8, "Incorrect size for type `UnitFinderData`. Expected: 8");

struct CUnitVulture
{
  u8 spiderMineCount;
};
static_assert(sizeof(CUnitVulture) == 1, "Incorrect size for type `CUnitVulture`. Expected: 1");

struct CUnitCarrier
{
  CUnit *pInHanger;
  CUnit *pOutHanger;
  u8 inHangerCount;
  u8 outHangerCount;
};
static_assert(sizeof(CUnitCarrier) == 12, "Incorrect size for type `CUnitCarrier`. Expected: 12");

struct CUnitBeacon
{
  u32 _unknown_00;
  u32 _unknown_04;
  u32 flagSpawnFrame;
};
static_assert(sizeof(CUnitBeacon) == 12, "Incorrect size for type `CUnitBeacon`. Expected: 12");

#pragma pack(push, 1)
struct __declspec(align(2)) CUnitBuilding
{
  CUnit *addon;
  UnitType addonBuildType;
  u16 upgradeResearchTime;
  Tech techType;
  u8 upgradeType;
  u8 larvaTimer;
  u8 landingTimer;
  u8 creepTimer;
  u8 upgradeLevel;
  u16 __E;
};
#pragma pack(pop)
static_assert(sizeof(CUnitBuilding) == 16, "Incorrect size for type `CUnitBuilding`. Expected: 16");

struct CUnitWorker
{
  CUnit *pPowerup;
  points targetResource;
  CUnit *targetResourceUnit;
  u16 repairResourceLossTimer;
  bool isCarryingSomething;
  u8 resourceCarryCount;
};
static_assert(sizeof(CUnitWorker) == 16, "Incorrect size for type `CUnitWorker`. Expected: 16");

struct CUnitResource
{
  u16 resourceCount;
  u8 resourceIscript;
  u8 gatherQueueCount;
  CUnit *nextGatherer;
  u8 resourceGroup;
  u8 resourceBelongsToAI;
};
static_assert(sizeof(CUnitResource) == 12, "Incorrect size for type `CUnitResource`. Expected: 12");

struct CUnitPowerup
{
  points origin;
};
static_assert(sizeof(CUnitPowerup) == 4, "Incorrect size for type `CUnitPowerup`. Expected: 4");

struct CUnitRally
{
  points position;
  CUnit *unit;
};
static_assert(sizeof(CUnitRally) == 8, "Incorrect size for type `CUnitRally`. Expected: 8");

struct CUnitFinder
{
  u32 left;
  u32 right;
  u32 top;
  u32 bottom;
};
static_assert(sizeof(CUnitFinder) == 16, "Incorrect size for type `CUnitFinder`. Expected: 16");

struct CUnitStatus
{
  u16 removeTimer;
  u16 defenseMatrixDamage;
  u8 defenseMatrixTimer;
  u8 stimTimer;
  u8 ensnareTimer;
  u8 lockdownTimer;
  u8 irradiateTimer;
  u8 stasisTimer;
  u8 plagueTimer;
  u8 stormTimer;
  CUnit *irradiatedBy;
  u8 irradiatePlayerID;
  u8 parasiteFlags;
  u8 cycleCounter;
  bool isBlind;
  u8 maelstromTimer;
  u8 _unused_0x125;
  u8 acidSporeCount;
  u8 acidSporeTime[9];
  u16 bulletBehaviour3by3AttackSequence;
  u16 _padding_0x132;
  void *pAI;
  u16 airStrength;
  u16 groundStrength;
};
static_assert(sizeof(CUnitStatus) == 44, "Incorrect size for type `CUnitStatus`. Expected: 44");

struct SuppliesPerRace
{
  s32 available[12];
  s32 used[12];
  s32 max[12];
};
static_assert(sizeof(SuppliesPerRace) == 144, "Incorrect size for type `SuppliesPerRace`. Expected: 144");

struct Counts
{
  s32 all[228][12];
  s32 completed[228][12];
  s32 killed[228][12];
  s32 dead[228][12];
};
static_assert(sizeof(Counts) == 43776, "Incorrect size for type `Counts`. Expected: 43776");

struct vr4entry
{
  BYTE cdata[8][8];
};
static_assert(sizeof(vr4entry) == 64, "Incorrect size for type `vr4entry`. Expected: 64");

struct UnitAvail
{
  u8 available[12][228];
};
static_assert(sizeof(UnitAvail) == 2736, "Incorrect size for type `UnitAvail`. Expected: 2736");

struct ScrollSpeeds
{
  u8 scroll[7];
};
static_assert(sizeof(ScrollSpeeds) == 7, "Incorrect size for type `ScrollSpeeds`. Expected: 7");

#pragma pack(push, 1)
struct __declspec(align(2)) SightStruct
{
  u32 tileSightWidth;
  u32 tileSightHeight;
  u32 unknown1;
  u32 unknown2;
  u32 unknown3;
  u32 tile_count;
  u32 tiles;
};
#pragma pack(pop)
static_assert(sizeof(SightStruct) == 28, "Incorrect size for type `SightStruct`. Expected: 28");

struct fontMemStruct
{
  u32 tFontData;
  u32 tFontUnknown;
  u16 x1;
  u16 y1;
  u16 x2;
  u16 y2;
};
static_assert(sizeof(fontMemStruct) == 16, "Incorrect size for type `fontMemStruct`. Expected: 16");

struct TileTypeFlags
{
  u8 buildability;
  u8 height;
};
static_assert(sizeof(TileTypeFlags) == 2, "Incorrect size for type `TileTypeFlags`. Expected: 2");

struct __declspec(align(4)) struct_a1
{
  void (__stdcall *pfunc0)(_DWORD, _DWORD, _DWORD, TileID, RECT *);
  int (__stdcall *isCreepCovered)(_DWORD, _DWORD);
  int (__stdcall *isTileVisible)(int, int);
  int (__stdcall *pfuncC)(_DWORD);
};
static_assert(sizeof(struct_a1) == 16, "Incorrect size for type `struct_a1`. Expected: 16");

struct MiniTileFlagArray
{
  u16 miniTile[16];
};
static_assert(sizeof(MiniTileFlagArray) == 32, "Incorrect size for type `MiniTileFlagArray`. Expected: 32");

struct Action
{
  u32 location;
  u32 string;
  u32 wavString;
  u32 time;
  u32 player;
  u32 number;
  u16 unit;
  u8 action;
  u8 number2;
  u8 flags;
};
static_assert(sizeof(Action) == 32, "Incorrect size for type `Action`. Expected: 32");

struct StringTbl
{
  u16 *buffer;
};
static_assert(sizeof(StringTbl) == 4, "Incorrect size for type `StringTbl`. Expected: 4");

struct MapSize
{
  u16 width;
  u16 height;
};
static_assert(sizeof(MapSize) == 4, "Incorrect size for type `MapSize`. Expected: 4");

#pragma pack(push, 4)
struct __declspec(align(4)) UpdateFunction
{
  RLEType index;
  void (__fastcall *update_function)(CImage *);
};
#pragma pack(pop)
static_assert(sizeof(UpdateFunction) == 8, "Incorrect size for type `UpdateFunction`. Expected: 8");

#pragma pack(push, 4)
struct __declspec(align(4)) RenderFunction
{
  RLEType index;
  void (__fastcall *RenderFunction1)(int, int, grpFrame *, rect *, int);
  void (__fastcall *RenderFunction2)(int, int, grpFrame *, rect *, int);
};
#pragma pack(pop)
static_assert(sizeof(RenderFunction) == 12, "Incorrect size for type `RenderFunction`. Expected: 12");

struct Box32
{
  s32 left;
  s32 top;
  s32 right;
  s32 bottom;
};
static_assert(sizeof(Box32) == 16, "Incorrect size for type `Box32`. Expected: 16");

struct Condition
{
  DWORD dwLocation;
  DWORD dwGroup;
  DWORD dwCount;
  WORD wUnitType;
  BYTE bComparisonType;
  BYTE bConditionType;
  BYTE bExtraType;
  BYTE bFlags;
  BYTE __bAlign[2];
};
static_assert(sizeof(Condition) == 20, "Incorrect size for type `Condition`. Expected: 20");

struct EstablishingShot
{
  const char *establishing_shot_name;
  MapData campaign_mission;
};
static_assert(sizeof(EstablishingShot) == 8, "Incorrect size for type `EstablishingShot`. Expected: 8");

struct s_evt
{
  DWORD dwFlags;
  DWORD dwPlayerId;
  BYTE *pData;
  DWORD dwSize;
};
static_assert(sizeof(s_evt) == 16, "Incorrect size for type `s_evt`. Expected: 16");

struct LO_Header
{
  u32 frameCount;
  u32 overlayCount;
  u32 frameOffsets[1];
};
static_assert(sizeof(LO_Header) == 12, "Incorrect size for type `LO_Header`. Expected: 12");

#pragma pack(push, 1)
struct __declspec(align(2)) ColorShiftData
{
  u32 index;
  void *data;
  char name[12];
};
#pragma pack(pop)
static_assert(sizeof(ColorShiftData) == 20, "Incorrect size for type `ColorShiftData`. Expected: 20");

#pragma pack(push, 1)
struct __declspec(align(2)) AiCaptain
{
  u16 region;
  u16 unknown_0x2;
  s8 playerId;
  u8 captainType;
  u8 unknown_0x6;
  u8 unknown_0x7;
  u8 captainFlags;
  u8 unknown_0x9;
  u8 unknown_0xA;
  u8 unknown_0xB;
  u16 unknown_0xC;
  u16 unknown_0xE;
  u16 regionGndStrength;
  u16 regionAirStrength;
  u16 fullGndStrength;
  u16 fullAirStrength;
  u16 unknown_0x18;
  u16 unknown_0x1A;
  CUnit *unknown_0x1C;
  CUnit *unknown_0x20;
  CUnit *slowestUnit;
  CUnit *followTarget;
  CUnit *mainMedic;
  void *town;
};
#pragma pack(pop)
static_assert(sizeof(AiCaptain) == 52, "Incorrect size for type `AiCaptain`. Expected: 52");

#pragma pack(push, 1)
struct __declspec(align(1)) GotFileValues
{
  GameType template_id;
  u8 unused1;
  u16 variation_id;
  int variation_value;
  VictoryConditions victory_conditions;
  ResourceType resource_type;
  UnitStats unit_stats;
  FogOfWar fog;
  StartingUnits starting_units;
  StartingPositions starting_positions;
  PlayerTypes player_types;
  u8 allies_allowed;
  u8 team_mode;
  u8 cheats;
  u8 tournament_mode;
  u32 victory_condition_value;
  u32 resource_type_value;
  u8 unused3[5];
};
#pragma pack(pop)
static_assert(sizeof(GotFileValues) == 32, "Incorrect size for type `GotFileValues`. Expected: 32");

#pragma pack(push, 1)
struct struct_6
{
  IDirectSoundBuffer *sound_buffer;
  __int16 unit_type;
  char sfxdata_flags_2;
  char sfxdata_flags_1;
  int field_8;
  SfxData sfx_id;
  int field_10;
  int volume_related_maybe;
};
#pragma pack(pop)
static_assert(sizeof(struct_6) == 24, "Incorrect size for type `struct_6`. Expected: 24");

#pragma pack(push, 1)
struct __declspec(align(2)) LobbySlot
{
  int field0;
  int field1;
  char field2[4];
  int name;
  int field4;
  int field5;
  int field6;
  int field7;
  int field8;
  int field9;
  int field10;
  int field11;
  int field12;
  int field13;
  int field14;
  int field15;
  int field16;
  int field17;
  int field18;
  int field19;
  int field20;
  char field21;
  char field211;
  char field212;
  char field213;
  int field22;
  __int16 field231;
  __int16 rect_left;
  __int16 rect_top;
  __int16 field242;
  int field25;
  int field26;
  int field27;
  RaceId race;
  char field281;
  char field282;
  char field283;
  int field29;
  int field30;
  int field31;
  int field32;
  int field33;
  int field34;
  int field35;
  int field36;
  int field37;
  int field38;
  int field39;
  int field40;
  int field41;
  int field42;
  dialog *some_dialog;
};
#pragma pack(pop)
static_assert(sizeof(LobbySlot) == 176, "Incorrect size for type `LobbySlot`. Expected: 176");

typedef void (__fastcall *FnAllocBackgroundImage)(const char *fileName, Bitmap *a2, PALETTEENTRY *palette, const char *source_filename, int source_line);

struct __declspec(align(4)) MapChunks
{
  u16 tbl_index_title;
  u16 tbl_index_description;
  u8 player_force[8];
  u16 tbl_index_force_name[4];
  ForceFlags force_flags[4];
  int version;
  int data7;
};
static_assert(sizeof(MapChunks) == 32, "Incorrect size for type `MapChunks`. Expected: 32");

union Char4
{
  unsigned __int32 as_number;
  Char4_characters as_characters;
};
static_assert(sizeof(Char4) == 4, "Incorrect size for type `Char4`. Expected: 4");

struct Box16
{
  u16 left;
  u16 top;
  u16 right;
  u16 bottom;
};
static_assert(sizeof(Box16) == 8, "Incorrect size for type `Box16`. Expected: 8");

struct SaiContour
{
  s16 v[3];
  u8 type;
  u8 unk_relation;
};
static_assert(sizeof(SaiContour) == 8, "Incorrect size for type `SaiContour`. Expected: 8");

struct __declspec(align(4)) SaiContourHub
{
  SaiContour *contours[4];
  s16 contourCount[4];
  s16 contourMax[4];
  s16 searchInner[4];
  s16 a;
  s16 b;
  s16 c;
  s16 d;
  s16 searchOuter[4];
};
static_assert(sizeof(SaiContourHub) == 56, "Incorrect size for type `SaiContourHub`. Expected: 56");

struct SaiSplit
{
  u16 minitileMask;
  u16 rgn1;
  u16 rgn2;
};
static_assert(sizeof(SaiSplit) == 6, "Incorrect size for type `SaiSplit`. Expected: 6");

struct UnknownTilesetRelated2
{
  PALETTEENTRY *advanced_cycle_data;
  unsigned __int8 has_next;
  _BYTE palette_entry_low;
  _BYTE palette_entry_high;
  _BYTE adv_cycle_count;
};
static_assert(sizeof(UnknownTilesetRelated2) == 8, "Incorrect size for type `UnknownTilesetRelated2`. Expected: 8");

struct MinimapSurfaceInfoRelated
{
  u8 a0;
  u8 a1;
  u8 a2;
  u8 a3;
  int b;
  int c;
  __int16 d;
  __int16 e;
};
static_assert(sizeof(MinimapSurfaceInfoRelated) == 16, "Incorrect size for type `MinimapSurfaceInfoRelated`. Expected: 16");

struct CycleStruct
{
  BYTE active;
  BYTE speed;
  BYTE wait;
  BYTE palette_entry_low;
  BYTE adv_cycle_pos;
  BYTE palette_entry_high;
  PALETTEENTRY *advanced_cycle_data;
  BYTE adv_cycle_count;
};
static_assert(sizeof(CycleStruct) == 16, "Incorrect size for type `CycleStruct`. Expected: 16");

struct __declspec(align(4)) SFX_related
{
  int f1;
  STREAMED head_maybe;
};
static_assert(sizeof(SFX_related) == 16, "Incorrect size for type `SFX_related`. Expected: 16");

struct __declspec(align(2)) CampaignMenuEntry
{
  unsigned __int16 glu_hist_tbl_index;
  MapData next_mission;
  Cinematic cinematic;
  char _padding0;
  RaceId race;
  bool hide;
};
static_assert(sizeof(CampaignMenuEntry) == 8, "Incorrect size for type `CampaignMenuEntry`. Expected: 8");

struct CinematicIntro
{
  Cinematic actual_cinematic;
  Cinematic intro_cinematic;
};
static_assert(sizeof(CinematicIntro) == 2, "Incorrect size for type `CinematicIntro`. Expected: 2");

struct __declspec(align(4)) MissionCheatRelated
{
  const char *campaign_id;
  RaceId race;
  _DWORD first_mission_index;
  _DWORD is_expansion;
};
static_assert(sizeof(MissionCheatRelated) == 16, "Incorrect size for type `MissionCheatRelated`. Expected: 16");

struct __declspec(align(2)) CheatHashMaybe
{
  u32 parts[4];
};
static_assert(sizeof(CheatHashMaybe) == 16, "Incorrect size for type `CheatHashMaybe`. Expected: 16");

struct __declspec(align(4)) GlueRelated
{
  int index;
  MenuPosition menu_position;
  char *path;
};
static_assert(sizeof(GlueRelated) == 12, "Incorrect size for type `GlueRelated`. Expected: 12");

struct UnitProperties
{
  UnitGroupFlags valid_group_flags;
  unsigned __int16 can_be_owned : 1;
  unsigned __int16 has_hp : 1;
  unsigned __int16 has_shields : 1;
  unsigned __int16 has_energy : 1;
  unsigned __int16 has_resource_amount : 1;
  unsigned __int16 has_hanger : 1;
  unsigned __int16 unused2 : 10;
  u8 player;
  u8 hp_percentage;
  u8 shield_percentage;
  u8 energy_percentage;
  u32 resource_amount;
  u16 hangar_count;
  UnitGroupFlags group_flags;
  u32 unused4;
};
static_assert(sizeof(UnitProperties) == 20, "Incorrect size for type `UnitProperties`. Expected: 20");

struct __declspec(align(2)) MusicTrackDescription
{
  char *wav_filename;
  MusicTrackType track_type;
  char fade_in_maybe;
  u8 next_music_track;
};
static_assert(sizeof(MusicTrackDescription) == 8, "Incorrect size for type `MusicTrackDescription`. Expected: 8");

struct __declspec(align(2)) RegistryOptions
{
  int game_speed;
  int mouse_scroll_speed;
  int key_scroll_speed;
  int music;
  int sfx;
  int tip;
  RegistryOptionsFlags flags;
  int MMouseScrollSpeed;
  int MKeyScrollSpeed;
  u8 field_24;
  u8 _unused;
  u8 field_26;
};
static_assert(sizeof(RegistryOptions) == 40, "Incorrect size for type `RegistryOptions`. Expected: 40");

struct ReplayPlayer
{
  int identifier;
  int slot;
  PlayerType player_type;
  RaceId race;
  u8 force_id;
  char name[25];
};
static_assert(sizeof(ReplayPlayer) == 36, "Incorrect size for type `ReplayPlayer`. Expected: 36");

struct __declspec(align(2)) TypeDropdownSelect
{
  PlayerType player_type;
  GluAllTblEntry tbl_entry;
};
static_assert(sizeof(TypeDropdownSelect) == 4, "Incorrect size for type `TypeDropdownSelect`. Expected: 4");

struct RaceDropdownSelect
{
  RaceId race;
  u8 f2;
};
static_assert(sizeof(RaceDropdownSelect) == 2, "Incorrect size for type `RaceDropdownSelect`. Expected: 2");

struct __declspec(align(4)) ButtonOrder
{
  u16 position;
  Icon icon_id;
  ButtonState (__fastcall *condition)(u16 variable, int player_id, CUnit *unit);
  void (__fastcall *action)(int, bool);
  u16 condition_variable;
  u16 action_variable;
  u16 condition_string_id;
  u16 action_string_id;
};
static_assert(sizeof(ButtonOrder) == 20, "Incorrect size for type `ButtonOrder`. Expected: 20");

struct ID
{
  ID *prev;
  ID *next;
  CharacterData data;
};
static_assert(sizeof(ID) == 120, "Incorrect size for type `ID`. Expected: 120");

struct struc_581D76
{
  u8 colors[8];
};
static_assert(sizeof(struc_581D76) == 8, "Incorrect size for type `struc_581D76`. Expected: 8");

#pragma pack(push, 1)
struct __declspec(align(1)) ReplaySpeedCommand
{
  CommandId command_id;
  byte is_paused;
  int game_speed;
  int replay_speed_multiplier;
};
#pragma pack(pop)
static_assert(sizeof(ReplaySpeedCommand) == 10, "Incorrect size for type `ReplaySpeedCommand`. Expected: 10");

#pragma pack(push, 1)
struct __declspec(align(1)) SaveGameCommand
{
  u8 command_id;
  int time;
  char filename[28];
};
#pragma pack(pop)
static_assert(sizeof(SaveGameCommand) == 33, "Incorrect size for type `SaveGameCommand`. Expected: 33");

struct CancelNukeCommand
{
  CommandId command_id;
};
static_assert(sizeof(CancelNukeCommand) == 1, "Incorrect size for type `CancelNukeCommand`. Expected: 1");

struct MergeDarkArchonCommand
{
  CommandId command_id;
};
static_assert(sizeof(MergeDarkArchonCommand) == 1, "Incorrect size for type `MergeDarkArchonCommand`. Expected: 1");

struct MergeArchonCommand
{
  CommandId command_id;
};
static_assert(sizeof(MergeArchonCommand) == 1, "Incorrect size for type `MergeArchonCommand`. Expected: 1");

struct DecloakCommand
{
  CommandId command_id;
  bool is_shift_used;
};
static_assert(sizeof(DecloakCommand) == 2, "Incorrect size for type `DecloakCommand`. Expected: 2");

struct UnburrowCommand
{
  CommandId command_id;
  bool is_shift_used;
};
static_assert(sizeof(UnburrowCommand) == 2, "Incorrect size for type `UnburrowCommand`. Expected: 2");

struct BurrowCommand
{
  CommandId command_id;
  bool is_shift_used;
};
static_assert(sizeof(BurrowCommand) == 2, "Incorrect size for type `BurrowCommand`. Expected: 2");

struct CancelAddonCommand
{
  CommandId command_id;
};
static_assert(sizeof(CancelAddonCommand) == 1, "Incorrect size for type `CancelAddonCommand`. Expected: 1");

struct CancelUpgradeCommand
{
  CommandId command_id;
};
static_assert(sizeof(CancelUpgradeCommand) == 1, "Incorrect size for type `CancelUpgradeCommand`. Expected: 1");

struct UpgradeCommand
{
  CommandId command_id;
  char action_variable;
};
static_assert(sizeof(UpgradeCommand) == 2, "Incorrect size for type `UpgradeCommand`. Expected: 2");

struct CancelResearchCommand
{
  CommandId command_id;
};
static_assert(sizeof(CancelResearchCommand) == 1, "Incorrect size for type `CancelResearchCommand`. Expected: 1");

struct ResearchCommand
{
  CommandId command_id;
  char action_variable;
};
static_assert(sizeof(ResearchCommand) == 2, "Incorrect size for type `ResearchCommand`. Expected: 2");

struct HoldPositionCommand
{
  CommandId command_id;
  bool is_shift_used;
};
static_assert(sizeof(HoldPositionCommand) == 2, "Incorrect size for type `HoldPositionCommand`. Expected: 2");

struct TrainFighterCommand
{
  CommandId command_id;
};
static_assert(sizeof(TrainFighterCommand) == 1, "Incorrect size for type `TrainFighterCommand`. Expected: 1");

struct CarrierStopCommand
{
  CommandId command_id;
};
static_assert(sizeof(CarrierStopCommand) == 1, "Incorrect size for type `CarrierStopCommand`. Expected: 1");

struct ReaverStopCommand
{
  CommandId command_id;
};
static_assert(sizeof(ReaverStopCommand) == 1, "Incorrect size for type `ReaverStopCommand`. Expected: 1");

struct StopCommand
{
  CommandId command_id;
  bool is_shift_used;
};
static_assert(sizeof(StopCommand) == 2, "Incorrect size for type `StopCommand`. Expected: 2");

struct CancelUnitMorphCommand
{
  CommandId command_id;
};
static_assert(sizeof(CancelUnitMorphCommand) == 1, "Incorrect size for type `CancelUnitMorphCommand`. Expected: 1");

struct CancelConstructionCommand
{
  CommandId command_id;
};
static_assert(sizeof(CancelConstructionCommand) == 1, "Incorrect size for type `CancelConstructionCommand`. Expected: 1");

struct SiegeCommand
{
  CommandId command_id;
  bool is_shift_used;
};
static_assert(sizeof(SiegeCommand) == 2, "Incorrect size for type `SiegeCommand`. Expected: 2");

struct UnsiegeCommand
{
  CommandId command_id;
  bool is_shift_used;
};
static_assert(sizeof(UnsiegeCommand) == 2, "Incorrect size for type `UnsiegeCommand`. Expected: 2");

#pragma pack(push, 1)
struct __declspec(align(1)) CancelTrainCommand
{
  CommandId command_id;
  __int16 action_variable;
};
#pragma pack(pop)
static_assert(sizeof(CancelTrainCommand) == 3, "Incorrect size for type `CancelTrainCommand`. Expected: 3");

#pragma pack(push, 1)
struct __declspec(align(1)) TrainCommand
{
  CommandId command_id;
  __int16 action_variable;
};
#pragma pack(pop)
static_assert(sizeof(TrainCommand) == 3, "Incorrect size for type `TrainCommand`. Expected: 3");

struct StimpackCommand
{
  CommandId command_id;
};
static_assert(sizeof(StimpackCommand) == 1, "Incorrect size for type `StimpackCommand`. Expected: 1");

struct CloakCommand
{
  CommandId command_id;
  bool is_shift_used;
};
static_assert(sizeof(CloakCommand) == 2, "Incorrect size for type `CloakCommand`. Expected: 2");

struct ReturnCargoCommand
{
  CommandId command_id;
  bool is_shift_used;
};
static_assert(sizeof(ReturnCargoCommand) == 2, "Incorrect size for type `ReturnCargoCommand`. Expected: 2");

#pragma pack(push, 1)
struct __declspec(align(1)) MorphCommand
{
  CommandId command_id;
  UnitType unit_type;
};
#pragma pack(pop)
static_assert(sizeof(MorphCommand) == 3, "Incorrect size for type `MorphCommand`. Expected: 3");

#pragma pack(push, 1)
struct __declspec(align(1)) BuildingMorphCommand
{
  CommandId command_id;
  UnitType unit_type;
};
#pragma pack(pop)
static_assert(sizeof(BuildingMorphCommand) == 3, "Incorrect size for type `BuildingMorphCommand`. Expected: 3");

struct UnloadAllCommand
{
  CommandId command_id;
  bool is_shift_used;
};
static_assert(sizeof(UnloadAllCommand) == 2, "Incorrect size for type `UnloadAllCommand`. Expected: 2");

#pragma pack(push, 1)
struct RightClickCommand
{
  char command_id;
  __int16 x;
  __int16 y;
  __int16 target_id;
  UnitType unit_type;
  char is_shift_used;
};
#pragma pack(pop)
static_assert(sizeof(RightClickCommand) == 10, "Incorrect size for type `RightClickCommand`. Expected: 10");

struct SetLatencyCommand
{
  CommandId command_id;
  u8 latency;
};
static_assert(sizeof(SetLatencyCommand) == 2, "Incorrect size for type `SetLatencyCommand`. Expected: 2");

struct RestartGameCommand
{
  CommandId command_id;
};
static_assert(sizeof(RestartGameCommand) == 1, "Incorrect size for type `RestartGameCommand`. Expected: 1");

struct TimerRelated
{
  swishTimer *timers;
  __int16 active_timers_count_maybe;
  char field_6[2];
  swishTimer active_timers_maybe[20];
};
static_assert(sizeof(TimerRelated) == 88, "Incorrect size for type `TimerRelated`. Expected: 88");

struct UnknownPathRelated
{
  __int16 f0[8];
  BYTE f1[8];
};
static_assert(sizeof(UnknownPathRelated) == 24, "Incorrect size for type `UnknownPathRelated`. Expected: 24");

struct BytePair
{
  BYTE x;
  BYTE y;
};
static_assert(sizeof(BytePair) == 2, "Incorrect size for type `BytePair`. Expected: 2");

struct IScript
{
  int size_maybe;
  u8 data[];
};
static_assert(sizeof(IScript) == 4, "Incorrect size for type `IScript`. Expected: 4");

struct IScriptProgram
{
  u32 scpe_magic;
  u16 headers[];
};
static_assert(sizeof(IScriptProgram) == 4, "Incorrect size for type `IScriptProgram`. Expected: 4");

#pragma pack(push, 1)
struct __declspec(align(1)) MinimapPingCommand
{
  CommandId command_id;
  __int16 x;
  __int16 y;
};
#pragma pack(pop)
static_assert(sizeof(MinimapPingCommand) == 5, "Incorrect size for type `MinimapPingCommand`. Expected: 5");

#pragma pack(push, 1)
struct __declspec(align(1)) GroupUnitsCommand
{
  CommandId command_id;
  char f1;
  char f2;
};
#pragma pack(pop)
static_assert(sizeof(GroupUnitsCommand) == 3, "Incorrect size for type `GroupUnitsCommand`. Expected: 3");

struct UnitGroupRelatedInner
{
  int unk0;
  int *unk1;
  BYTE unk[40];
};
static_assert(sizeof(UnitGroupRelatedInner) == 48, "Incorrect size for type `UnitGroupRelatedInner`. Expected: 48");

struct TSAVEGAME
{
  ListNode node;
  int unk0;
  char unk[52];
};
static_assert(sizeof(TSAVEGAME) == 64, "Incorrect size for type `TSAVEGAME`. Expected: 64");

struct GlueEffectBuffer
{
  BYTE data[16];
};
static_assert(sizeof(GlueEffectBuffer) == 16, "Incorrect size for type `GlueEffectBuffer`. Expected: 16");

struct StatDataDescriptor
{
  CUnit *xxx;
  _WORD word4;
  StatDataDescriptorType type;
  u16 value;
};
static_assert(sizeof(StatDataDescriptor) == 12, "Incorrect size for type `StatDataDescriptor`. Expected: 12");

struct ParallaxData
{
  u16 layer_count;
  u16 layers[5];
};
static_assert(sizeof(ParallaxData) == 12, "Incorrect size for type `ParallaxData`. Expected: 12");

struct ParallaxLayerData
{
  u16 data1;
  u16 data2;
  ParallaxItem *unknown_offset;
};
static_assert(sizeof(ParallaxLayerData) == 8, "Incorrect size for type `ParallaxLayerData`. Expected: 8");

struct ParallaxItem
{
  u16 width;
  u16 height;
  u8 data[];
};
static_assert(sizeof(ParallaxItem) == 4, "Incorrect size for type `ParallaxItem`. Expected: 4");

typedef struct _EH3_EXCEPTION_REGISTRATION EH3_EXCEPTION_REGISTRATION;

struct CPPEH_RECORD
{
  DWORD old_esp;
  EXCEPTION_POINTERS *exc_ptr;
  struct _EH3_EXCEPTION_REGISTRATION registration;
};
static_assert(sizeof(CPPEH_RECORD) == 24, "Incorrect size for type `CPPEH_RECORD`. Expected: 24");

struct dialog_btn
{
  dialog *pDlg;
  rect responseRct;
  u16 wHighlight;
  u16 wUnknown_0x40;
  void *pSmk;
  rect textRct;
  u16 wAlignment;
};
static_assert(sizeof(dialog_btn) == 32, "Incorrect size for type `dialog_btn`. Expected: 32");

#pragma pack(push, 1)
struct __declspec(align(1)) dialog_list
{
  dialog *pDlg;
  dialog *pScrlBar;
  char **ppStrs;
  u8 *pbStrFlags;
  u32 *pdwData;
  u8 bStrs;
  u8 unknown_0x47;
  u8 bCurrStr;
  u8 bSpacing;
  u8 bItemsPerPage;
  u8 bUnknown_0x4B;
  u8 bDirection;
  u8 bOffset;
  u8 bSelectedIndex;
  u8 bUnknown_0x4F;
  u16 wVerticalOffset;
  FnDrawItem pDrawItemFcn;
};
#pragma pack(pop)
static_assert(sizeof(dialog_list) == 36, "Incorrect size for type `dialog_list`. Expected: 36");

struct dlgEvent
{
  EventUser dwUser;
  u16 wSelection;
  u16 wUnk_0x06;
  u16 wVirtKey;
  u16 wUnk_0x0A;
  EventNo wNo;
  pt cursor;
  u16 wUnk_0x12;
};
static_assert(sizeof(dlgEvent) == 20, "Incorrect size for type `dlgEvent`. Expected: 20");

struct __declspec(align(4)) CSprite
{
  CSprite *prev;
  CSprite *next;
  u16 spriteID;
  u8 playerID;
  u8 selectionIndex;
  u8 visibilityFlags;
  u8 elevationLevel;
  CSpriteFlags flags;
  u8 selectionTimer;
  u16 index;
  u8 unkflags_12;
  u8 unkflags_13;
  Position position;
  CImage *pImagePrimary;
  CImage *pImageHead;
  CImage *pImageTail;
};
static_assert(sizeof(CSprite) == 36, "Incorrect size for type `CSprite`. Expected: 36");

struct __declspec(align(4)) CImage
{
  CImage *prev;
  CImage *next;
  u16 imageID;
  RLEType paletteType;
  u8 direction;
  ImageFlags flags;
  s8 horizontalOffset;
  s8 verticalOffset;
  IScriptProgramState iscript_program;
  u16 frameSet;
  u16 frameIndex;
  Position mapPosition;
  Position screenPosition;
  rect grpBounds;
  grpHead *GRPFile;
  void *coloringData;
  void (__fastcall *renderFunction)(int, int, grpFrame *, rect *, int);
  void (__fastcall *updateFunction)(CImage *);
  CSprite *spriteOwner;
};
static_assert(sizeof(CImage) == 64, "Incorrect size for type `CImage`. Expected: 64");

#pragma pack(push, 2)
struct __declspec(align(2)) grpHead
{
  u16 wFrames;
  s16 width;
  s16 height;
  grpFrame frames[1];
};
#pragma pack(pop)
static_assert(sizeof(grpHead) == 14, "Incorrect size for type `grpHead`. Expected: 14");

struct __declspec(align(4)) COrder
{
  struct COrder *prev;
  struct COrder *next;
  Order orderID;
  u16 unitType;
  Target_ target;
};
static_assert(sizeof(COrder) == 20, "Incorrect size for type `COrder`. Expected: 20");

struct CUnitHatchery
{
  rect harvestValue;
};
static_assert(sizeof(CUnitHatchery) == 8, "Incorrect size for type `CUnitHatchery`. Expected: 8");

union CUnitFields1
{
  CUnitVulture vulture;
  CUnitCarrier carrier;
  CUnitFighter fighter;
  CUnitBeacon beacon;
  CUnitBuilding building;
  CUnitWorker worker;
};
static_assert(sizeof(CUnitFields1) == 16, "Incorrect size for type `CUnitFields1`. Expected: 16");

union CUnitFields3
{
  CUnitRally rally;
  CUnitPsiProvider psyProvider;
};
static_assert(sizeof(CUnitFields3) == 8, "Incorrect size for type `CUnitFields3`. Expected: 8");

struct AllScoresStruct
{
  s32 allUnitsTotal[12];
  s32 allUnitsProduced[12];
  s32 allUnitsOwned[12];
  s32 allUnitsLost[12];
  s32 allUnitsKilled[12];
  s32 allUnitScore[12];
  s32 allKillScore[12];
  s32 allBuildingsTotal[12];
  s32 allBuildingsConstructed[12];
  s32 allBuildingsOwned[12];
  s32 allBuildingsLost[12];
  s32 allBuildingsRazed[12];
  s32 allBuildingScore[12];
  s32 allRazingScore[12];
  s32 allFactoriesConstructed[12];
  s32 allFactoriesOwned[12];
  s32 allFactoriesLost[12];
  s32 allFactoriesRazed[12];
  SuppliesPerRace supplies[3];
  s32 customScore[12];
  Counts unitCounts;
};
static_assert(sizeof(AllScoresStruct) == 45120, "Incorrect size for type `AllScoresStruct`. Expected: 45120");

struct __declspec(align(4)) baseLocation
{
  Position position;
  BYTE mineralClusters;
  BYTE gasGeysers;
  BYTE isStartLocation;
  BYTE bFlags;
  DWORD remainingMinerals;
  DWORD remainingGas;
  DWORD unk_10[8];
};
static_assert(sizeof(baseLocation) == 48, "Incorrect size for type `baseLocation`. Expected: 48");

struct CBullet
{
  CBullet *prev;
  CBullet *next;
  int exists;
  CSprite *sprite;
  Target_ moveTarget;
  Position targetPosition;
  Position targetPosition2;
  u8 movementFlags;
  u8 direction1;
  u8 flingyTurnRadius;
  u8 direction2;
  u16 type;
  u8 unknown_0x26;
  u8 flingyMoveControl;
  Position position;
  point halt;
  s32 flingyTopSpeed;
  s32 unknown_0x38;
  s32 unknown_0x3C;
  point current_speed;
  u16 flingyAcceleration;
  u8 currentDirection;
  u8 velocityDirection;
  u8 srcPlayer;
  BulletState behaviourTypeInternal;
  u8 unknown_0x4E;
  u8 unknown_0x4F;
  u16 someUnitType;
  u16 unknown_0x52;
  s32 unknown_0x54;
  Target_ attackTarget;
  WeaponType weaponType;
  u8 time_remaining;
  u8 hitFlags;
  u8 remainingBounces;
  CUnit *sourceUnit;
  CUnit *nextBounceUnit;
  u32 cyclicMissileIndex;
};
static_assert(sizeof(CBullet) == 112, "Incorrect size for type `CBullet`. Expected: 112");

struct TileType
{
  u16 index;
  TileTypeFlags flags;
  u16 leftEdge;
  u16 topEdge;
  u16 rightEdge;
  u16 bottomEdge;
  u16 _1;
  u16 _2;
  u16 _3;
  u16 _4;
  u16 megaTileRef[16];
};
static_assert(sizeof(TileType) == 52, "Incorrect size for type `TileType`. Expected: 52");

struct MiniTileMaps_type
{
  MiniTileFlagArray tile[65536];
};
static_assert(sizeof(MiniTileMaps_type) == 2097152, "Incorrect size for type `MiniTileMaps_type`. Expected: 2097152");

#pragma pack(push, 1)
struct __declspec(align(2)) AI_Main
{
  s32 oreCollection;
  s32 gasCollection;
  s32 supplyCollection;
  s32 ore;
  s32 gas;
  s32 supply;
  u8 unknown_0x18;
  u8 newBuildType;
  UnitType nextBuildType;
  AiCaptain *pTownMain;
  u32 unknown_0x20[124];
  u8 unknown_0x210;
  u8 builtSomething;
  u8 AI_NukeRate;
  u8 AI_Attacks;
  u32 AI_LastNukeTime;
  AI_Flags flags;
  u16 AI_PanicBlock;
  u16 AI_MaxForce;
  u16 AI_AttackGroup;
  u16 waitForceCount;
  u8 AI_DefaultMin;
  u8 unknown_0x223;
  u32 lastIndividualUpdateTime;
  u32 AI_AttackTimer;
  u8 unknown_0x22C;
  u8 spellcasterTimer;
  u8 attackManagerTimer;
  u8 AI_IfDif;
  u16 AI_AttackGroups[64];
  u32 AI_DefenseBuild_GG[10];
  u32 AI_DefenseBuild_AG[10];
  u32 AI_DefenseBuild_GA[10];
  u32 AI_DefenseBuild_AA[10];
  u32 AI_DefenseUse_GG[10];
  u32 AI_DefenseUse_AG[10];
  u32 AI_DefenseUse_GA[10];
  u32 AI_DefenseUse_AA[10];
  u8 AI_DefineMax[228];
  CUnit *mainMedic;
  Box32 genCmdLoc;
};
#pragma pack(pop)
static_assert(sizeof(AI_Main) == 1256, "Incorrect size for type `AI_Main`. Expected: 1256");

struct Trigger
{
  Condition conditions[16];
  Action actions[64];
  DWORD dwExecutionFlags;
  BYTE bExecuteFor[27];
  BYTE bCurrentActionIndex;
};
static_assert(sizeof(Trigger) == 2400, "Incorrect size for type `Trigger`. Expected: 2400");

struct Location
{
  Box32 dimensions;
  u16 stringId;
  u16 flags;
};
static_assert(sizeof(Location) == 20, "Incorrect size for type `Location`. Expected: 20");

struct GotFile
{
  u8 version;
  char name[32];
  char label[32];
  GotFileValues values;
};
static_assert(sizeof(GotFile) == 97, "Incorrect size for type `GotFile`. Expected: 97");

#pragma pack(push, 1)
struct __declspec(align(1)) GameData
{
  int data;
  char player_name[24];
  int save_timestamp;
  __int16 width;
  __int16 height;
  char active_human_players;
  char max_players;
  char game_speed;
  char approval_status;
  GameType game_type;
  char game_type_unk;
  __int16 game_type_param;
  int cdkey_hash;
  Tileset tileset;
  BYTE is_replay;
  BYTE active_computer_players;
  char host_name[25];
  char map_name[32];
  GotFileValues got_file_values;
};
#pragma pack(pop)
static_assert(sizeof(GameData) == 141, "Incorrect size for type `GameData`. Expected: 141");

struct __declspec(align(4)) Map
{
  MapChunks chunks;
};
static_assert(sizeof(Map) == 32, "Incorrect size for type `Map`. Expected: 32");

struct __declspec(align(4)) SectionData
{
  byte *next_section;
  byte *start_address;
  Char4 chunk_name;
  int size;
};
static_assert(sizeof(SectionData) == 16, "Incorrect size for type `SectionData`. Expected: 16");

struct SaiRegion
{
  SaiAccessabilityFlags accessabilityFlags;
  u16 groupIndex;
  u16 tileCount;
  u8 pathCount;
  u8 neighborCount;
  union SaiRegionUser user;
  u16 *neighbors;
  u32 rgnCenterX;
  u32 rgnCenterY;
  Box16 rgnBox;
  u8 defencePriority;
  u8 neighborProperty;
  u16 unk_22;
  u32 unk_24;
  u32 unk_28;
  u16 localBuffer[10];
};
static_assert(sizeof(SaiRegion) == 64, "Incorrect size for type `SaiRegion`. Expected: 64");

#pragma pack(push, 1)
struct __declspec(align(2)) PathCreateRelated
{
  Position position;
  MapSize map_size;
};
#pragma pack(pop)
static_assert(sizeof(PathCreateRelated) == 8, "Incorrect size for type `PathCreateRelated`. Expected: 8");

struct UnknownTilesetRelated1
{
  int x;
  UnknownTilesetRelated2 y[];
};
static_assert(sizeof(UnknownTilesetRelated1) == 4, "Incorrect size for type `UnknownTilesetRelated1`. Expected: 4");

struct __declspec(align(4)) CFlingy
{
  CUnit *prev;
  CUnit *next;
  int hitPoints;
  CSprite *sprite;
  Target_ moveTarget;
  Position nextMovementWaypoint;
  Position nextTargetWaypoint;
  u8 movementFlags;
  u8 currentDirection1;
  u8 flingyTurnRadius;
  u8 velocityDirection1;
  u16 flingyID;
  u8 _unknown_0x026;
  u8 flingyMovementType;
  Position position;
  point halt;
  u32 flingyTopSpeed;
  s32 current_speed1;
  s32 current_speed2;
  point current_speed;
  u16 flingyAcceleration;
  u8 currentDirection2;
  u8 velocityDirection2;
};
static_assert(sizeof(CFlingy) == 76, "Incorrect size for type `CFlingy`. Expected: 76");

struct __declspec(align(4)) TPROVIDER
{
  TPROVIDER *previous;
  TPROVIDER *next;
  _DWORD dword8;
  Char4 provider_id;
  _DWORD dword10;
  char name[84];
  _BYTE gap94[44];
  char description[128];
  char *requirements;
};
static_assert(sizeof(TPROVIDER) == 280, "Incorrect size for type `TPROVIDER`. Expected: 280");

struct CheatHashRelated
{
  CheatHashMaybe f0;
  int f2[9];
};
static_assert(sizeof(CheatHashRelated) == 52, "Incorrect size for type `CheatHashRelated`. Expected: 52");

struct Chunk
{
  Char4 ID;
  unsigned int size;
  byte data[];
};
static_assert(sizeof(Chunk) == 8, "Incorrect size for type `Chunk`. Expected: 8");

struct ChunkUnitEntry
{
  u32 id;
  Position position;
  UnitType unit_type;
  unsigned __int16 unknown0 : 9;
  unsigned __int16 is_nydus_link : 1;
  unsigned __int16 is_addon_link : 1;
  unsigned __int16 unknown1 : 5;
  UnitProperties properties;
  u32 linked_unit_id;
};
static_assert(sizeof(ChunkUnitEntry) == 36, "Incorrect size for type `ChunkUnitEntry`. Expected: 36");

struct BriefingEntry
{
  Condition conditions[16];
  Action actions[64];
  DWORD dwExecutionFlags;
  BYTE bExecuteFor[27];
  BYTE bCurrentActionIndex;
};
static_assert(sizeof(BriefingEntry) == 2400, "Incorrect size for type `BriefingEntry`. Expected: 2400");

struct __declspec(align(4)) struct_a1_1
{
  CUnit *unit;
  Position unk_position1;
  Position unk_position2;
  CUnit *unk_unit;
  rect unk_rect;
  Position unk_posintion3;
  _BYTE byte1C;
  _BYTE byte1D;
  _BYTE byte1E;
  _BYTE byte1F;
  _WORD word20[50];
  Position positions[1];
  _BYTE gap88[200];
  __int16 word150;
  __int16 word152;
  u8 byte154;
  _BYTE byte155;
  _BYTE byte156;
  _BYTE byte157;
  pt word158;
  pt word15C;
  _WORD region1;
  _WORD region2;
  _WORD word164;
  _WORD word166;
  _WORD word168;
  _WORD word16A;
};
static_assert(sizeof(struct_a1_1) == 364, "Incorrect size for type `struct_a1_1`. Expected: 364");

struct __declspec(align(1)) struct_path_related
{
  Position unk_pos1;
  int dword4;
  Position unk_pos2;
  _DWORD dwordC;
  u32 colliding_unit_id;
  int speed;
  _BYTE byte18;
  _BYTE byte19;
  _BYTE gap1A;
  _BYTE byte1B;
  char char1C;
  _BYTE byte1D;
  char char1E;
  _BYTE byte1F;
  int buffer[24];
};
static_assert(sizeof(struct_path_related) == 128, "Incorrect size for type `struct_path_related`. Expected: 128");

struct StatFlufDialog
{
  rect position;
  dialog *dialog;
};
static_assert(sizeof(StatFlufDialog) == 12, "Incorrect size for type `StatFlufDialog`. Expected: 12");

struct TemplateListEntry
{
  TemplateListEntry *prev;
  TemplateListEntry *next;
  char template_name[32];
  char template_label[32];
  GotFileValues got_file_values;
  struct_a4 unk;
};
static_assert(sizeof(TemplateListEntry) == 400, "Incorrect size for type `TemplateListEntry`. Expected: 400");

struct __unaligned __declspec(align(1)) LiftCommand
{
  CommandId command_id;
  Position position;
};
static_assert(sizeof(LiftCommand) == 5, "Incorrect size for type `LiftCommand`. Expected: 5");

struct __declspec(align(2)) PlaceBuildingCommand
{
  CommandId command_id;
  char f2;
  Position position;
  UnitType unit_type;
};
static_assert(sizeof(PlaceBuildingCommand) == 8, "Incorrect size for type `PlaceBuildingCommand`. Expected: 8");

struct __declspec(align(1)) Portrait
{
  Bitmap bitmap;
  HANDLE video;
  _WORD flags;
  _WORD portrait_id;
  _BYTE gap10;
  _BYTE byte11;
  _BYTE byte12;
  _BYTE byte13;
};
static_assert(sizeof(Portrait) == 20, "Incorrect size for type `Portrait`. Expected: 20");

struct UnitGroupRelated
{
  UnitGroupRelatedInner unk[18];
};
static_assert(sizeof(UnitGroupRelated) == 864, "Incorrect size for type `UnitGroupRelated`. Expected: 864");

union dialog_fields
{
  dialog_ctrl ctrl;
  dialog_dlg dlg;
  dialog_btn btn;
  dialog_optn optn;
  dialog_edit edit;
  dialog_scroll scroll;
  dialog_list list;
};
static_assert(sizeof(dialog_fields) == 36, "Incorrect size for type `dialog_fields`. Expected: 36");

union CUnitFields2
{
  CUnitResource resource;
  CUnitNydus nydus;
  CUnitGhost ghost;
  CUnitPylon pylon;
  CUnitSilo silo;
  CUnitHatchery hatchery;
  CUnitPowerup powerup;
  CUnitGatherer gatherer;
};
static_assert(sizeof(CUnitFields2) == 12, "Incorrect size for type `CUnitFields2`. Expected: 12");

struct TriggerListEntry
{
  struct TriggerListEntry *prev;
  struct TriggerListEntry *next;
  Trigger container;
};
static_assert(sizeof(TriggerListEntry) == 2408, "Incorrect size for type `TriggerListEntry`. Expected: 2408");

#pragma pack(push, 1)
struct __declspec(align(2)) ChunkData
{
  ChunkData *previous;
  ChunkData *next;
  SectionData section_data;
};
#pragma pack(pop)
static_assert(sizeof(ChunkData) == 24, "Incorrect size for type `ChunkData`. Expected: 24");

struct SAI_Paths
{
  __int16 regionCount;
  u16 unknown;
  void *globalBuffer_ptr;
  void *splitTiles_end;
  u16 mapTileRegionId[256][256];
  SaiSplit splitTiles[25000];
  SaiRegion regions[5000];
  u16 globalBuffer[10000];
  SaiContourHub *contours;
};
static_assert(sizeof(SAI_Paths) == 621088, "Incorrect size for type `SAI_Paths`. Expected: 621088");

struct __declspec(align(4)) MapDirEntry
{
  struct MapDirEntry *previous;
  struct MapDirEntry *next;
  char name[65];
  char title[32];
  char description[256];
  char unknown[32];
  char unknown7[28];
  char unknown_x478[35];
  char computer_players_string[35];
  char human_players_string[35];
  char map_dimension_string[35];
  char tileset_string[35];
  int listbox_index_maybe;
  int fully_loaded;
  int error;
  int unknown2;
  int save_hash;
  MapDirEntryFlags flags;
  char full_path[260];
  char filename[260];
  __int16 map_width_tiles;
  __int16 map_height_tiles;
  Tileset tileset;
  char unknown3;
  BYTE human_player_slots_maybe;
  BYTE computer_slots;
  BYTE human_player_slots;
  BYTE unknown4[16];
  GameData game_data;
  BYTE unknown5[28];
  BYTE player_forces[8];
  MapData campaign_mission;
  int unknown6;
};
static_assert(sizeof(MapDirEntry) == 1348, "Incorrect size for type `MapDirEntry`. Expected: 1348");

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) ReplayHeader
{
  char is_expansion;
  int ReplayFrames;
  MapData campaign_index;
  struct_v2 seed_related;
  GameData game_data;
  ReplayPlayer players[12];
  int field_251;
  int field_255;
  int field_259;
  int field_25D;
  int field_261;
  int field_265;
  int field_269;
  int field_26D;
  byte playerForce[8];
};
#pragma pack(pop)
static_assert(sizeof(ReplayHeader) == 633, "Incorrect size for type `ReplayHeader`. Expected: 633");

struct __declspec(align(4)) struct_a2
{
  _DWORD fully_loaded;
  _BYTE flags;
  _BYTE gap5[3];
  _DWORD computer_slots;
  _DWORD human_player_slots;
  _DWORD human_player_slots_maybe;
  _DWORD dword14;
  _DWORD dword18;
  _DWORD dword1C;
  _DWORD dword20;
  GameData game_data;
};
static_assert(sizeof(struct_a2) == 180, "Incorrect size for type `struct_a2`. Expected: 180");

#pragma pack(push, 1)
struct __declspec(align(2)) dialog
{
  dialog *pNext;
  rect rct;
  Bitmap srcBits;
  char *pszText;
  DialogFlags lFlags;
  u16 wUnk_0x1C;
  u16 wUnk_0x1E;
  s16 wIndex;
  DialogType wCtrlType;
  u16 wUser;
  void *lUser;
  FnInteract pfcnInteract;
  FnUpdate pfcnUpdate;
  dialog_fields fields;
};
#pragma pack(pop)
static_assert(sizeof(dialog) == 86, "Incorrect size for type `dialog`. Expected: 86");

struct __declspec(align(4)) CUnit
{
  CUnit *prev;
  CUnit *next;
  int hitPoints;
  CSprite *sprite;
  Target_ moveTarget;
  Position nextMovementWaypoint;
  Position nextTargetWaypoint;
  u8 movementFlags;
  u8 currentDirection1;
  u8 flingyTurnRadius;
  u8 velocityDirection1;
  u16 flingyID;
  u8 _unknown_0x026;
  u8 flingyMovementType;
  Position position;
  point halt;
  u32 flingyTopSpeed;
  s32 current_speed1;
  s32 current_speed2;
  point current_speed;
  u16 flingyAcceleration;
  u8 currentDirection2;
  u8 velocityDirection2;
  u8 playerID;
  Order orderID;
  u8 orderState;
  u8 orderSignal;
  u16 orderUnitType;
  u16 __0x52;
  u8 mainOrderTimer;
  u8 groundWeaponCooldown;
  u8 airWeaponCooldown;
  u8 spellCooldown;
  Target_ orderTarget;
  u32 shieldPoints;
  UnitType unitType;
  u16 __0x66;
  CUnit *previousPlayerUnit;
  CUnit *nextPlayerUnit;
  CUnit *subUnit;
  COrder *orderQueueHead;
  COrder *orderQueueTail;
  CUnit *autoTargetUnit;
  CUnit *connectedUnit;
  u8 orderQueueCount;
  u8 orderQueueTimer;
  u8 _unknown_0x086;
  u8 attackNotifyTimer;
  UnitType previousUnitType;
  u8 lastEventTimer;
  u8 lastEventColor;
  u16 _unused_0x08C;
  u8 rankIncrease;
  u8 killCount;
  u8 lastAttackingPlayer;
  u8 secondaryOrderTimer;
  u8 AIActionFlag;
  u8 userActionFlags;
  u16 currentButtonSet;
  bool isCloaked;
  UnitMovementState movementState;
  UnitType buildQueue[5];
  u16 energy;
  u8 buildQueueSlot;
  u8 uniquenessIdentifier;
  Order secondaryOrderID;
  u8 buildingOverlayState;
  u16 hpGain;
  u16 shieldGain;
  u16 remainingBuildTime;
  u16 previousHP;
  u16 loadedUnitIndex[8];
  CUnitFields1 fields1;
  CUnitFields2 fields2;
  StatusFlags statusFlags;
  u8 resourceType;
  u8 wireframeRandomizer;
  u8 secondaryOrderState;
  u8 recentOrderTimer;
  s32 visibilityStatus;
  Position secondaryOrderPosition;
  CUnit *currentBuildUnit;
  CUnit *previousBurrowedUnit;
  CUnit *nextBurrowedUnit;
  CUnitFields3 fields3;
  struct_path_related *path;
  u8 pathingCollisionInterval;
  u8 pathingFlags;
  u8 _unused_0x106;
  bool isBeingHealed;
  rect contourBounds;
  CUnitStatus status;
  CUnitFinder finder;
  u8 _repulseUnknown;
  u8 repulseAngle;
  u8 bRepMtxX;
  u8 bRepMtxY;
};
static_assert(sizeof(CUnit) == 336, "Incorrect size for type `CUnit`. Expected: 336");

}

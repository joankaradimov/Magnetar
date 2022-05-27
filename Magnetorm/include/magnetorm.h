#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ddraw.h>

extern "C"
{

BOOL __stdcall SNetGetProviderCaps(char* caps);
HANDLE __stdcall SNetRegisterEventHandler(int type, void(__stdcall* event)(struct s_evt* evt));
BOOL __stdcall SNetEnumGamesEx(int a1, int a2, int(__fastcall* callback)(DWORD, DWORD, DWORD), int* hintnextcall);
BOOL __stdcall SNetCreateGame(char* Source, char* a2, char* a3, int a4, int a5, int a6, char* a7, char* a8, int* playerid);
BOOL __stdcall SNetDestroy();
BOOL __stdcall SNetCreateLadderGame(char* gameName, char* gamePassword, char* gameDataString, DWORD gameType, int a5, int a6, char* GameTemplateData, int GameTemplateSize, int playerCount, char* playerName, char* a11, int* playerID);
BOOL __stdcall SNetJoinGame(unsigned int a1, char* gameName, char* gamePassword, char* playerName, char* userStats, int* playerid);
BOOL __stdcall SNetInitializeProvider(DWORD providerName, int* providerData, int* userData, int* a4, int* a5);
BOOL __stdcall SNetPerformUpgrade(DWORD* upgradestatus);
int __stdcall SNetLeagueLogout(char* bnetName);
BOOL __stdcall SNetGetNumPlayers(int* firstplayerid, int* lastplayerid, int* activeplayers);
BOOL __stdcall SNetGetPlayerName(int playerid, char* buffer, size_t buffersize);
BOOL __stdcall SNetSetGameMode(DWORD modeFlags, char a2);
BOOL __stdcall SNetLeaveGame(int type);
BOOL __stdcall SNetReportGameResult(unsigned int a1, int size, int a3, int a4, int a5);
BOOL __stdcall SNetGetPlayerNames(char* buffer);
BOOL __stdcall SNetGetGameInfo(int type, char* src, unsigned int length, int* byteswritten);
BOOL __stdcall SNetDropPlayer(int playerid, DWORD flags);
BOOL __stdcall SNetSendMessage(unsigned int playerID, char* data, unsigned int databytes);
BOOL __stdcall SNetReceiveMessage(int* senderplayerid, BYTE** data, int* databytes);
BOOL __stdcall SNetReceiveTurns(int a1, int arraysize, char** arraydata, unsigned int* arraydatabytes, DWORD* arrayplayerstatus);
BOOL __stdcall SNetSendTurn(char* data, unsigned int databytes);
BOOL __stdcall SNetEnumProviders(int minicaps, int(__stdcall* callback)(int, char*, char*, int));
int __stdcall SNetGetLeaguePlayerName(int* curPlayerID, size_t nameSize);
int __stdcall SNetGetLeagueName(int leagueID);
BOOL __stdcall SNetInitializeDevice(int a1, int a2, int a3, int a4, int* a5);
int __stdcall SNetSelectGame(int a1, int a2, int a3, int a4, int a5, int* playerid);
BOOL __stdcall SNetDisconnectAll(DWORD flags);
BOOL __stdcall SNetGetTurnsInTransit(int* turns);
int __stdcall SNetSendServerChatCommand(const char* command);
int __stdcall SNetSendLeagueCommand(char* cmd, void* callback);
BOOL __stdcall SNetGetPlayerCaps(char playerid, int* caps);
int __stdcall SNetSendReplayPath(char* a1, int a2, char* replayPath);
BOOL __stdcall SNetEnumDevices(int(__stdcall* callback)(DWORD, DWORD, DWORD, DWORD), int mincaps);

BOOL __stdcall SDlgSetBitmapE(HWND hWnd, int a2, char* src, int mask1, int flags, int a6, int a7, int width, int a9, int mask2);
HANDLE __stdcall SDlgDialogBoxIndirectParam(HMODULE hModule, LPCSTR lpName, HWND hWndParent, LPVOID lpParam, LPARAM lParam);
HGDIOBJ __stdcall SDlgDefDialogProc(HWND hDlg, signed int DlgType, HDC textLabel, HWND hWnd);
BOOL __stdcall SDlgEndDialog(HWND hDlg, HANDLE nResult);
BOOL __stdcall SDlgSetControlBitmaps(HWND parentwindow, int* id, int a3, char* buffer2, char* buffer, int flags, int mask);
BOOL __stdcall SDlgBltToWindowE(HWND hWnd, HRGN a2, char* a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
int __stdcall SDlg224(int a1);

BOOL __stdcall SFileGetFileName(HANDLE hFile, char* buffer, int length);
BOOL __stdcall SFileSetIoErrorMode(int mode, void* callback);
BOOL __stdcall SFileOpenFile(const char* filename, HANDLE handle);
LONG __stdcall SFileGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
BOOL __stdcall SFileReadFile(HANDLE hFile, void* buffer, DWORD nNumberOfBytesToRead, int* read, LONG lpDistanceToMoveHigh);
BOOL __stdcall SFileCloseFile(HANDLE hFile);
BOOL __stdcall SFileLoadFileEx(void* hArchive, char* filename, int a3, int a4, int a5, DWORD searchScope, struct _OVERLAPPED* lpOverlapped);
BOOL __stdcall SFileUnloadFile(HANDLE hFile);
BOOL __stdcall SFileDdaInitialize(HANDLE directsound);
BOOL __stdcall SFileDdaBeginEx(HANDLE directsound, DWORD flags, DWORD mask, unsigned __int32 lDistanceToMove, signed __int32 volume, signed int a6, int a7);
BOOL __stdcall SFileDdaEnd(HANDLE directsound);
BOOL __stdcall SFileDdaDestroy();
BOOL __stdcall SFileDdaGetPos(HANDLE directsound, int a2, int a3);
BOOL __stdcall SFileGetFileArchive(HANDLE hFile, HANDLE archive);
BOOL __stdcall SFileGetArchiveName(HANDLE hArchive, char* name, int length);
BOOL __stdcall SFileDdaSetVolume(HANDLE directsound, signed int bigvolume, signed int volume);
BOOL __stdcall SFileLoadFile(char* filename, void* buffer, int buffersize, int a4, int a5);
BOOL __stdcall SFileOpenArchive(char* archivename, DWORD dwPriority, DWORD dwFlags, HANDLE handle);
void __stdcall SFileSetLocale(LCID lcLocale);
BOOL __stdcall SFileOpenFileEx(HANDLE hMpq, const char* szFileName, DWORD dwSearchScope, HANDLE* phFile);
BOOL __stdcall SFileCloseArchive(HANDLE hArchive);
BOOL __stdcall SFileDestroy();

BOOL __stdcall SBltROP3(int maxiterations, int lpSurface, int width, int height, int width2, int pitch, int a7, DWORD rop);
BOOL __stdcall SBltROP3Clipped(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);

HANDLE __stdcall SBmpAllocLoadImage(const char* fileName, int* palette, void** buffer, int* width, int* height, int unused6, int unused7, void* (__stdcall* allocFunction)(DWORD));
BOOL __stdcall SBmpLoadImage(const char* fileName, int size, void* buffer, int buffersize, int width, int height, int depth);
BOOL __stdcall SBmpDecodeImage(int type, signed int* srcbuffer, unsigned int a3, int a4, void* dstbuffer, int size, int a7, int a8, int a9);

BOOL __stdcall SCodeCompile(char* directives1, char* directives2, char* loopstring, unsigned int maxiterations, unsigned int flags, HANDLE handle);
int __stdcall SCodeExecute(HANDLE handle, int a2);
BOOL __stdcall SCodeDelete(HANDLE handle);

BOOL __stdcall SDrawCaptureScreen(char* source);
HWND __stdcall SDrawGetFrameWindow(HWND sdraw_framewindow);
BOOL __stdcall SDrawUpdatePalette(unsigned int firstentry, unsigned int numentries, PALETTEENTRY* pPalEntries, int a4);
BOOL __stdcall SDrawManualInitialize(HWND hWnd, IDirectDraw* ddInterface, IDirectDrawSurface* primarySurface, int a4, int a5, IDirectDrawSurface* backSurface, IDirectDrawPalette* palette, int a8);
BOOL __stdcall SDrawRealizePalette();
BOOL __stdcall SDrawUnlockSurface(int surfacenumber, void* lpSurface, int a3, RECT* lpRect);
BOOL __stdcall SDrawLockSurface(int surfacenumber, RECT* lpDestRect, void** lplpSurface, int* lpPitch, int arg_unused);

BOOL __stdcall SGdiSelectObject(HANDLE hObject);
BOOL __stdcall SGdiExtTextOut(int a1, int a2, int a3, int a4, unsigned int a8, signed int a6, signed int a7, const char* string, unsigned int arg20);
BOOL __stdcall SGdiSetPitch(int pitch);
BOOL __stdcall SGdiDeleteObject(HANDLE handle);
BOOL __stdcall SGdiImportFont(HGDIOBJ handle, HANDLE* windowsfont);
BOOL __stdcall SGdi393(char* string, int, int);

void* __stdcall SMemAlloc(int amount, const char* logfilename, int logline, int defaultValue);
int __stdcall SMemZero(void* location, size_t length);
int __stdcall SMemFill(void* location, size_t length, char fillWith);
int __stdcall SMemCmp(void* location1, void* location2, size_t size);
BOOL __stdcall SMemFree(void* location, char* logfilename, int logline, char defaultValue);
void __stdcall SMemCopy(void* dest, const void* source, size_t size);

BOOL __stdcall SRegSaveString(char* keyname, char* valuename, BYTE flags, char* string);
BOOL __stdcall SRegLoadData(char* keyname, char* valuename, BYTE flags, LPBYTE lpData, int size, LPDWORD lpcbData);
BOOL __stdcall SRegSaveData(char* keyname, char* valuename, int size, BYTE* lpData, DWORD cbData);
BOOL __stdcall SRegSaveValue(char* keyname, char* valuename, BYTE flags, DWORD result);
BOOL __stdcall SRegDeleteValue(char* keyname, char* valuename, BYTE flags);
BOOL __stdcall SRegLoadString(char* keyname, char* valuename, BYTE flags, char* buffer, size_t buffersize);
BOOL __stdcall SRegLoadValue(char* keyname, char* valuename, BYTE flags, int* value);

BOOL __stdcall STransPointInMask(int a1, int a2, int a3, char* a4, size_t a5);
BOOL __stdcall STransBlt(int handle, int a2, int a3, int a4, int a5);
BOOL __stdcall STransDelete(int handle);
BOOL __stdcall STransCreateE(int a1, int a2, int a3, int bpp, int a5, int bufferSize, int a7);
BOOL __stdcall STransIntersectDirtyArray(int handle, int dirtyarraymask, unsigned __int8 dirtyarray, int sourcemask);
BOOL __stdcall STransSetDirtyArrayInfo(int width, int height, int depth, int bits);
BOOL __stdcall STransInvertMask(int handle, int sourcemask);
BOOL __stdcall STransDuplicate(int handle, int source);
BOOL __stdcall STransCombineMasks(int handle, int a2, int a3, int a4, int depth, int a6);
BOOL __stdcall STransBltUsingMask(int lpSurface, int a2, int pitch, int width, int handle);

BOOL __stdcall SVidPlayContinueSingle(HANDLE video, int a2, int a3);
BOOL __stdcall SVidPlayEnd(HANDLE video);
BOOL __stdcall SVidPlayBegin(char* filename, int arg4, int a3, int a4, int a5, int a6, HANDLE video);
BOOL __stdcall SVidInitialize(HANDLE video);
BOOL __stdcall SVidDestroy();
BOOL __stdcall SVidGetSize(HANDLE video, int width, int height, int zero);

DWORD __stdcall SErrGetLastError();
void __stdcall SErrSetLastError(DWORD dwErrCode);
BOOL __stdcall SErrGetErrorStr(DWORD dwErrCode, char* buffer, size_t bufferchars);
void __stdcall SErrSuppressErrors(bool suppressErrors);

DWORD __stdcall SStrHash(const char* string, DWORD flags, DWORD Seed);
int __stdcall SStrCmp(const char* string1, const char* string2, size_t size);
char* __stdcall SStrUpper(char* string);
int __stdcall SStrLen(const char* string);
char* __stdcall SStrChr(const char* string, char c);
int __stdcall SStrCmpI(const char* string1, const char* string2, size_t size);
int __stdcall SStrCopy(char* dest, const char* source, size_t size);
char* __stdcall SStrChrR(const char* string, char c);
size_t SStrVPrintf(char* dest, size_t size, const char* format, ...);
int __stdcall SStrNCat(char* base, const char* new_, int max_length);

int __stdcall SBigPowMod(void* buffer1, void* buffer2, int a3, int a4);
int __stdcall SBigDel(void* buffer);
int __stdcall SBigToBinaryBuffer(void* buffer, int length, int a3, int a4);
int __stdcall SBigFromBinary(void* buffer, const void* str, size_t size);
int __stdcall SBigNew(void** buffer);

void __stdcall SRgn523(int handle, RECT* rect, int reserved, int a4);
void __stdcall SRgn529i(int handle, int a2, int a3);
void __stdcall SRgnCreate(int handle, int reserved);
void __stdcall SRgnDelete(int handle);

}

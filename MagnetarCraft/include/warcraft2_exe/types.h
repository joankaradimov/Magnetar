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

namespace game::warcraft2
{
typedef __int8 _BYTE;
typedef __int16 _WORD;
typedef __int32 _DWORD;
typedef __int64 _QWORD;
typedef __int8 _TBYTE;

struct _LIST_ENTRY;
struct _iobuf;
struct HICON__;
struct _FILETIME;
struct HINSTANCE__;
struct _SCOPETABLE_ENTRY;
struct _COMMTIMEOUTS;
struct HDC__;
struct HBRUSH__;
struct HWND__;
struct tagPAINTSTRUCT;
struct tagPOINT;
struct _SECURITY_ATTRIBUTES;
struct WSAData;
struct _SYSTEMTIME;
struct _EH3_EXCEPTION_REGISTRATION;
struct tagTEXTMETRICA;
struct tagRECT;
struct tagSIZE;
struct _cpinfo;
struct _OSVERSIONINFOA;
struct _DCB;
struct _RTL_CRITICAL_SECTION;
struct _STARTUPINFOA;
struct _MEMORY_BASIC_INFORMATION;
struct CPPEH_RECORD;
struct tagMSG;
struct _WIN32_FIND_DATAA;
struct sockaddr;
struct tagWNDCLASSA;
struct _RTL_CRITICAL_SECTION_DEBUG;
struct _TIME_ZONE_INFORMATION;



typedef void *LPVOID;

struct _LIST_ENTRY
{
  _LIST_ENTRY *Flink;
  _LIST_ENTRY *Blink;
};
static_assert(sizeof(_LIST_ENTRY) == 8, "Incorrect size for type `_LIST_ENTRY`. Expected: 8");

typedef int LONG;

typedef char CHAR;

struct _iobuf
{
  char *_ptr;
  int _cnt;
  char *_base;
  int _flag;
  int _file;
  int _charbuf;
  int _bufsiz;
  char *_tmpfname;
};
static_assert(sizeof(_iobuf) == 32, "Incorrect size for type `_iobuf`. Expected: 32");

typedef HDC__ *HDC;

struct HICON__
{
  int unused;
};
static_assert(sizeof(HICON__) == 4, "Incorrect size for type `HICON__`. Expected: 4");

typedef _RTL_CRITICAL_SECTION_DEBUG *PRTL_CRITICAL_SECTION_DEBUG;

typedef unsigned __int8 BYTE;

typedef unsigned int ULONG_PTR;

typedef int BOOL;

typedef unsigned int UINT;

struct _FILETIME
{
  DWORD dwLowDateTime;
  DWORD dwHighDateTime;
};
static_assert(sizeof(_FILETIME) == 8, "Incorrect size for type `_FILETIME`. Expected: 8");

struct HINSTANCE__
{
  int unused;
};
static_assert(sizeof(HINSTANCE__) == 4, "Incorrect size for type `HINSTANCE__`. Expected: 4");

struct _SCOPETABLE_ENTRY
{
  int EnclosingLevel;
  void *FilterFunc;
  void *HandlerFunc;
};
static_assert(sizeof(_SCOPETABLE_ENTRY) == 12, "Incorrect size for type `_SCOPETABLE_ENTRY`. Expected: 12");

typedef void *PVOID;

typedef unsigned int UINT_PTR;

typedef int LONG_PTR;

typedef wchar_t WCHAR;

typedef unsigned __int16 WORD;

struct _COMMTIMEOUTS
{
  DWORD ReadIntervalTimeout;
  DWORD ReadTotalTimeoutMultiplier;
  DWORD ReadTotalTimeoutConstant;
  DWORD WriteTotalTimeoutMultiplier;
  DWORD WriteTotalTimeoutConstant;
};
static_assert(sizeof(_COMMTIMEOUTS) == 20, "Incorrect size for type `_COMMTIMEOUTS`. Expected: 20");

typedef unsigned __int16 USHORT;

struct HDC__
{
  int unused;
};
static_assert(sizeof(HDC__) == 4, "Incorrect size for type `HDC__`. Expected: 4");

struct HBRUSH__
{
  int unused;
};
static_assert(sizeof(HBRUSH__) == 4, "Incorrect size for type `HBRUSH__`. Expected: 4");

typedef struct _EH3_EXCEPTION_REGISTRATION *PEH3_EXCEPTION_REGISTRATION;

struct HWND__
{
  int unused;
};
static_assert(sizeof(HWND__) == 4, "Incorrect size for type `HWND__`. Expected: 4");

typedef struct _SCOPETABLE_ENTRY *PSCOPETABLE_ENTRY;

typedef ULONG_PTR DWORD_PTR;

struct tagPAINTSTRUCT
{
  HDC hdc;
  BOOL fErase;
  RECT rcPaint;
  BOOL fRestore;
  BOOL fIncUpdate;
  BYTE rgbReserved[32];
};
static_assert(sizeof(tagPAINTSTRUCT) == 64, "Incorrect size for type `tagPAINTSTRUCT`. Expected: 64");

struct tagPOINT
{
  LONG x;
  LONG y;
};
static_assert(sizeof(tagPOINT) == 8, "Incorrect size for type `tagPOINT`. Expected: 8");

typedef _iobuf FILE;

typedef UINT_PTR WPARAM;

struct _SECURITY_ATTRIBUTES
{
  DWORD nLength;
  LPVOID lpSecurityDescriptor;
  BOOL bInheritHandle;
};
static_assert(sizeof(_SECURITY_ATTRIBUTES) == 12, "Incorrect size for type `_SECURITY_ATTRIBUTES`. Expected: 12");

struct WSAData
{
  WORD wVersion;
  WORD wHighVersion;
  char szDescription[257];
  char szSystemStatus[129];
  unsigned __int16 iMaxSockets;
  unsigned __int16 iMaxUdpDg;
  char *lpVendorInfo;
};
static_assert(sizeof(WSAData) == 400, "Incorrect size for type `WSAData`. Expected: 400");

struct _SYSTEMTIME
{
  WORD wYear;
  WORD wMonth;
  WORD wDayOfWeek;
  WORD wDay;
  WORD wHour;
  WORD wMinute;
  WORD wSecond;
  WORD wMilliseconds;
};
static_assert(sizeof(_SYSTEMTIME) == 16, "Incorrect size for type `_SYSTEMTIME`. Expected: 16");

struct _EH3_EXCEPTION_REGISTRATION
{
  struct _EH3_EXCEPTION_REGISTRATION *Next;
  PVOID ExceptionHandler;
  PSCOPETABLE_ENTRY ScopeTable;
  DWORD TryLevel;
};
static_assert(sizeof(_EH3_EXCEPTION_REGISTRATION) == 16, "Incorrect size for type `_EH3_EXCEPTION_REGISTRATION`. Expected: 16");

#pragma pack(push, 4)
struct tagTEXTMETRICA
{
  LONG tmHeight;
  LONG tmAscent;
  LONG tmDescent;
  LONG tmInternalLeading;
  LONG tmExternalLeading;
  LONG tmAveCharWidth;
  LONG tmMaxCharWidth;
  LONG tmWeight;
  LONG tmOverhang;
  LONG tmDigitizedAspectX;
  LONG tmDigitizedAspectY;
  BYTE tmFirstChar;
  BYTE tmLastChar;
  BYTE tmDefaultChar;
  BYTE tmBreakChar;
  BYTE tmItalic;
  BYTE tmUnderlined;
  BYTE tmStruckOut;
  BYTE tmPitchAndFamily;
  BYTE tmCharSet;
};
#pragma pack(pop)
static_assert(sizeof(tagTEXTMETRICA) == 56, "Incorrect size for type `tagTEXTMETRICA`. Expected: 56");

typedef _LIST_ENTRY LIST_ENTRY;

struct tagRECT
{
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
};
static_assert(sizeof(tagRECT) == 16, "Incorrect size for type `tagRECT`. Expected: 16");

struct tagSIZE
{
  LONG cx;
  LONG cy;
};
static_assert(sizeof(tagSIZE) == 8, "Incorrect size for type `tagSIZE`. Expected: 8");

typedef LONG_PTR LRESULT;

struct _cpinfo
{
  UINT MaxCharSize;
  BYTE DefaultChar[2];
  BYTE LeadByte[12];
};
static_assert(sizeof(_cpinfo) == 20, "Incorrect size for type `_cpinfo`. Expected: 20");

typedef LONG_PTR LPARAM;

struct _OSVERSIONINFOA
{
  DWORD dwOSVersionInfoSize;
  DWORD dwMajorVersion;
  DWORD dwMinorVersion;
  DWORD dwBuildNumber;
  DWORD dwPlatformId;
  CHAR szCSDVersion[128];
};
static_assert(sizeof(_OSVERSIONINFOA) == 148, "Incorrect size for type `_OSVERSIONINFOA`. Expected: 148");

struct _DCB
{
  DWORD DCBlength;
  DWORD BaudRate;
  unsigned __int32 fBinary : 1;
  unsigned __int32 fParity : 1;
  unsigned __int32 fOutxCtsFlow : 1;
  unsigned __int32 fOutxDsrFlow : 1;
  unsigned __int32 fDtrControl : 2;
  unsigned __int32 fDsrSensitivity : 1;
  unsigned __int32 fTXContinueOnXoff : 1;
  unsigned __int32 fOutX : 1;
  unsigned __int32 fInX : 1;
  unsigned __int32 fErrorChar : 1;
  unsigned __int32 fNull : 1;
  unsigned __int32 fRtsControl : 2;
  unsigned __int32 fAbortOnError : 1;
  unsigned __int32 fDummy2 : 17;
  WORD wReserved;
  WORD XonLim;
  WORD XoffLim;
  BYTE ByteSize;
  BYTE Parity;
  BYTE StopBits;
  char XonChar;
  char XoffChar;
  char ErrorChar;
  char EofChar;
  char EvtChar;
  WORD wReserved1;
};
static_assert(sizeof(_DCB) == 28, "Incorrect size for type `_DCB`. Expected: 28");

typedef BYTE *LPBYTE;

#pragma pack(push, 8)
struct _RTL_CRITICAL_SECTION
{
  PRTL_CRITICAL_SECTION_DEBUG DebugInfo;
  LONG LockCount;
  LONG RecursionCount;
  HANDLE OwningThread;
  HANDLE LockSemaphore;
  ULONG_PTR SpinCount;
};
#pragma pack(pop)
static_assert(sizeof(_RTL_CRITICAL_SECTION) == 24, "Incorrect size for type `_RTL_CRITICAL_SECTION`. Expected: 24");

typedef const CHAR *LPCSTR;

typedef CHAR *LPSTR;

typedef _FILETIME FILETIME;

typedef ULONG_PTR SIZE_T;

typedef USHORT ADDRESS_FAMILY;

struct _STARTUPINFOA
{
  DWORD cb;
  LPSTR lpReserved;
  LPSTR lpDesktop;
  LPSTR lpTitle;
  DWORD dwX;
  DWORD dwY;
  DWORD dwXSize;
  DWORD dwYSize;
  DWORD dwXCountChars;
  DWORD dwYCountChars;
  DWORD dwFillAttribute;
  DWORD dwFlags;
  WORD wShowWindow;
  WORD cbReserved2;
  LPBYTE lpReserved2;
  HANDLE hStdInput;
  HANDLE hStdOutput;
  HANDLE hStdError;
};
static_assert(sizeof(_STARTUPINFOA) == 68, "Incorrect size for type `_STARTUPINFOA`. Expected: 68");

struct _MEMORY_BASIC_INFORMATION
{
  PVOID BaseAddress;
  PVOID AllocationBase;
  DWORD AllocationProtect;
  SIZE_T RegionSize;
  DWORD State;
  DWORD Protect;
  DWORD Type;
};
static_assert(sizeof(_MEMORY_BASIC_INFORMATION) == 28, "Incorrect size for type `_MEMORY_BASIC_INFORMATION`. Expected: 28");

struct CPPEH_RECORD
{
  DWORD old_esp;
  EXCEPTION_POINTERS *exc_ptr;
  struct _EH3_EXCEPTION_REGISTRATION registration;
};
static_assert(sizeof(CPPEH_RECORD) == 24, "Incorrect size for type `CPPEH_RECORD`. Expected: 24");

typedef _SYSTEMTIME SYSTEMTIME;

struct tagMSG
{
  HWND hwnd;
  UINT message;
  WPARAM wParam;
  LPARAM lParam;
  DWORD time;
  POINT pt;
};
static_assert(sizeof(tagMSG) == 28, "Incorrect size for type `tagMSG`. Expected: 28");

struct _WIN32_FIND_DATAA
{
  DWORD dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD nFileSizeHigh;
  DWORD nFileSizeLow;
  DWORD dwReserved0;
  DWORD dwReserved1;
  CHAR cFileName[260];
  CHAR cAlternateFileName[14];
};
static_assert(sizeof(_WIN32_FIND_DATAA) == 320, "Incorrect size for type `_WIN32_FIND_DATAA`. Expected: 320");

struct sockaddr
{
  ADDRESS_FAMILY sa_family;
  CHAR sa_data[14];
};
static_assert(sizeof(sockaddr) == 16, "Incorrect size for type `sockaddr`. Expected: 16");

struct tagWNDCLASSA
{
  UINT style;
  WNDPROC lpfnWndProc;
  int cbClsExtra;
  int cbWndExtra;
  HINSTANCE hInstance;
  HICON hIcon;
  HCURSOR hCursor;
  HBRUSH hbrBackground;
  LPCSTR lpszMenuName;
  LPCSTR lpszClassName;
};
static_assert(sizeof(tagWNDCLASSA) == 40, "Incorrect size for type `tagWNDCLASSA`. Expected: 40");

struct _RTL_CRITICAL_SECTION_DEBUG
{
  WORD Type;
  WORD CreatorBackTraceIndex;
  _RTL_CRITICAL_SECTION *CriticalSection;
  LIST_ENTRY ProcessLocksList;
  DWORD EntryCount;
  DWORD ContentionCount;
  DWORD Flags;
  WORD CreatorBackTraceIndexHigh;
  WORD SpareWORD;
};
static_assert(sizeof(_RTL_CRITICAL_SECTION_DEBUG) == 32, "Incorrect size for type `_RTL_CRITICAL_SECTION_DEBUG`. Expected: 32");

typedef struct _EH3_EXCEPTION_REGISTRATION EH3_EXCEPTION_REGISTRATION;

struct _TIME_ZONE_INFORMATION
{
  LONG Bias;
  WCHAR StandardName[32];
  SYSTEMTIME StandardDate;
  LONG StandardBias;
  WCHAR DaylightName[32];
  SYSTEMTIME DaylightDate;
  LONG DaylightBias;
};
static_assert(sizeof(_TIME_ZONE_INFORMATION) == 172, "Incorrect size for type `_TIME_ZONE_INFORMATION`. Expected: 172");

}

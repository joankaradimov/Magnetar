#include "CSprite.h"
#include "starcraft.h"
#include "patching.h"
#include "magnetorm.h"

SpriteTileDataEx _SpritesOnTileRow;

int __stdcall ReadSpritesArray_(FILE* a1)
{
    u16 v19;
    if (fread(&v19, 2, 1, a1) != 1)
    {
        return 0;
    }
    memset(SpriteTable, 0, sizeof(SpriteTable));
    if (v19)
    {
        int v17 = 38 * v19;
        unsigned __int16* v4 = (unsigned __int16*)SMemAlloc(v17, "Starcraft\\SWAR\\lang\\CSprite.cpp", 1860, 0);
        unsigned __int16* v18 = v4;
        memset(v4, 0, v17);
        if (!DecompressRead(v4, v17, a1))
        {
            SMemFree(v4, "Starcraft\\SWAR\\lang\\CSprite.cpp", 1866, 0);
            return 0;
        }
        int v6 = v19;
        while (v6)
        {
            const void* v7 = v4 + 1;
            CSprite* v8 = &SpriteTable[*v4];
            v4 += 19;
            --v6;
            memcpy(v8, v7, sizeof(CSprite));
        }
        SMemFree(v18, "Starcraft\\SWAR\\lang\\CSprite.cpp", 1884, 0);
    }
    for (int i = 0; i < _countof(SpriteTable); i++)
    {
        unpackSpriteData(SpriteTable + i);
    }
    initializeSpriteArray();
    if (fread(_SpritesOnTileRow.heads, sizeof(_SpritesOnTileRow.heads), 1, a1) != 1)
    {
        return 0;
    }
    int v11 = map_size.height;
    while (v11)
    {
        v11--;
        CSprite* v12 = _SpritesOnTileRow.heads[v11];
        if (v12)
        {
            v12 = (CSprite*)&dword_629D74[9 * (_DWORD)v12];
        }
        _SpritesOnTileRow.heads[v11] = v12;
    }
    if (fread(_SpritesOnTileRow.tails, sizeof(_SpritesOnTileRow.tails), 1, a1) != 1)
    {
        return 0;
    }
    int v13 = map_size.height;
    while (v13)
    {
        v13--;
        CSprite* v14 = _SpritesOnTileRow.tails[v13];
        if (v14)
        {
            v14 = (CSprite*)&dword_629D74[9 * (_DWORD)v14];
        }
        _SpritesOnTileRow.tails[v13] = v14;
    }
    int v15 = 0;
    for (int i = 0; i < _countof(SpriteTable); ++i)
    {
        SpriteTable[i].index = v15++;
    }
    return 1;
}

FUNCTION_PATCH(ReadSpritesArray, ReadSpritesArray_, "starcraft");

BOOL __stdcall writeSprites_(FILE* file)
{
    for (int i = 0; i < _countof(SpriteTable); i++)
    {
        packSpriteData(SpriteTable + i);
    }
    int v3 = 0;
    CImage** v4 = &SpriteTable[1].pImageHead;
    do
    {
        if (*(v4 - 9))
        {
            ++v3;
        }
        if (*v4)
        {
            ++v3;
        }
        if (v4[9])
        {
            ++v3;
        }
        if (v4[18])
        {
            ++v3;
        }
        if (v4[27])
        {
            ++v3;
        }
        v4 += 45;
    } while ((int)v4 < (int)&byte_63FD30[56]);
    size_t v6 = fwrite(&v3, 2, 1, file);
    int v7 = v6 == 1;
    if (v6 == 1 && (_WORD)v3)
    {
        int v8 = 38 * v3;
        _WORD* v9 = (_WORD*) SMemAlloc(v8, "Starcraft\\SWAR\\lang\\CSprite.cpp", 1789, 0);
        memset(v9, 0, v8);
        void* location = v9;
        for (int i = 0; i < _countof(SpriteTable); i++)
        {
            if (SpriteTable[i].pImageHead)
            {
                *v9 = i;
                memcpy(v9 + 1, SpriteTable + i, sizeof(CSprite));
                v9 += 19;
            }
        }
        v7 = CompressWrite(location, v8, file);
        SMemFree(location, "Starcraft\\SWAR\\lang\\CSprite.cpp", 1811, 0);
    }
    for (int i = 0; i < _countof(SpriteTable); i++)
    {
        unpackSpriteData(SpriteTable + i);
    }
    if (!v7)
    {
        return 0;
    }
    int v15 = map_size.height;
    while (v15)
    {
        v15--;
        CSprite* v16 = _SpritesOnTileRow.heads[v15];
        CSprite* v17;
        if (v16)
        {
            v17 = (CSprite*)(v16 - SpriteTable + 1);
        }
        else
        {
            v17 = 0;
        }
        _SpritesOnTileRow.heads[v15] = v17;
    }
    size_t v18 = fwrite(_SpritesOnTileRow.heads, sizeof(_SpritesOnTileRow.heads), 1, file);
    int v19 = map_size.height;
    int v20 = map_size.height;
    BOOL result = v18 == 1;
    while (v20)
    {
        v20--;
        CSprite* v22 = _SpritesOnTileRow.heads[v20];
        if (v22)
        {
            v22 = (CSprite*)&dword_629D74[9 * (_DWORD)v22];
        }
        _SpritesOnTileRow.heads[v20] = v22;
    }
    if (result)
    {
        for (; v19;)
        {
            v19--;
            CSprite* v23 = _SpritesOnTileRow.tails[v19];
            if (v23)
            {
                _SpritesOnTileRow.tails[v19] = (CSprite*)(v23 - SpriteTable + 1);
            }
            else
            {
                _SpritesOnTileRow.tails[v19] = 0;
            }
        }
        size_t v25 = fwrite(_SpritesOnTileRow.tails, sizeof(_SpritesOnTileRow.tails), 1, file);
        int v26 = map_size.height;
        result = v25 == 1;
        while (v26)
        {
            v26--;
            CSprite* v27 = _SpritesOnTileRow.tails[v26];
            if (v27)
            {
                v27 = (CSprite*)&dword_629D74[9 * (_DWORD)v27];
            }
            _SpritesOnTileRow.tails[v26] = v27;
        }
    }
    return result;
}

FUNCTION_PATCH(writeSprites, writeSprites_, "starcraft");

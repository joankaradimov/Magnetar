#include <stdio.h>
#include <StormLib.h>

int main(int argc, char** argv)
{
    HANDLE magnetar_dat = NULL;

    // TODO: check if the MPQ needs rebuilding

    // TODO: count files
    int file_count = 200;

    remove("MagnetarDat.mpq");

    if (!SFileCreateArchive("MagnetarDat.mpq", MPQ_CREATE_LISTFILE, file_count * 3 / 2, &magnetar_dat))
    {
        int error = GetLastError();
        printf("Error %d\n", error);
        return error;
    }

    // TODO: add files

    SFileCloseArchive(magnetar_dat);

    return 0;
}

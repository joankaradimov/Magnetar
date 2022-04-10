#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <StormLib.h>
#include <vector>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::filesystem::path executable_path = argv[0];

        std::cout << "Error! Expected a directory to compress" << std::endl << std::endl;
        std::cout << "  Usage: " << executable_path.filename().generic_string() << " path-to-mpq-root" << std::endl << std::endl;

        return 1;
    }

    std::filesystem::path input_path = argv[1];
    std::filesystem::path output_path = "MagnetarDat.mpq";
    std::vector<std::filesystem::path> file_paths;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_path))
    {
        // TODO: handle symlinks, maybe?
        if (entry.is_regular_file())
        {
            file_paths.push_back(entry.path());
        }
    }

    // TODO: check if the MPQ needs rebuilding

    remove(output_path.generic_string().c_str());

    HANDLE magnetar_dat = NULL;
    if (!SFileCreateArchive(output_path.string().c_str(), MPQ_CREATE_LISTFILE, file_paths.size() * 3 / 2, &magnetar_dat))
    {
        int error = GetLastError();
        printf("Error in SFileCreateArchive %d\n", error);
        return error;
    }

    for (const auto& file_path : file_paths)
    {
        if (!SFileAddFile(magnetar_dat, file_path.string().c_str(), file_path.string().c_str() + input_path.string().length() + 1, MPQ_FILE_COMPRESS))
        {
            int error = GetLastError();
            printf("Error in SFileAddFile %d\n", error);
            return error;
        }
    }

    if (!SFileCloseArchive(magnetar_dat))
    {
        int error = GetLastError();
        printf("Error in SFileCloseArchive %d\n", error);
        return error;
    }

    return 0;
}

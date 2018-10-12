#include "fileoperation.h"

std::string fileop::filehandler::Next(std::string extension)
{
    int i;
    while((dp = readdir(pDir)) != NULL)
    {
        //filter "." and ".."
        if(dp->d_name[0] == '.')
            continue;
        i = 1;
        if(extension == "")
        {
            //filter folders
            while(i < dp->d_namlen && dp->d_name[i] != '.')
                ++i;
            if(i < dp->d_namlen)
                continue;
            return dp->d_name;
        }
        else
        {
            //filter ".",".." and too small files
            if((i = dp->d_namlen - extension.length()) <= 1)
                continue;
            //check if extension matches
            if(dp->d_name[i++ - 1] != '.')
                continue;
            while(dp->d_name[i] == extension[dp->d_namlen - i] && i < dp->d_namlen)
                continue;
            return dp->d_name;
        }
    }
    return "";
}
int fileop::filehandler::CountFolders()
{
    rewinddir(pDir);
    int i;
    int FolderCount = 0;
    while((dp = readdir(pDir)) != NULL)
    {
        i = 0;
        //filter "." and ".." as well as files
        while(i < dp->d_namlen && dp->d_name[i] != '.')
            ++i;
        if(i < dp->d_namlen)
            continue;
        ++FolderCount;
    }
    //reset folder
    Rewind();
    return FolderCount;
}
int fileop::filehandler::CountFiles(std::string extension)
{
    rewinddir(pDir);
    int i = 1;
    int FileCount = 0;
    while((dp = readdir(pDir)) != NULL)
    {
        //filter "." and ".."
        if(dp->d_name[0] == '.')
            continue;
        i = 1;
        if(extension == "")
        {
            //filter folders
            while(i < dp->d_namlen && dp->d_name[i] != '.')
                ++i;
            if(i >= dp->d_namlen)
                continue;
            ++FileCount;
        }
        else
        {
            //filter ".",".." and too small files
            if((i = dp->d_namlen - extension.length()) <= 1)
                continue;
            //check if extension matches
            if(dp->d_name[i++ - 1] != '.')
                continue;
            while(dp->d_name[i] == extension[dp->d_namlen - i] && i < dp->d_namlen)
                continue;
            ++FileCount;
        }
    }
    //reset folder
    Rewind();
    return FileCount;
}

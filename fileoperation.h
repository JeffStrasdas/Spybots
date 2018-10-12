#ifndef FILEOPERATION_H
#define FILEOPERATION_H
#include <dirent.h>
#include <string>
#include <cstdio>

namespace fileop
{
class filehandler
{
public:
    filehandler(std::string path)
    {
        if((pDir = opendir(path.c_str())) == NULL)
        {
            printf("Cannot open %s!\n",path.c_str());
        }
        dp = NULL;
    };
    //Returns "" after last element
    //empty extension returns folders and files without extension
    std::string Next(std::string extension = "");
    void Rewind()
    {
        rewinddir(pDir);
        dp = NULL;
    };
    //Also counts files with no extension!
    int CountFolders();
    //Doesn't count files with no extension!
    int CountFiles(std::string extension = "");
private:
    DIR* pDir;
    struct dirent *dp;
};
}
#endif

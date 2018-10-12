#include<dirent.h>

namespace fileop
{
class filehandler
{
public:
    filehandler(std::string path)
    {
        if((pDir = opendir(path)) == NULL)
        {
            perror("Cannot open specified path");
        }
    }
private:
    DIR* pDir;

};
}

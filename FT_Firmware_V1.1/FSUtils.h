#ifndef __FS_UTILS__
#define __FS_UTILS__

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <cstring>

// Define the permission macros if not defined
#ifndef S_IRWXU
#define S_IRWXU 00700 // Read, write, execute/search by owner
#define S_IRUSR 00400 // Read permission, owner
#define S_IWUSR 00200 // Write permission, owner
#define S_IXUSR 00100 // Execute/search permission, owner
#endif

#ifndef S_IRWXG
#define S_IRWXG 00070 // Read, write, execute/search by group
#define S_IRGRP 00040 // Read permission, group
#define S_IWGRP 00020 // Write permission, group
#define S_IXGRP 00010 // Execute/search permission, group
#endif

#ifndef S_IRWXO
#define S_IRWXO 00007 // Read, write, execute/search by others
#define S_IROTH 00004 // Read permission, others
#define S_IWOTH 00002 // Write permission, others
#define S_IXOTH 00001 // Execute/search permission, others
#endif

class FSUtils
{
private:
    /* data */
public:

    static std::string joinPath(const std::string &dirPath, const std::string &fileName);
    static bool directoryExists(const std::string& dirPath);
    static bool createDirectory(const std::string& dirPath);
    static bool fileExists(const std::string& path);
    static bool writeFile(const std::string &filename, const std::string &fileContent);
    static std::string readFile(const std::string &filename);

    FSUtils();
    ~FSUtils();
};




#endif
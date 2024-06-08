#include "FSUtils.h"

FSUtils::FSUtils()
{
}

FSUtils::~FSUtils()
{
}

// Public Methods

bool FSUtils::fileExists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFREG) != 0; // Check if it's a regular file
}

//bool FSUtils::createDirectory(const std::string& dirPath) {
//    int status = mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
//    return status == 0;
//}
bool FSUtils::createDirectory(const std::string& dirPath) {
    struct stat st;
    if (stat(dirPath.c_str(), &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            // Directory already exists
            return true;
        } else {
            // Path exists but is not a directory
            std::cerr << "Error: " << dirPath << " exists but is not a directory." << std::endl;
            return false;
        }
    }

#ifdef _WIN32
    // On Windows, mkdir takes one argument
    int status = mkdir(dirPath.c_str());
#else
    // On POSIX systems, mkdir takes two arguments
    int status = mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

    if (status != 0) {
        std::cerr << "Error creating directory " << dirPath << ": " << std::strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool FSUtils::directoryExists(const std::string& dirPath) {
    struct stat info;
    if (stat(dirPath.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

std::string FSUtils::joinPath(const std::string &dirPath, const std::string &fileName)
{
    if (dirPath.empty() || fileName.empty()) {
        return ""; // Return empty string if either directory or filename is empty
    }

    // Ensure that there is exactly one separator between directory and filename. No additional checking is being done
    char separator = '/';
    if (dirPath.back() == '/' || fileName.front() == '/') {
        separator = '\0'; // No need for a separator if one path ends or begins with it
    }

    return dirPath + separator + fileName;
}

bool FSUtils::writeFile(const std::string &filename, const std::string &fileContent)
{
    std::ofstream outputFile(filename);

    if (!outputFile.is_open())
    {
        std::cerr << "Failed to open file " << filename << " for writing." << std::endl;
        return false;
    }

    // Write data to the file
    outputFile << fileContent;

    outputFile.close();

    return true;
}

std::string FSUtils::readFile(const std::string &filename)
{
    std::ifstream inputFile(filename);
    std::string fileContent;

    if (!inputFile.is_open()) {
        std::cerr << "Failed to open file " << filename << " for reading." << std::endl;
        return std::string();
    }

    std::string line;

    while (std::getline(inputFile, line))
    {
        fileContent += line;
    }

    inputFile.close();

    return fileContent;
}
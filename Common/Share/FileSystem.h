#pragma once
#include <Hpp/NonCopyable.h>
#include <vector>
#include <string>

class FileStream : public NonCopyable
{
public:
    FileStream(FILE *fp);
    ~FileStream();
public:
    std::string readAll();
    bool read(std::string& buff, uint32_t toRead);
    bool readStringUntilRN(std::string& str, uint32_t& length);
    uint32_t getLength()
    {
        return length_;
    }
    bool isOpen()
    {
        return fp_ != NULL;
    }
    bool isValid()
    {
        return fp_ != NULL;
    }
private:
    FILE*	fp_{NULL};
    uint32_t length_{0};
};

class FileSystem : public NonCopyable
{
public:
    static FileSystem& getIns()
    {
        static FileSystem fs;
        return fs;
    }
public:
    FILE* open(const std::string& fileName);

    void addFindPath(std::string path)
    {
        return paths_.push_back(std::move(path));
    }
private:
    FileSystem(){}
    std::vector<std::string> paths_;
};


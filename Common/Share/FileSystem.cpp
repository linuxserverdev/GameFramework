#include "FileSystem.h"
#include "CommonException.h"

FileStream::FileStream(FILE *fp)
    : fp_(fp)
{
    if(fp_)
    {
        long now = ftell(fp);
        fseek(fp,0,SEEK_SET);
        long begin = ftell(fp);
        fseek(fp,0,SEEK_END);
        long end = ftell(fp);
        length_ = end - begin;
        fseek(fp,now,SEEK_SET);
    }
}

FileStream::~FileStream()
{
    if(fp_)
    {
        fclose(fp_);
        fp_ = NULL;
    }
}

std::string FileStream::readAll()
{
    std::string content;
    content.resize(length_);
    do{
        size_t size = fread((void*)content.data(), 1, length_, fp_);
        if(size == 0)
        {
            std::string().swap(content);
            break;
        }

        if (length_ != size) // eof error or read error(bad file, access)
        {
            if (!feof(fp_))
            {
                std::string().swap(content);
                break;
            }
            content.resize(size);
        }
    }while(0);

    return content;
}

bool FileStream::read(std::string& buff, uint32_t toRead)
{
    if (toRead == 0)
        return true;
    buff.resize(toRead);
    size_t size = fread((void*)buff.data(), 1, toRead, fp_);
    if(!size)return false;

    if (toRead != size) // eof error or read error(bad file, access)
    {
        if (!feof(fp_))
        {
            return false;
        }
        buff.resize(size);
    }
    return true;
}

bool FileStream::readStringUntilRN(std::string& str, uint32_t& length)
{
    if (length == 0)
        return true;

    if(feof(fp_))return false;

    char c;
    uint32_t index = 0;
    while(fread(&c,1,sizeof(c),fp_))
    {
        if(c == '\r')
        {
            char c;
            if(fread(&c,1,sizeof(c),fp_))
            {
                if(c != '\n')
                {
                    fseek(fp_,-1,SEEK_CUR);
                }

                break;
            }
            else
            {
                break;
            }
        }
        else if(c == '\n')
        {
            char c;
            if(fread(&c,1,sizeof(c),fp_))
            {
                if(c != '\r')
                {
                    fseek(fp_,-1,SEEK_CUR);
                }

                break;
            }
            else
            {
                break;
            }
        }
        str.append(c, 1);
        ++index;
        if(index >= length - 1)break;
    }
    str.append('0', 1);
    length = index;
    return true;
}

FILE* FileSystem::open(const std::string &fileName)
{
    FILE *fp = fopen(fileName.c_str(), "rb");
    if(fp)
    {
        return fp;
    }
    else
    {
        uint32_t size = paths_.size();
        for(uint32_t i=0; i<size; ++i)
        {
            char name[255];
            snprintf(name, sizeof(name) - 1, "%s/%s", paths_[i].c_str(), fileName.c_str());
            FILE *fp = fopen(name, "rb");
            if(fp)
            {
                return fp;
            }
        }
    }
    throw CommonException("FileSystem::open --> open file " + fileName + " failed");
}

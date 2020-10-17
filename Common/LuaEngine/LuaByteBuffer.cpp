#include "LuaByteBuffer.h"

void LuaByteBuffer::pushBool(bool flag)
{
    buffer_ << flag;
}

void LuaByteBuffer::pushInt(int value)
{
    buffer_ << value;
}

void LuaByteBuffer::pushChar(char value)
{
    buffer_ << value;
}

void LuaByteBuffer::pushDouble(double value)
{
    buffer_ << value;
}

void LuaByteBuffer::pushChars(const char* str)
{
    buffer_ << str;
}

void LuaByteBuffer::pushString(const std::string& str)
{
    buffer_ << str;
}

void LuaByteBuffer::pushLString(const char* str, int lengh)
{
    buffer_ << (uint16_t)lengh;
    buffer_.append(str, lengh);
}

void LuaByteBuffer::pushPbc(const char* str, int lengh)
{
    buffer_.append(str, lengh);
}

bool LuaByteBuffer::popBool()
{
    bool value;
    buffer_ >> value;
    return value;
}

int	LuaByteBuffer::popInt()
{
    int value;
    buffer_ >> value;
    return value;
}

char LuaByteBuffer::popChar()
{
    char value;
    buffer_ >> value;
    return value;
}

float LuaByteBuffer::popFloat()
{
    float value;
    buffer_ >> value;
    return value;
}

double LuaByteBuffer::popDouble()
{
    double value;
    buffer_ >> value;
    return value;
}

long long LuaByteBuffer::popInt64()
{
    int64_t value;
    buffer_ >> value;
    return value;
}

void LuaByteBuffer::clear()
{
    buffer_.clear();
}

std::string LuaByteBuffer::popString()
{
    std::string str;
    buffer_ >> str;
    return str;
}

std::string	LuaByteBuffer::popPbc()
{
    std::string str;
    str.append((char*)(buffer_.contents() + buffer_.rpos()), buffer_.size() - buffer_.rpos());
    return str;
}

void LuaByteBuffer::popInts(std::vector<int>& ints)
{
    buffer_ >> ints;
}
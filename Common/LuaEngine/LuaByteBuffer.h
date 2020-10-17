#pragma once
#include <Share/ByteBuffer.h>
class LuaByteBuffer
{
public:
    void pushBool(bool flag);
    void pushInt(int value);
    void pushChar(char value);
    void pushDouble(double value);
    void pushChars(const char* str);
    void pushString(const std::string& str);
    void pushLString(const char* str, int lengh);
    void pushPbc(const char* str, int lengh);

    bool	popBool();
    int		popInt();
    char	popChar();
    float	popFloat();
    double	popDouble();
    long long popInt64();

    void    clear();
    std::string	popString();
    std::string	popPbc();

    int size() { return buffer_.size(); }
    int realSize() { return buffer_.realSize(); }
    void	popInts(std::vector<int>& ints);

    ByteBuffer& getByteBuffer()
    {
        return buffer_;
    }

    void append(ByteBuffer& buf)
    {
        buffer_.append(buf, buf.rpos());
    }

    void append(LuaByteBuffer* buf)
    {
        append(buf->getByteBuffer());
    }
private:
    ByteBuffer buffer_;
};

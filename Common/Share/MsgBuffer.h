#pragma once
#include "Hpp/NonCopyable.h"
#include "Poco/Net/StreamSocket.h"
#include <vector>
#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#ifdef _WIN32
using ssize_t = std::intptr_t;
#endif

static constexpr size_t kBufferDefaultLength{2048};
static constexpr char CRLF[]{"\r\n"};

struct MsgReal
{
    MsgReal(uint64_t real = 0, uint8_t prec = 2) :_real(real), _prec(prec) {}
    uint64_t _real;
    uint8_t  _prec;
    double realValue()
    {
        return _real / (_prec * 1.0);
    }
};

/**
 * @brief This class represents a memory buffer used for sending and receiving
 * data.
 *
 */
class MsgBuffer
{
  public:
    /**
     * @brief Construct a new message buffer instance.
     *
     * @param len The initial size of the buffer.
     */
    MsgBuffer(size_t len = kBufferDefaultLength);

    /**
     * @brief Get the beginning of the buffer.
     *
     * @return const char*
     */
    const char *peek() const
    {
        return begin() + head_;
    }

    /**
     * @brief Get the end of the buffer where new data can be written.
     *
     * @return const char*
     */
    const char *beginWrite() const
    {
        return begin() + tail_;
    }
    char *beginWrite()
    {
        return begin() + tail_;
    }

    /**
     * @brief Get a byte value from the buffer.
     *
     * @return uint8_t
     */
    uint8_t peekInt8() const;
    /**
     * @brief Get a unsigned short value from the buffer.
     *
     * @return uint16_t
     */
    uint16_t peekInt16() const;
    /**
     * @brief Get a unsigned int value from the buffer.
     *
     * @return uint32_t
     */
    uint32_t peekInt32() const;

    /**
     * @brief Get a unsigned int64 value from the buffer.
     *
     * @return uint64_t
     */
    uint64_t peekInt64() const;

    /**
     * @brief Get and remove some bytes from the buffer.
     *
     * @param len
     * @return std::string
     */
    std::string read(size_t len);

    /**
     * @brief Get the remove a byte value from the buffer.
     *
     * @return uint8_t
     */
    uint8_t readInt8();
    MsgBuffer& operator>>(uint8_t& value)
    {
        value = readInt8();
        return *this;
    }
    MsgBuffer& operator>>(bool& value)
    {
        value = readInt8();
        return *this;
    }
    /**
     * @brief Get and remove a unsigned short value from the buffer.
     *
     * @return uint16_t
     */
    uint16_t readInt16();
    MsgBuffer& operator>>(uint16_t& value)
    {
        value = readInt16();
        return *this;
    }
    /**
     * @brief Get and remove a unsigned int value from the buffer.
     *
     * @return uint32_t
     */
    uint32_t readInt32();
    MsgBuffer& operator>>(uint32_t& value)
    {
        value = readInt32();
        return *this;
    }
    /**
     * @brief Get and remove a unsigned int64 value from the buffer.
     *
     * @return uint64_t
     */
    uint64_t readInt64();
    MsgBuffer& operator>>(uint64_t& value)
    {
        value = readInt64();
        return *this;
    }
    MsgBuffer& operator>>(MsgReal& r)
    {
        r._real = readInt64();
        r._prec = readInt8();
        return *this;
    }
    /**
     * @brief swap the buffer with another.
     *
     * @param buf
     */
    void swap(MsgBuffer &buf) noexcept;

    /**
     * @brief Return the size of the data in the buffer.
     *
     * @return size_t
     */
    size_t readableBytes() const
    {
        return tail_ - head_;
    }

    /**
     * @brief Return the size of the empty part in the buffer
     *
     * @return size_t
     */
    size_t writableBytes() const
    {
        return buffer_.size() - tail_;
    }

    /**
     * @brief Append new data to the buffer.
     *
     */
    void append(const MsgBuffer &buf);
    MsgBuffer& operator<<(const MsgBuffer& buf)
    {
        append(buf);
        return *this;
    }
    template <int N>
    void append(const char (&buf)[N])
    {
        assert(strnlen(buf, N) == N - 1);
        append(buf, N - 1);
    }
    template <int N>
    MsgBuffer& operator<<(const char(&buf)[N])
    {
        append(buf);
        return *this;
    }
    void append(const char *buf, size_t len);
    void append(const std::string &buf)
    {
        append(buf.c_str(), buf.length());
    }
    MsgBuffer& operator<<(const std::string& buf)
    {
        appendInt64(buf.length());
        append(buf.c_str(), buf.length());
        return *this;
    }
    MsgBuffer& operator>>(std::string& value)
    {
        value.clear();
        value = read(readInt64());
        return *this;
    }
    /**
     * @brief Append a byte value to the end of the buffer.
     *
     * @param b
     */
    void appendInt8(const uint8_t b)
    {
        append(static_cast<const char *>((void *)&b), 1);
    }
    MsgBuffer& operator<<(const uint8_t b)
    {
        appendInt8(b);
        return *this;
    }
    MsgBuffer& operator<<(const bool b)
    {
        appendInt8((uint8_t)b);
        return *this;
    }
    /**
     * @brief Append a unsigned short value to the end of the buffer.
     *
     * @param s
     */
    void appendInt16(const uint16_t s);
    MsgBuffer& operator<<(const uint16_t s)
    {
        appendInt16(s);
        return *this;
    }
    /**
     * @brief Append a unsigned int value to the end of the buffer.
     *
     * @param i
     */
    void appendInt32(const uint32_t i);
    MsgBuffer& operator<<(const uint32_t i)
    {
        appendInt32(i);
        return *this;
    }
    /**
     * @brief Appaend a unsigned int64 value to the end of the buffer.
     *
     * @param l
     */
    void appendInt64(const uint64_t l);
    MsgBuffer& operator<<(const uint64_t l)
    {
        appendInt64(l);
        return *this;
    }
    MsgBuffer& operator<<(const MsgReal r)
    {
        appendInt64(r._real);
        appendInt8(r._prec);
        return *this;
    }
    /**
     * @brief Put new data to the beginning of the buffer.
     *
     * @param buf
     * @param len
     */
    void addInFront(const char *buf, size_t len);

    /**
     * @brief Put a byte value to the beginning of the buffer.
     *
     * @param b
     */
    void addInFrontInt8(const uint8_t b)
    {
        addInFront(static_cast<const char *>((void *)&b), 1);
    }

    /**
     * @brief Put a unsigned short value to the beginning of the buffer.
     *
     * @param s
     */
    void addInFrontInt16(const uint16_t s);

    /**
     * @brief Put a unsigned int value to the beginning of the buffer.
     *
     * @param i
     */
    void addInFrontInt32(const uint32_t i);

    /**
     * @brief Put a unsigned int64 value to the beginning of the buffer.
     *
     * @param l
     */
    void addInFrontInt64(const uint64_t l);

    /**
     * @brief Remove all data in the buffer.
     *
     */
    void retrieveAll();

    /**
     * @brief Remove some bytes in the buffer.
     *
     * @param len
     */
    void retrieve(size_t len);

    /**
     * @brief Read data from a file descriptor and put it into the buffer.˝
     *
     * @param fd The file descriptor. It is usually a socket.
     * @param retErrno The error code when reading.
     * @return ssize_t The number of bytes read from the file descriptor. -1 is
     * returned when an error occurs.
     */
    ssize_t readStreamSocket(Poco::Net::StreamSocket& socket);

    /**
     * @brief Remove the data before a certain position from the buffer.
     *
     * @param end The position.
     */
    void retrieveUntil(const char *end)
    {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    /**
     * @brief Find the position of the buffer where the CRLF is found.
     *
     * @return const char*
     */
    const char *findCRLF() const
    {
        const char *crlf = std::search(peek(), beginWrite(), CRLF, CRLF + 2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    /**
     * @brief Make sure the buffer has enough spaces to write data.
     *
     * @param len
     */
    void ensureWritableBytes(size_t len);

    /**
     * @brief Move the write pointer forward when the new data has been written
     * to the buffer.
     *
     * @param len
     */
    void hasWritten(size_t len)
    {
        assert(len <= writableBytes());
        tail_ += len;
    }

    /**
     * @brief Move the write pointer backward to remove data in the end of the
     * buffer.
     *
     * @param offset
     */
    void unwrite(size_t offset)
    {
        assert(readableBytes() >= offset);
        tail_ -= offset;
    }

    /**
     * @brief Access a byte in the buffer.
     *
     * @param offset
     * @return const char&
     */
    const char &operator[](size_t offset) const
    {
        assert(readableBytes() >= offset);
        return peek()[offset];
    }
    char &operator[](size_t offset)
    {
        assert(readableBytes() >= offset);
        return begin()[head_ + offset];
    }

    static void	bulidHexString(std::string& targetBuffer, const char* pInValue, int nInLen);
    static void	queryHexString(std::string& targetBuffer, const char* pInValue, int nInLen);

  private:
    size_t head_;
    size_t initCap_;
    std::vector<char> buffer_;
    size_t tail_;
    const char *begin() const
    {
        return &buffer_[0];
    }
    char *begin()
    {
        return &buffer_[0];
    }
};

inline void swap(MsgBuffer &one, MsgBuffer &two) noexcept
{
    one.swap(two);
}

namespace std
{
template <>
inline void swap(MsgBuffer &one, MsgBuffer &two) noexcept
{
    one.swap(two);
}
}  // namespace std


template <typename T>
MsgBuffer& operator<<(MsgBuffer& b, std::vector<T> v)
{
    b << (uint32_t)v.size();
    for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); i++)
    {
        b << *i;
    }
    return b;
}

template <typename T>
MsgBuffer& operator>>(MsgBuffer& b, std::vector<T>& v)
{
    uint32_t vsize;
    b >> vsize;
    v.clear();
    while (vsize--)
    {
        T t;
        b >> t;
        v.push_back(t);
    }
    return b;
}

template <typename T>
MsgBuffer& operator<<(MsgBuffer& b, std::list<T> v)
{
    b << (uint32_t)v.size();
    for (typename std::list<T>::iterator i = v.begin(); i != v.end(); i++)
    {
        b << *i;
    }
    return b;
}

template <typename T>
MsgBuffer& operator>>(MsgBuffer& b, std::list<T>& v)
{
    uint32_t vsize;
    b >> vsize;
    v.clear();
    while (vsize--)
    {
        T t;
        b >> t;
        v.push_back(t);
    }
    return b;
}

template <typename K, typename V>
MsgBuffer& operator<<(MsgBuffer& b, std::map<K, V>& m)
{
    b << (uint32_t)m.size();
    for (typename std::map<K, V>::iterator i = m.begin(); i != m.end(); i++)
    {
        b << i->first << i->second;
    }
    return b;
}

template <typename K, typename V>
MsgBuffer& operator>>(MsgBuffer& b, std::map<K, V>& m)
{
    uint32_t msize;
    b >> msize;
    m.clear();
    while (msize--)
    {
        K k;
        V v;
        b >> k >> v;
        m.insert(std::make_pair(k, v));
    }
    return b;
}

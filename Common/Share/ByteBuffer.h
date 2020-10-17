#pragma once
#include "CommonException.h"
#include <string>
#include <vector>
#include <list>
#include <map>
#include <string.h>

class ByteBuffer
{
public:
	const static size_t DEFAULT_SIZE = 512;
	ByteBuffer(): _rpos(0), _wpos(0)
	{
		_storage.reserve(DEFAULT_SIZE);
	}
	explicit ByteBuffer(size_t size): _rpos(0), _wpos(0)
	{
		_storage.reserve(size);
	}
	ByteBuffer(uint8_t* pBuf, size_t len, size_t size): _rpos(0), _wpos(0)
	{
		_storage.reserve(size);
		append(pBuf, len);
	}
	ByteBuffer(const ByteBuffer &buf): _rpos(buf._rpos), _wpos(buf._wpos), _storage(buf._storage) { }

	void clear()
	{
		_storage.clear();
		_rpos = _wpos = 0;
	}
	inline void reset()
	{
		_rpos = _wpos = 0;
	}

	int realSize() { return _wpos - _rpos; }

	template <typename T> void append(T value)
	{
		append((uint8_t *)&value, sizeof(value));
	}

    void append(const uint8_t *src, size_t cnt)
    {
        if (cnt <= 0) return;

        if (_storage.size() < _wpos + cnt)
            _storage.resize(_wpos + cnt);
        memcpy(&_storage[_wpos], src, cnt);
        _wpos += cnt;
    }

    void append(const std::string& str)
    {
        append((uint8_t *)str.c_str(),str.size() + 1);
    }

    void append(const char *src, size_t cnt)
    {
        return append((const uint8_t *)src, cnt);
    }

    void append(const ByteBuffer& buffer)
    {
        if(buffer.size()) append(buffer.contents(),buffer.size());
    }

    void append(const ByteBuffer& buffer, size_t pos)
    {
        if(buffer.size() && (buffer.size() - pos) > 0) append(&(buffer._storage[pos]),buffer.size()-pos);
    }

    void put(size_t pos, const uint8_t *src, size_t cnt)
	{
		if (pos + cnt > size())
		{
			throw CommonException("ByteBuffer::put --> pos + cnt > size()");
		}
		memcpy(&_storage[pos], src, cnt);
	}

	template <typename T> void put(size_t pos, T value)
	{
		put(pos,(uint8_t *)&value,sizeof(value));
	}

	ByteBuffer &operator<<(bool value)
	{
		append<char>((char)value);
		return *this;
	}
	ByteBuffer &operator<<(uint8_t value)
	{
		append<uint8_t>(value);
		return *this;
	}
	ByteBuffer &operator<<(uint16_t value)
	{
		append<uint16_t>(value);
		return *this;
	}
	ByteBuffer &operator<<(uint32_t value)
	{
		append<uint32_t>(value);
		return *this;
	}
	ByteBuffer &operator<<(uint64_t value)
	{
		append<uint64_t>(value);
		return *this;
	}
    ByteBuffer &operator<<(char value)
    {
        append<char>(value);
        return *this;
    }
	ByteBuffer &operator<<(int8_t value)
	{
		append<int8_t>(value);
		return *this;
	}
	ByteBuffer &operator<<(int16_t value)
	{
		append<int16_t>(value);
		return *this;
	}
	ByteBuffer &operator<<(int32_t value)
	{
		append<int32_t>(value);
		return *this;
	}
	ByteBuffer &operator<<(int64_t value)
	{
		append<int64_t>(value);
		return *this;
	}
	ByteBuffer &operator<<(void* value)
	{
		append<void*>(value);
		return *this;
	}
	ByteBuffer &operator<<(float value)
	{
		append<float>(value);
		return *this;
	}
	ByteBuffer &operator<<(double value)
	{
		append<double>(value);
		return *this;
	}
	ByteBuffer &operator<<(const std::string &value)
	{
		append<uint16_t>(value.length());
		append((uint8_t *)value.c_str(), value.length());
		return *this;
	}
	ByteBuffer &operator<<(const char *str)
	{
		if (str)	
		{
            uint16_t len = strlen(str);
			append(len);
			append((uint8_t *)str, len);
		}
		else
		{
			append((uint16_t)0);
		}
		return *this;
	}


    template <typename T> T read() const
    {
        T r=read<T>(_rpos);
        _rpos += sizeof(T);
        return r;
    };
    template <typename T> T read(size_t pos) const
    {
        if (pos + sizeof(T) > size())
        {
			throw CommonException("ByteBuffer::template <typename T> T read(size_t pos) --> pos + sizeof(T) > size(),pos=" + std::to_string(pos));
        }
        return *((T*)&_storage[pos]);
    }

    void read(uint8_t *dest, size_t len) const
    {
		if (_rpos + len > size())
		{
			throw CommonException("ByteBuffer::read(uint8_t *dest, size_t len) --> _rpos + len > size()");
		}
        memcpy(dest, &_storage[_rpos], len);
        _rpos += len;
    }

    void read(ByteBuffer& buffer, size_t len) const
    {
		if (_rpos + len > size())
		{
			throw CommonException("ByteBuffer::read(ByteBuffer& buffer, size_t len) --> _rpos + len > size()");
		}
        buffer.append(&_storage[_rpos], len);
        _rpos += len;
    }

	const ByteBuffer &operator>>(bool &value) const
	{
		value = read<char>() > 0 ? true : false;
		return *this;
	}
	const ByteBuffer &operator>>(uint8_t &value) const
	{
		value = read<uint8_t>();
		return *this;
	}
	const ByteBuffer &operator>>(uint16_t &value) const
	{
		value = read<uint16_t>();
		return *this;
	}
	const ByteBuffer &operator>>(uint32_t &value) const
	{
		value = read<uint32_t>();
		return *this;
	}
	const ByteBuffer &operator>>(uint64_t &value) const
	{
		value = read<uint64_t>();
		return *this;
	}

    const ByteBuffer &operator>>(char &value) const
    {
        value = read<char>();
        return *this;
    }

	const ByteBuffer &operator>>(int8_t &value) const
	{
		value = read<int8_t>();
		return *this;
	}
	const ByteBuffer &operator>>(int16_t &value) const
	{
		value = read<int16_t>();
		return *this;
	}
	const ByteBuffer &operator>>(int32_t &value) const
	{
		value = read<int32_t>();
		return *this;
	}
	const ByteBuffer &operator>>(int64_t &value) const
	{
		value = read<int64_t>();
		return *this;
	}

	const ByteBuffer &operator>>(void* &value) const
	{
		value = read<void*>();
		return *this;
	}
	const ByteBuffer &operator>>(float &value) const
	{
		value = read<float>();
		return *this;
	}
	const ByteBuffer &operator>>(double &value) const
	{
		value = read<double>();
		return *this;
	}
	const ByteBuffer &operator>>(std::string& value) const
	{
		value.clear();
		unsigned short len = read<unsigned short>();
		if (len > 0)
		{
			if (len > size() - _rpos)
			{
				value = "error";
			}		
			else
			{
				if (len < 512)
				{
					char buff[512];
					read((uint8_t*)buff, len);
					value.append(buff, len);
				}
				else if (len < 2048)
				{
					char buff[2048];
					read((uint8_t*)buff, len);
					value.append(buff, len);
				}
				else
				{
					char buff[65535];
					read((uint8_t*)buff, len);
					value.append(buff, len);
				}
			}		
		}		
		return *this;
	}

    uint8_t operator[](size_t pos) const
	{
		return read<uint8_t>(pos);
	}

	size_t rpos() const
	{
		return _rpos;
	};

	size_t rpos(size_t rpos)
	{
		_rpos = rpos;
		return _rpos;
	};

	size_t wpos() const
	{
		return _wpos;
	}

	size_t wpos(size_t wpos)
	{
		_wpos = wpos;
		return _wpos;
	}

	const uint8_t *contents() const { return &_storage[0]; };

	inline size_t size() const { return _storage.size(); };

	void resize(size_t newsize)
	{
		_storage.resize(newsize);
		_rpos = 0;
		_wpos = size();
	};
	void reserve(size_t ressize)
	{
		if (ressize > size()) _storage.reserve(ressize);
	};

	void swap(ByteBuffer & other)
	{
		std::swap(_rpos, other._rpos);
		std::swap(_wpos, other._wpos);
		_storage.swap(other._storage);
	}

	static void	bulidHexString(std::string& targetBuffer, const char* pInValue, int nInLen);
    static void	queryHexString(std::string& targetBuffer, const char* pInValue, int nInLen);

private:
	mutable size_t _rpos, _wpos;
	std::vector<uint8_t> _storage;
};


template <typename T>
ByteBuffer &operator<<(ByteBuffer &b, std::vector<T> v)
{
	b << (uint32_t)v.size();
	for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); i++)
	{
		b << *i;
	}
	return b;
}

template <typename T>
ByteBuffer &operator>>(ByteBuffer &b, std::vector<T> &v)
{
    uint32_t vsize;
	b >> vsize;
	v.clear();
	while(vsize--)
	{
		T t;
		b >> t;
		v.push_back(t);
	}
	return b;
}

template <typename T>
ByteBuffer &operator<<(ByteBuffer &b, std::list<T> v)
{
	b << (uint32_t)v.size();
	for (typename std::list<T>::iterator i = v.begin(); i != v.end(); i++)
	{
		b << *i;
	}
	return b;
}

template <typename T>
ByteBuffer &operator>>(ByteBuffer &b, std::list<T> &v)
{
    uint32_t vsize;
	b >> vsize;
	v.clear();
	while(vsize--)
	{
		T t;
		b >> t;
		v.push_back(t);
	}
	return b;
}

template <typename K, typename V>
ByteBuffer &operator<<(ByteBuffer &b, std::map<K, V> &m)
{
	b << (uint32_t)m.size();
	for (typename std::map<K, V>::iterator i = m.begin(); i != m.end(); i++)
	{
		b << i->first << i->second;
	}
	return b;
}

template <typename K, typename V>
ByteBuffer &operator>>(ByteBuffer &b, std::map<K, V> &m)
{
    uint32_t msize;
	b >> msize;
	m.clear();
	while(msize--)
	{
		K k;
		V v;
		b >> k >> v;
		m.insert(make_pair(k, v));
	}
	return b;
}


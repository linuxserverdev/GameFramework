#pragma once
#include <Logger/Logger.h>
#include <lua.hpp>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>


template <typename T>
struct init_value_traits_t
{
    inline static T value(){ return T(); }
};

template <>
struct init_value_traits_t<std::string>
{
    inline static const char* value(){ return ""; }
};

template<typename T>
struct lua_op_t
{
};

template<>
struct lua_op_t<const char*>
{
    static void push_stack(lua_State* ls_, const char* arg_)
    {
        lua_pushstring(ls_, arg_);
    }
};

template<>
struct lua_op_t<char*>
{
    static void push_stack(lua_State* ls_, const char* arg_)
    {
        lua_pushstring(ls_, arg_);
    }
};

struct Type_Null;
template<>
struct lua_op_t<Type_Null>
{
    static void push_stack(lua_State* ls_, const Type_Null& arg_)
    {
        lua_pushnil (ls_);
    }
    static int get_lua_value(lua_State* ls_, int pos_, Type_Null& param_)
    {
        return 0;
    }
};

template<>
struct lua_op_t<int64_t>
{
    static void push_stack(lua_State* ls_, int64_t arg_)
    {
#if LUA_VERSION_NUM >= 503
        lua_pushinteger(ls_, arg_);
#else
        std::stringstream ss;
        ss << arg_;
        std::string str = ss.str();
        lua_pushlstring(ls_, str.c_str(), str.length());
#endif
    }

    static int get_lua_value(lua_State* ls_, int pos_, int64_t& param_)
    {
#if LUA_VERSION_NUM >= 503
        if (!lua_isinteger(ls_, pos_))
		{
			return -1;
		}
		param_ = lua_tointeger(ls_, pos_);
#else
        if (!lua_isstring(ls_, pos_))
        {
            return -1;
        }

        size_t len  = 0;
        const char* src = lua_tolstring(ls_, pos_, &len);
        param_ = (int64_t)strtoll(src, NULL, 10);
#endif
        return 0;
    }

};

template<> struct lua_op_t<uint64_t>
{
    static void push_stack(lua_State* ls_, uint64_t arg_)
    {
        std::stringstream ss;
        ss << arg_;
        std::string str = ss.str();
        lua_pushlstring(ls_, str.c_str(), str.length());
    }

    static int get_lua_value(lua_State* ls_, int pos_, uint64_t& param_)
    {
        if (!lua_isstring(ls_, pos_))
        {
            return -1;
        }

        size_t len  = 0;
        const char* src = lua_tolstring(ls_, pos_, &len);
        param_ = (uint64_t)strtoull(src, NULL, 10);
        return 0;
    }
};

template<> struct lua_op_t<int8_t>
{
    static void push_stack(lua_State* ls_, int8_t arg_)
    {
        lua_pushnumber(ls_, (lua_Number)arg_);
    }

    static int get_lua_value(lua_State* ls_, int pos_, int8_t& param_)
    {
        if (!lua_isnumber(ls_, pos_))
        {
            return -1;
        }
        param_ = (int8_t)lua_tonumber(ls_, pos_);
        return 0;
    }
};

template<> struct lua_op_t<uint8_t>
{
    static void push_stack(lua_State* ls_, uint8_t arg_)
    {
        lua_pushnumber(ls_, (lua_Number)arg_);
    }

    static int get_lua_value(lua_State* ls_, int pos_, uint8_t& param_)
    {
        if (!lua_isnumber(ls_, pos_))
        {
            return -1;
        }
        param_ = (uint8_t)lua_tonumber(ls_, pos_);
        return 0;
    }
};

template<> struct lua_op_t<char>
{
    static void push_stack(lua_State* ls_, char arg_)
    {
        lua_pushnumber(ls_, (lua_Number)arg_);
    }
    static int get_ret_value(lua_State* ls_, int pos_, char& param_)
    {
        if (!lua_isnumber(ls_, pos_))
        {
            return -1;
        }
        param_ = (char)lua_tonumber(ls_, pos_);
        return 0;
    }
};

template<> struct lua_op_t<int16_t>
{
    static void push_stack(lua_State* ls_, int16_t arg_)
    {
        lua_pushnumber(ls_, (lua_Number)arg_);
    }
    static int get_lua_value(lua_State* ls_, int pos_, int16_t& param_)
    {
        if (!lua_isnumber(ls_, pos_))
        {
            return -1;
        }
        param_ = (int16_t)lua_tonumber(ls_, pos_);
        return 0;
    }
};
template<> struct lua_op_t<uint16_t>
{
    static void push_stack(lua_State* ls_, uint16_t arg_)
    {
        lua_pushnumber(ls_, (lua_Number)arg_);
    }
    static int get_lua_value(lua_State* ls_, int pos_, uint16_t& param_)
    {
        if (!lua_isnumber(ls_, pos_))
        {
            return -1;
        }
        param_ = (uint16_t)lua_tonumber(ls_, pos_);
        return 0;
    }
};
template<> struct lua_op_t<int32_t>
{
    static void push_stack(lua_State* ls_, int32_t arg_)
    {
        lua_pushnumber(ls_, (lua_Number)arg_);
    }
    static int get_lua_value(lua_State* ls_, int pos_, int32_t& param_)
    {
        if (!lua_isnumber(ls_, pos_))
        {
            return -1;
        }
        param_ = (int32_t)lua_tonumber(ls_, pos_);
        return 0;
    }
};
template<> struct lua_op_t<uint32_t>
{
    static void push_stack(lua_State* ls_, uint32_t arg_)
    {
        lua_pushnumber(ls_, (lua_Number)arg_);
    }
    static int get_lua_value(lua_State* ls_, int pos_, uint32_t& param_)
    {
        if (!lua_isnumber(ls_, pos_))
        {
            return -1;
        }
        param_ = (uint32_t)lua_tonumber(ls_, pos_);
        return 0;
    }
};

template<>
struct lua_op_t<bool>
{
    static void push_stack(lua_State* ls_, bool arg_)
    {
        lua_pushboolean(ls_, arg_);
    }
    static int get_lua_value(lua_State* ls_, int pos_, bool& param_)
    {
        //! nil 自动转换为false
        if (lua_isnil(ls_, pos_))
        {
            param_ = false;
            return 0;
        }
        if (!lua_isboolean(ls_, pos_))
        {
            return -1;
        }

        param_ = (0 != lua_toboolean(ls_, pos_));
        return 0;
    }
};

template<>
struct lua_op_t<std::string>
{
    static void push_stack(lua_State* ls_, const std::string& arg_)
    {
        lua_pushlstring(ls_, arg_.c_str(), arg_.length());
    }
    static int get_lua_value(lua_State* ls_, int pos_, std::string& param_)
    {
        if (!lua_isstring(ls_, pos_))
        {
            return -1;
        }

        lua_pushvalue(ls_, pos_);
        size_t len  = 0;
        const char* src = lua_tolstring(ls_, -1, &len);
        param_.assign(src, len);
        lua_pop(ls_, 1);

        return 0;
    }
};

template<> struct lua_op_t<const std::string&>
{
    static void push_stack(lua_State *ls_, const std::string &arg_)
    {
        lua_pushlstring(ls_, arg_.c_str(), arg_.length());
    }
};

template<> struct lua_op_t<float>
{
    static void push_stack(lua_State* ls_, float arg_)
    {
        lua_pushnumber(ls_, (lua_Number)arg_);
    }
    static int get_lua_value(lua_State* ls_, int pos_, float& param_)
    {
        if (!lua_isnumber(ls_, pos_))
        {
            return -1;
        }
        param_ = (float)lua_tonumber(ls_, pos_);
        return 0;
    }
};

template<> struct lua_op_t<double>
{
    static void push_stack(lua_State* ls_, double arg_)
    {
        lua_pushnumber(ls_, (lua_Number)arg_);
    }
    static int get_lua_value(lua_State* ls_, int pos_, double& param_)
    {
        if (!lua_isnumber(ls_, pos_))
        {
            return -1;
        }
        param_ = (double)lua_tonumber(ls_, pos_);
        return 0;
    }
};

template<>
struct lua_op_t<void*>
{
    static void push_stack(lua_State* ls_, void* arg_)
    {
        lua_pushlightuserdata(ls_, arg_);
    }

    static int get_lua_value(lua_State* ls_, int pos_, void* & param_)
    {
        if (!lua_isuserdata(ls_, pos_))
        {
            char buff[128];
            snprintf(buff, sizeof(buff), "userdata param expected, but type<%s> provided",
                      lua_typename(ls_, lua_type(ls_, pos_)));
            LOG_ERROR << buff;
            return -1;
        }

        param_ = lua_touserdata(ls_, pos_);
        return 0;
    }
};

template<typename T>
struct lua_op_t<std::vector<T>>
{
    static void push_stack(lua_State* ls_, const std::vector<T>& arg_)
    {
        lua_newtable(ls_);
        typename std::vector<T>::const_iterator it = arg_.begin();
        for (int i = 1; it != arg_.end(); ++it, ++i)
        {
            lua_op_t<int>::push_stack(ls_, i);
            lua_op_t<T>::push_stack(ls_, *it);
            lua_settable(ls_, -3);
        }
    }
    static int get_lua_value(lua_State* ls_, int pos_, std::vector<T>& param_)
    {
        if (0 == lua_istable(ls_, pos_))
        {
            return -1;
        }
        lua_pushnil(ls_);
        int real_pos = pos_;
        if (pos_ < 0) real_pos = real_pos - 1;

        while (lua_next(ls_, real_pos) != 0)
        {
            param_.push_back(T());
            if (lua_op_t<T>::get_lua_value(ls_, -1, param_[param_.size() - 1]) < 0)
            {
                return -1;
            }
            lua_pop(ls_, 1);
        }
        return 0;
    }
};

template<typename T>
struct lua_op_t<std::list<T>>
{
    static void push_stack(lua_State* ls_, const std::list<T>& arg_)
    {
        lua_newtable(ls_);
        typename std::list<T>::const_iterator it = arg_.begin();
        for (int i = 1; it != arg_.end(); ++it, ++i)
        {
            lua_op_t<int>::push_stack(ls_, i);
            lua_op_t<T>::push_stack(ls_, *it);
            lua_settable(ls_, -3);
        }
    }
    static int get_lua_value(lua_State* ls_, int pos_, std::list<T>& param_)
    {
        if (0 == lua_istable(ls_, pos_))
        {
            return -1;
        }
        lua_pushnil(ls_);
        int real_pos = pos_;
        if (pos_ < 0) real_pos = real_pos - 1;

        while (lua_next(ls_, real_pos) != 0)
        {
            param_.push_back(T());
            if (lua_op_t<T>::get_lua_value(ls_, -1, (param_.back())) < 0)
            {
                return -1;
            }
            lua_pop(ls_, 1);
        }
        return 0;
    }
};

template<typename T>
struct lua_op_t<std::set<T>>
{
    static void push_stack(lua_State* ls_, const std::set<T>& arg_)
    {
        lua_newtable(ls_);
        typename std::set<T>::const_iterator it = arg_.begin();
        for (int i = 1; it != arg_.end(); ++it, ++i)
        {
            lua_op_t<int>::push_stack(ls_, i);
            lua_op_t<T>::push_stack(ls_, *it);
            lua_settable(ls_, -3);
        }
    }
    static int get_lua_value(lua_State* ls_, int pos_, std::set<T>& param_)
    {
        if (0 == lua_istable(ls_, pos_))
        {
            return -1;
        }
        lua_pushnil(ls_);
        int real_pos = pos_;
        if (pos_ < 0) real_pos = real_pos - 1;

        while (lua_next(ls_, real_pos) != 0)
        {
            T val = init_value_traits_t<T>::value();
            if (lua_op_t<T>::get_lua_value(ls_, -1, val) < 0)
            {
                return -1;
            }
            param_.insert(val);
            lua_pop(ls_, 1);
        }
        return 0;
    }
};
template<typename K, typename V>
struct lua_op_t<std::map<K, V>>
{
    static void push_stack(lua_State* ls_, const std::map<K, V>& arg_)
    {
        lua_newtable(ls_);
        typename std::map<K, V>::const_iterator it = arg_.begin();
        for (; it != arg_.end(); ++it)
        {
            lua_op_t<K>::push_stack(ls_, it->first);
            lua_op_t<V>::push_stack(ls_, it->second);
            lua_settable(ls_, -3);
        }
    }
    static int get_lua_value(lua_State* ls_, int pos_, std::map<K, V>& param_)
    {
        if (0 == lua_istable(ls_, pos_))
        {
            return -1;
        }
        lua_pushnil(ls_);
        int real_pos = pos_;
        if (pos_ < 0) real_pos = real_pos - 1;

        while (lua_next(ls_, real_pos) != 0)
        {
            K key = init_value_traits_t<K>::value();
            V val = init_value_traits_t<V>::value();

            if (lua_op_t<K>::get_lua_value(ls_, -2, key) < 0 ||
                lua_op_t<V>::get_lua_value(ls_, -1, val) < 0)
            {
                return -1;
            }
            param_.insert(make_pair(key, val));
            lua_pop(ls_, 1);
        }
        return 0;
    }
};

#include "LuaFunctor.h"

Type_Null nil;

bool findLuaMethod(lua_State* L, const char* func_name)
{
    int top = lua_gettop(L);

    std::string handler_name = func_name;
    // do we have any dots in the handler name? if so we grab the function as a table field
    std::string::size_type i = handler_name.find_first_of('.');
    if (i!=std::string::npos)
    {
        // split the rest of the string up in parts seperated by '.'
        // TODO: count the dots and size the vector accordingly from the beginning.
        std::vector<std::string> parts;
        std::string::size_type start = 0;
        do
        {
            parts.push_back(handler_name.substr(start,i-start));
            start = i+1;
            i = handler_name.find_first_of('.',start);
        } while(i!=std::string::npos);

        // add last part
        parts.push_back(handler_name.substr(start));

        // first part is the global
        lua_getglobal(L, parts[0].c_str());
        if (!lua_istable(L,-1))
        {
            lua_settop(L,top);
            return false;
        }
        // if there is more than two parts, we have more tables to go through
        std::vector<std::string>::size_type visz = parts.size();
        if (visz-- > 2) // avoid subtracting one later on
        {
            // go through all the remaining parts to (hopefully) have a valid Lua function in the end
            std::vector<std::string>::size_type vi = 1;
            while (vi<visz)
            {
                // push key, and get the next table
                lua_pushstring(L,parts[vi].c_str());
                lua_gettable(L,-2);
                if (!lua_istable(L,-1))
                {
                    lua_settop(L,top);
                    return false;
                }
                // get rid of the last table and move on
                lua_remove(L,-2);
                vi++;
            }
        }
        // now we are ready to get the function to call ... phew :)
        lua_pushstring(L,parts[visz].c_str());
        lua_gettable(L,-2);
        lua_remove(L,-2); // get rid of the table
    }
        // just a regular global function
    else
    {
        lua_getglobal(L, handler_name.c_str());
    }
    // is it a function
    if (!lua_isfunction(L,-1))
    {
        lua_settop(L, top);
        return false;
    }
    return true;
}

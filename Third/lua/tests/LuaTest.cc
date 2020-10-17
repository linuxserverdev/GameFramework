#include <lua/lua.hpp>
#include <iostream>
using namespace std;

int main()
{
    lua_State* state = luaL_newstate();
    luaL_openlibs(state);

    if (luaL_dostring(state, "print([[hello world]])") != 0)
    {
        cout << "excute lua file failed!" << endl;
    }
    lua_close(state);
}
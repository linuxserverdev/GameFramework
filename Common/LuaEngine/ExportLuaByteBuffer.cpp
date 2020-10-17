/*
** Lua binding: ExportLuaByteBuffer
** Generated automatically by tolua++-1.0.92 on Wed Oct 14 09:43:03 2020.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_ExportLuaByteBuffer_open (lua_State* tolua_S);

#include "LuaByteBuffer.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"LuaByteBuffer");
}

/* method: pushBool of class  LuaByteBuffer */
#ifndef TOLUA_DISABLE_tolua_ExportLuaByteBuffer_LuaByteBuffer_pushBool00
static int tolua_ExportLuaByteBuffer_LuaByteBuffer_pushBool00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaByteBuffer",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaByteBuffer* self = (LuaByteBuffer*)  tolua_tousertype(tolua_S,1,0);
  bool flag = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pushBool'", NULL);
#endif
  {
   self->pushBool(flag);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pushBool'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pushInt of class  LuaByteBuffer */
#ifndef TOLUA_DISABLE_tolua_ExportLuaByteBuffer_LuaByteBuffer_pushInt00
static int tolua_ExportLuaByteBuffer_LuaByteBuffer_pushInt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaByteBuffer",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaByteBuffer* self = (LuaByteBuffer*)  tolua_tousertype(tolua_S,1,0);
  int value = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pushInt'", NULL);
#endif
  {
   self->pushInt(value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pushInt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pushChar of class  LuaByteBuffer */
#ifndef TOLUA_DISABLE_tolua_ExportLuaByteBuffer_LuaByteBuffer_pushChar00
static int tolua_ExportLuaByteBuffer_LuaByteBuffer_pushChar00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaByteBuffer",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaByteBuffer* self = (LuaByteBuffer*)  tolua_tousertype(tolua_S,1,0);
  char value = ((char)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pushChar'", NULL);
#endif
  {
   self->pushChar(value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pushChar'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pushDouble of class  LuaByteBuffer */
#ifndef TOLUA_DISABLE_tolua_ExportLuaByteBuffer_LuaByteBuffer_pushDouble00
static int tolua_ExportLuaByteBuffer_LuaByteBuffer_pushDouble00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaByteBuffer",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaByteBuffer* self = (LuaByteBuffer*)  tolua_tousertype(tolua_S,1,0);
  double value = ((double)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pushDouble'", NULL);
#endif
  {
   self->pushDouble(value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pushDouble'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pushChars of class  LuaByteBuffer */
#ifndef TOLUA_DISABLE_tolua_ExportLuaByteBuffer_LuaByteBuffer_pushChars00
static int tolua_ExportLuaByteBuffer_LuaByteBuffer_pushChars00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaByteBuffer",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaByteBuffer* self = (LuaByteBuffer*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pushChars'", NULL);
#endif
  {
   self->pushChars(str);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pushChars'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pushString of class  LuaByteBuffer */
#ifndef TOLUA_DISABLE_tolua_ExportLuaByteBuffer_LuaByteBuffer_pushString00
static int tolua_ExportLuaByteBuffer_LuaByteBuffer_pushString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaByteBuffer",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaByteBuffer* self = (LuaByteBuffer*)  tolua_tousertype(tolua_S,1,0);
  const std::string str = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pushString'", NULL);
#endif
  {
   self->pushString(str);
   tolua_pushcppstring(tolua_S,(const char*)str);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pushString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pushLString of class  LuaByteBuffer */
#ifndef TOLUA_DISABLE_tolua_ExportLuaByteBuffer_LuaByteBuffer_pushLString00
static int tolua_ExportLuaByteBuffer_LuaByteBuffer_pushLString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaByteBuffer",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaByteBuffer* self = (LuaByteBuffer*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  int lengh = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pushLString'", NULL);
#endif
  {
   self->pushLString(str,lengh);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pushLString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pushPbc of class  LuaByteBuffer */
#ifndef TOLUA_DISABLE_tolua_ExportLuaByteBuffer_LuaByteBuffer_pushPbc00
static int tolua_ExportLuaByteBuffer_LuaByteBuffer_pushPbc00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaByteBuffer",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaByteBuffer* self = (LuaByteBuffer*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  int lengh = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pushPbc'", NULL);
#endif
  {
   self->pushPbc(str,lengh);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pushPbc'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: append of class  LuaByteBuffer */
#ifndef TOLUA_DISABLE_tolua_ExportLuaByteBuffer_LuaByteBuffer_append00
static int tolua_ExportLuaByteBuffer_LuaByteBuffer_append00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaByteBuffer",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"LuaByteBuffer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaByteBuffer* self = (LuaByteBuffer*)  tolua_tousertype(tolua_S,1,0);
  LuaByteBuffer* buf = ((LuaByteBuffer*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'append'", NULL);
#endif
  {
   self->append(buf);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'append'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_ExportLuaByteBuffer_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"LuaByteBuffer","LuaByteBuffer","",NULL);
  tolua_beginmodule(tolua_S,"LuaByteBuffer");
   tolua_function(tolua_S,"pushBool",tolua_ExportLuaByteBuffer_LuaByteBuffer_pushBool00);
   tolua_function(tolua_S,"pushInt",tolua_ExportLuaByteBuffer_LuaByteBuffer_pushInt00);
   tolua_function(tolua_S,"pushChar",tolua_ExportLuaByteBuffer_LuaByteBuffer_pushChar00);
   tolua_function(tolua_S,"pushDouble",tolua_ExportLuaByteBuffer_LuaByteBuffer_pushDouble00);
   tolua_function(tolua_S,"pushChars",tolua_ExportLuaByteBuffer_LuaByteBuffer_pushChars00);
   tolua_function(tolua_S,"pushString",tolua_ExportLuaByteBuffer_LuaByteBuffer_pushString00);
   tolua_function(tolua_S,"pushLString",tolua_ExportLuaByteBuffer_LuaByteBuffer_pushLString00);
   tolua_function(tolua_S,"pushPbc",tolua_ExportLuaByteBuffer_LuaByteBuffer_pushPbc00);
   tolua_function(tolua_S,"append",tolua_ExportLuaByteBuffer_LuaByteBuffer_append00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_ExportLuaByteBuffer (lua_State* tolua_S) {
 return tolua_ExportLuaByteBuffer_open(tolua_S);
};
#endif


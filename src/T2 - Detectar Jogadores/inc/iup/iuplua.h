/** \file
 * \brief IUP Binding for Lua.
 *
 * See Copyright Notice in iup.h
 * $Id: iuplua.h,v 1.8 2004/07/27 00:49:26 mark Exp $
 */
 
#ifndef __IUPLUA_H 
#define __IUPLUA_H

#include <iup.h>
#include <lua.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LUA_NOOBJECT  /* Lua 3 */
int iuplua_open (void);
int iupkey_open (void);
int iuplua_dofile (char *filename);
lua_Object iuplua_register (Ihandle* element);
void iuplua_pushihandle(Ihandle *n);
#endif

#ifdef LUA_NOREF  /* Lua 4 or 5 */
void iuplua_open(lua_State *L);
void iuplua_changeEnv(lua_State *L);
void iuplua_returnEnv(lua_State *L);
int iupkey_open(lua_State *L);
void iuplua_pushihandle(lua_State *L, Ihandle *n);
int iuplua_dofile(lua_State *L, char *filename);
int iuplua_dostring(lua_State *L, const char *string, const char *chunk_name);

#endif

#ifdef __cplusplus
}
#endif

#endif

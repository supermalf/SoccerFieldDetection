/** \file
 * \brief iupmatrix Binding for Lua.
 *
 * See Copyright Notice in iup.h
 * $Id: luamatrix.h,v 1.3 2003/06/17 22:05:03 scuri Exp $
 */
 
#ifndef __LUAMATRIX_H 
#define __LUAMATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LUA_NOOBJECT  /* Lua 3 */
int matrixlua_open (void);
#endif

#ifdef LUA_NOREF  /* Lua 4 or 5 */
void matrixlua_open(lua_State * L);
#endif

#ifdef __cplusplus
}
#endif

#endif

/** \file
 * \brief iupspin Binding for Lua.
 *
 * See Copyright Notice in iup.h
 * $Id: luaspin.h,v 1.1 2005/03/14 20:13:29 scuri Exp $
 */
 
#ifndef __LUASPIN_H 
#define __LUASPIN_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LUA_NOOBJECT  /* Lua 3 */
int spinlua_open (void);
#endif

#ifdef LUA_NOREF  /* Lua 4 or 5 */
void spinlua_open (lua_State * L);
void spinboxlua_open(lua_State * L);
#endif

#ifdef __cplusplus
}
#endif

#endif

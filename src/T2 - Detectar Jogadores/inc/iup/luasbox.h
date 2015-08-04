/** \file
 * \brief iupsbox Binding for Lua.
 *
 * See Copyright Notice in iup.h
 * $Id: luasbox.h,v 1.1 2004/02/09 17:40:45 mark Exp $
 */
 
#ifndef __LUASBOX_H 
#define __LUASBOX_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LUA_NOOBJECT  /* Lua 3 */
int sboxlua_open (void);
#endif

#ifdef LUA_NOREF  /* Lua 4 or 5 */
void sboxlua_open (lua_State * L);
#endif

#ifdef __cplusplus
}
#endif

#endif

/** \file
 * \brief iupcolorbar Binding for Lua.
 *
 * See Copyright Notice in iup.h
 * $Id: luacolorbar.h,v 1.1 2005/04/16 21:11:37 scuri Exp $
 */
 
#ifndef __LUACOLORBAR_H 
#define __LUACOLORBAR_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LUA_NOOBJECT  /* Lua 3 */
int colorbarlua_open (void);
#endif

#ifdef LUA_NOREF  /* Lua 4 or 5 */
void colorbarlua_open (lua_State * L);
#endif

#ifdef __cplusplus
}
#endif

#endif

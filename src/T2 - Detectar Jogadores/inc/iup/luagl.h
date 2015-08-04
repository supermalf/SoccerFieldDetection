/** \file
 * \brief Bindig of iupglcanvas to Lua.
 *
 * See Copyright Notice in iup.h
 * $Id: luagl.h,v 1.4 2004/08/10 00:32:27 scuri Exp $
 */
 
#ifndef __LUAGL_H 
#define __LUAGL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LUA_NOOBJECT  /* Lua 3 */
int gllua_open(void);
#endif

#ifdef LUA_NOREF  /* Lua 4 or 5 */
void gllua_open (lua_State * L);
#endif

#ifdef __cplusplus
}
#endif

#endif

/** \file
 * \brief iupgetcolor Binding for Lua.
 *
 * See Copyright Notice in iup.h
 * $Id: luagc.h,v 1.5 2004/08/10 00:34:43 scuri Exp $
 */
 
#ifndef __LUAGC_H 
#define __LUAGC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LUA_NOOBJECT  /* Lua 3 */
int gclua_open (void);
#endif

#ifdef LUA_NOREF  /* Lua 4 or 5 */
void gclua_open (lua_State * L);
#endif

#ifdef __cplusplus
}
#endif

#endif

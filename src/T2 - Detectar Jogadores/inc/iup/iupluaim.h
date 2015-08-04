/** \file
 * \brief Bindig of iupim functions to Lua.
 *
 * See Copyright Notice in iup.h
 * $Id: iupluaim.h,v 1.2 2004/08/10 00:32:26 scuri Exp $
 */
 
#ifndef __IUPLUAIM_H
#define __IUPLUAIM_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LUA_NOOBJECT  /* Lua 3 */
void iupluaim_open(void);
#endif

#ifdef LUA_NOREF  /* Lua 4 or 5 */
void iupluaim_open(lua_State * L);
#endif

#ifdef __cplusplus
}
#endif

#endif

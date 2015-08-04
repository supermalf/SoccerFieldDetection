/** \file
 * \brief iuptree Binding for Lua.
 *
 * See Copyright Notice in iup.h
 * $Id: luatree.h,v 1.3 2003/06/17 22:05:03 scuri Exp $
 */
 
#ifndef __LUATREE_H 
#define __LUATREE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LUA_NOOBJECT  /* Lua 3 */
int treelua_open (void);
#endif

#ifdef LUA_NOREF  /* Lua 4 or 5 */
void treelua_open (lua_State * L);
#endif

#ifdef __cplusplus
}
#endif

#endif

/** \file
 * \brief iupspeech Binding for Lua.
 *
 * See Copyright Notice in iup.h
 * $Id: luaspeech.h,v 1.1 2003/07/11 21:09:24 mark Exp $
 */
 
#ifndef _LUASPEECH_H 
#define _LUASPEECH_H 

#ifdef __cplusplus
extern "C" {
#endif
               
#ifdef LUA_NOOBJECT  /* Lua 3 */
void speechlua_open (void);
#endif

#ifdef LUA_NOREF  /* Lua 4 or 5 */
void speechlua_open (lua_State * L);
#endif

#ifdef __cplusplus
}
#endif

#endif

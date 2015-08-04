/** \file
 * \brief iupcontrols Binding for Lua.
 *
 * See Copyright Notice in iup.h
 * $Id: luacontrols.h,v 1.9 2005/04/16 21:11:37 scuri Exp $
 */
 
#ifndef __LUACONTROLS_H 
#define __LUACONTROLS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "luaval.h"
#include "luagauge.h"
#include "luatabs.h"
#include "luatree.h"
#include "luamatrix.h"
#include "luamask.h"
#include "luadial.h"
#include "luacb.h"
#include "luagc.h"
#include "luasbox.h"
#include "luaspin.h"
#include "luacolorbar.h"

#ifdef LUA_NOOBJECT  /* Lua 3 */
int controlslua_open (void);
#endif

#ifdef LUA_NOREF  /* Lua 4 or 5 */
int controlslua_open (lua_State * L);
#endif

#ifdef __cplusplus
}
#endif

#endif

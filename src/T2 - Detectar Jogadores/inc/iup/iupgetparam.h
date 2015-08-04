/** \file
 * \brief iupgetparam dialog.
 *
 * See Copyright Notice in iup.h
 * $Id: iupgetparam.h,v 1.3 2004/06/22 13:37:00 scuri Exp $
 */
 
#ifndef __IUPGETPARAM_H 
#define __IUPGETPARAM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*Iparamcb)(Ihandle* dialog, int param_index, void* user_data);

int IupGetParam(const char* title, Iparamcb action, void* user_data, const char* format,...);

#ifdef __cplusplus
}
#endif

#endif

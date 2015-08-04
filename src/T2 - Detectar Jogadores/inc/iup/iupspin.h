/** \file
 * \brief iupspin control.
 *
 * See Copyright Notice in iup.h
 * $Id: iupspin.h,v 1.3 2005/04/01 16:49:15 scuri Exp $
 */
 
#ifndef __IUPSPIN_H 
#define __IUPSPIN_H

#ifdef __cplusplus
extern "C" {
#endif

Ihandle *IupSpin(void);
Ihandle* IupSpinbox(Ihandle* ctrl);
void IupSpinOpen(void);
void IupSpinClose(void);

#ifdef __cplusplus
}
#endif

#endif

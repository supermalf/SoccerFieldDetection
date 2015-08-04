/** \file
 * \brief initializes iupdial, iupgauge, iuptabs, iupcb, iupgc and iupval controls.
 *
 * See Copyright Notice in iup.h
 * $Id: iupcontrols.h,v 1.12 2005/04/16 04:04:22 scuri Exp $
 */
 
#ifndef __IUPCONTROLS_H 
#define __IUPCONTROLS_H

#include "iupdial.h"
#include "iupgauge.h"
#include "iuptabs.h"
#include "iupval.h"
#include "iupmatrix.h"
#include "iuptree.h"
#include "iupsbox.h"
#include "iupmask.h"
#include "iupgc.h"
#include "iupcb.h"
#include "iupspin.h"
#include "iupcolorbar.h"
#include "iupgetparam.h"

#ifdef __cplusplus
extern "C" {
#endif

int IupControlsOpen(void);
int IupControlsClose(void);

#ifdef __cplusplus
}
#endif

#endif

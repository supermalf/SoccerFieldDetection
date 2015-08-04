/** \file
 * \brief iupcolorbrowser control.
 *
 * See Copyright Notice in iup.h
 * $Id: iupcb.h,v 1.4 2004/08/10 00:32:26 scuri Exp $
 */
 
#ifndef __IUPCB_H 
#define __IUPCB_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************\
* Initializes the color browser interface.                                  *
\***************************************************************************/
void IupColorBrowserOpen(void);

/***************************************************************************\
* Creates the color browser element.                                        *
\***************************************************************************/
Ihandle *IupColorBrowser(void);

/***************************************************************************\
* Control special attributes.                                               *
\***************************************************************************/
#define IUP_RGB "RGB"
#define IUP_CHANGE_CB "CHANGE_CB"
#define IUP_DRAG_CB "DRAG_CB"

#ifdef __cplusplus
}
#endif

#endif


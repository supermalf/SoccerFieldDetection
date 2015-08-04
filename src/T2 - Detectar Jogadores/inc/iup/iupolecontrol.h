/*
 * iupolecontrol.h
 */

#ifndef __iupolecontrol_h
#define __iupolecontrol_h

#ifdef __cplusplus
extern "C" {
#endif

Ihandle *IupOleControl(char *);
void IupOleControlOpen(void);
void *IupOleControl_getclass(void);

#ifdef __cplusplus
}
#endif

#endif


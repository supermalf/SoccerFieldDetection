/** \file
 * \brief RAW File Format
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_raw.h,v 1.1 2005/04/02 22:07:00 scuri Exp $
 */

#ifndef __IM_RAW_H
#define __IM_RAW_H

#if	defined(__cplusplus)
extern "C" {
#endif


/** Opens a RAW image file.
 * \ingroup raw */
imFile* imFileOpenRaw(const char* file_name, int *error);
                   
/** Creates a RAW image file.
 * \ingroup raw */
imFile* imFileNewRaw(const char* file_name, int *error);
                   

#if defined(__cplusplus)
}
#endif

#endif

/** \file
 * \brief Palette Generators
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_palette.h,v 1.1 2005/04/02 22:07:00 scuri Exp $
 */

#ifndef __IM_PALETTE_H
#define __IM_PALETTE_H

#if	defined(__cplusplus)
extern "C" {
#endif


/** \defgroup palette Palette Generators
 * \par
 * Creates several standard palettes
 * \par
 * See \ref im_palette.h
 * \ingroup util */


/** Searches for the nearest color on the table and returns the color index if successful. 
 * It looks in all palette entries and finds the minimum euclidian square distance. 
 * If the color matches the given color it returns immediately.
 * \ingroup palette */
int imPaletteFindNearest(const long *palette, int palette_count, long color);

/** Searches for the color on the table and returns the color index if successful. 
 * If the tolerance is 0 search for the exact match in the palette else search for the 
 * first color that fits in the tolerance range.
 * \ingroup palette */
int imPaletteFindColor(const long *palette, int palette_count, long color, unsigned char tol);

/** Creates a palette of gray scale values.
 * The colors are arranged from black to white.
 * \ingroup palette */
long* imPaletteGray(void);

/** Creates a palette of a gradient of red colors.
 * The colors are arranged from black to pure red.
 * \ingroup palette */
long* imPaletteRed(void);

/** Creates a palette of a gradient of green colors.
 * The colors are arranged from black to pure green.
 * \ingroup palette */
long* imPaletteGreen(void);

/** Creates a palette of a gradient of blue colors.
 * The colors are arranged from black to pure blue.
 * \ingroup palette */
long* imPaletteBlue(void);

/** Creates a palette of a gradient of yellow colors.
 * The colors are arranged from black to pure yellow.
 * \ingroup palette */
long* imPaletteYellow(void);

/** Creates a palette of a gradient of magenta colors.
 * The colors are arranged from black to pure magenta.
 * \ingroup palette */
long* imPaletteMagenta(void);

/** Creates a palette of a gradient of cian colors.
 * The colors are arranged from black to pure cian.
 * \ingroup palette */
long* imPaletteCian(void);

/** Creates a palette of rainbow colors.
 * The colors are arranged in the light wave length spectrum order (starting from purple).
 * \ingroup palette */
long* imPaletteRainbow(void);

/** Creates a palette of hues with maximum saturation.
 * \ingroup palette */
long* imPaletteHues(void);

/** Creates a palette of a gradient of blue colors.
 * The colors are arranged from pure blue to white.
 * \ingroup palette */
long* imPaletteBlueIce(void);

/** Creates a palette of a gradient from black to white passing trough red and orange.
 * \ingroup palette */
long* imPaletteHotIron(void);

/** Creates a palette of a gradient from black to white passing trough red and yellow.
 * \ingroup palette */
long* imPaletteBlackBody(void);

/** Creates a palette with high contrast colors.
 * \ingroup palette */
long* imPaletteHighContrast(void);

/** Creates a palette of an uniform range of colors from black to white.
   This is a 2^(2.6) bits per pixel palette.
 * \ingroup palette */
long* imPaletteUniform(void);

/** Returns the index of the correspondent RGB color of an uniform palette.
 * \ingroup palette */
int imPaletteUniformIndex(long color);

/** Returns the index of the correspondent RGB color of an uniform palette.
 * Uses an 8x8 ordered dither to lookup the index in a halftone matrix.
 * The spatial position used by the halftone method.
 * \ingroup palette */
int imPaletteUniformIndexHalftoned(long color, int x, int y);


#if defined(__cplusplus)
}
#endif

#endif


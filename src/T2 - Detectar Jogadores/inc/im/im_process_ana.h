/** \file
 * \brief Image Statistics and Analysis
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_process_ana.h,v 1.2 2005/06/06 18:12:14 scuri Exp $
 */

#ifndef __IM_PROC_ANA_H
#define __IM_PROC_ANA_H

#include "im_image.h"

#if	defined(__cplusplus)
extern "C" {
#endif



/** \defgroup stats Image Statistics Calculations
 * \par
 * Operations to calculate some statistics over images.
 * \par
 * See \ref im_process_ana.h
 * \ingroup process */

/** Calculates the RMS error between 2 images (Root Mean Square Error).
 * \ingroup stats */
float imCalcRMSError(const imImage* image1, const imImage* image2);

/** Calculates the SNR of an image and its noise (Signal Noise Ratio).
 * \ingroup stats */
float imCalcSNR(const imImage* src_image, const imImage* noise_image);

/** Count the number of different colors in an image. \n
 * Image must be IM_BYTE, but all color spaces except IM_CMYK.
 * \ingroup stats */
unsigned long imCalcCountColors(const imImage* src_image);

/** Calculates the histogram of a IM_BYTE data. \n
 * Histogram is always 256 positions long. \n
 * When cumulative is different from zero it calculates the cumulative histogram.
 * \ingroup stats */
void imCalcHistogram(const unsigned char* data, int count, unsigned long* histo, int cumulative);

/** Calculates the histogram of a IM_USHORT data. \n
 * Histogram is always 65535 positions long. \n
 * When cumulative is different from zero it calculates the cumulative histogram.
 * \ingroup stats */
void imCalcUShortHistogram(const unsigned short* data, int count, unsigned long* histo, int cumulative);

/** Calculates the gray histogram of an image. \n
 * Image must be IM_BYTE/(IM_RGB, IM_GRAY, IM_BINARY or IM_MAP). \n
 * If the image is IM_RGB then the histogram of the luma component is calculated. \n
 * Histogram is always 256 positions long. \n
 * When cumulative is different from zero it calculates the cumulative histogram.
 * \ingroup stats */
void imCalcGrayHistogram(const imImage* src_image, unsigned long* histo, int cumulative);

/** Numerical Statistics Structure
 * \ingroup stats */
typedef struct _imStats
{
  float max;                /**< Maximum value              */
  float min;                /**< Minimum value              */
  unsigned long positive;   /**< Number of Positive Values  */
  unsigned long negative;   /**< Number of Negative Values  */
  unsigned long zeros;      /**< Number of Zeros            */
  float mean;               /**< Mean                       */
  float stddev;             /**< Standard Deviation         */
} imStats;

/** Calculates the statistics about the image data. \n
 * There is one stats for each depth plane. For ex: stats[0]=red stats, stats[0]=green stats, ... \n
 * Supports all data types except IM_COMPLEX. \n
 * \ingroup stats */
void imCalcImageStatistics(const imImage* src_image, imStats* stats);

/** Calculates the statistics about the image histogram data.\n
 * There is one stats for each depth plane. For ex: stats[0]=red stats, stats[0]=green stats, ... \n
 * Only IM_BYTE images are supported.
 * \ingroup stats */
void imCalcHistogramStatistics(const imImage* src_image, imStats* stats);

/** Calculates some extra statistics about the image histogram data.\n
 * There is one stats for each depth plane. \n
 * Only IM_BYTE images are supported. \n
 * mode will be -1 if more than one max is found.
 * \ingroup stats */
void imCalcHistoImageStatistics(const imImage* src_image, int* median, int* mode);



/** \defgroup analyze Image Analysis
 * \par
 * See \ref im_process_ana.h
 * \ingroup process */

/** Find white regions in binary image. \n
 * Result is IM_USHORT type. Regions can be 4 connected or 8 connected. \n
 * Returns the number of regions found. Background is marked as 0. \n
 * Regions touching the border are considered only if touch_border=1.
 * \ingroup analyze */
int imAnalyzeFindRegions(const imImage* src_image, imImage* dst_image, int connect, int touch_border);

/** Measure the actual area of all regions. Holes are not included. \n
 * This is the number of pixels of each region. \n
 * Source image is IM_USHORT type (the result of \ref imAnalyzeFindRegions). \n
 * data has size the number of regions.
 * \ingroup analyze */
void imAnalyzeMeasureArea(const imImage* image, int* area);

/** Measure the polygonal area limited by the perimeter line of all regions. Holes are not included. \n
 * Notice that some regions may have polygonal area zero. \n
 * Source image is IM_USHORT type (the result of \ref imAnalyzeFindRegions). \n
 * data has size the number of regions.
 * \ingroup analyze */
void imAnalyzeMeasurePerimArea(const imImage* image, float* perimarea);

/** Calculate the centroid position of all regions. Holes are not included. \n
 * Source image is IM_USHORT type (the result of \ref imAnalyzeFindRegions). \n
 * data has size the number of regions. If area is NULL will be internally calculated.
 * \ingroup analyze */
void imAnalyzeMeasureCentroid(const imImage* image, const int* area, int region_count, float* cx, float* cy);

/** Calculate the principal major axis slope of all regions. \n
 * Source image is IM_USHORT type (the result of \ref imAnalyzeFindRegions). \n
 * data has size the number of regions. If area or centroid are NULL will be internally calculated. \n
 * Principal (major and minor) axes are defined to be those axes that pass through the
 * centroid, about which the moment of inertia of the region is, respectively maximal or minimal.
 * \ingroup analyze */
void imAnalyzeMeasurePrincipalAxis(const imImage* image, const int* data_area, const float* cx, const float* cy, 
                                   const int region_count, float* major_slope, float* major_length, 
                                                           float* minor_slope, float* minor_length);

/** Measure the number and area of holes of all regions. \n
 * Source image is IM_USHORT type (the result of \ref imAnalyzeFindRegions). \n
 * data has size the number of regions. If some data is NULL it will be not calculated.
 * \ingroup analyze */
void imAnalyzeMeasureHoles(const imImage* image, int connect, int* holes_count, int* area, float* perim);

/** Measure the total perimeter of all regions (external and internal). \n
 * Source image is IM_USHORT type (the result of imAnalyzeFindRegions). \n
 * It uses a half-pixel inter distance for 8 neighboors in a perimeter of a 4 connected region. \n
 * This function can also be used to measure line lenght. \n
 * data has size the number of regions.
 * \ingroup analyze */
void imAnalyzeMeasurePerimeter(const imImage* image, float* perim);

/** Isolates the perimeter line of gray integer images. Background is defined as being black (0). \n
 * It just checks if at least one of the 4 connected neighboors is non zero. Image borders are extended with zeros.
 * \ingroup analyze */
void imProcessPerimeterLine(const imImage* src_image, imImage* dst_image);

/** Eliminates regions that have size outside the given interval. \n
 * Source and destiny are a binary images. Regions can be 4 connected or 8 connected. \n
 * Can be done in-place. end_size can be zero to ignore big objects.
 * \ingroup analyze */
void imProcessPrune(const imImage* src_image, imImage* dst_image, int connect, int start_size, int end_size);

/** Fill holes inside white regions. \n
 * Source and destiny are a binary images. Regions can be 4 connected or 8 connected. \n
 * Can be done in-place. 
 * \ingroup analyze */
void imProcessFillHoles(const imImage* src_image, imImage* dst_image, int connect);


#if defined(__cplusplus)
}
#endif

#endif

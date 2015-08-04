/** \file
 * \brief Image Processing - Local Operations
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_process_loc.h,v 1.2 2005/06/06 18:12:14 scuri Exp $
 */

#ifndef __IM_PROCESS_LOC_H
#define __IM_PROCESS_LOC_H

#include "im_image.h"

#if	defined(__cplusplus)
extern "C" {
#endif



/** \defgroup resize Image Resize
 * \par
 * Operations to change the image size.
 * \par
 * See \ref im_process_loc.h
 * \ingroup process */

/** Only reduze the image size using the given decimation order. \n
 * Supported decimation orders:
 * \li 0 - zero order (mean) 
 * \li 1 - first order (bilinear decimation)
 * Images must be of the same type. \n
 * Returns zero if the counter aborted.
 * \ingroup resize */
int imProcessReduce(const imImage* src_image, imImage* dst_image, int order);

/** Change the image size using the given interpolation order. \n
 * Supported interpolation orders:
 * \li 0 - zero order (near neighborhood) 
 * \li 1 - first order (bilinear interpolation) 
 * \li 3 - third order (bicubic interpolation)
 * Images must be of the same type. \n
 * Returns zero if the counter aborted.
 * \ingroup resize */
int imProcessResize(const imImage* src_image, imImage* dst_image, int order);

/** Reduze the image area by 4 (w/2,h/2). \n
 * Images must be of the same type. Destiny image size must be source image width/2, height/2.
 * \ingroup resize */
void imProcessReduceBy4(const imImage* src_image, imImage* dst_image);

/** Extract a rectangular region from an image. \n
 * Images must be of the same type. Destiny image size must be smaller than source image width-xmin, height-ymin. \n
 * ymin and xmin must be >0 and <size.
 * \ingroup resize */
void imProcessCrop(const imImage* src_image, imImage* dst_image, int xmin, int ymin);

/** Insert a rectangular region in an image. \n
 * Images must be of the same type. Region image size can be larger than source image. \n
 * ymin and xmin must be >0 and <size. \n
 * Source and destiny must be of the same size. Can be done in place.
 * \ingroup resize */
void imProcessInsert(const imImage* src_image, const imImage* region_image, imImage* dst_image, int xmin, int ymin);

/** Increase the image size by adding pixels with zero value. \n
 * Images must be of the same type. Destiny image size must be greatter than source image width+xmin, height+ymin.
 * \ingroup resize */
void imProcessAddMargins(const imImage* src_image, imImage* dst_image, int xmin, int ymin);



/** \defgroup geom Geometric Operations
 * \par
 * Operations to change the shape of the image.
 * \par
 * See \ref im_process_loc.h
 * \ingroup process */

/** Calculates the size of the new image after rotation.
 * \ingroup geom */
void imProcessCalcRotateSize(int width, int height, int *new_width, int *new_height, double cos0, double sin0);

/** Rotates the image using the given interpolation order (see \ref imProcessResize). \n
 * Images must be of the same type. The destiny size can be calculated using \ref imProcessCalcRotateSize. \n
 * Returns zero if the counter aborted.
 * \ingroup geom */
int imProcessRotate(const imImage* src_image, imImage* dst_image, double cos0, double sin0, int order);

/** Rotate the image in 90 degrees counterclockwise or clockwise. Swap columns by lines. \n
 * Images must be of the same type. Destiny width and height must be source height and width. 
 * \ingroup geom */
void imProcessRotate90(const imImage* src_image, imImage* dst_image, int dir);

/** Rotate the image in 180 degrees. Swap columns and swap lines. \n
 * Images must be of the same type and size.
 * \ingroup geom */
void imProcessRotate180(const imImage* src_image, imImage* dst_image);

/** Mirrors the image in a horizontal flip. Swap columns. \n
 * Images must be of the same type and size.
 * \ingroup geom */
void imProcessMirror(const imImage* src_image, imImage* dst_image);

/** Apply a vertical flip. Swap lines. \n
 * Images must be of the same type and size.
 * \ingroup geom */
void imProcessFlip(const imImage* src_image, imImage* dst_image);

/** Apply a radial distortion using the given interpolation order (see imProcessResize). \n
 * Images must be of the same type and size. Returns zero if the counter aborted.
 * \ingroup geom */
int imProcessRadial(const imImage* src_image, imImage* dst_image, float k1, int order);



/** \defgroup morphgray Morphology Operations for Gray Images
 * \par
 * See \ref im_process_loc.h
 * \ingroup process */

/** Base gray morphology convolution. \n
 * Supports all data types except IM_COMPLEX. Can be applied on color images. \n
 * Kernel is always IM_INT. Use kernel size odd for better results. \n
 * You can use the maximum value or else the minimum value. \n
 * No border extensions are used. 
 * All the gray morphology operations use this function. \n
 * If the kernel image attribute "Description" exists it is used by the counter.
 * \ingroup morphgray */
int imProcessGrayMorphConvolve(const imImage* src_image, imImage* dst_image, const imImage* kernel, int ismax);

/** Gray morphology convolution with a kernel full of "0"s and use minimum value.
 * \ingroup morphgray */
int imProcessGrayMorphErode(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Gray morphology convolution with a kernel full of "0"s and use maximum value.
 * \ingroup morphgray */
int imProcessGrayMorphDilate(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Erode+Dilate.
 * \ingroup morphgray */
int imProcessGrayMorphOpen(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Dilate+Erode.
 * \ingroup morphgray */
int imProcessGrayMorphClose(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Open+Difference.
 * \ingroup morphgray */
int imProcessGrayMorphTopHat(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Close+Difference.
 * \ingroup morphgray */
int imProcessGrayMorphWell(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Difference(Erode, Dilate).
 * \ingroup morphgray */
int imProcessGrayMorphGradient(const imImage* src_image, imImage* dst_image, int kernel_size);



/** \defgroup morphbin Morphology Operations for Binary Images
 * \par
 * See \ref im_process_loc.h
 * \ingroup process */

/** Base binary morphology convolution. \n
 * Images are all IM_BINARY. Kernel is IM_INT. Use kernel size odd for better results. \n
 * Hit white means hit=1 and miss=0, or else hit=0 and miss=1. \n
 * Use -1 for don't care positions in kernel. \n
 * The operation can be repeated by a number of iterations. 
 * The border is zero extended. \n
 * Almost all the binary morphology operations use this function.\n
 * If the kernel image attribute "Description" exists it is used by the counter.
 * \ingroup morphbin */
int imProcessBinMorphConvolve(const imImage* src_image, imImage* dst_image, const imImage* kernel, int hit_white, int iter);

/** Binary morphology convolution with a kernel full of "1"s and hit white.
 * \ingroup morphbin */
int imProcessBinMorphErode(const imImage* src_image, imImage* dst_image, int kernel_size, int iter);

/** Binary morphology convolution with a kernel full of "0"s and hit black.
 * \ingroup morphbin */
int imProcessBinMorphDilate(const imImage* src_image, imImage* dst_image, int kernel_size, int iter);

/** Erode+Dilate.
 * When iteration is more than one it means Erode+Erode+Erode+...+Dilate+Dilate+Dilate+...
 * \ingroup morphbin */
int imProcessBinMorphOpen(const imImage* src_image, imImage* dst_image, int kernel_size, int iter);

/** Dilate+Erode.
 * \ingroup morphbin */
int imProcessBinMorphClose(const imImage* src_image, imImage* dst_image, int kernel_size, int iter);

/** Erode+Difference. \n
 * The difference from the source image is applied only once.
 * \ingroup morphbin */
int imProcessBinMorphOutline(const imImage* src_image, imImage* dst_image, int kernel_size, int iter);

/** Thins the supplied binary image using Rosenfeld's parallel thinning algorithm. \n
 * Reference: \n
 * "Efficient Binary Image Thinning using Neighborhood Maps" \n
 * by Joseph M. Cychosz, 3ksnn64@ecn.purdue.edu              \n
 * in "Graphics Gems IV", Academic Press, 1994
 * \ingroup morphbin */
void imProcessBinMorphThin(const imImage* src_image, imImage* dst_image);



/** \defgroup rank Rank Convolution Operations
 * \par
 * All the rank convolution use the same base function. Near the border the base function 
 * includes only the real image pixels in the rank. No border extensions are used.
 * \par
 * See \ref im_process_loc.h
 * \ingroup process */

/** Rank convolution using the median value. \n
 * Returns zero if the counter aborted. \n
 * Supports all data types except IM_COMPLEX. Can be applied on color images.
 * \ingroup rank */
int imProcessMedianConvolve(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Rank convolution using (maximum-minimum) value. \n
 * Returns zero if the counter aborted. \n
 * Supports all data types except IM_COMPLEX. Can be applied on color images.
 * \ingroup rank */
int imProcessRangeConvolve(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Rank convolution using the closest maximum or minimum value. \n
 * Returns zero if the counter aborted. \n
 * Supports all data types except IM_COMPLEX. Can be applied on color images.
 * \ingroup rank */
int imProcessRankClosestConvolve(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Rank convolution using the maximum value. \n
 * Returns zero if the counter aborted. \n
 * Supports all data types except IM_COMPLEX. Can be applied on color images.
 * \ingroup rank */
int imProcessRankMaxConvolve(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Rank convolution using the minimum value. \n
 * Returns zero if the counter aborted. \n
 * Supports all data types except IM_COMPLEX. Can be applied on color images.
 * \ingroup rank */
int imProcessRankMinConvolve(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Threshold using a rank convolution with a range contrast function. \n
 * Supports all integer IM_GRAY images as source, and IM_BINARY as destiny. \n
 * Local variable threshold by the method of Bernsen. \n
 * Extracted from XITE, Copyright 1991, Blab, UiO \n
 * http://www.ifi.uio.no/~blab/Software/Xite/
\verbatim
  Reference:	
    Bernsen, J: "Dynamic thresholding of grey-level images"
		Proc. of the 8th ICPR, Paris, Oct 1986, 1251-1255.
  Author:     Oivind Due Trier
\endverbatim
 * Returns zero if the counter aborted.
 * \ingroup threshold */
int imProcessRangeContrastThreshold(const imImage* src_image, imImage* dst_image, int kernel_size, int min_range);

/** Threshold using a rank convolution with a local max function.  \n
 * Returns zero if the counter aborted. \n
 * Supports all integer IM_GRAY images as source, and IM_BINARY as destiny.
 * \ingroup threshold */
int imProcessLocalMaxThreshold(const imImage* src_image, imImage* dst_image, int kernel_size, int min_thres);



/** \defgroup convolve Convolution Operations
 * \par
 * See \ref im_process_loc.h
 * \ingroup process */

/** Base Convolution with a kernel. \n
 * Kernel can be IM_INT or IM_FLOAT, but always IM_GRAY. Use kernel size odd for better results. \n
 * Supports all data types. The border is mirrored. \n
 * Returns zero if the counter aborted. Most of the convolutions use this function.\n
 * If the kernel image attribute "Description" exists it is used by the counter.
 * \ingroup convolve */
int imProcessConvolve(const imImage* src_image, imImage* dst_image, const imImage* kernel);

/** Repeats the convolution a number of times. \n
 * Returns zero if the counter aborted.\n
 * If the kernel image attribute "Description" exists it is used by the counter.
 * \ingroup convolve */
int imProcessConvolveRep(const imImage* src_image, imImage* dst_image, const imImage* kernel, int count);

/** Convolve with a kernel rotating it 8 times and getting the absolute maximum value. \n
 * Kernel must be square. \n
 * The rotation is implemented only for kernel sizes 3x3, 5x5 and 7x7. \n
 * Supports all data types except IM_COMPLEX.
 * Returns zero if the counter aborted.\n
 * If the kernel image attribute "Description" exists it is used by the counter.
 * \ingroup convolve */
int imProcessCompassConvolve(const imImage* src_image, imImage* dst_image, imImage* kernel);

/** Utility function to rotate a kernel one time.
 * \ingroup convolve */
void imProcessRotateKernel(imImage* kernel);

/** Difference(Gaussian1, Gaussian2). \n
 * Supports all data types, 
 * but if source is IM_BYTE or IM_USHORT destiny image must be of type IM_INT.
 * \ingroup convolve */
int imProcessDiffOfGaussianConvolve(const imImage* src_image, imImage* dst_image, float stddev1, float stddev2);

/** Difference(Gaussian1, Gaussian2) using gaussian repetitions. \n
 * Supports all data types, 
 * but if source is IM_BYTE or IM_USHORT destiny image must be of type IM_INT.
 * \ingroup convolve */
int imProcessDiffOfGaussianConvolveRep(const imImage* src_image, imImage* dst_image, float stddev1, float stddev2);

/** Convolution with a laplacian of a gaussian kernel. \n
 * Supports all data types, 
 * but if source is IM_BYTE or IM_USHORT destiny image must be of type IM_INT.
 * \ingroup convolve */
int imProcessLapOfGaussianConvolve(const imImage* src_image, imImage* dst_image, float stddev);

/** Convolution with a kernel full of "1"s inside a circle. \n
 * Supports all data types.
 * \ingroup convolve */
int imProcessMeanConvolve(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Convolution with a gaussian kernel. The gaussian in obtained by repetition of a base 3x3 IM_INT kernel. \n
 * Supports all data types.
 * \ingroup convolve */
int imProcessGaussianConvolveRep(const imImage* src_image, imImage* dst_image, float stddev);

/** Convolution with a float gaussian kernel. \n
 * Supports all data types.
 * \ingroup convolve */
int imProcessGaussianConvolve(const imImage* src_image, imImage* dst_image, float stddev);

/** Magnitude of the sobel convolution. \n
 * Supports all data types except IM_COMPLEX.
 * \ingroup convolve */
int imProcessSobelConvolve(const imImage* src_image, imImage* dst_image);

/** Finds the zero crossings of IM_INT and IM_FLOAT images. Crossings are marked with non zero values
 * indicating the intensity of the edge. It is usually used after a second derivative, laplace. \n
 * Extracted from XITE, Copyright 1991, Blab, UiO \n
 * http://www.ifi.uio.no/~blab/Software/Xite/
 * \ingroup convolve */
void imProcessZeroCrossing(const imImage* src_image, imImage* dst_image);

/** First part of the Canny edge detector. Includes the gaussian filtering and the nonmax suppression. \n
 * After using this you could apply a Hysteresis Threshold, see \ref imProcessHysteresisThreshold. \n
 * Image must be IM_BYTE/IM_GRAY. \n
 * Implementation from the book:
 \verbatim
    J. R. Parker
    "Algoritms for Image Processing and Computer Vision"
    WILEY
 \endverbatim
 * \ingroup convolve */
void imProcessCanny(const imImage* src_image, imImage* dst_image, float stddev);

/** Calculates the number of 3x3 gaussian repetitions given the standard deviation.
 * \ingroup convolve */
int imGaussianStdDev2Repetitions(float stddev);

/** Calculates the kernel size given the standard deviation.
 * \ingroup convolve */
int imGaussianStdDev2KernelSize(float stddev);


#if defined(__cplusplus)
}
#endif

#endif

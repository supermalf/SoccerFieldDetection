/** \file
 * \brief Image Manipulation
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_image.h,v 1.3 2005/07/15 19:57:23 scuri Exp $
 */

#ifndef __IM_IMAGE_H
#define __IM_IMAGE_H

#if	defined(__cplusplus)
extern "C" {
#endif


/** \defgroup imgclass imImage 
 *
 * \par
 *  Base definitions and functions for image representation. \n
 * Only the image processing operations depends on these definitions, 
 * Image Storage and Image Capture are completely independent.
 * \par
 * You can also initialize a structure with your own memory buffer, see \ref imImageInit.
 * To release the structure without releasing the buffer, 
 * set "data[0]" to 0 before calling imImageDestroy.
 * \par
 * See \ref im_image.h
 * \ingroup imagerep */



/** \brief imImage Structure Definition.  
 *
 * \par
 * An image representation than supports all the color spaces, but no alpha channel, 
 * planes are always unpacked and the orientation is always bottom up.
 * \ingroup imgclass */
typedef struct _imImage
{
  /* main parameters */
  int width;          /**< Number of columns */
  int height;         /**< Number of lines.  */
  int color_space;    /**< Color space descriptor. */
  int data_type;      /**< Data type descriptor.   */

  /* secondary parameters */
  int depth;          /**< Number of planes                      (ColorSpaceDepth)         */
  int line_size;      /**< Number of bytes per line in one plane (width * DataTypeSize)    */
  int plane_size;     /**< Number of bytes per plane.            (line_size * height)      */
  int size;           /**< Number of bytes occupied by the image (plane_size * depth)      */
  int count;          /**< Number of pixels                      (width * height)          */

  /* image data */
  void** data;        /**< Image data organized as a 2D matrix with several planes. 
                           But plane 0 is also a pointer to the full data.  (data[i] = data[0] + i*plane_size) */

  /* image attributes */
  long *palette;      /**< Used when depth=1. Otherwise is NULL. */
  int palette_count;  /**< The palette is always 256 colors allocated, but can have less colors used. */

  void* attrib_table; /**< in fact is a imAttribTable, but we hide this here */
} imImage;


/** Creates a new image.
 * \ingroup imgclass */
imImage* imImageCreate(int width, int height, int color_space, int data_type);

/** Initializes the image structure but does not allocates image data.
 * \ingroup imgclass */
imImage* imImageInit(int width, int height, int color_space, int data_type, void* data_buffer, long* palette, int palette_count);

/** Destroys the image and frees the memory used.
 * image data is destroyed only if its data[0] is not NULL.
 * \ingroup imgclass */
void imImageDestroy(imImage* image);

/** Changes the buffer size. Reallocate internal buffers if they are larger than original.
 * \ingroup imgclass */
void imImageReshape(imImage* image, int width, int height);

/** Copy image data and attributes from one image to another. \n
 * Images must have the same size and type.
 * \ingroup imgclass */
void imImageCopy(const imImage* src_image, imImage* dst_image);

/** Copy image data only one image to another. \n
 * Images must have the same size and type.
 * \ingroup imgclass */
void imImageCopyData(const imImage* src_image, imImage* dst_image);

/** Creates a copy of the image.
 * \ingroup imgclass */
imImage* imImageDuplicate(const imImage* image);

/** Creates a clone of the image. i.e. same attributes but ignore contents.
 * \ingroup imgclass */
imImage* imImageClone(const imImage* image);

/** Changes an extended attribute. \n
 * The data will be internally duplicated. \n
 * If data is NULL the attribute is removed. \n
 * If count is -1 and data_type is IM_BYTE then data is zero terminated.
 * \ingroup imgclass */
void imImageSetAttribute(imImage* image, const char* attrib, int data_type, int count, const void* data);

/** Returns an extended attribute. \n
 * Returns NULL if not found.
 * \ingroup imgclass */
const void* imImageGetAttribute(const imImage* image, const char* attrib, int *data_type, int *count);

/** Returns a list of the attribute names. \n
 * "attrib" must contain room enough for "attrib_count" names. Use "attrib=NULL" to return only the count.
 * \ingroup imgclass */
void imImageGetAttributeList(const imImage* image, char** attrib, int *attrib_count);

/** Sets all image data to zero.
 * \ingroup imgclass */
void imImageClear(imImage* image);

/** Indicates that the image can be viewed in common graphic devices.     
 * Data type must be IM_BYTE. Color mode can be IM_RGB, IM_MAP, IM_GRAY or IM_BINARY.
 * \ingroup imgclass */
int imImageIsBitmap(const imImage* image);

/** Changes the image palette.
 * This will destroy the existing palette and replace it with the given palette buffer.
 * \ingroup imgclass */
void imImageSetPalette(imImage* image, long* palette, int palette_count);

/** Copies the image attributes from src to dst.
 * \ingroup imgclass */
void imImageCopyAttributes(const imImage* src_image, imImage* dst_image);

/** Returns 1 if the images match width and height. Returns 0 otherwise.
 * \ingroup imgclass */
int imImageMatchSize(const imImage* image1, const imImage* image2);

/** Returns 1 if the images match color mode and data type. Returns 0 otherwise.
 * \ingroup imgclass */
int imImageMatchColor(const imImage* image1, const imImage* image2);

/** Returns 1 if the images match width, height and data type. Returns 0 otherwise.
 * \ingroup imgclass */
int imImageMatchDataType(const imImage* image1, const imImage* image2);

/** Returns 1 if the images match width, height and color space. Returns 0 otherwise.
 * \ingroup imgclass */
int imImageMatchColorSpace(const imImage* image1, const imImage* image2);

/** Returns 1 if the images match in width, height, data type and color space. Returns 0 otherwise.
 * \ingroup imgclass */
int imImageMatch(const imImage* image1, const imImage* image2);

/** Changes the image space from gray to binary by just changing color_space and the palette.
 * \ingroup imgclass */
void imImageSetBinary(imImage* image);

/** Changes a gray data into a binary data.
 * \ingroup imgclass */
void imImageMakeBinary(imImage *image);



/** \defgroup imgfile imImage Storage
 *
 * \par
 *  Functions to simplify the process of reading and writting imImage structures.
 * \par
 * See \ref im_image.h
 * \ingroup file */


/** Loads an image from an already open file. Returns NULL if failed. \n
 * This will call \ref imFileReadImageInfo and \ref imFileReadImageData. \n
 * index specifies the image number between 0 and image_count-1. \n
 * The returned imagem will be of the same color_space and data_type of the image in the file. \n
 * \ingroup imgfile */
imImage* imFileLoadImage(imFile* ifile, int index, int *error);

/** Loads an image from an already open file. Returns NULL if failed. \n
 * This function assumes that the image in the file has the same parameters as the given image. \n
 * This will call \ref imFileReadImageInfo and \ref imFileReadImageData. \n
 * index specifies the image number between 0 and image_count-1. \n
 * The returned imagem will be of the same color_space and data_type of the image in the file. \n
 * \ingroup imgfile */
void imFileLoadImageFrame(imFile* ifile, int index, imImage* image, int *error);

/** Loads an image from an already open file, but forces the image to be a bitmap.\n
 * The returned imagem will be always a Bitmap image, with color_space RGB, MAP, GRAY or BINARY, and data_type IM_BYTE. \n
 * index specifies the image number between 0 and image_count-1. \n
 * Returns NULL if failed.
 * \ingroup imgfile */
imImage* imFileLoadBitmap(imFile* ifile, int index, int *error);

/** Loads an image from an already open file, but forces the image to be a bitmap.\n
 * This function assumes that the image in the file has the same parameters as the given image. \n
 * The imagem must be a Bitmap image, with color_space RGB, MAP, GRAY or BINARY, and data_type IM_BYTE. \n
 * index specifies the image number between 0 and image_count-1. \n
 * Returns NULL if failed.
 * \ingroup imgfile */
void imFileLoadBitmapFrame(imFile* ifile, int index, imImage* image, int *error);

/** Saves the image to an already open file. \n
 * This will call \ref imFileWriteImageInfo and \ref imFileWriteImageData. \n
 * Returns error code.
 * \ingroup imgfile */
int imFileSaveImage(imFile* ifile, const imImage* image);

/** Loads an image from file. Open, loads and closes the file. \n
 * index specifies the image number between 0 and image_count-1. \n
 * Returns NULL if failed.
 * \ingroup imgfile */
imImage* imFileImageLoad(const char* file_name, int index, int *error);

/** Loads an image from file, but forces the image to be a bitmap. Open, loads and closes the file. \n
 * index specifies the image number between 0 and image_count-1. \n
 * Returns NULL if failed.
 * \ingroup imgfile */
imImage* imFileImageLoadBitmap(const char* file_name, int index, int *error);

/** Saves the image to file. Open, saves and closes the file. \n
 * Returns error code.
 * \ingroup imgfile */
int imFileImageSave(const char* file_name, const char* format, const imImage* image);



/** Utility macro to draw the image in a CD library canvas.
 * Works only for data_type IM_BYTE, and color spaces: IM_RGB, IM_MAP, IMGRAY and IM_BINARY.
 * \ingroup imgclass */
#define imPutBitmap(_image, _x, _y, _w, _h, _xmin, _xmax, _ymin, _ymax)     \
  {                                                                         \
    if (_image->color_space == IM_RGB)                                      \
      cdPutImageRectRGB(_image->width, _image->height,                      \
                        (unsigned char*)_image->data[0],                    \
                        (unsigned char*)_image->data[1],                    \
                        (unsigned char*)_image->data[2],                    \
                        _x, _y, _w, _h, _xmin, _xmax, _ymin, _ymax);        \
    else                                                                    \
      cdPutImageRectMap(_image->width, _image->height,                      \
                        (unsigned char*)_image->data[0], _image->palette,   \
                        _x, _y, _w, _h, _xmin, _xmax, _ymin, _ymax);        \
  }                                                                               


#if	defined(__cplusplus)
}
#endif

#endif

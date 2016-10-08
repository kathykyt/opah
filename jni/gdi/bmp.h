#ifndef __BMP_H__
#define __BMP_H__

#define FILEHEADSIZE 14 
#define INFOHEADSIZE 40
#define COREHEADSIZE 12
#define BI_RGB		0L
#define BI_RLE8		1L
#define BI_RLE4		2L
#define BI_BITFIELDS	3L
/* In-core mono and color image structure*/
#define MWIMAGE_UPSIDEDOWN	01	/* compression flag: upside down image*/
#define MWIMAGE_BGR		00	/* compression flag: BGR byte order*/
#define MWIMAGE_RGB		02	/* compression flag: RGB not BGR bytes*/
#define MWIMAGE_ALPHA_CHANNEL   04	/* compression flag: 32-bit w/alpha */


typedef unsigned char	MWUCHAR;	/* unsigned char*/
typedef unsigned long	MWCOLORVAL;	/* device-independent color value*/


typedef struct {  /* structure for reading images from buffer   */
	unsigned char *start;	/* The pointer to the beginning of the buffer */
	unsigned long offset;	/* The current offset within the buffer       */
	unsigned long size;	/* The total size of the buffer               */
} buffer_t;

typedef struct {
	/* BITMAPFILEHEADER*/
	BYTE	bfType[2];
	DWORD	bfSize;
	WORD	bfReserved1;
	WORD	bfReserved2;
	DWORD	bfOffBits;
} BMPFILEHEAD;



typedef struct {
	/* BITMAPCOREHEADER*/
	DWORD	bcSize;
	WORD	bcWidth;
	WORD	bcHeight;
	WORD	bcPlanes;
	WORD	bcBitCount;
} BMPCOREHEAD;

/* windows style*/
typedef struct {
	/* BITMAPINFOHEADER*/
	DWORD	BiSize;
	DWORD	BiWidth;
	DWORD	BiHeight;
	WORD	BiPlanes;
	WORD	BiBitCount;
	DWORD	BiCompression;
	DWORD	BiSizeImage;
	DWORD	BiXpelsPerMeter;
	DWORD	BiYpelsPerMeter;
	DWORD	BiClrUsed;
	DWORD	BiClrImportant;
} BMPINFOHEAD;

/* In-core color palette structure*/
typedef struct {
	MWUCHAR	r;
	MWUCHAR	g;
	MWUCHAR	b;
	MWUCHAR _padding;
} MWPALENTRY;


typedef struct {
	int		width;		/* image width in pixels*/
	int		height;		/* image height in pixels*/
	int		planes;		/* # image planes*/
	int		bpp;		/* bits per pixel (1, 4 or 8)*/
	int		pitch;		/* bytes per line*/
	int		bytesperpixel;	/* bytes per pixel*/
	int		compression;	/* compression algorithm*/
	int		palsize;	/* palette size*/
	long		transcolor;	/* transparent color or -1 if none*/
	MWPALENTRY *	palette;	/* palette*/
	MWUCHAR *	imagebits;	/* image bits (dword right aligned)*/
} MWIMAGEHDR, *PMWIMAGEHDR;

# define wswap(x)	(x)
# define dwswap(x)	(x)

#endif


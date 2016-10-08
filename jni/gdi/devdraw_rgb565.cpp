#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "bmp.h"
#include "devdraw.h"
#include "log.h"
#include "memorymanage.h"



//david try convert RGBA to RGB565 temprary, make a one line long buffer
unsigned int rgb24to565_line_buff[LCDW];

PIXEL* CreateArea(int width, int height)
{
	PIXEL* pTemp;
	pTemp = (PIXEL*)uimalloc(width*height*EVERY_PIXEL_BYTES+8);	
	*(unsigned int*)pTemp = width;
	*((unsigned int*)pTemp+1) = height;	
	//AreaFill(pTemp, 0, 0, width, height, COLOR_DARK);
	return pTemp;
}

void DelArea(PIXEL* pArea)
{
	if(pArea != NULL)
		uifree((void*)pArea);
}

// 将屏幕的区域置为某个颜色
//color is RGBA format.
void AreaFill(PIXEL* pDesBuf, int x, int y, int width, int height, PIXEL color)
{
	int i,j;
	PIXEL* p;
	unsigned short *p_rgb565;
	unsigned short data;
	PIXEL *temp;
	int desW,desH;
	int drawW,drawH;
        unsigned short *fb_outp;
	unsigned char *col_p;
        unsigned short rgb565_col;
 	int out_to_screen = 0;

	if((pDesBuf == GetBackupBuffer()) || (pDesBuf == GetDisplayBuffer()))
	{
		desW = LCDW;
		desH = LCDH;
		temp = pDesBuf;
		out_to_screen = 1;
	}
	else
	{
		//don't know what is this for
	    #if (COLORPIXEL565 == 1)
		desW = *(unsigned int*)pDesBuf;
		desH = *((unsigned int*)pDesBuf+1);
		temp = (unsigned short*)pDesBuf+4;	
	    #else
		desW = *(unsigned int*)pDesBuf;
		desH = *((unsigned int*)pDesBuf+1);
		temp = (unsigned int*)pDesBuf+2;
            #endif
		out_to_screen = 0;
	}

        if (out_to_screen == 1) {
	      	//construct the color, for RGB565 output, this color set will not have trasparent effect.
	      	col_p = (unsigned char *)&color;
		//rgb565_col = ( ((*col_p++ & 0xf8) >> 3) || ((*col_p++ & 0xfC) << 3) || ((*col_p++ & 0xf8) << 8) );
		data = 0;
		data |= ((*col_p++ & 0xf8) >> 3);
		data |= ((*col_p++ & 0xfC) << 3);
		data |= ((*col_p++ & 0xf8) << 8);
		rgb565_col = data;

		if((x >= desW) || (y >= desH))
			return;
		drawW = GETMIN(x+width, desW)-x;
		drawH = GETMIN(y+height, desH)-y;
		for(j=y; j<y+drawH; j++)
		{
			p_rgb565 = (unsigned short *)temp+j*desW+x;
			for(i=0; i<drawW; i++) {
				*p_rgb565++ = rgb565_col;			
			}
		}	
	} else {

		if((x >= desW) || (y >= desH))
		return;
		drawW = GETMIN(x+width, desW)-x;
		drawH = GETMIN(y+height, desH)-y;
		for(j=y; j<y+drawH; j++)
		{
			p = temp+j*desW+x;
			for(i=0; i<drawW; i++,p++)
				*p = color;
		}	
	}
}

// 将图片对应的某一个内存copy到某一个内存中(也包括显示内存)，copy的宽高可以指定，常用的模式为REPLACE_STYLE
// REPLACE_STYLE模式为直接替换
// TRANS_STYLE模式将检测图片的像素点的颜色是否为TRANSCOLOR，如果是将不替换颜色而显示背景颜色，常用于显示底图
// ALPHA_STYLE模式将检测图片的像素点的颜色和屏幕中对应位置的颜色进行ALPHA合成。常用于加强显示的效果。
int AreaDraw(PIXEL *desBuf_in, int desX, int desY, PIXEL *srcBuf_in, int srcX, int srcY,
             int W, int H, int Mode)
{
	int srcW,srcH;
	int desW,desH;
	int drawW,drawH;
	int i,j,k;
 	int out_to_screen = 0;
 	int read_from_screen = 0;

	//david hack, convert the 24 bit RGBA to RGB565
	int ii=0; 
	unsigned char *ptmp, *dtmp; 
	unsigned short *outp = (unsigned short *)&rgb24to565_line_buff[0];
        unsigned char tmpd = 0;
	//if we are rgb565
	unsigned short data;
	unsigned short *fb_outp, *fb_inp;
	PIXEL *desBuf;
	PIXEL *srcBuf;
        unsigned int transcolor_flag;

	if((W <= 0) || (H <= 0))
		return 0;

	//david test.
	//printf("AreaDraw\n");

	desBuf = desBuf_in;
	srcBuf = srcBuf_in;

	if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
	{
		//printf("AreaDraw 1\n");
		desW = LCDW;
		desH = LCDH;
		out_to_screen = 1;
		//printf("AreaDraw, out to screen\n");
	}
	else
	{
            #if (COLORPIXEL565 == 1)
		desW = *(unsigned int*)desBuf;
		desH = *((unsigned int*)desBuf+1);
		desBuf = (unsigned short*)desBuf+4;	
	    #else
		desW = *(unsigned int*)desBuf;
		desH = *((unsigned int*)desBuf+1);		
		desBuf = (unsigned int*)desBuf+2;
	    #endif
	     out_to_screen = 0;	   
	}
	if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
	{
		srcW = LCDW;
		srcH = LCDH;
		//printf("AreaDraw, read src from screen\n");
		read_from_screen = 1;
	}
	else
	{    
	    #if (COLORPIXEL565 == 1)
		srcW = *(unsigned int*)srcBuf;
		srcH = *((unsigned int*)srcBuf+1);
		srcBuf = (unsigned short*)srcBuf+4;	
	    #else
		srcW = *(unsigned int*)srcBuf;
		srcH = *((unsigned int*)srcBuf+1);
		srcBuf = (unsigned int*)srcBuf+2;
            #endif
	    read_from_screen = 0;
	}	
	if((srcW == 0) || (srcH == 0) || (desW == 0) || (desH == 0))
	{
		ERROR_X("error, AreaDraw WH should not be zero\n");
		return -1;
	}
	if((srcX >= srcW) || (srcY >= srcH))
		return 0;		
	if((desX >= desW) || (desY >= desH))
		return 0;		
	drawW = GETMIN(GETMIN(desX+W, desW)-desX, GETMIN(srcX+W, srcW)-srcX);
	if(desX < 0){
		srcX = abs(desX);
		desX = 0;
		drawW = GETMIN(GETMIN(desX + W, desW) - desX, GETMIN(srcX + W, srcW) - srcX);
	}
	drawH = GETMIN(desY+H, desH)-desY;
	if(Mode == REPLACE_STYLE)
	{
		//printf("devdraw, Mode == REPLACE_STYLE, srcY = %d, srcX = %d, drawW=%d, drawH=%d\n", srcY, srcX, drawW, drawH);
		//printf("devdraw, Mode == REPLACE_STYLE, desX= %d, desY = %d, desW = %d, desH = %d\n", desX, desY, desW, desH);
		
		if (out_to_screen == 1) {

		     for(j=0; j<drawH; j++)
		     {
			if(srcY+j >= srcH)
				break;

			//david hack, convert the 24 bit RGBA to RGB565
			outp = (unsigned short *)&rgb24to565_line_buff[0];
			ptmp = (unsigned char *)(srcBuf+ (srcY+j)*srcW+srcX);
			fb_outp = (unsigned short*)desBuf + ((desY+j)*desW+desX);
			fb_inp = (unsigned short*)srcBuf + (srcY+j)*srcW+srcX;
			//dtmp = (unsigned char *)fb_outp;

			if (read_from_screen == 0) {
			   for (ii=0; ii < drawW; ii++) {	
			      tmpd = 0;
			      *outp = 0;

			      //Image source are in BGRA format.	
			      tmpd = *ptmp++ & 0xf8;
			      *outp |= tmpd >> 3;
			      tmpd = *ptmp++ & 0xfC;
			      *outp |= tmpd << 3;
                              tmpd = *ptmp++ & 0xf8;
			      *outp |= tmpd << 8; 	 
			      *ptmp++;

			     //*outp++ = ( ((*ptmp++ & 0xf8) << 8) || ((*ptmp++ & 0xfC) << 3) || ((*ptmp++ & 0xf8) >> 3) );
			     
			     *(fb_outp + ii) = *outp;			     
 			   }

		       } // end of if read from screen
		       else {
			   for (ii=0; ii < drawW; ii++) {
				*(fb_outp + ii) = *(fb_inp + ii);
			   }
		       }

		    }
		} //end of if
		else {

		        for(j=0; j<drawH; j++)
		        {
			    if(srcY+j >= srcH)
				break;

			    memcpy(desBuf+(desY+j)*desW+desX, srcBuf+(srcY+j)*srcW+srcX, drawW*sizeof(PIXEL));

		        }

		}
 
	}
	else if(Mode == TRANS_STYLE)
	{
#ifdef NOTUSE_ACCELERATION

#else
		printf("AreaDraw, Mode == TRANS_STYLE\n");
		if (out_to_screen == 1) {
		      for(j=0; j<drawH; j++)
		      {
			  //david hack, convert the 24 bit RGBA to RGB565
			  outp = (unsigned short *)&rgb24to565_line_buff[0];
			  ptmp = (unsigned char *)(srcBuf+ (srcY+j)*srcW+srcX);
			  fb_outp = (unsigned short*)desBuf + ((desY+j)*desW+desX);

			  for(i=0; i<drawW; i++)
			  {

			    //If we are output to a RGB565 framebuffer
			      tmpd = 0;
			      data = 0;
			     
			     //Image source are in BGRA format.	
			      tmpd = *ptmp++ & 0xf8;
			      data |= tmpd >> 3;
			      tmpd = *ptmp++ & 0xfC;
			      data |= tmpd << 3;
                              tmpd = *ptmp++ & 0xf8;
			      data |= tmpd << 8; 	 
			     
			     if (*ptmp++ != 0x00 /*TRANSCOLOR */)
					*(fb_outp + i) = data;					

			  }
		     }
		} //end of if out_to_screen == 1
		else {

			   for(j=0; j<drawH; j++)
			   {
				for(i=0; i<drawW; i++)
				{

			     		k = *(srcBuf+(srcY+j)*srcW+srcX+i);
                             
					if(k != TRANSCOLOR)
					    *(desBuf+(desY+j)*desW+desX+i) = k;

				}
			   }
	 	}  //end of if

#endif 
	}
	else if(Mode == ALPHA_STYLE)
	{
#ifdef NOTUSE_ACCELERATION

#else
		RGBQUAD rgbDest, rgbSrc;
		int nBlend;
		unsigned char fbRed, fbGreen, fbBlue;
		unsigned char desFbRed, desFbGreen, desFbBlue;
		
		//printf(" AreaDraw, Mode == ALPHA_STYLE\n");
		if (out_to_screen == 1) {		    	        

			for(j=0; j<drawH; j++)
		    	{

				for(i=0; i<drawW; i++)
			
				{
					fb_outp = (unsigned short *)desBuf+ (desY+j)*desW+desX;
					
					rgbSrc = *((RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i));
					
					if(rgbSrc.rgbReserved == 0xFF)
					{
					   data = 0;
					   data |= ((rgbSrc.rgbRed & 0xf8) << 8);
					   data |= ((rgbSrc.rgbGreen & 0xfC) << 3);
					   data |= ((rgbSrc.rgbBlue & 0xf8) >> 3);
					   *(fb_outp + i) = data;
					 //  *(desBuf+(desY+j)*desW+desX+i) = *(srcBuf+(srcY+j)*srcW+srcX+i);
					 
					}
					else if(rgbSrc.rgbReserved > 0)
					{
					  //Here, because the framebuffer is in RGB565 format,
					  // We need to format the RGB565 to and blend first. 
					  // THen we write it back to RGB565 framebufer.
					   nBlend = rgbSrc.rgbReserved;
					   
					   data = *(fb_outp + i);
					   fbRed = (data & 0xf800) >> 8; //8
					   fbGreen = (data & 0x07E0) >> 3; //3
					   fbBlue = (data & 0x001F) << 3;  //3
	
					  // rgbDest = *((RGBQUAD *)(desBuf+(desY+j)*desW+desX+i));
					   desFbRed = (nBlend * rgbSrc.rgbRed + (256-nBlend) * fbRed)>>8;
					   desFbGreen = (nBlend * rgbSrc.rgbGreen + (256-nBlend) * fbGreen)>>8;
					   desFbBlue = (nBlend * rgbSrc.rgbBlue + (256-nBlend) * fbBlue)>>8;
					   data = 0;
					   data |= ((desFbRed & 0xf8) << 8);
					   data |= ((desFbGreen & 0xfC) << 3);
					   data |= ((desFbBlue & 0xf8) >> 3);
					  *(fb_outp + i) = data;
					 
					  //printf("outdata = %x\n", *(fb_outp + i));
					   //*(desBuf+(desY+j)*desW+desX+i) = *(PIXEL *)(&rgbDest);
					}
				}

			}
		    
		}
		else {

		     	for(j=0; j<drawH; j++)
		    	{

				for(i=0; i<drawW; i++)
			
				{
					rgbSrc = *((RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i));
					if(rgbSrc.rgbReserved == 0xFF)
					{
					   *(desBuf+(desY+j)*desW+desX+i) = *(srcBuf+(srcY+j)*srcW+srcX+i);
					}
					else if(rgbSrc.rgbReserved > 0)
					{
					   nBlend = rgbSrc.rgbReserved;
					   rgbDest = *((RGBQUAD *)(desBuf+(desY+j)*desW+desX+i));
					   rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256-nBlend) * rgbDest.rgbRed)>>8;
					   rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256-nBlend) * rgbDest.rgbGreen)>>8;
					   rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256-nBlend) * rgbDest.rgbBlue)>>8;
					   *(desBuf+(desY+j)*desW+desX+i) = *(PIXEL *)(&rgbDest);
					}
				}

			}
		} // end of else

#endif 
	}
	return 0;
}

// 判断srcBuf像素点对应的alpha为0时，将desBuf对应的像素点的alpha值也置为0，该函数可用于显示多层的图片进行Alpha重叠的效果
int AreaAlphaMask(PIXEL *desBuf, int desX, int desY, PIXEL *srcBuf, int srcX, int srcY,
                  int W, int H)
{
	int srcW,srcH;
	int desW,desH;
	int drawW,drawH;	
	int i,j,k;
	RGBQUAD* prgbDest;
	RGBQUAD* prgbSrc;

	printf(" AreaAlphaMask\n");
	printf("Not support here \n"); for(;;){}

#ifdef NOTUSE_ACCELERATION

#endif 
	if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
	{
		desW = LCDW;
		desH = LCDH;
	}
	else
	{
#if (COLORPIXEL565 == 1)
		desW = *(unsigned int*)desBuf;
		desH = *((unsigned int*)desBuf+1);
		desBuf = (unsigned short*)desBuf+4;	
	    #else
		desW = *(unsigned int*)desBuf;
		desH = *((unsigned int*)desBuf+1);
		desBuf = (unsigned int*)desBuf+2;
#endif

	}
	if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
	{
		srcW = LCDW;
		srcH = LCDH;
		printf("AreaAlphaMask, read src from screen\n");
	}
	else
	{
#if (COLORPIXEL565 == 1)
		desW = *(unsigned int*)srcBuf;
		desH = *((unsigned int*)srcBuf+1);
		srcBuf = (unsigned short*)srcBuf+4;	
	    #else
		srcW = *(unsigned int*)srcBuf;
		srcH = *((unsigned int*)srcBuf+1);
		srcBuf = (unsigned int*)srcBuf+2;
#endif

	}	
	if((srcW == 0) || (srcH == 0) || (desW == 0) || (desH == 0))
	{
		ERROR_X("error, AreaDraw WH should not be zero\n");
		return -1;
	}
	if((srcX >= srcW) || (srcY >= srcH))
		return 0;		
	if((desX >= desW) || (desY >= desH))
		return 0;		
	drawW = GETMIN(GETMIN(desX+W, desW)-desX, GETMIN(srcX+W, srcW)-srcX);
	drawH = GETMIN(desY+H, desH)-desY;
	
#if 1	

	for(j=0; j<drawH; j++)
	{
		if((srcY+j >= srcH) || (desY+j >= desH))
			continue;
		for(i=0; i<drawW; i++)
		{
			if((srcX+i >= srcW) || (desX+i >= desW))
				continue;
			prgbSrc = (RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i);
			if(prgbSrc->rgbReserved == 0)
			{
				prgbDest = (RGBQUAD *)(desBuf+(desY+j)*desW+desX+i);
				prgbDest->rgbReserved = 0;
			}
		}
	}

#endif
    return 0;
}


int DrawString(PIXEL *desBuf_in, int desX, int desY, PIXEL *srcBuf_in, int srcX, int srcY,
               int W, int H, int Mode)
{
	int srcW,srcH;
	int desW,desH;
	int drawW,drawH;
	int i,j,k;
	int out_to_screen = 0;
	int read_from_screen = 0;

	//david hack, convert the 24 bit RGBA to RGB565
	int ii=0; 
	unsigned char *ptmp, *dtmp; 
	unsigned short *outp = (unsigned short *)&rgb24to565_line_buff[0];
		unsigned char tmpd = 0;
	//if we are rgb565
	unsigned short data;
	unsigned short *fb_outp, *fb_inp;
	PIXEL *desBuf;
	PIXEL *srcBuf;
		unsigned int transcolor_flag;

	if((W <= 0) || (H <= 0))
		return 0;

	//david test.
	//printf("AreaDraw\n");

	desBuf = desBuf_in;
	srcBuf = srcBuf_in;

	if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
	{
		//printf("AreaDraw 1\n");
		desW = LCDW;
		desH = LCDH;
		out_to_screen = 1;
		//printf("AreaDraw, out to screen\n");
	}
	else
	{
        #if (COLORPIXEL565 == 1)
		desW = *(unsigned int*)desBuf;
		desH = *((unsigned int*)desBuf+1);
		desBuf = (unsigned short*)desBuf+4; 
    #else
		desW = *(unsigned int*)desBuf;
		desH = *((unsigned int*)desBuf+1);		
		desBuf = (unsigned int*)desBuf+2;
    #endif
		 out_to_screen = 0;    
	}
	if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
	{
		srcW = LCDW;
		srcH = LCDH;
		//printf("AreaDraw, read src from screen\n");
		read_from_screen = 1;
	}
	else
	{	 
    #if (COLORPIXEL565 == 1)
		srcW = *(unsigned int*)srcBuf;
		srcH = *((unsigned int*)srcBuf+1);
		srcBuf = (unsigned short*)srcBuf+4; 
    #else
		srcW = *(unsigned int*)srcBuf;
		srcH = *((unsigned int*)srcBuf+1);
		srcBuf = (unsigned int*)srcBuf+2;
        #endif
		read_from_screen = 0;
	}	
	if((srcW == 0) || (srcH == 0) || (desW == 0) || (desH == 0))
	{
		ERROR_X("error, AreaDraw WH should not be zero\n");
		return -1;
	}
	if((srcX >= srcW) || (srcY >= srcH))
		return 0;		
	if((desX >= desW) || (desY >= desH))
		return 0;		
	drawW = GETMIN(GETMIN(desX+W, desW)-desX, GETMIN(srcX+W, srcW)-srcX);
	if(desX < 0){
		srcX = abs(desX);
		desX = 0;
		drawW = GETMIN(GETMIN(desX + W, desW) - desX, GETMIN(srcX + W, srcW) - srcX);
	}
	drawH = GETMIN(desY+H, desH)-desY;
	if(Mode == REPLACE_STYLE)
	{
#ifdef NOTUSE_ACCELERATION

#else
		//printf("devdraw, Mode == REPLACE_STYLE, srcY = %d, srcX = %d, drawW=%d, drawH=%d\n", srcY, srcX, drawW, drawH);
		//printf("devdraw, Mode == REPLACE_STYLE, desX= %d, desY = %d, desW = %d, desH = %d\n", desX, desY, desW, desH);
		
		if (out_to_screen == 1) {

			 for(j=0; j<drawH; j++)
			 {
			if(srcY+j >= srcH)
				break;

			//david hack, convert the 24 bit RGBA to RGB565
			outp = (unsigned short *)&rgb24to565_line_buff[0];
			ptmp = (unsigned char *)(srcBuf+ (srcY+j)*srcW+srcX);
			fb_outp = (unsigned short*)desBuf + ((desY+j)*desW+desX);
			fb_inp = (unsigned short*)srcBuf + (srcY+j)*srcW+srcX;
			//dtmp = (unsigned char *)fb_outp;

			if (read_from_screen == 0) {
			   for (ii=0; ii < drawW; ii++) {	
				  tmpd = 0;
				  *outp = 0;

				  //Image source are in BGRA format.	
				  tmpd = *ptmp++ & 0xf8;
				  *outp |= tmpd >> 3;
				  tmpd = *ptmp++ & 0xfC;
				  *outp |= tmpd << 3;
							  tmpd = *ptmp++ & 0xf8;
				  *outp |= tmpd << 8;	 
				  *ptmp++;

				 //*outp++ = ( ((*ptmp++ & 0xf8) << 8) || ((*ptmp++ & 0xfC) << 3) || ((*ptmp++ & 0xf8) >> 3) );
				 
				 *(fb_outp + ii) = *outp;				 
			   }

			   } // end of if read from screen
			   else {
			   for (ii=0; ii < drawW; ii++) {
				*(fb_outp + ii) = *(fb_inp + ii);
			   }
			   }

			}
		} //end of if
		else {

				for(j=0; j<drawH; j++)
				{
				if(srcY+j >= srcH)
				break;

				memcpy(desBuf+(desY+j)*desW+desX, srcBuf+(srcY+j)*srcW+srcX, drawW*sizeof(PIXEL));

				}

		}

#endif 
	}
	else if(Mode == TRANS_STYLE)
	{
#ifdef NOTUSE_ACCELERATION

#else
		printf("AreaDraw, Mode == TRANS_STYLE\n");
		if (out_to_screen == 1) {
			  for(j=0; j<drawH; j++)
			  {
			  //david hack, convert the 24 bit RGBA to RGB565
			  outp = (unsigned short *)&rgb24to565_line_buff[0];
			  ptmp = (unsigned char *)(srcBuf+ (srcY+j)*srcW+srcX);
			  fb_outp = (unsigned short*)desBuf + ((desY+j)*desW+desX);

			  for(i=0; i<drawW; i++)
			  {

				//If we are output to a RGB565 framebuffer
				  tmpd = 0;
				  data = 0;
				 
				 //Image source are in BGRA format. 
				  tmpd = *ptmp++ & 0xf8;
				  data |= tmpd >> 3;
				  tmpd = *ptmp++ & 0xfC;
				  data |= tmpd << 3;
							  tmpd = *ptmp++ & 0xf8;
				  data |= tmpd << 8;	 
				 
				 if (*ptmp++ != 0x00 /*TRANSCOLOR */)
					*(fb_outp + i) = data;					

			  }
			 }
		} //end of if out_to_screen == 1
		else {

			   for(j=0; j<drawH; j++)
			   {
				for(i=0; i<drawW; i++)
				{

						k = *(srcBuf+(srcY+j)*srcW+srcX+i);
							 
					if(k != TRANSCOLOR)
						*(desBuf+(desY+j)*desW+desX+i) = k;

				}
			   }
		}  //end of if

#endif 
	}
	else if(Mode == ALPHA_STYLE)
	{
#ifdef NOTUSE_ACCELERATION

#else
		RGBQUAD rgbDest, rgbSrc;
		int nBlend;
		unsigned char fbRed, fbGreen, fbBlue;
		unsigned char desFbRed, desFbGreen, desFbBlue;
		
		//printf(" AreaDraw, Mode == ALPHA_STYLE\n");
		if (out_to_screen == 1) {						

			for(j=0; j<drawH; j++)
				{

				for(i=0; i<drawW; i++)
			
				{
					fb_outp = (unsigned short *)desBuf+ (desY+j)*desW+desX;
					
					rgbSrc = *((RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i));
					
					if(rgbSrc.rgbReserved == 0xFF)
					{
					   data = 0;
					   data |= ((rgbSrc.rgbRed & 0xf8) << 8);
					   data |= ((rgbSrc.rgbGreen & 0xfC) << 3);
					   data |= ((rgbSrc.rgbBlue & 0xf8) >> 3);
					   *(fb_outp + i) = data;
					 //  *(desBuf+(desY+j)*desW+desX+i) = *(srcBuf+(srcY+j)*srcW+srcX+i);
					 
					}
					else if(rgbSrc.rgbReserved > 0)
					{
					  //Here, because the framebuffer is in RGB565 format,
					  // We need to format the RGB565 to and blend first. 
					  // THen we write it back to RGB565 framebufer.
					   nBlend = rgbSrc.rgbReserved;
					   
					   data = *(fb_outp + i);
					   fbRed = (data & 0xf800) >> 8; //8
					   fbGreen = (data & 0x07E0) >> 3; //3
					   fbBlue = (data & 0x001F) << 3;  //3
	
					  // rgbDest = *((RGBQUAD *)(desBuf+(desY+j)*desW+desX+i));
					   desFbRed = (nBlend * rgbSrc.rgbRed + (256-nBlend) * fbRed)>>8;
					   desFbGreen = (nBlend * rgbSrc.rgbGreen + (256-nBlend) * fbGreen)>>8;
					   desFbBlue = (nBlend * rgbSrc.rgbBlue + (256-nBlend) * fbBlue)>>8;
					   data = 0;
					   data |= ((desFbRed & 0xf8) << 8);
					   data |= ((desFbGreen & 0xfC) << 3);
					   data |= ((desFbBlue & 0xf8) >> 3);
					  *(fb_outp + i) = data;
					 
					  //printf("outdata = %x\n", *(fb_outp + i));
					   //*(desBuf+(desY+j)*desW+desX+i) = *(PIXEL *)(&rgbDest);
					}
				}

			}
			
		}
		else {

				for(j=0; j<drawH; j++)
				{

				for(i=0; i<drawW; i++)
			
				{
					rgbSrc = *((RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i));
					if(rgbSrc.rgbReserved == 0xFF)
					{
					   *(desBuf+(desY+j)*desW+desX+i) = *(srcBuf+(srcY+j)*srcW+srcX+i);
					}
					else if(rgbSrc.rgbReserved > 0)
					{
					   nBlend = rgbSrc.rgbReserved;
					   rgbDest = *((RGBQUAD *)(desBuf+(desY+j)*desW+desX+i));
					   rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256-nBlend) * rgbDest.rgbRed)>>8;
					   rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256-nBlend) * rgbDest.rgbGreen)>>8;
					   rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256-nBlend) * rgbDest.rgbBlue)>>8;
					   *(desBuf+(desY+j)*desW+desX+i) = *(PIXEL *)(&rgbDest);
					}
				}

			}
		} // end of else

#endif 
	}
	return 0;

}


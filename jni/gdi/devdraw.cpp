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
void AreaFill(PIXEL* pDesBuf, int x, int y, int width, int height, PIXEL color)
{
	int i,j;
	PIXEL* p;
	PIXEL *temp;
	int desW,desH;
	int drawW,drawH;
        unsigned short *fb_outp;
	unsigned char *col_p;
        unsigned short rgb565_col;

	if((pDesBuf == GetBackupBuffer()) || (pDesBuf == GetDisplayBuffer()))
	{
		desW = LCDW;
		desH = LCDH;
		temp = pDesBuf;
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
	}


	if((x >= desW) || (y >= desH))
		return;
	drawW = GETMIN(x+width, desW)-x;
	drawH = GETMIN(y+height, desH)-y;
	for(j=y; j<y+drawH; j++)
	{
	
#if defined(LCD_ROTATE_90)

		if((pDesBuf == GetBackupBuffer()) || (pDesBuf == GetDisplayBuffer()))
		{
			for(i=0; i<drawW; i++)
				*(temp + (x+i)*desH + (LCDH-1)-j) = color;
		}
		else
		{
			p = temp+j*desW+x;
			for(i=0; i<drawW; i++,p++)
				*p = color;
		}

#elif defined(LCD_ROTATE_270)

		if((pDesBuf == GetBackupBuffer()) || (pDesBuf == GetDisplayBuffer()))
		{
			for(i=0; i<drawW; i++)
				*(temp + ((LCDW-1)-(x+i))*desH + j) = color;
		}
		else
		{
			p = temp+j*desW+x;
			for(i=0; i<drawW; i++,p++)
				*p = color;
		}

#else
		p = temp+j*desW+x;
		for(i=0; i<drawW; i++,p++)
			*p = color;
#endif

	}	
}

// 将图片对应的某一个内存copy到某一个内存中(也包括显示内存)，copy的宽高可以指定，常用的模式为REPLACE_STYLE
// REPLACE_STYLE模式为直接替换
// TRANS_STYLE模式将检测图片的像素点的颜色是否为TRANSCOLOR，如果是将不替换颜色而显示背景颜色，常用于显示底图
// ALPHA_STYLE模式将检测图片的像素点的颜色和屏幕中对应位置的颜色进行ALPHA合成。常用于加强显示的效果。
int AreaDraw(PIXEL *desBuf, int desX, int desY, PIXEL *srcBuf, int srcX, int srcY,
             int W, int H, int Mode)
{
	int srcW,srcH;
	int desW,desH;
	int drawW,drawH;
	int i,j,k;

	//david hack, convert the 24 bit RGBA to RGB565
	int ii=0; 
	unsigned char *ptmp; 
	unsigned short *outp = (unsigned short *)&rgb24to565_line_buff[0];
        unsigned char tmpd = 0;
	//if we are rgb565
	unsigned short data;
	unsigned short *fb_outp;

	if((W <= 0) || (H <= 0))
		return 0;

	//david test.
	//printf("AreaDraw\n");

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
#ifdef DELETEME_ACCELERATION

#else
		//printf("devdraw, Mode == REPLACE_STYLE, srcY = %d, srcX = %d, drawW=%d, drawH=%d\n", srcY, srcX, drawW, drawH);
		
		for(j=0; j<drawH; j++)
		{
			if(srcY+j >= srcH)
				break;
#if defined(LCD_ROTATE_90)

			if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
			{
				for(i = 0; i < drawW; i ++ )
				{
					if(srcX +i >= srcW)
						break;
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						memcpy(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j), srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j), sizeof(PIXEL));
					}
					else
					{
						memcpy(desBuf + (desY+j)*desW + (desX+i), srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j), sizeof(PIXEL));
					}

				}
			}
			else
			{
				for(i = 0; i < drawW; i ++ )
				{
					if(srcX +i >= srcW)
						break;

					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						memcpy(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j), srcBuf + (srcY+j)*srcW + srcX+i, sizeof(PIXEL));
					}
					else
					{
						memcpy(desBuf+(desY+j)*desW+desX, srcBuf+(srcY+j)*srcW+srcX, drawW*sizeof(PIXEL));
					}
				}
			}

#elif defined(LCD_ROTATE_270)

			if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
			{
				for(i = 0; i < drawW; i ++ )
				{
					if(srcX +i >= srcW)
						break;

					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						memcpy(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j), srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j), sizeof(PIXEL));
					}
					else
					{
						memcpy(desBuf + (desY+j)*desW + (desX+i), srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j), sizeof(PIXEL));
					}
				}

			}
			else
			{
				for(i = 0; i < drawW; i ++ )
				{
					if(srcX +i >= srcW)
						break;
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						memcpy(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j), srcBuf + (srcY+j)*srcW + srcX+i, sizeof(PIXEL));
					}
					else
					{
						memcpy(desBuf+(desY+j)*desW+desX, srcBuf+(srcY+j)*srcW+srcX, drawW*sizeof(PIXEL));
					}
				}
			}

#else
			memcpy(desBuf+(desY+j)*desW+desX, srcBuf+(srcY+j)*srcW+srcX, drawW*sizeof(PIXEL));
#endif

		}
#endif 
	}
	else if(Mode == TRANS_STYLE)
	{
#ifdef DELETEME_ACCELERATION

#else
		for(j=0; j<drawH; j++)
		{
			for(i=0; i<drawW; i++)
			{

#if defined(LCD_ROTATE_90)

				if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						k = *(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j));
						if(k != TRANSCOLOR)
							*(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)) = k;
					}
					else
					{
						k = *(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j));
						if(k != TRANSCOLOR)
							*(desBuf+(desY+j)*desW+desX+i) = k;
					}
				}
				else
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						k = *(srcBuf+(srcY+j)*srcW+srcX+i);
						if(k != TRANSCOLOR)
							*(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)) = k;
					}
					else
					{
						k = *(srcBuf+(srcY+j)*srcW+srcX+i);
						if(k != TRANSCOLOR)
							*(desBuf+(desY+j)*desW+desX+i) = k;
					}
				}

#elif defined(LCD_ROTATE_270)

				if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						k = *(srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j));
						if(k != TRANSCOLOR)
							*(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)) = k;
					}
					else
					{
						k = *(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j));
						if(k != TRANSCOLOR)
							*(desBuf+(desY+j)*desW+desX+i) = k;
					}
				}
				else
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						k = *(srcBuf+(srcY+j)*srcW+srcX+i);
						if(k != TRANSCOLOR)
							*(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)) = k;
					}
					else
					{
						k = *(srcBuf+(srcY+j)*srcW+srcX+i);
						if(k != TRANSCOLOR)
							*(desBuf+(desY+j)*desW+desX+i) = k;
					}
				}

#else
				k = *(srcBuf+(srcY+j)*srcW+srcX+i);
				if(k != TRANSCOLOR)
					*(desBuf+(desY+j)*desW+desX+i) = k;
#endif

			}
		}
#endif 
	}
	else if(Mode == ALPHA_STYLE)
	{
#ifdef DELETEME_ACCELERATION

#else
		RGBQUAD rgbDest, rgbSrc;
		int nBlend;
		for(j=0; j<drawH; j++)
		{
			for(i=0; i<drawW; i++)
#if 1			
			{

#if defined(LCD_ROTATE_90)

				if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j)));
						if(rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)) = *(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j));
						}
						else if(rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256-nBlend) * rgbDest.rgbRed)>>8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256-nBlend) * rgbDest.rgbGreen)>>8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256-nBlend) * rgbDest.rgbBlue)>>8;
							*(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)) = *(PIXEL *)(&rgbDest);
						}
					}
					else
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j)));
						if(rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf+(desY+j)*desW+desX+i) = *(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j));
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
				else
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						rgbSrc = *((RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i));
						if(rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)) = *(srcBuf+(srcY+j)*srcW+srcX+i);
						}
						else if(rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256-nBlend) * rgbDest.rgbRed)>>8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256-nBlend) * rgbDest.rgbGreen)>>8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256-nBlend) * rgbDest.rgbBlue)>>8;
							*(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)) = *(PIXEL *)(&rgbDest);
						}
					}
					else
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
				
#elif defined(LCD_ROTATE_270)

				if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j)));
						if(rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)) = *(srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j));
						}
						else if(rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256-nBlend) * rgbDest.rgbRed)>>8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256-nBlend) * rgbDest.rgbGreen)>>8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256-nBlend) * rgbDest.rgbBlue)>>8;
							*(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)) = *(PIXEL *)(&rgbDest);
						}

					}
					else
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j)));
						if(rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf+(desY+j)*desW+desX+i) = *(srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j));
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
				else
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						rgbSrc = *((RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i));
						if(rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)) = *(srcBuf+(srcY+j)*srcW+srcX+i);
						}
						else if(rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256-nBlend) * rgbDest.rgbRed)>>8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256-nBlend) * rgbDest.rgbGreen)>>8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256-nBlend) * rgbDest.rgbBlue)>>8;
							*(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)) = *(PIXEL *)(&rgbDest);
						}
					}
					else
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


#else
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
				
#endif

			}
#endif
		}

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
#ifdef DELETEME_ACCELERATION

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
	
	for(j=0; j<drawH; j++)
	{
		if((srcY+j >= srcH) || (desY+j >= desH))
			continue;
		for(i=0; i<drawW; i++)
		{
			if((srcX+i >= srcW) || (desX+i >= desW))
				continue;

#if defined(LCD_ROTATE_90)

			if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
			{
				if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
				{
					prgbSrc = (RGBQUAD *)(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j));
					if(prgbSrc->rgbReserved == 0)
					{
						prgbDest = (RGBQUAD *)(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j));
						prgbDest->rgbReserved = 0;
					}
				}
				else
				{
					prgbSrc = (RGBQUAD *)(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j));
					if(prgbSrc->rgbReserved == 0)
					{
						prgbDest = (RGBQUAD *)(desBuf+(desY+j)*desW+desX+i);
						prgbDest->rgbReserved = 0;
					}
				}
			}
			else
			{
				if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
				{
					prgbSrc = (RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i);
					if(prgbSrc->rgbReserved == 0)
					{
						prgbDest = (RGBQUAD *)(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j));
						prgbDest->rgbReserved = 0;
					}
				}
				else
				{
					prgbSrc = (RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i);
					if(prgbSrc->rgbReserved == 0)
					{
						prgbDest = (RGBQUAD *)(desBuf+(desY+j)*desW+desX+i);
						prgbDest->rgbReserved = 0;
					}
				}
			}

#elif defined(LCD_ROTATE_270)

			if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
			{
				if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
				{
					prgbSrc = (RGBQUAD *)(srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j));
					if(prgbSrc->rgbReserved == 0)
					{
						prgbDest = (RGBQUAD *)(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j));
						prgbDest->rgbReserved = 0;
					}

				}
				else
				{
					prgbSrc = (RGBQUAD *)(srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j));
					if(prgbSrc->rgbReserved == 0)
					{
						prgbDest = (RGBQUAD *)(desBuf+(desY+j)*desW+desX+i);
						prgbDest->rgbReserved = 0;
					}

				}
			}
			else
			{
				if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
				{
					prgbSrc = (RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i);
					if(prgbSrc->rgbReserved == 0)
					{
						prgbDest = (RGBQUAD *)(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j));
						prgbDest->rgbReserved = 0;
					}

				}
				else
				{
					prgbSrc = (RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i);
					if(prgbSrc->rgbReserved == 0)
					{
						prgbDest = (RGBQUAD *)(desBuf+(desY+j)*desW+desX+i);
						prgbDest->rgbReserved = 0;
					}
				}
			}

#else
			prgbSrc = (RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i);
			if(prgbSrc->rgbReserved == 0)
			{
				prgbDest = (RGBQUAD *)(desBuf+(desY+j)*desW+desX+i);
				prgbDest->rgbReserved = 0;
			}
#endif
				
			
		}
	}
	return 0;
}

int DrawString(PIXEL *desBuf, int desX, int desY, PIXEL *srcBuf, int srcX, int srcY,
             int W, int H, int Mode)
{
	int srcW,srcH;
	int desW,desH;
	int drawW,drawH;
	int i,j,k;

	//david hack, convert the 24 bit RGBA to RGB565
	int ii=0; 
	unsigned char *ptmp; 
	unsigned short *outp = (unsigned short *)&rgb24to565_line_buff[0];
        unsigned char tmpd = 0;
	//if we are rgb565
	unsigned short data;
	unsigned short *fb_outp;

	if((W <= 0) || (H <= 0))
		return 0;

	//david test.
	//printf("AreaDraw\n");

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
	if(Mode == REPLACE_STYLE)
	{
#ifdef DELETEME_ACCELERATION
	
#else
		//printf("devdraw, Mode == REPLACE_STYLE, srcY = %d, srcX = %d, drawW=%d, drawH=%d\n", srcY, srcX, drawW, drawH);
		
		for(j=0; j<drawH; j++)
		{
			if(srcY+j >= srcH)
				break;
#if defined(LCD_ROTATE_90)

			if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
			{
				for(i = 0; i < drawW; i ++ )
				{
					if(srcX +i >= srcW)
						break;
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						memcpy(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j), srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j), sizeof(PIXEL));
					}
					else
					{
						memcpy(desBuf + (desY+j)*desW + (desX+i), srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j), sizeof(PIXEL));
					}

				}
			}
			else
			{
				for(i = 0; i < drawW; i ++ )
				{
					if(srcX +i >= srcW)
						break;

					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						memcpy(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j), srcBuf + (srcY+j)*srcW + srcX+i, sizeof(PIXEL));
					}
					else
					{
						memcpy(desBuf+(desY+j)*desW+desX, srcBuf+(srcY+j)*srcW+srcX, drawW*sizeof(PIXEL));
					}
				}
			}

#elif defined(LCD_ROTATE_270)

			if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
			{
				for(i = 0; i < drawW; i ++ )
				{
					if(srcX +i >= srcW)
						break;

					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						memcpy(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j), srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j), sizeof(PIXEL));
					}
					else
					{
						memcpy(desBuf + (desY+j)*desW + (desX+i), srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j), sizeof(PIXEL));
					}
				}

			}
			else
			{
				for(i = 0; i < drawW; i ++ )
				{
					if(srcX +i >= srcW)
						break;
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						memcpy(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j), srcBuf + (srcY+j)*srcW + srcX+i, sizeof(PIXEL));
					}
					else
					{
						memcpy(desBuf+(desY+j)*desW+desX, srcBuf+(srcY+j)*srcW+srcX, drawW*sizeof(PIXEL));
					}
				}
			}

#else
			memcpy(desBuf+(desY+j)*desW+desX, srcBuf+(srcY+j)*srcW+srcX, drawW*sizeof(PIXEL));
#endif

		}
#endif 
	}
	else if(Mode == TRANS_STYLE)
	{
		for(j=0; j<drawH; j++)
		{
			for(i=0; i<drawW; i++)
			{

#if defined(LCD_ROTATE_90)

				if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						k = *(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j));
						if(k != TRANSCOLOR)
							*(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)) = k;
					}
					else
					{
						k = *(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j));
						if(k != TRANSCOLOR)
							*(desBuf+(desY+j)*desW+desX+i) = k;
					}
				}
				else
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						k = *(srcBuf+(srcY+j)*srcW+srcX+i);
						if(k != TRANSCOLOR)
							*(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)) = k;
					}
					else
					{
						k = *(srcBuf+(srcY+j)*srcW+srcX+i);
						if(k != TRANSCOLOR)
							*(desBuf+(desY+j)*desW+desX+i) = k;
					}
				}

#elif defined(LCD_ROTATE_270)

				if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						k = *(srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j));
						if(k != TRANSCOLOR)
							*(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)) = k;
					}
					else
					{
						k = *(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j));
						if(k != TRANSCOLOR)
							*(desBuf+(desY+j)*desW+desX+i) = k;
					}
				}
				else
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						k = *(srcBuf+(srcY+j)*srcW+srcX+i);
						if(k != TRANSCOLOR)
							*(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)) = k;
					}
					else
					{
						k = *(srcBuf+(srcY+j)*srcW+srcX+i);
						if(k != TRANSCOLOR)
							*(desBuf+(desY+j)*desW+desX+i) = k;
					}
				}

#else
				k = *(srcBuf+(srcY+j)*srcW+srcX+i);
				if(k != TRANSCOLOR)
					*(desBuf+(desY+j)*desW+desX+i) = k;
#endif

			}
		}
#endif 
	}
	else if(Mode == ALPHA_STYLE)
	{
#ifdef DELETEME_ACCELERATION

#else
		RGBQUAD rgbDest, rgbSrc;
		int nBlend;
		for(j=0; j<drawH; j++)
		{
			for(i=0; i<drawW; i++)
#if 1			
			{

#if defined(LCD_ROTATE_90)

				if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j)));
						if(rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)) = *(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j));
						}
						else if(rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256-nBlend) * rgbDest.rgbRed)>>8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256-nBlend) * rgbDest.rgbGreen)>>8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256-nBlend) * rgbDest.rgbBlue)>>8;
							*(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)) = *(PIXEL *)(&rgbDest);
						}
					}
					else
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j)));
						if(rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf+(desY+j)*desW+desX+i) = *(srcBuf + (srcX+i)*srcH + (LCDH-1)-(srcY+j));
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
				else
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						rgbSrc = *((RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i));
						if(rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)) = *(srcBuf+(srcY+j)*srcW+srcX+i);
						}
						else if(rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256-nBlend) * rgbDest.rgbRed)>>8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256-nBlend) * rgbDest.rgbGreen)>>8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256-nBlend) * rgbDest.rgbBlue)>>8;
							*(desBuf + (desX+i)*desH + (LCDH-1)-(desY+j)) = *(PIXEL *)(&rgbDest);
						}
					}
					else
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
				
#elif defined(LCD_ROTATE_270)

				if((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j)));
						if(rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)) = *(srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j));
						}
						else if(rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256-nBlend) * rgbDest.rgbRed)>>8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256-nBlend) * rgbDest.rgbGreen)>>8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256-nBlend) * rgbDest.rgbBlue)>>8;
							*(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)) = *(PIXEL *)(&rgbDest);
						}

					}
					else
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j)));
						if(rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf+(desY+j)*desW+desX+i) = *(srcBuf + ((LCDW-1)-(srcX+i))*srcH + (srcY+j));
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
				else
				{
					if((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						rgbSrc = *((RGBQUAD *)(srcBuf+(srcY+j)*srcW+srcX+i));
						if(rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)) = *(srcBuf+(srcY+j)*srcW+srcX+i);
						}
						else if(rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256-nBlend) * rgbDest.rgbRed)>>8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256-nBlend) * rgbDest.rgbGreen)>>8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256-nBlend) * rgbDest.rgbBlue)>>8;
							*(desBuf + ((LCDW-1)-(desX+i))*desH + (desY+j)) = *(PIXEL *)(&rgbDest);
						}
					}
					else
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


#else
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
				
#endif

			}
#endif
		}

#endif 
	}
	return 0;
}

	
#if 0
int DrawString(PIXEL *desBuf, Rect* destRect, PIXEL *srcBuf,
	Rect* srcRect, int Mode){
	int drawW, drawH;
	int i, j, k;

	if ((srcRect->w <= 0) || (srcRect->h <= 0))
		return 0;

	if ((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
	{
		destRect->w = LCDW;
		destRect->h = LCDH;
	}
	else
	{
#if (COLORPIXEL565 == 1)
		destRect->w = *(unsigned int*)desBuf;
		destRect->h = *((unsigned int*)desBuf + 1);
		desBuf = (unsigned short*)desBuf + 4;
#else
		destRect->w = *(unsigned int*)desBuf;
		destRect->h = *((unsigned int*)desBuf + 1);
		desBuf = (unsigned int*)desBuf + 2;
#endif
	}
	if ((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
	{
		srcRect->w = LCDW;
		srcRect->h = LCDH;
	}
	else
	{
#if (COLORPIXEL565 == 1)
		srcRect->w = *(unsigned int*)srcBuf;
		srcRect->h = *((unsigned int*)srcBuf + 1);
		srcBuf = (unsigned short*)srcBuf + 4;
#else
		srcRect->w = *(unsigned int*)srcBuf;
		srcRect->h = *((unsigned int*)srcBuf + 1);
		srcBuf = (unsigned int*)srcBuf + 2;
#endif
	}
	if ((srcRect->w == 0) || (srcRect->h == 0) || (destRect->w == 0) || (destRect->h == 0))
	{
		ERROR_X("error, AreaDraw WH should not be zero\n");
		return -1;
	}

	if ((destRect->x >= destRect->w) || (destRect->y >= destRect->h))
		return 0;
	drawW = GETMIN(GETMIN(destRect->x + srcRect->w, destRect->w) - destRect->x, srcRect->w);
	drawH = GETMIN(destRect->y + srcRect->h, destRect->h) - destRect->y;
	if (Mode == REPLACE_STYLE)
	{
#ifndef DELETEME_ACCELERATION
		for (j = 0; j<drawH; j++)
		{
			if (j >= srcRect->h)
				break;
#if defined(LCD_ROTATE_90)

			if ((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
			{
				for (i = 0; i < drawW; i++)
				{
					if (i >= srcRect->w)
						break;
					if ((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						memcpy(desBuf + (destRect->x + i)*destRect->h + (LCDH - 1) - (destRect->y + j), srcBuf + i*srcRect->h + (LCDH - 1) - j, sizeof(PIXEL));
					}
					else
					{
						memcpy(desBuf + (destRect->y + j)*destRect->w + (destRect->x + i), srcBuf + i*srcRect->h + (LCDH - 1) - j, sizeof(PIXEL));
					}

				}
			}
			else
			{
				for (i = 0; i < drawW; i++)
				{
					if (i >= srcRect->w)
						break;

					if ((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						memcpy(desBuf + (destRect->x + i)*destRect->h + (LCDH - 1) - (destRect->y + j), srcBuf + j*srcRect->w + i, sizeof(PIXEL));
					}
					else
					{
						memcpy(desBuf + (destRect->y + j)*destRect->w + destRect->x, srcBuf + j*srcRect->w, drawW*sizeof(PIXEL));
					}
				}
			}

#elif defined(LCD_ROTATE_270)

			if ((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
			{
				for (i = 0; i < drawW; i++)
				{
					if (i >= srcRect->w)
						break;

					if ((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						memcpy(desBuf + ((LCDW - 1) - (destRect->x + i))*destRect->h + (destRect->y + j), srcBuf + ((LCDW - 1) - i)*srcRect->h + (srcY + j), sizeof(PIXEL));
					}
					else
					{
						memcpy(desBuf + (destRect->y + j)*destRect->w + (destRect->x + i), srcBuf + ((LCDW - 1) - i)*srcRect->h + j, sizeof(PIXEL));
					}
				}

			}
			else
			{
				for (i = 0; i < drawW; i++)
				{
					if (i >= srcRect->w)
						break;
					if ((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						memcpy(desBuf + ((LCDW - 1) - (destRect->x + i))*destRect->h + (destRect->y + j), srcBuf + j*srcRect->w + i, sizeof(PIXEL));
					}
					else
					{
						memcpy(desBuf + (destRect->y + j)*destRect->w + destRect->x, srcBuf + j *srcRect->w, drawW*sizeof(PIXEL));
					}
				}
			}

#else
			memcpy(desBuf + (destRect->y + j)*destRect->w + destRect->x, srcBuf + j *srcRect->w, drawW*sizeof(PIXEL));
#endif

		}
#endif 
	}
	else if (Mode == TRANS_STYLE)
	{
#ifndef DELETEME_ACCELERATION

		for (j = 0; j<drawH; j++)
		{
			for (i = 0; i<drawW; i++)
			{

#if defined(LCD_ROTATE_90)
				if ((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
				{
					if ((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						k = *(srcBuf + i*srcRect->h + (LCDH - 1) - j);
						if (k != TRANSCOLOR)
							*(desBuf + (destRect->x + i)*destRect->h + (LCDH - 1) - (destRect->y + j)) = k;
					}
					else
					{
						k = *(srcBuf + i*srcRect->h + (LCDH - 1) - j);
						if (k != TRANSCOLOR)
							*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = k;
					}
				}
				else
				{
					if ((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						k = *(srcBuf + j *srcRect->w + i);
						if (k != TRANSCOLOR)
							*(desBuf + (destRect->x + i)*destRect->h + (LCDH - 1) - (destRect->y + j)) = k;
					}
					else
					{
						k = *(srcBuf + j*srcRect->w + i);
						if (k != TRANSCOLOR)
							*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = k;
					}
				}

#elif defined(LCD_ROTATE_270)

				if ((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
				{
					if ((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						k = *(srcBuf + ((LCDW - 1) - i)*srcRect->h + j);
						if (k != TRANSCOLOR)
							*(desBuf + ((LCDW - 1) - (destRect->x + i))*destRect->h + (destRect->y + j)) = k;
					}
					else
					{
						k = *(srcBuf + i*srcRect->h + (LCDH - 1) - j);
						if (k != TRANSCOLOR)
							*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = k;
					}
				}
				else
				{
					if ((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						k = *(srcBuf + j*srcRect->w + i);
						if (k != TRANSCOLOR)
							*(desBuf + ((LCDW - 1) - (destRect->x + i))*destRect->h + (destRect->y + j)) = k;
					}
					else
					{
						k = *(srcBuf + j*srcRect->w + i);
						if (k != TRANSCOLOR)
							*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = k;
					}
				}

#else
				k = *(srcBuf + j*srcRect->w + i);
				if (k != TRANSCOLOR)
					*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = k;
#endif

			}
		}
#endif 
	}
	else if (Mode == ALPHA_STYLE)
	{
#ifndef DELETEME_ACCELERATION
		RGBQUAD rgbDest, rgbSrc;
		int nBlend;
		for (j = 0; j<drawH; j++)
		{
			for (i = 0; i<drawW; i++)
#if 1			
			{

#if defined(LCD_ROTATE_90)

				if ((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
				{
					if ((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + i*srcRect->h + (LCDH - 1) - j));
						if (rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + (destRect->x + i)*destRect->h + (LCDH - 1) - (destRect->y + j)) = *(srcBuf + i*srcRect->h + (LCDH - 1) - j);
						}
						else if (rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + (destRect->x + i)*destRect->h + (LCDH - 1) - (destRect->y + j)));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256 - nBlend) * rgbDest.rgbRed) >> 8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256 - nBlend) * rgbDest.rgbGreen) >> 8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256 - nBlend) * rgbDest.rgbBlue) >> 8;
							*(desBuf + (destRect->x + i)*destRect->h + (LCDH - 1) - (destRect->y + j)) = *(PIXEL *)(&rgbDest);
						}
					}
					else
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + i*srcRect->h + (LCDH - 1) - j));
						if (rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = *(srcBuf + i*srcRect->h + (LCDH - 1) - j);
						}
						else if (rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + (destRect->y + j)*destRect->w + destRect->x + i));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256 - nBlend) * rgbDest.rgbRed) >> 8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256 - nBlend) * rgbDest.rgbGreen) >> 8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256 - nBlend) * rgbDest.rgbBlue) >> 8;
							*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = *(PIXEL *)(&rgbDest);
						}

					}
				}
				else
				{
					if ((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + j*srcRect->w + i));
						if (rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + (destRect->x + i)*destRect->h + (LCDH - 1) - (destRect->y + j)) = *(srcBuf + j*srcRect->w + i);
						}
						else if (rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + (destRect->x + i)*destRect->h + (LCDH - 1) - (destRect->y + j)));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256 - nBlend) * rgbDest.rgbRed) >> 8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256 - nBlend) * rgbDest.rgbGreen) >> 8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256 - nBlend) * rgbDest.rgbBlue) >> 8;
							*(desBuf + (destRect->x + i)*destRect->h + (LCDH - 1) - (destRect->y + j)) = *(PIXEL *)(&rgbDest);
						}
					}
					else
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + j*srcRect->w + i));
						if (rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = *(srcBuf + j*srcRect->w + i);
						}
						else if (rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + (destRect->y + j)*destRect->w + destRect->x + i));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256 - nBlend) * rgbDest.rgbRed) >> 8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256 - nBlend) * rgbDest.rgbGreen) >> 8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256 - nBlend) * rgbDest.rgbBlue) >> 8;
							*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = *(PIXEL *)(&rgbDest);
						}

					}
				}

#elif defined(LCD_ROTATE_270)

				if ((srcBuf == GetBackupBuffer()) || (srcBuf == GetDisplayBuffer()))
				{
					if ((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + ((LCDW - 1) - i)*srcRect->h + j));
						if (rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + ((LCDW - 1) - (destRect->x + i))*destRect->h + (destRect->y + j)) = *(srcBuf + ((LCDW - 1) - i)*srcRect->h + j);
						}
						else if (rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + ((LCDW - 1) - (destRect->x + i))*destRect->h + (destRect->y + j)));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256 - nBlend) * rgbDest.rgbRed) >> 8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256 - nBlend) * rgbDest.rgbGreen) >> 8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256 - nBlend) * rgbDest.rgbBlue) >> 8;
							*(desBuf + ((LCDW - 1) - (destRect->x + i))*destRect->h + (destRect->y + j)) = *(PIXEL *)(&rgbDest);
						}

					}
					else
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + ((LCDW - 1) - i)*srcRect->h + j));
						if (rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = *(srcBuf + ((LCDW - 1) - i)*srcRect->h + j);
						}
						else if (rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + (destRect->y + j)*destRect->w + destRect->x + i));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256 - nBlend) * rgbDest.rgbRed) >> 8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256 - nBlend) * rgbDest.rgbGreen) >> 8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256 - nBlend) * rgbDest.rgbBlue) >> 8;
							*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = *(PIXEL *)(&rgbDest);
						}

					}
				}
				else
				{
					if ((desBuf == GetBackupBuffer()) || (desBuf == GetDisplayBuffer()))
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + j*srcRect->w + i));
						if (rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + ((LCDW - 1) - (destRect->x + i))*destRect->h + (destRect->y + j)) = *(srcBuf + j*srcRect->w + i);
						}
						else if (rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + ((LCDW - 1) - (destRect->x + i))*destRect->h + (destRect->y + j)));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256 - nBlend) * rgbDest.rgbRed) >> 8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256 - nBlend) * rgbDest.rgbGreen) >> 8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256 - nBlend) * rgbDest.rgbBlue) >> 8;
							*(desBuf + ((LCDW - 1) - (destRect->x + i))*destRect->h + (destRect->y + j)) = *(PIXEL *)(&rgbDest);
						}
					}
					else
					{
						rgbSrc = *((RGBQUAD *)(srcBuf + j*srcRect->w + i));
						if (rgbSrc.rgbReserved == 0xFF)
						{
							*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = *(srcBuf + j*srcRect->w + i);
						}
						else if (rgbSrc.rgbReserved > 0)
						{
							nBlend = rgbSrc.rgbReserved;
							rgbDest = *((RGBQUAD *)(desBuf + (destRect->y + j)*destRect->w + destRect->x + i));
							rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256 - nBlend) * rgbDest.rgbRed) >> 8;
							rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256 - nBlend) * rgbDest.rgbGreen) >> 8;
							rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256 - nBlend) * rgbDest.rgbBlue) >> 8;
							*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = *(PIXEL *)(&rgbDest);
						}

					}
				}


#else
				rgbSrc = *((RGBQUAD *)(srcBuf + j*srcRect->w + i));
				if (rgbSrc.rgbReserved == 0xFF)
				{
					*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = *(srcBuf + j*srcRect->w + i);
				}
				else if (rgbSrc.rgbReserved > 0)
				{
					nBlend = rgbSrc.rgbReserved;
					rgbDest = *((RGBQUAD *)(desBuf + (destRect->y + j)*destRect->w + destRect->x + i));
					rgbDest.rgbRed = (nBlend * rgbSrc.rgbRed + (256 - nBlend) * rgbDest.rgbRed) >> 8;
					rgbDest.rgbGreen = (nBlend * rgbSrc.rgbGreen + (256 - nBlend) * rgbDest.rgbGreen) >> 8;
					rgbDest.rgbBlue = (nBlend * rgbSrc.rgbBlue + (256 - nBlend) * rgbDest.rgbBlue) >> 8;
					*(desBuf + (destRect->y + j)*destRect->w + destRect->x + i) = *(PIXEL *)(&rgbDest);
				}

#endif

			}
#endif
		}

#endif 
	}
	return 0;

}
#endif




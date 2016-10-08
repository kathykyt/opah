#ifndef _SLIDE_H_
#define _SLIDE_H_

#include "image.h"

typedef struct _SlideItem SlideItem;
struct _SlideItem
{
	int SlideIndex;		//	Slide的索引
	Image* pSlideItemImage;
	int xshowstart;		//	item图片对应显示的起始坐标偏移值
	int yshowstart;
	int widthshow;
	int heightshow;
	_SlideItem* PreSlideItem;
	_SlideItem* NextSlideItem;	
};

typedef struct _SlideSonCtlData SlideSonCtlData;
struct _SlideSonCtlData
{
	Widget* pFocusWidget;
	int SlideSonCtlTimerID;
	Touch_Event downevent;
};

class Slide:public Image
{
    private:
		Touch_Event prets;
		int prets_diff;
		int ts_diff_dec_count;

	protected:
    public:
		SlideSonCtlData SonCtlData;
		SlideItem* pSlide;
		int SlideOrient;
		int SlideSpring;
		int SlideBoundary;
		int SlideCycle;
		int SlideTimerID;
		int SlideCurrent;
		int SlideDes;
		int SlideRate;
		int SlideSpeed;
		int SlideLength;
		int SlideMaxIndex;
		int SlideInterval;
		int AutoSlideTimerID;
		Slide(int x, int y, int w, int h, int Interval, BYTE Rate, int Boundary, int slideMode);
		~Slide(void);
		int AddSlideItem(unsigned char* pPic);
		SlideItem* GetSlideItemFromIndex(int ItemIndex);
		int GetSlideShowItem(void);
		int SlideItemAddChild(int ItemIndex, CtlObject* pCtl);
		int SlideSetInterval(int Interval);
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);		
};

#endif

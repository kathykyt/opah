#ifndef _NEWSLIDE_H_
#define _NEWSLIDE_H_

#include "image.h"
#include "text.h"
#include "mulimage.h"


typedef struct _NewSlideItem NewSlideItem;
struct _NewSlideItem
{	
	bool showBig;
	int value;
	int SlideIndex;		//	Slide的索引
	Text* pSlideItemImage;
	Text* pTempText;
	int xshowstart;		//	item图片对应显示的起始坐标偏移值
	int yshowstart;
	int widthshow;
	int heightshow;
	_NewSlideItem* PreSlideItem;
	_NewSlideItem* NextSlideItem;
	int ItemWidth;	
};

typedef void (*SlideListener)(int index,void* data);

class NewSlide:public Image
{
    private:
		Touch_Event prets;
		int prets_diff;
		int ts_diff_dec_count;
		SlideListener listener;
		
	protected:
    public:
		int indexDes;
		NewSlideItem* pSlide;	
		double SlieItemHeightPos;	
		int ShowIndexMax;
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
		SlideListener GetSlideListener();
		void SetSlideListener(SlideListener sListener);
		void SetSlideDes(int index);//ryze  test
		
		NewSlide(int x, int y, int w, int h, int Interval, BYTE Rate, int Boundary, int slideMode);
		~NewSlide(void);
		int AddSlideItem(const char* pPic,int  width,int height);
		NewSlideItem* GetSlideItemFromIndex(int ItemIndex);
		int GetSlideShowItem(void);
		int SlideItemAddChild(int ItemIndex, CtlObject* pCtl);
		int SlideSetInterval(int Interval);
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);
		int NewSlideSetBmpFT(int width,int heght,const char* fontColor);
		int SetSlideItemMiddleSize(int width,int heght,const char* fontColor);
		int SlideItemPos(double height);
		int SlideRefresh(int showIndexMax);
		bool isScroll;
		int changeSlideItem(const char* tx,int index);
};

#endif

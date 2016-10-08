#include "NewSlide.h"
#include "draw.h"
#include "GetBmpFT.h"
#include "mulimage.h"
#include "text.h"

//freetype
static GetBmpFT* pGetBmpFT;
static GetBmpFT* middleBmpFT;
static GetBmpFT* BmpFT;
static GetBmpFT* upDownBmpFT;
static GetBmpFT* normalBmpFT;
static unsigned char* pic_text0_9[10];
static unsigned char* pic_punctuation[8];
static unsigned char* pic_texta_z[26];
static unsigned char* pic_textA_Z[26];
static unsigned char* pic_time_morning[4];

static unsigned char* pic_text[10];
static unsigned char* pic_text_up_down[10];
static unsigned char* pic_text_up_down_normal[10];

static unsigned char* mid_text0_9[10];
static unsigned char* mid_punctuation[8];
static unsigned char* mid_texta_z[26];
static unsigned char* mid_textA_Z[26];
static unsigned char* mid_time_morning[4];
static void SlideTimerCallback(void* pctl);
static int textTempWidth;
static int textTempHeight;


int tempHeight;


void SetBmpFT(int width,int height,const char* fontColor){
    
    BmpFT = new GetBmpFT("/data/YDIYGO330.ttf",width,height,fontColor);   
    BmpFT->GetNumber(pic_text);
}
void SetUpDownBmpFT(int width,int height,const char* fontColor){
    
    upDownBmpFT = new GetBmpFT("/data/YDIYGO330.ttf",width,height,fontColor);    
    upDownBmpFT->GetNumber(pic_text_up_down);   
}

void SetNormalBmpFT(int width,int height,const char* fontColor){
    
    normalBmpFT = new GetBmpFT("/data/YDIYGO330.ttf",width,height,fontColor);    
    normalBmpFT->GetNumber(pic_text_up_down_normal);   
}

int NewSlide::NewSlideSetBmpFT(int width,int height,const char* fontColor){
    
    pGetBmpFT = new GetBmpFT("/data/YDIYGO330.ttf",width,height,fontColor);
   
    pGetBmpFT->GetLowercase(pic_texta_z);
   
    pGetBmpFT->GetNumber(pic_text0_9);
  
    
    pGetBmpFT->GetCapital(pic_textA_Z);
    
    pGetBmpFT->GetPunctuation(pic_punctuation);
   
    pGetBmpFT->GetWord(pic_time_morning, 0, 0xC624);
    pGetBmpFT->GetWord(pic_time_morning, 1, 0xD6C4);
    pGetBmpFT->GetWord(pic_time_morning, 2, 0xC544);
    pGetBmpFT->GetWord(pic_time_morning, 3, 0xCE68);
}


int NewSlide::SetSlideItemMiddleSize(int width,int height,const char* fontColor){
    
    middleBmpFT = new GetBmpFT("/data/YDIYGO330.ttf",width,height,fontColor);
   
    middleBmpFT->GetLowercase(mid_texta_z);
   
    middleBmpFT->GetNumber(mid_text0_9);
    
    middleBmpFT->GetCapital(mid_textA_Z);
    
    middleBmpFT->GetPunctuation(mid_punctuation);
   
    middleBmpFT->GetWord(mid_time_morning, 0, 0xC624);
    middleBmpFT->GetWord(mid_time_morning, 1, 0xD6C4);
    middleBmpFT->GetWord(mid_time_morning, 2, 0xC544);
    middleBmpFT->GetWord(mid_time_morning, 3, 0xCE68);
}

void NewSlide::SetSlideListener(SlideListener sListener){
	listener = sListener;

}
SlideListener NewSlide::GetSlideListener(){
	return listener;

}
// slide控件加载多幅图片控件，对图片进行滑动处理
// slide加载的图片子控件可以包括类似button...等孙子控件，方便灵活扩展
// Interval:slide控件间隔多少ms自行滑动到下一张图片，若为0则不定时滑动，最小值为100ms
// Rate:触屏Up后滑动的快慢。数值越大滑动越快。最小值为2
// Boundary:子控件间隔的像素点，中间用背景色填充，
// slideMode:  方向-->MOVELEFTRIGHT/MOVEUPDOWN  
//             循环-->MOVECYCLE  
//             在MOVECYCLE=0时，滑动到边缘时显示类似弹簧的效果-->MOVESPRING
NewSlide::NewSlide(int x, int y, int w, int h, int Interval, BYTE Rate, int Boundary, int slideMode):Image(x, y, w, h)
{
	memset(&prets, 0, sizeof(Touch_Event));
	prets_diff = 0;
	ts_diff_dec_count = 0;
	pSlide = NULL;
	listener = NULL;
	isScroll= false;
	ShowIndexMax = 0;
	indexDes =0;
	SlieItemHeightPos = 0;
	AutoSlideTimerID = -1;
	SlideTimerID = -1;
	SlideCurrent = 0;
	SlideSpeed = 0;
	SlideDes = 0;
	SlideInterval = Interval;
	if((SlideInterval != 0) && (SlideInterval < 100))
		SlideInterval = 100;
	SlideBoundary = Boundary;
	SlideOrient = slideMode&BIT0;
	//UILog("SlideOrient:%d\n",SlideOrient);
	SlideCycle = (slideMode&BIT1)>>1;
	SlideSpring = (slideMode&BIT2)>>2;
	if(SlideOrient == MOVELEFTRIGHT){
		SlideRate = Rate*w/LCDW;
		SetBmpFT(18,24,"gray");
}
	else{
		SetUpDownBmpFT(24,24,"white");
		
		SlideRate = Rate*h/LCDH;
}
	if(SlideRate <= 2)
		SlideRate = 2;
	SlideLength = 0;
	SlideMaxIndex = 0;
	pWidget->pMyCtlObject = this;
}

NewSlide::~NewSlide(void)
{
	NewSlideItem* pItem;
	NewSlideItem* pPreItem;
	if(pSlide != NULL)
	{
		TimerDel(&AutoSlideTimerID);
		TimerDel(&SlideTimerID);
		while(pSlide->NextSlideItem != NULL)
		{
			pItem = pSlide;
			pPreItem = pSlide;
			while(pItem->NextSlideItem != NULL)
			{
				pPreItem = pItem;
				pItem = pItem->NextSlideItem;
			}
			delete(pItem->pSlideItemImage);
			uifree((void*)pItem);
			pPreItem->NextSlideItem = NULL;
		}
		uifree((void*)pSlide);
	}
}

// slide控件循环时计算坐标是否超出边界
static int CycleCalc(int cyclemode, int currentnum, int length)
{
	if(cyclemode)
	{
		if(currentnum < 0)
			return currentnum-(currentnum/length)*length+length;
		else if(currentnum >= length)
			return currentnum-(currentnum/length)*length;
	}
	return currentnum;
}

// slide控件中某一子项控件中的子控件，即孙子控件的显示
static PIXEL* SlideSonCtlShow(NewSlideItem* pItem, Widget* pSlideWidget)
{
	PIXEL* pTemp;
	pTemp = CreateArea(pSlideWidget->width, pSlideWidget->height);
	AreaDraw(pTemp, 0, 0, pItem->pSlideItemImage->Bgimagebuff, 0, 0,
	         pSlideWidget->width, pSlideWidget->height, REPLACE_STYLE);
	pSlideWidget->pSonWidget->WidgetSetFocus(1);
	pSlideWidget->pSonWidget->WidgetSetDirty(1);
	pSlideWidget->pSonWidget->WidgetOnDraw(NULL, pTemp);
	return pTemp;
}

// slide控件滑动效果定时器启动函数
static int SlideStartTimer(void* pctl)
{
	int TimerID;
#if (LCDW>=800)			//	根据实际分辨率和效果调整
	TimerID = AddTimerAction(30, 0, SlideTimerCallback, pctl);
#else
	TimerID = AddTimerAction(10, 0, SlideTimerCallback, pctl);
#endif
	return TimerID;
}

// slide控件的自动滑动定时器callback函数
static void SlideAutoTimerCallback(void* pctl)
{
        printf("SlideAutoTimerCallback\n");
	NewSlide* pCtl;
	int i;
	pCtl = (NewSlide*)pctl;
	//i = pCtl->GetSlideShowItem();
	i =pCtl->indexDes;
	if(i < pCtl->SlideMaxIndex-pCtl->ShowIndexMax)
	{
		if(pCtl->SlideOrient == MOVELEFTRIGHT)
			pCtl->SlideDes = pCtl->GetSlideItemFromIndex(i)->xshowstart;
		else
			pCtl->SlideDes = pCtl->GetSlideItemFromIndex(i)->yshowstart;
		pCtl->SlideTimerID = SlideStartTimer(pctl);
		((CtlObject*)pCtl)->CtlRefresh();
	}
	else if(pCtl->SlideCycle == 0)
	{
		TimerDel(&(pCtl->AutoSlideTimerID));
	}
	/*else
	{
		if(pCtl->SlideOrient == MOVELEFTRIGHT)
			pCtl->SlideDes = pCtl->GetSlideItemFromIndex(0)->xshowstart;
		else
			pCtl->SlideDes = pCtl->GetSlideItemFromIndex(0)->yshowstart;
		pCtl->SlideTimerID = SlideStartTimer(pctl);
		((CtlObject*)pCtl)->CtlRefresh();
	}*/
}


// 取slide控件index对应的item项
NewSlideItem* NewSlide::GetSlideItemFromIndex(int ItemIndex)
{
	NewSlideItem* pItem;
	int i;
	if(pSlide == NULL)
	{
		ERROR_X("error! Slide have no item\n");
		return NULL;
	}
	if(ItemIndex >= SlideMaxIndex)
	{
		ERROR_X("error! Slide ItemIndex exceed MaxIndex\n");
		return NULL;
	}
	pItem = pSlide;
	for(i=0;i<ItemIndex;i++)
	{
		if(pItem->NextSlideItem == NULL)
		{
			ERROR_X("error! SlideItem Index:%d not found\n", ItemIndex);
			return NULL;
		}
		pItem=pItem->NextSlideItem;
	}
	return pItem;
}
	


// 取slide控件正在显示的item项的Index值
int NewSlide::GetSlideShowItem(void)
{
	NewSlideItem* pItem;
	int i;
	int result;
	int showindex;
	int temp;
	if(pSlide == NULL)
	{
		ERROR_X("error! Slide have no item\n");
		return NULL;
	}
	showindex = 0;
	result = SlideLength;
	if(SlideOrient == MOVEUPDOWN)
	result = abs(SlideLength);
	pItem = pSlide;
	for(i=0;i<SlideMaxIndex-ShowIndexMax;i++)
	{
		if(SlideOrient == MOVELEFTRIGHT)
			temp = GETABS(SlideCurrent-pItem->xshowstart);
		else
			temp = GETABS(SlideCurrent-pItem->yshowstart);
		//UILog("temp%d,SlideLength:%d\n",temp,result);
		if(result >= temp)
		{	//UILog("temp%d,SlideLength:%d\n",temp,SlideLength);
			result = temp;
			showindex = i;
		}
		pItem=pItem->NextSlideItem;
	}
	if(SlideOrient == MOVELEFTRIGHT){	
		if(showindex > SlideMaxIndex-5){
			showindex = SlideMaxIndex-5;
		}	
	}
	else{
		if(showindex < 2)
		showindex = 2;
	
	}
	return showindex;
}
// slide控件定时器callback函数
static void SlideTimerCallback(void* pctl)
{
	
	int i;
	int step;
	int slideeffect = 10;		//	滑动快慢效果调节
	int steparray[256];
	
	NewSlide* pCtl;
	NewSlideItem*pItem;
	
	pCtl = (NewSlide*)pctl;	

	if(pCtl->SlideOrient == MOVELEFTRIGHT){
		if(pCtl->SlideDes > pCtl->GetSlideItemFromIndex(pCtl->SlideMaxIndex - 5)->xshowstart)
              	pCtl->SlideDes = pCtl->GetSlideItemFromIndex(pCtl->SlideMaxIndex - 5)->xshowstart; 
		else if(pCtl->SlideDes < pCtl->GetSlideItemFromIndex(0)->xshowstart)
		pCtl->SlideDes = pCtl->GetSlideItemFromIndex(0)->xshowstart;
         }

	if((pCtl->SlideDes == pCtl->SlideCurrent) || (pCtl->SlideDes == pCtl->SlideCurrent-pCtl->SlideLength))
	{
		if(TimerDel(&(pCtl->SlideTimerID)) == 0)
			pCtl->SlideCurrent = pCtl->SlideDes;
		if((pCtl->AutoSlideTimerID == -1) && (pCtl->SlideInterval > 0))
		{
				
			if((pCtl->SlideCycle != 0) || (pCtl->GetSlideShowItem() < pCtl->SlideMaxIndex-1))
				pCtl->AutoSlideTimerID = AddTimerAction(pCtl->SlideInterval, 0, SlideAutoTimerCallback, pctl);
		}
		return;
	}
	memset(steparray, 0, sizeof(steparray)/sizeof(int));
	for(i=pCtl->SlideRate;i>0;i--)
		steparray[pCtl->SlideRate-i] = i;
	if((pCtl->SlideDes == 0) && (pCtl->SlideCurrent > pCtl->SlideLength/2))
		step = GETABS(pCtl->SlideDes+pCtl->SlideLength-pCtl->SlideCurrent);
	else
		step = GETABS(pCtl->SlideDes-pCtl->SlideCurrent);		
		
	for(i=1;i<pCtl->SlideRate+1;i++)
	{
		if(step >= steparray[i]*slideeffect)
		{
			i--;
			if((pCtl->SlideDes == 0) && (pCtl->SlideCurrent > pCtl->SlideLength/2))
				step = steparray[i];
			else if(pCtl->SlideDes >= pCtl->SlideCurrent)
				step = steparray[i];
			else
				step = 0-steparray[i];
			break;
		}
	}
	//UILog("pCtl->SlideCurrent%d,step%d\n",pCtl->SlideCurrent,step);
	pCtl->SlideCurrent = pCtl->SlideCurrent+step;	
	SlideListener listener = pCtl->GetSlideListener();
	if(listener != NULL){
		int index = pCtl->GetSlideShowItem();
		listener(index,NULL);
	}		
	((CtlObject*)pCtl)->CtlRefresh();
	
	
}

int NewSlide::SlideItemPos(double height){

		SlieItemHeightPos = height;
}
// slide控件加载子图片控件，只需要指定对应的图片即可
// 想象有一块虚拟的连续内存，保存有slide控件的多个子图片控件的图像，子图片控件在内存的位置为该控件的x、y、width、height
// 每个图片在slide显示出来，对应内存中显示的位置为xshowstart、yshowstart、widthshow、heightshow
// --------
// |     B| A-------
// | (1)  |  | (2) |  
// |      |  -------
// --------
// 例如显示窗口(2),窗口(2)在虚拟内存位置A为Image控件的x/y/width/height。
// 但窗口(2)需要显示居中，所以应从B点显示窗口(2)，将窗口(1)的一部分也显示了。B的位置为xshowstart、yshowstart、widthshow、heightshow
// 将各种位置先计算出来可以加速显示，简化计算
int NewSlide::AddSlideItem(const char* pPic,int width,int  height)
{
    	
	Text* pImage;
	Text* pTemp;
	NewSlideItem* pItem;
	int length;
	int offset;
	textTempWidth = width;
	textTempHeight = height;
	if((width > pWidget->width) || (height > pWidget->height))
	{
		ERROR_X("error! SlideItem Width/Height exceed Slide Width/Height\n");
		return -1;
	}
	offset = 0;	
	if(pSlide != NULL)
		offset = SlideBoundary;
	if((SlideCycle != 0) && (SlideLength > 0))
		SlideLength = SlideLength-SlideBoundary;
	if(SlideOrient == MOVELEFTRIGHT){	
		tempHeight = (pWidget->height-height)/SlieItemHeightPos;
		pImage = new Text(SlideLength+offset,tempHeight,width,height,0,pPic);
		pTemp =  new Text(SlideLength+offset-5,tempHeight-6,20,30,0,pPic);	
		
   	 for (int i = 0; i<sizeof (pic_text0_9)/sizeof (unsigned char*); i++)
       	 	MulImageLoadPic(pImage->pTextPic, pic_text0_9[i],0,0,'0'+ i);
		MulImageLoadPic(pImage->pTextPic, pic_punctuation[5],0,0,' ');
			//UILog("pItem->xshowstart:%d\n",pItem->xshowstart);
	 		for (int i = 0; i<sizeof (mid_text0_9)/sizeof (unsigned char*); i++)
        		MulImageLoadPic(pTemp->pTextPic, mid_text0_9[i], 0, 0, '0' + i);
			MulImageLoadPic(pTemp->pTextPic, mid_punctuation[5], 0, 0, ' ');
		
		}
	
	else{
		if(pSlide == NULL){SlideLength = 88; offset = SlideBoundary;};
		textTempWidth = (pWidget->width-width)/2;
		pImage = new Text((pWidget->width-width)/2,SlideLength+offset, width, height,0,pPic);	
		int value = atoi(pPic);
		if(value > 100)	
		 pTemp =  new Text((pWidget->width-46)/2,SlideLength+offset-4,46,24,0,pPic);
		else	
		pTemp =  new Text((pWidget->width-24)/2,SlideLength+offset-4,24,24,0,pPic);
					
    for (int i = 0; i<sizeof (pic_text0_9)/sizeof (unsigned char*); i++)
        MulImageLoadPic(pImage->pTextPic, pic_text0_9[i],0,0,'0' + i);
	MulImageLoadPic(pImage->pTextPic, pic_punctuation[5],0,0,' ');

	 		for (int i = 0; i<sizeof (pic_text_up_down)/sizeof (unsigned char*); i++)
        		MulImageLoadPic(pTemp->pTextPic, pic_text_up_down[i], 0, 0, '0' + i);
			MulImageLoadPic(pTemp->pTextPic, mid_punctuation[5], 0, 0, ' ');
}
	
	if(pSlide == NULL)
	{
		pSlide = (NewSlideItem*)uimalloc(sizeof(NewSlideItem));
		pSlide->SlideIndex = 0;
		pSlide->pSlideItemImage = pImage;
		pSlide->pTempText = pTemp;
		pSlide->PreSlideItem = NULL;
		pSlide->NextSlideItem = NULL;
		pItem = pSlide;
	}
	else
	{
		pItem = pSlide;
		while(pItem->NextSlideItem != NULL)
			pItem = pItem->NextSlideItem;
		pItem->NextSlideItem = (NewSlideItem*)uimalloc(sizeof(NewSlideItem));
		pItem = pItem->NextSlideItem;
		pItem->SlideIndex = SlideMaxIndex;
		pItem->pSlideItemImage = pImage;
		pItem->pTempText = pTemp;
		pItem->PreSlideItem = pItem;
		pItem->NextSlideItem = NULL;
	}
	if(SlideOrient == MOVELEFTRIGHT)
	{
		pItem->xshowstart = SlideLength+offset;//+width/2-pWidget->width/2;
		pItem->yshowstart = 0;
		length = width+offset;
	}
	else
	{	pItem->ItemWidth = width;
		pItem->xshowstart = (pWidget->width-width)/2;
		pItem->yshowstart = SlideLength+offset;//+height/2-pWidget->height/2;		
		length = height+offset;
	//UILog("pItem->yshowstart:%d, SlideLength:%d,width:%d pWidget->width_diff:%d\n",pItem->yshowstart,SlideLength,pWidget->width, prets_diff);
	}
	pItem->widthshow = pWidget->width;
	pItem->heightshow = pWidget->height;
if(SlideOrient == MOVELEFTRIGHT){
	SlideLength = SlideLength+length;
}else{
	SlideLength = SlideLength-length;
	//UILog("SlideLength:%d,length%d\n",SlideLength,length);
}
	
	if(SlideCycle != 0)
		SlideLength = SlideLength+SlideBoundary;
	SlideMaxIndex++;
	//printf("%d --->x:%d,y:%d,w:%d,h:%d\n", SlideMaxIndex, pItem->xshowstart, pItem->yshowstart, pItem->widthshow, pItem->heightshow);
	return 0;
}


// slide控件index对应的item项增加子控件
int NewSlide::SlideItemAddChild(int ItemIndex, CtlObject* pCtl)
{
	NewSlideItem* pItem;
	pItem = GetSlideItemFromIndex(ItemIndex);
	if(pItem != NULL)
	{
		pItem->pSlideItemImage->BindChild(pCtl);
		return 0;
	}
	return -1;
}

// slide控件设置是否自动滑动，Interval为0时不自动滑动
int NewSlide::SlideSetInterval(int Interval)
{
	SlideInterval = Interval;
	if((SlideInterval != 0) && (SlideInterval < 100))
		SlideInterval = 100;	
	TimerDel(&AutoSlideTimerID);
	if(SlideInterval > 0)
		AutoSlideTimerID = AddTimerAction(SlideInterval, 0, SlideAutoTimerCallback, (void*)this);
	return 0;
}

//ryze  test
void NewSlide::SetSlideDes(int index){
	indexDes = index;
	//SlideTimerID = SlideStartTimer(pctl);
	SlideTimerID = SlideStartTimer((void*)this);
	//SlideSetInterval(3);
}
int NewSlide::CtlFocus(int focus)
{
	if(focus)
	{
		if((AutoSlideTimerID == -1) && (SlideInterval > 0))
			AutoSlideTimerID = AddTimerAction(SlideInterval, 0, SlideAutoTimerCallback, (void*)this);
	}
	else
	{
		TimerDel(&AutoSlideTimerID);
		TimerDel(&SlideTimerID);
	}
	return 0;
}



int NewSlide::CtlEvent(Event* pEvent)
{	int lenght;
	int i,j;
	int spring;
	NewSlideItem* pItem;
	Widget* pSlideWidget;
	Touch_Event tsevent;
	int length;
	int prelength;
	Text*newText_2_left;
	Text*newText_1_left;
	Text*textItem_1_left;
	Text*textItem_2_left;
	Text*newText_2_up;
	Text*newText_1_up;
	Text*textItem_1_up;
	Text*textItem_2_up;
	
	int springeffect = 160;			//	弹簧效果阻尼的大小
	Any_Event *pany_event = NULL;
	pany_event = &(pEvent->uEvent.any_event);

	
	
	if(isTSMsg(pany_event->event_type))
	{	
		TimerDel(&AutoSlideTimerID);
		if(pany_event->event_type == MsgTypeMouseDown)
		{
			if(TimerDel(&SlideTimerID) == 0)
			{
			}
			else
			{
				pItem = GetSlideItemFromIndex(GetSlideShowItem());
				pSlideWidget = pItem->pSlideItemImage->pWidget;
			
			}
			SlideSpeed = 0;
			prets_diff = 0;
			ts_diff_dec_count = 0;
			//printf("*** down ***\n");
		}
		else if(pany_event->event_type == MsgTypeMouseMove)
		{
			if(SlideOrient == MOVELEFTRIGHT)
			{
				i = prets.x-pEvent->uEvent.ts_event.x;				
				if(GETABS(prets.y-pEvent->uEvent.ts_event.y) > GETABS(i))
				{
					memcpy(&prets, &(pEvent->uEvent.ts_event), sizeof(Touch_Event));
					return 0;
				}
				spring = pWidget->width;
			}
			else
			{
				i = prets.y-pEvent->uEvent.ts_event.y;
				if(GETABS(prets.x-pEvent->uEvent.ts_event.x) > GETABS(i))
				{
					memcpy(&prets, &(pEvent->uEvent.ts_event), sizeof(Touch_Event));
					return 0;
				}
				spring = -pWidget->height;
			}
			if(GETABS(i) > GETABS(prets_diff))
			{
				ts_diff_dec_count = 0;
				SlideSpeed = i;
			}
			else
			{
				ts_diff_dec_count++;
				if(ts_diff_dec_count > 1)
					prets_diff = 0;
			}
			prets_diff = i;						
			SlideCurrent = SlideCurrent+i;

			
			CtlRefresh();
		}		
		else if(pany_event->event_type == MsgTypeMouseUp)
		{
				
		

 			if(SlideOrient == MOVELEFTRIGHT)
			{	
				
					
					if(GetSlideShowItem() == SlideMaxIndex - 5)
						SlideDes = GetSlideItemFromIndex(SlideMaxIndex - 5)->xshowstart;
					if(GetSlideShowItem() == 0)
						SlideDes = GetSlideItemFromIndex(0)->xshowstart;
					else if(prets_diff > 5)
					 SlideDes = GetSlideItemFromIndex(GetSlideShowItem()+4)->xshowstart;
					else if(prets_diff < -5)
					SlideDes = GetSlideItemFromIndex(GetSlideShowItem()-4)->xshowstart;
					else
					SlideDes = GetSlideItemFromIndex(GetSlideShowItem())->xshowstart;
					// UILog("GetSlideShowItem%d,SlideMacIndex%d\n",GetSlideShowItem(),SlideMaxIndex);			
				
			}
			else{		
					
					if(GetSlideShowItem() == SlideMaxIndex-ShowIndexMax)
						SlideDes = GetSlideItemFromIndex(SlideMaxIndex-ShowIndexMax)->yshowstart;
					if(GetSlideShowItem() == 2)
						SlideDes = GetSlideItemFromIndex(2)->yshowstart;
					else if(prets_diff > 5 && GetSlideShowItem()-4 > 2)
					 SlideDes = GetSlideItemFromIndex(GetSlideShowItem()-4)->yshowstart;
					else if(prets_diff < -5 && GetSlideShowItem()+4 < SlideMaxIndex-ShowIndexMax)
					SlideDes = GetSlideItemFromIndex(GetSlideShowItem()+4)->yshowstart;
					else
					SlideDes = GetSlideItemFromIndex(GetSlideShowItem())->yshowstart;	

				//UILog("GetSlideShowItem():%d,SlideDes:%d\n",GetSlideShowItem(),SlideDes);
			}
			SlideTimerID = SlideStartTimer((void*)this);
			//UILog("up SlideCurrent:%d, SlideDes:%d\n", SlideCurrent, SlideDes);
		}
		memcpy(&prets, &(pEvent->uEvent.ts_event), sizeof(Touch_Event));
	}
	return 0;
}

int NewSlide::CtlShow(PIXEL* pDesBuf)
{	
	
	int i,j;
	NewSlideItem* pItem;
	Widget* pSlideWidget;
	Widget*pTextWidget;
	PIXEL* pTemp;
	volatile char *p;
	MulImagePic* pPic;
	int offset;
	bool nextItem;
	int itemX;
	int itemY;
	int textX;
	int textY;
	if(Bgimagebuff != NULL)
	{
		AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, Bgimagebuff, 0, 0,
				 DIFF(BgimageWidth,BgimageOffsetX,pWidget->width), DIFF(BgimageHeight,BgimageOffsetY,pWidget->height), BltMode);
	}
	else if(BgColor != TRANSCOLOR)
	{
		AreaFill(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, pWidget->width-BgimageOffsetX, pWidget->height-BgimageOffsetY, BgColor);
	}	

	for(i=0;i<SlideMaxIndex-ShowIndexMax;i++)
	{
		pItem = GetSlideItemFromIndex(i);
		pSlideWidget = pItem->pSlideItemImage->pWidget;
		//slide left right
		if(SlideOrient == MOVELEFTRIGHT)
		{	//UILog("newslide moverightleft\n");
			if(pSlideWidget->x > SlideCurrent+pItem->widthshow)
				continue;
			if(pSlideWidget->x+pSlideWidget->width < SlideCurrent)
				continue;
			if(pItem->pSlideItemImage->pTextStr != NULL){	
					
						offset = 0;
						if(i == GetSlideShowItem()+2){
						pTextWidget = pItem->pTempText->pWidget;
						p = pItem->pTempText->pTextStr;
						}
						else{
						pTextWidget = pItem->pSlideItemImage->pWidget;
						p = pItem->pSlideItemImage->pTextStr;
						}
						while(*p != 0){
							if(i == GetSlideShowItem()+2)
							pPic = GetMulImageIndex(pItem->pTempText->pTextPic, (int)*p);
							else
							pPic = GetMulImageIndex(pItem->pSlideItemImage->pTextPic, (int)*p);	
							if(pPic != NULL)
							{
						if(pPic->MulImagebuff != NULL)
						{	
					if(offset < pTextWidget->width-(BgimageOffsetX+pPic->MulImageOffsetX))
						{	
					
			itemX =  pWidget->x+BgimageOffsetX+pTextWidget->x-SlideCurrent+offset;//+pTextWidget->x+BgimageOffsetX+pPic->MulImageOffsetX;	
			//UILog("itemX%d\n",itemX);
			itemY =pWidget->y+BgimageOffsetY+pTextWidget->y;
			textX =GETMIN(SlideCurrent+pItem->widthshow-(pTextWidget->x+offset), pTextWidget->width);
			textY =  pTextWidget->height;
					
			//UILog("pSlideWidget->x:%d,SlideCurrent%d\n",pSlideWidget->x ,SlideCurrent);				
			if(pSlideWidget->x < SlideCurrent)
			{	
				AreaDraw(pDesBuf,pWidget->x+BgimageOffsetX, itemY, 
					pPic->MulImagebuff, SlideCurrent-(pTextWidget->x+offset),0, 
					pTextWidget->width-SlideCurrent+pTextWidget->x+offset, textY, BltMode);	
			}
			else
			{
				j = pWidget->x+BgimageOffsetX+pTextWidget->x+offset-SlideCurrent;
				if(INRANGE(j, pWidget->x, pWidget->x+pWidget->width))
				{	
					AreaDraw(pDesBuf,itemX, itemY, pPic->MulImagebuff, 0, 0,textX, textY, BltMode);	
				}
			}
				offset = offset+DIFF(pPic->MulImageWidth,BgimageOffsetX+pPic->MulImageOffsetX,pTextWidget->width)+pItem->pSlideItemImage->TextBoundary;
					}
					p++;
					
				}
				
			}
			else
			{
				ERROR_X("error! Text char:%c not found!\n", *p);
			}
		}
 			
	}
}
			
		else
		{	
			//UILog("new slide move up and down");
			if(pSlideWidget->y > SlideCurrent+pItem->heightshow)
				continue;
			if(pSlideWidget->y+pSlideWidget->height < SlideCurrent)
				continue;
			if(pItem->pSlideItemImage->pTextStr != NULL){
						offset = 0;
						if(i == GetSlideShowItem()-1){
						pTextWidget = pItem->pTempText->pWidget;
						p = pItem->pTempText->pTextStr;
						}
						else{
						pTextWidget = pItem->pSlideItemImage->pWidget;
						p = pItem->pSlideItemImage->pTextStr;
						}
			while(*p != 0){
				if(i == GetSlideShowItem()-1)
				pPic = GetMulImageIndex(pItem->pTempText->pTextPic, (int)*p);
				else
				pPic = GetMulImageIndex(pItem->pSlideItemImage->pTextPic, (int)*p);
				
			if(pPic != NULL)
							{
						if(pPic->MulImagebuff != NULL)
						{	
					if(offset < pTextWidget->width-(BgimageOffsetX+pPic->MulImageOffsetX)){	
			//UILog("pSlideWidget->y:%d,SlideCurrent%d\n",pSlideWidget->y ,SlideCurrent);
			if(pTextWidget->y < SlideCurrent)
			{	//UILog("way  one===========\n");
				AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX+pTextWidget->x+offset,pWidget->y+BgimageOffsetY, 
				         pPic->MulImagebuff,0,SlideCurrent-pTextWidget->y,
				         pTextWidget->width, pTextWidget->height-SlideCurrent+pTextWidget->y, BltMode);	
			}else
			{	//UILog("way  two*************\n");
				j = pWidget->y+BgimageOffsetY+pTextWidget->y-SlideCurrent;
				if(INRANGE(j, pWidget->y, pWidget->y+pWidget->height))
				{	AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX+pTextWidget->x+offset, j, 
					         pPic->MulImagebuff, 0,0, 
					         pTextWidget->width, GETMIN(SlideCurrent+pItem->heightshow-pTextWidget->y, 							pTextWidget->height), BltMode);
				}
			}
				offset = offset+DIFF(pPic->MulImageWidth,BgimageOffsetX+pPic->MulImageOffsetX,pTextWidget->width)	+pItem->pSlideItemImage->TextBoundary;
									
				}
				p++;
				}
			}else
			{
				ERROR_X("error! Text char:%c not found!\n", *p);
			}
				}
				

		}
	
			
		}
	}
	return 0;
}



int NewSlide::SlideRefresh(int showIndexMax){

	ShowIndexMax = showIndexMax;
	//UILog("showIndexMax%d",showIndexMax);	
	CtlRefresh();

}
int NewSlide::changeSlideItem(const char* tx,int index){
        
		 NewSlideItem*pItem = GetSlideItemFromIndex(index);
		pItem->pSlideItemImage->SetTextStr(" ");
		pItem->pTempText->SetTextStr(" ");
		pItem->pSlideItemImage->SetTextStr(tx);
		pItem->pTempText->SetTextStr(tx);		
	
 	CtlRefresh();
}



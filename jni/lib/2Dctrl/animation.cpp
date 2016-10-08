#include "animation.h"
#include "draw.h"

// gif控件每隔gifinterval毫秒，定时刷新图片
Animation::Animation(int x, int y, int w, int h, int interval): Image(x, y, w, h)
{
    TimerID = -1;
    Interval = interval;
    pWidget->pMyCtlObject = this;
}

Animation::~Animation(void)
{
    free(aniPic);
    TimerDel(&TimerID);
}

static void AnimationTimerCallback(void *pctl)
{
    ((Animation *)pctl)->refreshAniPic();
    ((CtlObject *)pctl)->CtlRefresh();
}

int Animation::CtlFocus(int focus)
{
    return 0;
}

void Animation::Start()
{
    clock = 0;
    TimerID = AddTimerAction(Interval, 0, AnimationTimerCallback, (void *)this);
}

void Animation::refreshAniPic()
{
    Bgimagebuff = (PIXEL*)aniPic[clock];
    clock ++;

    if (clock >= num)
        clock = 0;
}

void Animation::LoadAniPic(unsigned char **pic, int number)
{
    num = number;
    int i = 0;
    int width, height;
    int picID;
    aniPic = (PIXEL**)malloc(number * sizeof(PIXEL));
	PIXEL* tmp;

    while (i < number) {
        GetBMP_wid(pic[i], &BgimageWidth, &BgimageHeight, &tmp, &picID);
		aniPic[i] = tmp;
        i ++;
    }

	Bgimagebuff = (PIXEL*)aniPic[0];
}

void Animation::Stop()
{
    TimerDel(&TimerID);

    if (stopCb != NULL) {
        stopCb(this);
        CtlRefresh();
    }
}

void Animation::SetStopCallback(StopCallback cb)
{
    stopCb = cb;
}

int Animation::CtlEvent(Event *pEvent)
{
    return 0;
}

int Animation::CtlShow(PIXEL *pDesBuf)
{
    if (Bgimagebuff != NULL) {
        AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX, pWidget->y + BgimageOffsetY, Bgimagebuff, 0, 0,
                 DIFF(BgimageWidth, BgimageOffsetX, pWidget->width), DIFF(BgimageHeight, BgimageOffsetY, pWidget->height), BltMode);
    }
	else if(BgColor != TRANSCOLOR)
	{
		AreaFill(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, pWidget->width-BgimageOffsetX, pWidget->height-BgimageOffsetY, BgColor);
	}

    return 0;
}

#include "ctlobject.h"
#include "draw.h"

// CtlObject控件定义了所有控件的基本类型
CtlObject::CtlObject(int x, int y, int w, int h)
{
	pWidget = new Widget();
	pWidget->WidgetSetPosition(x, y);
	pWidget->WidgetSetSize(w, h);
	pWidget->pMyCtlObject = this;
}

CtlObject::~CtlObject(void)
{
	if(pWidget != NULL)
		delete(pWidget);
}

// 控件的callback函数，如果控件没有调用该函数则callback函数不执行
int CtlObject::CtlCallback(int (*pCallback)(Event* pEvent))
{
	if(pCallback != NULL) {
		pWidget->WidgetSetCallback(pCallback);
	}
	return 0;
}

// 控件变化后更新该控件的显示
int CtlObject::CtlRefresh(void)
{
	//CheckDrawThread();
	pWidget->WidgetSetDirty(1);
	SetDrawThread(DRAWTHREADONE, EFFECT_NONE);
	return 0;
}

// 当界面将控件置为焦点并且显示时将调用控件的该函数，该函数提供给控件的是否为焦点的信息
int CtlObject::CtlFocus(int focus)
{
	return 0;
}

// 当界面的触摸事件在控件的窗口内部时，将调用控件的该函数
int CtlObject::CtlEvent(Event* pEvent)
{
	return 0;
}

// 界面重新绘制时，判断控件窗口如果为dirty，如果是将调用控件的该函数
int CtlObject::CtlShow(PIXEL* pDesBuf)
{
	return 0;
}

// 控件的窗口注册子控件，子控件的窗口必须在本控件窗口的内部
// 绘图时将先绘制本控件，再绘制子控件。
// 如果触摸事件落在子控件中，将回调子控件的callback函数
void CtlObject::AddChild(CtlObject* pSon)
{
	pWidget->WidgetAddChild(pSon->pWidget, 1);
}

// 控件的窗口绑定子控件，子控件的窗口位置必须由父控件自己计算控制，其他和AddChild一样
// 常用于需要子窗口和父窗口同时滑动的情况
void CtlObject::BindChild(CtlObject* pSon)
{
	pWidget->WidgetAddChild(pSon->pWidget, 0);
}



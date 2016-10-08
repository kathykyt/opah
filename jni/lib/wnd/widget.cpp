#include "widget.h"
#include "memorymanage.h"
#include "flagdefine.h"
#include "log.h"

Widget::Widget(void)
{
	pMyWidget = this;
	pMyCtlObject = NULL;
	x = y = width = height = 0;
	z = 0;
	focus = 0;
	usingClutter = 0;
	pWidgetName = NULL;
	loadbmpflag = 0;
	dirty = 1;
	pPrivateData = NULL;
	pParentWidget = NULL;
	pSiblingWidget = NULL;
	pSonWidget = NULL;
	pCallback = NULL;
}

Widget::~Widget(void)
{
	if(pWidgetName != NULL)
		uifree((void*)pWidgetName);
	if(pPrivateData != NULL)
		uifree(pPrivateData);
}

// 对窗口及其子窗口摧毁
void Widget::WidgetDel(void)
{
	Widget* pWidget;
	Widget* pSibling;
	Widget* pTemp;
	if(pMyWidget == NULL)
	{
		ERROR_X("error! WidgetDel NULL Widget\n");
		return;
	}	
	while(pSonWidget != NULL)
	{
		pWidget = pSonWidget;
		do
		{
			while(pWidget->pSiblingWidget != NULL)
				pWidget = pWidget->pSiblingWidget;
			while(pWidget->pSonWidget != NULL)
				pWidget = pWidget->pSonWidget;
		}while(pWidget->pSiblingWidget != NULL);
		pSibling = pWidget->pParentWidget->pSonWidget;
		if(pSibling != pWidget)
		{
			do
			{
				pTemp = pSibling;
				pSibling = pSibling->pSiblingWidget;
			}while(pSibling != pWidget);
			pTemp->pSiblingWidget = NULL;
		}
		else
		{
			pWidget->pParentWidget->pSonWidget = NULL;
		}
		delete(pSibling);
	}
	if(pSiblingWidget != NULL)
	{
		pSibling = pParentWidget->pSonWidget;
		do
		{
			pTemp = pSibling;
			pSibling = pSibling->pSiblingWidget;
		}while(pSibling != pMyWidget);
		pTemp->pSiblingWidget = pSiblingWidget;
	}
	delete(pMyWidget);
}

Widget* WidgetGetSonSiblingWidgetFromName(Widget* pParent, const char* pName)
{
	Widget* pResult;
	if((pParent->pWidgetName != NULL) && (strcmp(pParent->pWidgetName, pName) == 0)) {
		return pParent->pMyWidget;
	}
	if(pParent->pSonWidget != NULL)
	{
		pResult = WidgetGetSonSiblingWidgetFromName(pParent->pSonWidget, pName);
		if(pResult != NULL)
			return pResult;
	}
	if(pParent->pSiblingWidget != NULL)
	{
		pResult = WidgetGetSonSiblingWidgetFromName(pParent->pSiblingWidget, pName);
		if(pResult != NULL)
			return pResult;
	}
	return NULL;
}

// 根据窗口的名字检索窗口
Widget* Widget::WidgetGetSonWidgetFromName(const char* pName)
{
	if((pWidgetName != NULL) && (strcmp(pWidgetName, pName) == 0)) {
		return pMyWidget;
	}
	if(pMyWidget->pSonWidget != NULL) {
		return WidgetGetSonSiblingWidgetFromName(pMyWidget->pSonWidget, pName);
	} else {
		return NULL;
	}
}

// 设置窗口的名字
void Widget::WidgetSetName(const char* pName)
{
	if(pName != NULL)
	{
		pWidgetName = (char*)uimalloc(strlen(pName) + 1);	
		strcpy((char*)pWidgetName, pName);
		*(char *)(pWidgetName + strlen(pWidgetName) + 1) = 0;
	}
}

// 设置窗口的大小
void Widget::WidgetSetSize(int widgetw, int widgeth)
{
	width = widgetw;
	height = widgeth;
}

// 设置窗口的位置
void Widget::WidgetSetPosition(int wingetx, int widgety)
{
	x = wingetx;
	y = widgety;
}

// 设置窗口的z序列
void Widget::WidgetSetZ(int wingetz)
{
	z = wingetz;
}

void WidgetSetSonSiblingFocus(Widget* pParent, int wingetfocus)
{
	pParent->focus = wingetfocus;
	pParent->pMyCtlObject->CtlFocus(wingetfocus);
	if(pParent->pSonWidget != NULL)
		WidgetSetSonSiblingFocus(pParent->pSonWidget, wingetfocus);
	if(pParent->pSiblingWidget != NULL)
		WidgetSetSonSiblingFocus(pParent->pSiblingWidget, wingetfocus);
}

// 设置窗口及其子窗口的焦点
void Widget::WidgetSetFocus(int wingetfocus)
{
	Widget* pSon;
	Widget* pSibling;	
	focus = wingetfocus;
	pMyCtlObject->CtlFocus(wingetfocus);
	if(pSonWidget != NULL)
		WidgetSetSonSiblingFocus(pSonWidget, wingetfocus);
}

void WidgetSetSonSiblingDirty(Widget* pParent, int wingetdirty)
{
	pParent->dirty = wingetdirty;
	if(pParent->pSonWidget != NULL)
		WidgetSetSonSiblingDirty(pParent->pSonWidget, wingetdirty);
	if(pParent->pSiblingWidget != NULL)
		WidgetSetSonSiblingDirty(pParent->pSiblingWidget, wingetdirty);
}

// 设置窗口及其子窗口的dirty，设置dirty的窗口将重新绘制
void Widget::WidgetSetDirty(int wingetdirty)
{
	Widget* pSon;
	Widget* pSibling;	
	dirty = wingetdirty;
	if(pSonWidget != NULL)
		WidgetSetSonSiblingDirty(pSonWidget, wingetdirty);
}

void WidgetSetSonSiblingLoadBmpFlag(Widget* pParent, int wingetloadbmp)
{
	pParent->loadbmpflag = wingetloadbmp;
	if(pParent->pSonWidget != NULL)
		WidgetSetSonSiblingLoadBmpFlag(pParent->pSonWidget, wingetloadbmp);
	if(pParent->pSiblingWidget != NULL)
		WidgetSetSonSiblingLoadBmpFlag(pParent->pSiblingWidget, wingetloadbmp);
}

// 设置窗口及其子窗口的图片加载完成的标志
void Widget::WidgetSetLoadBmpFlag(int wingetloadbmp)
{
	Widget* pSon;
	Widget* pSibling;	
	loadbmpflag = wingetloadbmp;
	if(pSonWidget != NULL)
		WidgetSetSonSiblingLoadBmpFlag(pSonWidget, wingetloadbmp);
}

void Widget::WidgetSetUsingClutter(int widgetusingclutter)
{
	usingClutter = widgetusingclutter;
}

// 设置窗口的私有数据
void Widget::WidgetSetPrivateData(void* pwidgetPrivateData, int PrivateDataSize)
{
	pPrivateData = (void*)uimalloc(PrivateDataSize);
	memcpy(pPrivateData, pwidgetPrivateData, PrivateDataSize);
}

void Widget::WidgetSetCallback(int (*callback)(Event* pEvent))
{
	pCallback = callback;
}



// 窗口增加子窗口
void Widget::WidgetAddChild(Widget* pChild, int Mode)
{
	Widget* pSon;
	Widget* pSibling;
	Widget* pTemp;
	if(pChild == NULL)
	{
		ERROR_X("error! Widget Add NULL Widget\n");
		return;
	}	
// 子窗口xy值增加父窗口的偏移值，后续调整
	if(Mode)
	{
		pChild->x = pChild->x+x;
		pChild->y = pChild->y+y;
		if((INRANGE(pChild->x, x, x+width) == 0) || (INRANGE(pChild->y, y, y+height) == 0))
		{
			ERROR_X("error! WidgetAddChild x,y out of range\n");
			return;
		}		
		if((INRANGE(pChild->x+pChild->width, x, x+width) == 0) || (INRANGE(pChild->y+pChild->height, y, y+height) == 0))
		{
			ERROR_X("error! WidgetAddChild width,height out of range\n");
			return;
		}		
	}
	if(pSonWidget == NULL)
	{
		pSonWidget = pChild;
		pChild->pParentWidget = pMyWidget;
	}
	else
	{
		pSon = pSonWidget;
		if(pSon->pSiblingWidget == NULL)
		{
			pSon->pSiblingWidget = pChild;
			pChild->pParentWidget = pMyWidget;
		}
		else
		{
			pSibling = pSon->pSiblingWidget;
			do
			{
				pTemp = pSibling;
				pSibling = pSibling->pSiblingWidget;
			}while(pSibling != NULL);
			pTemp->pSiblingWidget = pChild;
			pChild->pParentWidget = pMyWidget;
		}
	}
}

Widget* WidgetTSFocusSonSibling(Widget* pParent, Touch_Event* pTS)
{
	Widget* pFocus = NULL;
	if((pParent->focus > 0) && (INRANGE(pTS->x, pParent->x, pParent->x+pParent->width) == 1) && (INRANGE(pTS->y, pParent->y, pParent->y+pParent->height) == 1))
	{
		if(pParent->pSonWidget != NULL)
		{
			pFocus = WidgetTSFocusSonSibling(pParent->pSonWidget, pTS);
			if(pFocus != NULL)
				return pFocus;
		}
		return pParent;
	}
	else
	{
		if(pParent->pSiblingWidget != NULL)
		{
			pFocus = WidgetTSFocusSonSibling(pParent->pSiblingWidget, pTS);
			if(pFocus != NULL)
				return pFocus;	
		}
		return NULL;
	}
}

// 设置窗口的焦点
Widget* Widget::WidgetTSFocus(Touch_Event* pTS)
{
	return WidgetTSFocusSonSibling(this, pTS);
}

int WidgetOnDrawSonSibling(Widget* pParent, int z, WidgetRefresh* pDraw, PIXEL* pBuf)
{
	//printf("WidgetOnDrawSonSibling\n");
	int zmax = 0;
	int ztemp;
	if(pParent->focus > 0)
	{
		if((pParent->z == z) && (pParent->usingClutter == 0))
		{
			if(pParent->dirty)
			{
				pParent->dirty = 0;
				pParent->pMyCtlObject->CtlShow(pBuf);
				if(pDraw != NULL)
				{
					pDraw->WidgetRefreshNum++;
					pDraw->pWidgetRefresh = pParent;
				}
			}
		}
		if(zmax < pParent->z)
			zmax = pParent->z;
	}
	if(pParent->pSonWidget != NULL)
	{
		ztemp = WidgetOnDrawSonSibling(pParent->pSonWidget, z, pDraw, pBuf);
		if(zmax < ztemp)
			zmax = ztemp;
	}
	if(pParent->pSiblingWidget != NULL)
	{
		ztemp = WidgetOnDrawSonSibling(pParent->pSiblingWidget, z, pDraw, pBuf);
		if(zmax < ztemp)
			zmax = ztemp;
	}

	//printf("WidgetOnDrawSonSibling end\n");
	return zmax;
}

// 绘制窗口及其子窗口
int Widget::WidgetOnDraw(WidgetRefresh* pDraw, PIXEL* pBuf)
{
	int zmax = 0;
	int ztemp;
	int i;
	if(focus > 0)
	{
		for(i=ZMIN; i<=zmax; i++)
		{
			if((i == z) && (usingClutter == 0))
			{
				if(dirty)
				{
					dirty = 0;
					pMyCtlObject->CtlShow(pBuf);
					if(pDraw != NULL)
					{
						pDraw->WidgetRefreshNum++;
						pDraw->pWidgetRefresh = pMyWidget;
					}
				}
			}
			if(zmax < z)
				zmax = z;
			if(pSonWidget != NULL)
			{
				ztemp = WidgetOnDrawSonSibling(pSonWidget, i, pDraw, pBuf);
				if(zmax < ztemp)
					zmax = ztemp;
			}			
		}
	}
	return 0;
}

void DepthPrintf(int depth)
{
	int i;
	for(i=0; i<depth; i++)
		printf("   ");
}

void WidgetSelfPrintf(Widget* pWidget, int depth)
{
	DepthPrintf(depth);
	if(pWidget->pWidgetName != NULL)
		printf("***WidgetName:%s***\n", pWidget->pWidgetName);
	else
		printf("***WidgetName:NULL***\n");
	DepthPrintf(depth);
	printf("x:%d, y:%d, width:%d, height:%d\n", pWidget->x, pWidget->y, pWidget->width, pWidget->height);
	DepthPrintf(depth);
	printf("z:%d, focus:%d, usingClutter:%d\n", pWidget->z, pWidget->focus, pWidget->usingClutter);
	DepthPrintf(depth);
	printf("dirty:%d, loadbmpflag:%d\n", pWidget->dirty, pWidget->loadbmpflag);
	printf("\n");
}

void WidgetSonSiblingPrintf(Widget* pParent, int depth)
{
	WidgetSelfPrintf(pParent, depth);
	if(pParent->pSonWidget != NULL)
		WidgetSonSiblingPrintf(pParent->pSonWidget, depth+1);
	if(pParent->pSiblingWidget != NULL)
		WidgetSonSiblingPrintf(pParent->pSiblingWidget, depth);
}

// 打印窗口
void Widget::WidgetPrintf(int depth)
{
	WidgetSelfPrintf(this, depth);
	if(pSonWidget != NULL)
		WidgetSonSiblingPrintf(pSonWidget, depth+1);
}


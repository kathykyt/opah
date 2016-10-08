#include "interface2.h"
#include "BmpDataArray.h"

static CtlRoot* pRoot;
static Image* pBg;
static Button* pButton;

static int Button2Callback(Event* pEvent)
{
	Any_Event *pany_event = NULL;
	printf("Button2Callback\n");
	pany_event = &(pEvent->uEvent.any_event);
	if(pany_event->event_type == MsgTypeMouseUp)	
	{
		//button2
		//pRoot->CtlRootChangeInterface("Interface1", EFFECT_RIGHTPUSHTOLEFT);
		pRoot->CtlRootChangeInterface("Interface1", EFFECT_NONE);
	}
	return 0;
}

static void LoadInterface2Bmp(void)
{
	pBg->LoadImagePic(pic_tips_tips_2);
	pButton->LoadButtonPic(pic_tips_btn_home_big_png, pic_tips_btn_home_big_png);
	pButton->SetImageBltMode(ALPHA_STYLE);
	pButton->CtlCallback(Button2Callback);
}

void RegisterInterface2(CtlRoot* proot, const char* pInterfaceName)
{
	pRoot = proot;
	pBg = new Image(0, 0, LCDW, LCDH);
	pButton = new Button(400, 10, 80, 70);

	proot->CtlRootAddInterface(pBg, pInterfaceName);
	pBg->AddChild(pButton);

	SendLoadBmpMsg(LoadInterface2Bmp, (void*)proot->CtlRootGetCtl(pInterfaceName));
}


#include "interface1.h"
#include "BmpDataArray.h"

static CtlRoot* pRoot;
static Image* pBg;
static Button* pButton;
static Gif* pGif;
static Slide* pSlide;
static Text* pText1;
static Text* pText2;
static Bar* pBar;
static Button* pButtonSlide0;
static Button* pButtonSlide1;

static int Button1Callback(Event* pEvent)
{
	Any_Event *pany_event = NULL;

	printf("Button1Callback\n");

	pany_event = &(pEvent->uEvent.any_event);
	if(pany_event->event_type == MsgTypeMouseUp)	
	{
//跳转到其它界面，并配置跳转时的界面切换效果
		printf("Button1Callback 1b\n");
		//pRoot->CtlRootChangeInterface("Interface2", EFFECT_LEFTPUSHTORIGHT);
		pRoot->CtlRootChangeInterface("Interface2", EFFECT_NONE);
	}

	printf("Button1Callback 2\n");
	return 0;
}

// 测试Slide控件的孙子控件ButtonSlide的效果
static int ButtonSlideCallback(Event* pEvent)
{
	Any_Event *pany_event = NULL;
	pany_event = &(pEvent->uEvent.any_event);
	if(pany_event->event_type == MsgTypeMouseDown)	
	{
		printf("Slide Button Down\n");
	}
	else if(pany_event->event_type == MsgTypeMouseMove)	
	{
		//printf("Slide Button Move\n");
	}
	else if(pany_event->event_type == MsgTypeMouseUp)	
	{
		printf("Slide Button Up\n");
	}
	return 0;
}

static void LoadInterface1Bmp(void)
{
	unsigned char* pic_gif[] = {

#if 0	
		pic_gif_zhengzhu_zhengzhu1, pic_gif_zhengzhu_zhengzhu2, pic_gif_zhengzhu_zhengzhu3, pic_gif_zhengzhu_zhengzhu4, pic_gif_zhengzhu_zhengzhu5, 
		pic_gif_zhengzhu_zhengzhu6, pic_gif_zhengzhu_zhengzhu7, pic_gif_zhengzhu_zhengzhu8, pic_gif_zhengzhu_zhengzhu9, pic_gif_zhengzhu_zhengzhu10, 
		pic_gif_zhengzhu_zhengzhu11, pic_gif_zhengzhu_zhengzhu12, pic_gif_zhengzhu_zhengzhu13, pic_gif_zhengzhu_zhengzhu14, pic_gif_zhengzhu_zhengzhu15, 
		pic_gif_zhengzhu_zhengzhu16, pic_gif_zhengzhu_zhengzhu17, pic_gif_zhengzhu_zhengzhu18, pic_gif_zhengzhu_zhengzhu19, pic_gif_zhengzhu_zhengzhu20, 
		pic_gif_zhengzhu_zhengzhu21, pic_gif_zhengzhu_zhengzhu22, pic_gif_zhengzhu_zhengzhu23, pic_gif_zhengzhu_zhengzhu24, pic_gif_zhengzhu_zhengzhu25, 
		pic_gif_zhengzhu_zhengzhu26,

	
		pic_gif_baotang_baotang1, pic_gif_baotang_baotang2, pic_gif_baotang_baotang3, pic_gif_baotang_baotang4, pic_gif_baotang_baotang5, 
		pic_gif_baotang_baotang6, pic_gif_baotang_baotang7, pic_gif_baotang_baotang8, pic_gif_baotang_baotang9, pic_gif_baotang_baotang10, 
		pic_gif_baotang_baotang11, pic_gif_baotang_baotang12, pic_gif_baotang_baotang13, pic_gif_baotang_baotang14, pic_gif_baotang_baotang15, 
		pic_gif_baotang_baotang16, pic_gif_baotang_baotang17, pic_gif_baotang_baotang18, pic_gif_baotang_baotang19, pic_gif_baotang_baotang20, 
		pic_gif_baotang_baotang21, pic_gif_baotang_baotang22, pic_gif_baotang_baotang23, pic_gif_baotang_baotang24, pic_gif_baotang_baotang25, 
		pic_gif_baotang_baotang26, 

		pic_gif_jianzha_jianzha1, pic_gif_jianzha_jianzha2, pic_gif_jianzha_jianzha3, pic_gif_jianzha_jianzha4, pic_gif_jianzha_jianzha5, 
		pic_gif_jianzha_jianzha6, pic_gif_jianzha_jianzha7, pic_gif_jianzha_jianzha8, pic_gif_jianzha_jianzha9, pic_gif_jianzha_jianzha10, 
		pic_gif_jianzha_jianzha11, pic_gif_jianzha_jianzha12, pic_gif_jianzha_jianzha13, pic_gif_jianzha_jianzha14, pic_gif_jianzha_jianzha15, 
		pic_gif_jianzha_jianzha16, pic_gif_jianzha_jianzha17, pic_gif_jianzha_jianzha18, pic_gif_jianzha_jianzha19, pic_gif_jianzha_jianzha20, 
		pic_gif_jianzha_jianzha21, pic_gif_jianzha_jianzha22, pic_gif_jianzha_jianzha23, pic_gif_jianzha_jianzha24, pic_gif_jianzha_jianzha25, 

		pic_gif_chaocai_chaocai1, pic_gif_chaocai_chaocai2, pic_gif_chaocai_chaocai3, pic_gif_chaocai_chaocai4, pic_gif_chaocai_chaocai5, 
		pic_gif_chaocai_chaocai6, pic_gif_chaocai_chaocai7, pic_gif_chaocai_chaocai8, pic_gif_chaocai_chaocai9, pic_gif_chaocai_chaocai10, 
		pic_gif_chaocai_chaocai11, pic_gif_chaocai_chaocai12, pic_gif_chaocai_chaocai13, pic_gif_chaocai_chaocai14, pic_gif_chaocai_chaocai15, 
		pic_gif_chaocai_chaocai16, pic_gif_chaocai_chaocai17, pic_gif_chaocai_chaocai18, pic_gif_chaocai_chaocai19, pic_gif_chaocai_chaocai20, 
		pic_gif_chaocai_chaocai21, pic_gif_chaocai_chaocai22, pic_gif_chaocai_chaocai23, pic_gif_chaocai_chaocai24, pic_gif_chaocai_chaocai25, 
		
		pic_gif_huoguo_huoguo1, pic_gif_huoguo_huoguo2, pic_gif_huoguo_huoguo3, pic_gif_huoguo_huoguo4, pic_gif_huoguo_huoguo5, 
		pic_gif_huoguo_huoguo6, pic_gif_huoguo_huoguo7, pic_gif_huoguo_huoguo8, pic_gif_huoguo_huoguo9, pic_gif_huoguo_huoguo10, 
		pic_gif_huoguo_huoguo11, pic_gif_huoguo_huoguo12, pic_gif_huoguo_huoguo13, pic_gif_huoguo_huoguo14, pic_gif_huoguo_huoguo15, 
		pic_gif_huoguo_huoguo16, pic_gif_huoguo_huoguo17, pic_gif_huoguo_huoguo18, pic_gif_huoguo_huoguo19, pic_gif_huoguo_huoguo20, 
		pic_gif_huoguo_huoguo21, pic_gif_huoguo_huoguo22, pic_gif_huoguo_huoguo23, pic_gif_huoguo_huoguo24, pic_gif_huoguo_huoguo25, 
		pic_gif_huoguo_huoguo26,
		
		pic_gif_shaoshui_shaoshui1, pic_gif_shaoshui_shaoshui2, pic_gif_shaoshui_shaoshui3, pic_gif_shaoshui_shaoshui4, pic_gif_shaoshui_shaoshui5, 
		pic_gif_shaoshui_shaoshui6, pic_gif_shaoshui_shaoshui7, pic_gif_shaoshui_shaoshui8, pic_gif_shaoshui_shaoshui9, pic_gif_shaoshui_shaoshui10, 
		pic_gif_shaoshui_shaoshui11, pic_gif_shaoshui_shaoshui12, pic_gif_shaoshui_shaoshui13, pic_gif_shaoshui_shaoshui14, pic_gif_shaoshui_shaoshui15, 
		pic_gif_shaoshui_shaoshui16, pic_gif_shaoshui_shaoshui17, pic_gif_shaoshui_shaoshui18, pic_gif_shaoshui_shaoshui19, pic_gif_shaoshui_shaoshui20, 
		pic_gif_shaoshui_shaoshui21, pic_gif_shaoshui_shaoshui22, pic_gif_shaoshui_shaoshui23, pic_gif_shaoshui_shaoshui24, pic_gif_shaoshui_shaoshui25, 
		pic_gif_shaoshui_shaoshui26,

#endif
	};	
	unsigned char* pic_slide[] = {
		pic_fail_error_E1, pic_fail_error_E2, pic_fail_error_E3, pic_fail_error_E4, pic_fail_error_E5,
		pic_fail_error_E6, pic_fail_error_E7, pic_fail_error_E8, pic_fail_error_EA, pic_fail_error_EB,
		pic_fail_error_EF, 
	};
	unsigned char* pic_text0_9[] = {
		pic_barcode_ppg_0_png, pic_barcode_ppg_1_png, pic_barcode_ppg_2_png, pic_barcode_ppg_3_png, pic_barcode_ppg_4_png,
		pic_barcode_ppg_5_png, pic_barcode_ppg_6_png, pic_barcode_ppg_7_png, pic_barcode_ppg_8_png, pic_barcode_ppg_9_png,
	};
	unsigned char* pic_textA_F[] = {
		pic_barcode_ppg_A_png, pic_barcode_ppg_B_png, pic_barcode_ppg_C_png, pic_barcode_ppg_D_png, pic_barcode_ppg_E_png,
		pic_barcode_ppg_F_png, 
	};
	unsigned char* pic_textblank[] = {
		pic_barcode_ppg_blank_png,
	};
	unsigned int i;

// load背景图片
	pBg->LoadImagePic(pic_heater_bg_newzhengzhu);

// load button的背景图片和注册callback函数
	pButton->LoadButtonPic(pic_heater_lefticonpress, pic_heater_lefticon);
	pButton->CtlCallback(Button1Callback);

// load gif的图片
	//for(i=0;i<sizeof(pic_gif)/sizeof(unsigned char*);i++)
	//	MulImageLoadPic(pGif->pGifPic, pic_gif[i], 0, 0, i);
// load gif的图片从第0副图循环显示到第25副图，并启动定时器
	//pGif->SetGifStartEndNum(0, 25);
// gif的图片显示模式为ALPHA
	//pGif->SetImageBltMode(ALPHA_STYLE);
// 配置gif的背景图片的颜色
	//pGif->SetImageBgColor(RGB888(241, 243, 243));
	
// 加载slide控件
	for(i=0;i<sizeof(pic_slide)/sizeof(unsigned char*);i++)
		pSlide->AddSlideItem(pic_slide[i]);
	pSlide->SetImageBgColor(COLOR_DARK);
// slide控件内部的第一个界面增加一个按键
	pButtonSlide0->LoadButtonPic(pic_heater_lefticonpress, pic_heater_lefticon);
	pButtonSlide0->CtlCallback(ButtonSlideCallback);
	pSlide->SlideItemAddChild(0, pButtonSlide0);
// slide控件内部的第二个界面增加一个按键
	pButtonSlide1->LoadButtonPic(pic_heater_lefticonpress, pic_heater_lefticon);
	pButtonSlide1->CtlCallback(ButtonSlideCallback);
	pSlide->SlideItemAddChild(1, pButtonSlide1);

// load text的图片，并配置每张图片对应imageIndex索引
	for(i=0;i<sizeof(pic_text0_9)/sizeof(unsigned char*);i++)
		MulImageLoadPic(pText1->pTextPic, pic_text0_9[i], 0, 0, '0'+i);
	for(i=0;i<sizeof(pic_textA_F)/sizeof(unsigned char*);i++)
		MulImageLoadPic(pText1->pTextPic, pic_textA_F[i], 0, 0, 'A'+i);
	for(i=0;i<sizeof(pic_textblank)/sizeof(unsigned char*);i++)
		MulImageLoadPic(pText1->pTextPic, pic_textblank[i], 0, 0, ' '+i);
	pText1->SetImageBltMode(ALPHA_STYLE);

//	重复的图片复用，不必要重新load
	MulImageLoadSamePic(&(pText2->pTextPic), &(pText1->pTextPic));	
	pText2->SetImageBltMode(ALPHA_STYLE);

// load bar的整张sel/notsel/icon的图片
	pBar->LoadBarPic(pic_heater_color_progress, pic_heater_bg_progress, pic_heater_circle_progress_png);
// 配置bar图片显示的offset值
	pBar->SetBarOffset(70, 21, 70, 21);
}

void RegisterInterface1(CtlRoot* proot, const char* pInterfaceName)
{
	pRoot = proot;
	pBg = new Image(0, 0, LCDW, LCDH);
	pButton = new Button(0, 0, 90, 60);
	//pGif = new Gif(36, 108, 86, 70, 70);
	pText1 = new Text(100, 10, 40, 17, 2, "12");
	pText2 = new Text(200, 10, 40, 17, 2, "34");
	pBar = new Bar(0, 200, LCDW, LCDH-200, MOVELEFTRIGHT);
	pSlide = new Slide(180, 42, 282, 160, 0, 100, 5, MOVELEFTRIGHT|MOVECYCLE|MOVESPRING);		
// pButtonSlide控件将绑定到Slide控件中，所以ButtonSlide的x,y值将以Slide控件的坐标为偏移起始点计算
	pButtonSlide0 = new Button(10, 10, 90, 60);
	pButtonSlide1 = new Button(50, 50, 90, 60);
	
// 注册本界面的窗口关系
	proot->CtlRootAddInterface(pBg, pInterfaceName);
	pBg->AddChild(pButton);
	//pBg->AddChild(pGif);
	pBg->AddChild(pSlide);
	pBg->AddChild(pText1);
	pBg->AddChild(pText2);
	pBg->AddChild(pBar);	

// 发送loadBmp的消息，在不耽误显示和按键操作的前提下，cpu空闲的时间尽快将各个界面的图片Load到内存
	SendLoadBmpMsg(LoadInterface1Bmp, (void*)proot->CtlRootGetCtl(pInterfaceName));
}




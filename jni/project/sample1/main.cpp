//#include <clutter/clutter.h>
#include "ctlobject.h"
#include "image.h"
#include "button.h"
#include "ctlroot.h"
#include "interface1.h"
#include "interface2.h"
#include "draw.h"

int main(int argc, char *argv[])
{
// 根控件
	CtlRoot root(0, 0, LCDW, LCDH);


// 初始化
	MemoryInit();
	TouchManageInit();
	TimerInit(10);
	MsgManageInit();
	ScreenInit();
	InitDrawThread(&root);
	INFO_X("icos Compile time:%s, %s\n",__DATE__, __TIME__);
	
// 注册界面
	RegisterInterface1(&root, "Interface1");
	RegisterInterface2(&root, "Interface2");
// 切换到指定界面显示
	root.CtlRootChangeInterface("Interface1", EFFECT_NONE);
	//root.CtlRootChangeInterface("Interface2", EFFECT_NONE);
// 显示配置的界面信息
	//root.CtlRootPrintf();
// 整个工程开始执行
	root.CtlRootRun();
	
	return 0;
}


//#include <clutter/clutter.h>
#include "ctlobject.h"
#include "image.h"
#include "button.h"
#include "ctlroot.h"
#include "interface2.h"
#include "draw.h"

int main(int argc, char *argv[])
{
	CtlRoot root(0, 0, LCDW, LCDH);

	MemoryInit();
	TouchManageInit();
	TimerInit(10);
	MsgManageInit();
	ScreenInit();
	InitDrawThread(&root);
	INFO_X("icos Compile time:%s, %s\n",__DATE__, __TIME__);

	RegisterInterface2(&root, "Interface2");
	root.CtlRootChangeInterface("Interface2", EFFECT_NONE);
	root.CtlRootRun();
	return 0;
}


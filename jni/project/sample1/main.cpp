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
// ���ؼ�
	CtlRoot root(0, 0, LCDW, LCDH);


// ��ʼ��
	MemoryInit();
	TouchManageInit();
	TimerInit(10);
	MsgManageInit();
	ScreenInit();
	InitDrawThread(&root);
	INFO_X("icos Compile time:%s, %s\n",__DATE__, __TIME__);
	
// ע�����
	RegisterInterface1(&root, "Interface1");
	RegisterInterface2(&root, "Interface2");
// �л���ָ��������ʾ
	root.CtlRootChangeInterface("Interface1", EFFECT_NONE);
	//root.CtlRootChangeInterface("Interface2", EFFECT_NONE);
// ��ʾ���õĽ�����Ϣ
	//root.CtlRootPrintf();
// �������̿�ʼִ��
	root.CtlRootRun();
	
	return 0;
}


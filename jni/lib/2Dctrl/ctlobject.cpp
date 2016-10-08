#include "ctlobject.h"
#include "draw.h"

// CtlObject�ؼ����������пؼ��Ļ�������
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

// �ؼ���callback����������ؼ�û�е��øú�����callback������ִ��
int CtlObject::CtlCallback(int (*pCallback)(Event* pEvent))
{
	if(pCallback != NULL) {
		pWidget->WidgetSetCallback(pCallback);
	}
	return 0;
}

// �ؼ��仯����¸ÿؼ�����ʾ
int CtlObject::CtlRefresh(void)
{
	//CheckDrawThread();
	pWidget->WidgetSetDirty(1);
	SetDrawThread(DRAWTHREADONE, EFFECT_NONE);
	return 0;
}

// �����潫�ؼ���Ϊ���㲢����ʾʱ�����ÿؼ��ĸú������ú����ṩ���ؼ����Ƿ�Ϊ�������Ϣ
int CtlObject::CtlFocus(int focus)
{
	return 0;
}

// ������Ĵ����¼��ڿؼ��Ĵ����ڲ�ʱ�������ÿؼ��ĸú���
int CtlObject::CtlEvent(Event* pEvent)
{
	return 0;
}

// �������»���ʱ���жϿؼ��������Ϊdirty������ǽ����ÿؼ��ĸú���
int CtlObject::CtlShow(PIXEL* pDesBuf)
{
	return 0;
}

// �ؼ��Ĵ���ע���ӿؼ����ӿؼ��Ĵ��ڱ����ڱ��ؼ����ڵ��ڲ�
// ��ͼʱ���Ȼ��Ʊ��ؼ����ٻ����ӿؼ���
// ��������¼������ӿؼ��У����ص��ӿؼ���callback����
void CtlObject::AddChild(CtlObject* pSon)
{
	pWidget->WidgetAddChild(pSon->pWidget, 1);
}

// �ؼ��Ĵ��ڰ��ӿؼ����ӿؼ��Ĵ���λ�ñ����ɸ��ؼ��Լ�������ƣ�������AddChildһ��
// ��������Ҫ�Ӵ��ں͸�����ͬʱ���������
void CtlObject::BindChild(CtlObject* pSon)
{
	pWidget->WidgetAddChild(pSon->pWidget, 0);
}



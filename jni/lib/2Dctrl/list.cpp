#include "list.h"
#include "draw.h"
void List::FillList(NewButton*bt){
	NewButton*temp = bt;
	Node*node;
	
	if(head == NULL){
	head = (Node*)uimalloc(sizeof(Node));
	head->index = 0;
	head->button = bt;
	head->pre = NULL;
	head->next = NULL;
	node = head;
	}
else{
		node = head;
		while(node->next != NULL)
			node = node->next;
		node->next = (Node*)uimalloc(sizeof(Node));
		node = node->next;
		node->index = indexMax;
		node->button = bt;
		node->pre = node;
		node->next = NULL;
	}
	//UILog("indexMax%d\n",indexMax);
	indexMax++;
}
Node* List::GetNodeFromIndex(int ItemIndex){
	Node* pItem;
	int i;
	if(head == NULL)
	{
		ERROR_X("error! Slide have no item\n");
		return NULL;
	}
	if(ItemIndex >= indexMax)
	{
		ERROR_X("error! Slide ItemIndex exceed MaxIndex\n");
		return NULL;
	}
	pItem = head;
	for(i=0;i<ItemIndex;i++)
	{
		if(pItem->next == NULL)
		{
			ERROR_X("error! SlideItem Index:%d not found\n", ItemIndex);
			return NULL;
		}
		pItem=pItem->next;
	}
	return pItem;

}

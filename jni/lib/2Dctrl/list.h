#ifndef _LIST_H_
#define _LIST_H_
#include "newbutton.h"


struct Node
{
	int index;
	NewButton*button;
	Node*next;
	Node*pre;		//	Slide������
		
};

class List
{
    	Node*head;
public:
	List(){
 	head = NULL;
	indexMax= 0;	
	}
	int indexMax;
	void FillList(NewButton*bt);	
	Node* GetNodeFromIndex(int ItemIndex); 	
};

#endif

#ifndef __RKEYBOARD_H__
#define __RKEYBOARD_H__

#include "button.h"
#include "mulimage.h"
#include "ctlobject.h"
#include "rbutton.h"
#include "msgmanage.h"

#define KB_MAX_ROW  5
#define KB_MAX_LINE  10
#define KB_MAX_BUF	 200

typedef void (*onTextChange)(void *data);

class RKeyboard : public Image {
private:

	//void ProcKeyCapClick(Event* pevent, void* data);	
protected:
    //for case keyboard only
    const static int numberOfRows = 4;
    char* pInputStr;
    int inputStrMaxLength;

public:
    //number of buttons in every line
    const static int firstLine = 10;
    const static int secondLine = 10;
    const static int thirdLine = 9;
    const static int fourthLine = 7;
    const static int fifthLine = 3; //useless	
    MulImage* pKeyboardPic;
    RKeyboard(int x, int y, int w, int h, CtlObject* root);
    ~RKeyboard(void);
	void InitKeyboardLayout(CtlObject* pParent);
	void SetOnTextChange(onTextChange listener);
    int CtlFocus(int focus);
    int CtlEvent(Event* pEvent);
    int CtlShow(PIXEL* pDesBuf);
	void ResetKeyboard();
};

#endif

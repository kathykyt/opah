#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "button.h"
#include "mulimage.h"

class Keyboard : public Image {
private:
protected:
    int buttonWidth;
    int buttonHeight;
    //number of buttons in every line
    const static int firstLine = 10;
    const static int secondLine = 10;
    const static int thirdLine = 9;
    const static int fourthLine = 7;
    const static int fifthLine = 3; //useless
    //for case keyboard only
    const static int numberOfRows = 4;

    Button* pButtonArray[5][10];

    double secondLineOffsetX;
    double thirdLineOffsetX;
    double fourthLineOffsetX;
    double spaceOffsetX;
    double spaceWidthX;
    double deleteOffsetX;

    char* pInputStr;
    int inputStrMaxLength;
    bool caseState;

public:
    MulImage* pKeyboardPic;
    Keyboard(int x, int y, int w, int h);
    ~Keyboard(void);
    char* GetInputStr(void);
    int CleanInputStr(void);
    int CtlFocus(int focus);
    int CtlEvent(Event* pEvent);
    int CtlShow(PIXEL* pDesBuf);
};

#endif

#include "keyboard.h"
#include "draw.h"

//for search pictures in class MulImage
char keyMapCapital[5][10] = {
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
    {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
    {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L'},
    {'Z', 'X', 'C', 'V', 'B', 'N', 'M'},
    {'!', '_', '$'}//corresponding to changeCase, space and delete
};
char keyMapLowercase[5][10] = {
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l'},
    {'z', 'x', 'c', 'v', 'b', 'n', 'm'},
    {'!', '_', '$'}//corresponding to changeCase, space and delete
};

Keyboard::Keyboard(int x, int y, int w, int h) : Image(x, y, w, h) {
    pKeyboardPic = MulImageInit();
    caseState = true;//true represents Capital, false represents lowercase
    pInputStr = NULL;
    inputStrMaxLength = 20;
    //allocation memory for pInputStr
    pInputStr = (char*) uimalloc(inputStrMaxLength);
    buttonWidth = w / 10;
    buttonHeight = h / 5;
    double slox = (firstLine - secondLine) * buttonWidth;
    double tlox = (firstLine - thirdLine) * buttonWidth;
    double flox = (firstLine - fourthLine) * buttonWidth;
    secondLineOffsetX = slox / 2;
    thirdLineOffsetX = tlox / 2;
    fourthLineOffsetX = flox / 2;
    spaceWidthX = 32; //fits for the specific picture
    spaceOffsetX = (w - spaceWidthX) / 2;
    deleteOffsetX = buttonWidth * 9;

    //initialize pButtonArray
    for (int countx = 0; countx < firstLine; countx++) {
        pButtonArray[0][countx] = new Button(x + buttonWidth * countx, y, buttonWidth, buttonHeight);
    }
    for (int countx = 0; countx < secondLine; countx++) {
        pButtonArray[1][countx] = new Button(x + buttonWidth * countx, y + buttonHeight, buttonWidth, buttonHeight);
    }
    for (int countx = 0; countx < thirdLine; countx++) {
        pButtonArray[2][countx] = new Button(x + buttonWidth * countx, y + buttonHeight * 2, buttonWidth, buttonHeight);
    }
    for (int countx = 0; countx < fourthLine; countx++) {
        pButtonArray[3][countx] = new Button(x + buttonWidth * countx, y + buttonHeight * 3, buttonWidth, buttonHeight);
    }
    //change case, space and delete in the fifth line.
    pButtonArray[4][0] = new Button(x, y + buttonHeight * 4, buttonWidth, buttonHeight);
    pButtonArray[4][1] = new Button(x + spaceOffsetX, y + buttonHeight * 4, spaceWidthX, buttonHeight);
    pButtonArray[4][2] = new Button(x + deleteOffsetX, y + buttonHeight * 4, buttonWidth, buttonHeight);

    pWidget->pMyCtlObject = this;
}

Keyboard::~Keyboard(void) {
    if (pInputStr != NULL) {
        uifree((void*) pInputStr);
    }
    MulImageDestory(pKeyboardPic);
}

char* Keyboard::GetInputStr(void) {
    return pInputStr;
}

int Keyboard::CleanInputStr(void) {
    if(pInputStr != NULL) {
	pInputStr = (char*) uimalloc(inputStrMaxLength);
    }
    return 0;
}

int Keyboard::CtlFocus(int focus) {
    return 0;
}

int Keyboard::CtlEvent(Event* pEvent) {
    Any_Event *pany_event = NULL;
    pany_event = &(pEvent->uEvent.any_event);
    if (isTSMsg(pany_event->event_type)) {
        if (pany_event->event_type == MsgTypeMouseDown) {
            //handle the click event
            for (int county = 0; county < numberOfRows; county++) {
                if (pEvent->uEvent.ts_event.y < pWidget->y + buttonHeight * (county + 1)) {
                    if (pEvent->uEvent.ts_event.y > pWidget->y + buttonHeight * county) {
                        int buttonsInRow;
                        double lineOffsetX;
                        if (county == 0) {
                            buttonsInRow = firstLine;
                            lineOffsetX = 0;
                        } else if (county == 1) {
                            buttonsInRow = secondLine;
                            lineOffsetX = secondLineOffsetX;
                        } else if (county == 2) {
                            buttonsInRow = thirdLine;
                            lineOffsetX = thirdLineOffsetX;
                        } else if (county == 3) {
                            buttonsInRow = fourthLine;
                            lineOffsetX = fourthLineOffsetX;
                        }
                        for (int countx = 0; countx < buttonsInRow; countx++) {
                            if (pEvent->uEvent.ts_event.x < pWidget->x + lineOffsetX + buttonWidth * (countx + 1)) {
                                if (pEvent->uEvent.ts_event.x > pWidget->x + lineOffsetX + buttonWidth * countx) {
                                    pButtonArray[county][countx]->SetButtonSelOrNot(BUTTONDOWN);
				    char print;
				    if(caseState == true){
                                        print = keyMapCapital[county][countx];
				    }
				    else{
				        print = keyMapLowercase[county][countx];
				    }
                                    int lengthOfInput = strlen(pInputStr);
                                    if (lengthOfInput == inputStrMaxLength) {
                                        UILog("Limitation of words length\n");
                                    } else {
                                        pInputStr[lengthOfInput] = print;
                                        pInputStr[lengthOfInput + 1] = '\0';
                                        UILog("Input = %s\n", pInputStr);
                                    }
                                    UILog("%c Button in Keyboard-MouseDown\n", print);
                                    if (pButtonArray[county][countx]->GetButtonNotSelbuff() != pButtonArray[county][countx]->GetButtonSelbuff()) {
                                        CtlRefresh();
                                    }
                                }
                            }
                        }
                    }
                }
            }
            //for the fifth line
            if (pEvent->uEvent.ts_event.y < pWidget->y + buttonHeight * 5 &&
                    pEvent->uEvent.ts_event.y > pWidget->y + buttonHeight * 4) {
                if (pEvent->uEvent.ts_event.x < pWidget->x + buttonWidth &&
                        pEvent->uEvent.ts_event.x > pWidget->x) {
                    if (caseState) {
                        caseState = false;
                    } else {
                        caseState = true;
                    }
                    UILog("Input = %s\n", pInputStr);
                    if (pButtonArray[4][0]->GetButtonNotSelbuff() != pButtonArray[4][0]->GetButtonSelbuff()) {
                        CtlRefresh();
                    }
                }
                if (pEvent->uEvent.ts_event.x < pWidget->x + spaceOffsetX + spaceWidthX &&
                        pEvent->uEvent.ts_event.x > pWidget->x + spaceOffsetX) {
                    UILog("Space button click\n");
                    int lengthOfInput = strlen(pInputStr);
                    if (lengthOfInput == inputStrMaxLength) {
                        UILog("Limitation of words length\n");
                    } else {
                        pInputStr[lengthOfInput] = ' ';
                        pInputStr[lengthOfInput + 1] = '\0';
                        UILog("Input = %s\n", pInputStr);
                    }
                    if (pButtonArray[4][1]->GetButtonNotSelbuff() != pButtonArray[4][1]->GetButtonSelbuff()) {
                        CtlRefresh();
                    }
                }
                if (pEvent->uEvent.ts_event.x < pWidget->x + deleteOffsetX + buttonWidth &&
                        pEvent->uEvent.ts_event.x > pWidget->x + deleteOffsetX) {
                    UILog("Delete button click\n");
                    int lengthOfInput = strlen(pInputStr);
                    pInputStr[lengthOfInput - 1] = '\0';
                    UILog("Input = %s\n", pInputStr);
                    if (pButtonArray[4][2]->GetButtonNotSelbuff() != pButtonArray[4][2]->GetButtonSelbuff()) {
                        CtlRefresh();
                    }
                }
            }
        } else if (pany_event->event_type == MsgTypeMouseUp) {
            for (int county = 0; county < numberOfRows; county++) {
                if (pEvent->uEvent.ts_event.y < pWidget->y + buttonHeight * (county + 1)) {
                    if (pEvent->uEvent.ts_event.y > pWidget->y + buttonHeight * county) {
                        int buttonsInRow;
                        double lineOffsetX;
                        if (county == 0) {
                            buttonsInRow = firstLine;
                            lineOffsetX = 0;
                        } else if (county == 1) {
                            buttonsInRow = secondLine;
                            lineOffsetX = secondLineOffsetX;
                        } else if (county == 2) {
                            buttonsInRow = thirdLine;
                            lineOffsetX = thirdLineOffsetX;
                        } else if (county == 3) {
                            buttonsInRow = fourthLine;
                            lineOffsetX = fourthLineOffsetX;
                        }
                        for (int countx = 0; countx < buttonsInRow; countx++) {
                            if (pEvent->uEvent.ts_event.x < pWidget->x + lineOffsetX + buttonWidth * (countx + 1)) {
                                if (pEvent->uEvent.ts_event.x > pWidget->x + lineOffsetX + buttonWidth * countx) {
                                    pButtonArray[county][countx]->SetButtonSelOrNot(BUTTONUP);
                                    char print;
				    if(caseState == true){
                                        print = keyMapCapital[county][countx];
				    }
				    else{
				        print = keyMapLowercase[county][countx];
				    }
                                    UILog("%c Button in Keyboard-MouseUp\n", print);
                                    if (pButtonArray[county][countx]->GetButtonNotSelbuff() != pButtonArray[county][countx]->GetButtonSelbuff()) {
                                        CtlRefresh();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int Keyboard::CtlShow(PIXEL* pDesBuf) {
    MulImagePic* pPic;
    if (Bgimagebuff != NULL) {
        AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX, pWidget->y + BgimageOffsetY, Bgimagebuff, 0, 0,
                DIFF(BgimageWidth, BgimageOffsetX, pWidget->width), DIFF(BgimageHeight, BgimageOffsetY, pWidget->height), BltMode);
    } else if (BgColor != TRANSCOLOR) {
        AreaFill(pDesBuf, pWidget->x + BgimageOffsetX, pWidget->y + BgimageOffsetY, pWidget->width - BgimageOffsetX, pWidget->height - BgimageOffsetY, BgColor);
    }

    //index of picMap should equal with the index of MulImage when Load pictures in interface.
    //36 means the quantity of case buttons
    char picMap[65] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
 '!', '_', '$',
'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    for (int countx = 0; countx < firstLine; countx++) {
        int picIndex;
	char word;
        if(caseState == true){
            word = keyMapCapital[0][countx];
	}
	else{
	    word = keyMapLowercase[0][countx];
	}
        for (int i = 0; i < 65; i++) {
            if (picMap[i] == word) {
                picIndex = i;
            }
        }
        pPic = GetMulImageIndex(pKeyboardPic, picIndex);
        if (pPic != NULL) {
            if (pPic->MulImagebuff != NULL) {
                if (pButtonArray[0][countx]->GetButtonSelOrNot() == BUTTONDOWN) {
                    AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX + buttonWidth * countx, pWidget->y + BgimageOffsetY, pPic->MulImagebuff, 0, 0,
                            DIFF(pPic->MulImageWidth, BgimageOffsetX, buttonWidth), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
                } else if (pButtonArray[0][countx]->GetButtonSelOrNot() == BUTTONUP) {
                    AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX + buttonWidth * countx, pWidget->y + BgimageOffsetY, pPic->MulImagebuff, 0, 0,
                            DIFF(pPic->MulImageWidth, BgimageOffsetX, buttonWidth), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
                }
            }
        }
    }
    //second line
    for (int countx = 0; countx < secondLine; countx++) {
        int picIndex;
        char word;
        if(caseState == true){
            word = keyMapCapital[1][countx];
	}
	else{
	    word = keyMapLowercase[1][countx];
	}
        for (int i = 0; i < 65; i++) {
            if (picMap[i] == word) {
                picIndex = i;
            }
        }
        pPic = GetMulImageIndex(pKeyboardPic, picIndex);
        if (pPic != NULL) {
            if (pPic->MulImagebuff != NULL) {
                if (pButtonArray[1][countx]->GetButtonSelOrNot() == BUTTONDOWN) {
                    AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX + buttonWidth * countx + secondLineOffsetX, pWidget->y + BgimageOffsetY + buttonHeight * 1, pPic->MulImagebuff, 0, 0,
                            DIFF(pPic->MulImageWidth, BgimageOffsetX, buttonWidth), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
                } else if (pButtonArray[1][countx]->GetButtonSelOrNot() == BUTTONUP) {
                    AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX + buttonWidth * countx + secondLineOffsetX, pWidget->y + BgimageOffsetY + buttonHeight * 1, pPic->MulImagebuff, 0, 0,
                            DIFF(pPic->MulImageWidth, BgimageOffsetX, buttonWidth), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
                }
            }
        }
    }
    //third line
    for (int countx = 0; countx < thirdLine; countx++) {
        int picIndex;
        char word;
        if(caseState == true){
            word = keyMapCapital[2][countx];
	}
	else{
	    word = keyMapLowercase[2][countx];
	}
        for (int i = 0; i < 65; i++) {
            if (picMap[i] == word) {
                picIndex = i;
            }
        }
        pPic = GetMulImageIndex(pKeyboardPic, picIndex);
        if (pPic != NULL) {
            if (pPic->MulImagebuff != NULL) {
                if (pButtonArray[2][countx]->GetButtonSelOrNot() == BUTTONDOWN) {
                    AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX + buttonWidth * countx + thirdLineOffsetX, pWidget->y + BgimageOffsetY + buttonHeight * 2, pPic->MulImagebuff, 0, 0,
                            DIFF(pPic->MulImageWidth, BgimageOffsetX, buttonWidth), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
                } else if (pButtonArray[2][countx]->GetButtonSelOrNot() == BUTTONUP) {
                    AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX + buttonWidth * countx + thirdLineOffsetX, pWidget->y + BgimageOffsetY + buttonHeight * 2, pPic->MulImagebuff, 0, 0,
                            DIFF(pPic->MulImageWidth, BgimageOffsetX, buttonWidth), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
                }
            }
        }
    }
    //fourth line
    for (int countx = 0; countx < fourthLine; countx++) {
        int picIndex;
        char word;
        if(caseState == true){
            word = keyMapCapital[3][countx];
	}
	else{
	    word = keyMapLowercase[3][countx];
	}
        for (int i = 0; i < 65; i++) {
            if (picMap[i] == word) {
                picIndex = i;
            }
        }
        pPic = GetMulImageIndex(pKeyboardPic, picIndex);
        if (pPic != NULL) {
            if (pPic->MulImagebuff != NULL) {
                if (pButtonArray[3][countx]->GetButtonSelOrNot() == BUTTONDOWN) {
                    AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX + buttonWidth * countx + fourthLineOffsetX, pWidget->y + BgimageOffsetY + buttonHeight * 3, pPic->MulImagebuff, 0, 0,
                            DIFF(pPic->MulImageWidth, BgimageOffsetX, buttonWidth), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
                } else if (pButtonArray[3][countx]->GetButtonSelOrNot() == BUTTONUP) {
                    AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX + buttonWidth * countx + fourthLineOffsetX, pWidget->y + BgimageOffsetY + buttonHeight * 3, pPic->MulImagebuff, 0, 0,
                            DIFF(pPic->MulImageWidth, BgimageOffsetX, buttonWidth), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
                }
            }
        }
    }
    //fifth line
    int changeCaseIndex = 36;
    pPic = GetMulImageIndex(pKeyboardPic, changeCaseIndex);
    if (pPic != NULL) {
        if (pPic->MulImagebuff != NULL) {
            if (pButtonArray[4][0]->GetButtonSelOrNot() == BUTTONDOWN) {
                AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX, pWidget->y + BgimageOffsetY + buttonHeight * 4, pPic->MulImagebuff, 0, 0,
                        DIFF(pPic->MulImageWidth, BgimageOffsetX, buttonWidth), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
            } else if (pButtonArray[4][0]->GetButtonSelOrNot() == BUTTONUP) {
                AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX, pWidget->y + BgimageOffsetY + buttonHeight * 4, pPic->MulImagebuff, 0, 0,
                        DIFF(pPic->MulImageWidth, BgimageOffsetX, buttonWidth), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
            }
        }
    }
    int spaceIndex = 37;
    pPic = GetMulImageIndex(pKeyboardPic, spaceIndex);
    if (pPic != NULL) {
        if (pPic->MulImagebuff != NULL) {
            if (pButtonArray[4][1]->GetButtonSelOrNot() == BUTTONDOWN) {
                AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX + spaceOffsetX, pWidget->y + BgimageOffsetY + buttonHeight * 4, pPic->MulImagebuff, 0, 0,
                        DIFF(pPic->MulImageWidth, BgimageOffsetX, spaceWidthX), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
            } else if (pButtonArray[4][1]->GetButtonSelOrNot() == BUTTONUP) {
                AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX + spaceOffsetX, pWidget->y + BgimageOffsetY + buttonHeight * 4, pPic->MulImagebuff, 0, 0,
                        DIFF(pPic->MulImageWidth, BgimageOffsetX, spaceWidthX), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
            }
        }
    }
    int deleteIndex = 38;
    pPic = GetMulImageIndex(pKeyboardPic, deleteIndex);
    if (pPic != NULL) {
        if (pPic->MulImagebuff != NULL) {
            if (pButtonArray[4][2]->GetButtonSelOrNot() == BUTTONDOWN) {
                AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX + deleteOffsetX, pWidget->y + BgimageOffsetY + buttonHeight * 4, pPic->MulImagebuff, 0, 0,
                        DIFF(pPic->MulImageWidth, BgimageOffsetX, buttonWidth), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
            } else if (pButtonArray[4][2]->GetButtonSelOrNot() == BUTTONUP) {
                AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX + deleteOffsetX, pWidget->y + BgimageOffsetY + buttonHeight * 4, pPic->MulImagebuff, 0, 0,
                        DIFF(pPic->MulImageWidth, BgimageOffsetX, buttonWidth), DIFF(pPic->MulImageHeight, BgimageOffsetY, buttonHeight), BltMode);
            }
        }
    }
    return 0;
}

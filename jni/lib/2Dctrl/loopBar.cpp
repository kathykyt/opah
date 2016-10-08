#include "loopBar.h"
#include "draw.h"
#include "math.h"

LoopBar::LoopBar(int x, int y, int w, int h) : Image(x, y, w, h) {
	
	Touchable = true;
	BarType = 1;

    BarNotSelbuff1 = NULL;
    BarSelbuff1 = NULL;
    BarNotSelbuff2 = NULL;
    BarSelbuff2 = NULL;
    BarNotSelbuff3 = NULL;
    BarSelbuff3 = NULL;
    BarIconbuff = NULL;

    touchError = 50;
    int radiusInt;
    if (w == 228) {
		radiusInt = 82;
	} else if (w == 200) {
		radiusInt = 67;
	} else {
		radiusInt = w > h ? w * 0.36 : h * 0.36;
	}
    Radius = radiusInt;
    Length = Radius * sin(45 * M_PI / 180); //half length of side of rectangle in circle

    OriginX = x + w / 2;
    OriginY = y + w / 2;
    //OriginY = y + h / 2;
    BarWidth = w;
    BarHeight = w;
    //BarHeight = h;

    BarIconWidth = 0;
    BarIconHeight = 0;
    BarValue = 200;
    pWidget->pMyCtlObject = this;
}

LoopBar::~LoopBar(void) {
    FreeBmp(BarNotSelbuff1);
    FreeBmp(BarSelbuff1);
    FreeBmp(BarNotSelbuff2);
    FreeBmp(BarSelbuff2);
    FreeBmp(BarNotSelbuff3);
    FreeBmp(BarSelbuff3);
    FreeBmp(BarIconbuff);
}

void LoopBar::LoadLoopBarPic(unsigned char* pSelPic1, unsigned char* pNotSelPic1, unsigned char* pSelPic2, unsigned char* pNotSelPic2,
        unsigned char* pSelPic3, unsigned char* pNotSelPic3, unsigned char* pIconPic) {
    int picID;

    GetBMP_wid(pSelPic1, &Width1, &Height1, &BarSelbuff1, &picID);
    GetBMP_wid(pSelPic2, &Width2, &Height2, &BarSelbuff2, &picID);
    GetBMP_wid(pSelPic3, &Width3, &Height3, &BarSelbuff3, &picID);

    if (pNotSelPic1 != pSelPic1) {
        GetBMP_wid(pNotSelPic1, &Width1, &Height1, &BarNotSelbuff1, &picID);
    } else {
        BarNotSelbuff1 = NULL;
    }
    if (pNotSelPic2 != pSelPic2) {
        GetBMP_wid(pNotSelPic2, &Width2, &Height2, &BarNotSelbuff2, &picID);
    } else {
        BarNotSelbuff2 = NULL;
    }
    if (pNotSelPic3 != pSelPic3) {
        GetBMP_wid(pNotSelPic3, &Width3, &Height3, &BarNotSelbuff3, &picID);
    } else {
        BarNotSelbuff3 = NULL;
    }

    if (pIconPic != NULL)
        GetBMP_wid(pIconPic, &BarIconWidth, &BarIconHeight, &BarIconbuff, &picID);
}

void LoopBar::SetLoopBarType(int type) {
	//daniel HACK here, 4->2 and 2->4
	if (type == 1) {
		BarType = 1;//10 to 40
	} else if (type == 4) {
		BarType = 2;//46 to 80
	} else if (type == 3) {
		BarType = 3;//0 to 9
	} else if (type == 2) {
		BarType = 4;//33 to 60
	} else if (type == 5) {
		BarType = 5;//45 to 80, every 5 degree
	}
}

int LoopBar::GetLoopBarValue(void) {
    if(BarType == 1) {
		return BarValue / 33 + 10;
	} else if (BarType == 2) {
		return BarValue / 29 + 46;
	} else if (BarType == 3) {
		return BarValue / 111;
	} else if (BarType == 4) {
		return BarValue / 37 + 33;
	} else if (BarType == 5) {
		return BarValue / 142 * 5 + 45;
	}
 }

void LoopBar::SetLoopBarValue(int value) {
	if(BarType == 1) {
		if(value >= 40) {
			BarValue = 1000;
		} else if (value <= 10) {
			BarValue = 0;
		} else {
			BarValue = (value - 10) * 33;
		}
	} else if (BarType == 2) {
		if(value >= 80) {
			BarValue = 1000;
		} else if (value <= 46) {
			BarValue = 0;
		} else {
			BarValue = (value - 46) * 29;
		}
	} else if (BarType == 3) {
		if(value >= 9) {
			BarValue = 1000;
		} else if (value <= 0) {
			BarValue = 0;
		} else {
			BarValue = value * 111;
		}
	} else if (BarType == 4) {
		if(value >= 60) {
			BarValue = 1000;
		} else if (value <= 33) {
			BarValue = 0;
		} else {
			BarValue = (value - 33) * 37;
		}
	} else if (BarType == 5) {
		if (value >= 80) {
			BarValue = 1000;
		} else if (value <= 45) {
			BarValue = 0;
		} else {
			BarValue = (value - 45) / 5 * 142;
		}
	}
	CtlRefresh();
}

void LoopBar::SetTouchable(bool touch) {
	Touchable = touch;
}

int LoopBar::CtlFocus(int focus) {
    return 0;
}

int LoopBar::CtlEvent(Event* pEvent) {
	if (Touchable) {
		Any_Event *pany_event = NULL;
		pany_event = &(pEvent->uEvent.any_event);
		int tmp;
		if (isTSMsg(pany_event->event_type)) {
			int divideY = OriginY - Length;
			int lowLimitY = OriginY + Length;
			int highLimitY = OriginY - Radius;

			int touchX = pEvent->uEvent.ts_event.x;
			int touchY = pEvent->uEvent.ts_event.y;

			//for the second bar(horizontal), value is 334-666
			if (highLimitY - touchError <= touchY && touchY <= divideY) {
				if (OriginX - Length - touchError <= touchX && touchX <= OriginX + Length + touchError) {
					if (touchX < OriginX - Length) {
						touchX = OriginX - Length;
					} else if (touchX > OriginX + Length) {
						touchX = OriginX + Length;
					}
			BarValue = (touchX - (OriginX - Length)) * 332 / (2 * Length) + 334;
				}
			}
			//for first and third 
			else if (divideY < touchY && touchY <= lowLimitY + touchError) {
				double radianAngle = acos((touchY - OriginY) / Radius);
				int angle = radianAngle / M_PI * 180;
				if (angle < 45) {
					angle = 45;
				} else if (angle > 135) {
					angle = 135;
				}
				int corrX;
				//for the first bar , value is 0-333
				if (touchX < OriginX) {
			double myRadianAngle = (angle - 90) * M_PI / 180;
					corrX = OriginX - Radius * cos(myRadianAngle);
					if (corrX - touchError < touchX && touchX < corrX + touchError) {
						BarValue = (angle - 45) * 333 / 90;
					}
				}
				//for the third bar, value is 667-1000
				if (touchX > OriginX) {
			double myRadianAngle = (angle - 90) * M_PI / 180;
					corrX = OriginX + Radius * cos(myRadianAngle);
					if (corrX - touchError < touchX && touchX < corrX + touchError) {
						BarValue = (-angle + 135) * 333 / 90 + 667;
					}
				}
			}
			CtlRefresh();
		}
	}
    return 0;
}

int LoopBar::CtlShow(PIXEL* pDesBuf) {
    int tmpx, tmpy;
    if (Bgimagebuff != NULL) {
        AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX, pWidget->y + BgimageOffsetY, Bgimagebuff, 0, 0,
                DIFF(BgimageWidth, BgimageOffsetX, pWidget->width), DIFF(BgimageHeight, BgimageOffsetY, pWidget->height), BltMode);
    } else if (BgColor != TRANSCOLOR) {
        AreaFill(pDesBuf, pWidget->x + BgimageOffsetX, pWidget->y + BgimageOffsetY, pWidget->width - BgimageOffsetX, pWidget->height - BgimageOffsetY, BgColor);
    }
    
    //Daniel, start from here
    int bar1X = pWidget->x;
    int bar1Y = pWidget->y + Height2;
    int bar2X = pWidget->x;
    int bar2Y = pWidget->y;
    int bar3X = pWidget->x + Width1;
    int bar3Y = pWidget->y + Height2;
    
    int posX, posY;

    if (BarValue <= 333) {
        if (BarSelbuff1 != NULL) {
            AreaDraw(pDesBuf, bar1X, bar1Y, BarSelbuff1, 0, 0, Width1, Height1, BltMode);
        }
        if (BarNotSelbuff2 != NULL) {
            AreaDraw(pDesBuf, bar2X, bar2Y, BarNotSelbuff2, 0, 0, Width2, Height2, BltMode);
        }
        if (BarNotSelbuff3 != NULL) {
            AreaDraw(pDesBuf, bar3X, bar3Y, BarNotSelbuff3, 0, 0, Width3, Height3, BltMode);
        }
        if (BarNotSelbuff1 != NULL) {
            double angle = BarValue * 90 / 333 + 45 + 1; //double to int => plus 1, can be deleted
            double radianAngle = (- angle + 90) * M_PI / 180;
            posY = OriginY + Radius * sin(radianAngle); //not for the circle 's sin and cos.
            posX = OriginX - Radius * cos(radianAngle);
            int heightY = posY - (OriginY - Length);
            AreaDraw(pDesBuf, bar1X, bar1Y, BarNotSelbuff1, 0, 0, Width1, heightY, BltMode);
        }
    } else if (333 < BarValue && BarValue <= 666) {
        if (BarSelbuff1 != NULL) {
            AreaDraw(pDesBuf, bar1X, bar1Y, BarSelbuff1, 0, 0, Width1, Height1, BltMode);
        }
        if (BarNotSelbuff2 != NULL) {
            AreaDraw(pDesBuf, bar2X, bar2Y, BarNotSelbuff2, 0, 0, Width2, Height2, BltMode);
        }
        if (BarNotSelbuff3 != NULL) {
            AreaDraw(pDesBuf, bar3X, bar3Y, BarNotSelbuff3, 0, 0, Width3, Height3, BltMode);
        }
        if (BarSelbuff2 != NULL) {
            posX = (BarValue - 334) * 2 * Length / 332   +   (OriginX - Length);
            double diffX = OriginX > posX ? OriginX - posX : posX - OriginX;
            posY = OriginY - cos(asin(diffX / Radius)) * Radius;
            AreaDraw(pDesBuf, bar2X, bar2Y, BarSelbuff2, 0, 0, posX - pWidget->x, Height2, BltMode);
        }
    } else if (BarValue > 666) {
        if (BarSelbuff1 != NULL) {
            AreaDraw(pDesBuf, bar1X, bar1Y, BarSelbuff1, 0, 0, Width1, Height1, BltMode);
        }
        if (BarSelbuff2 != NULL) {
            AreaDraw(pDesBuf, bar2X, bar2Y, BarSelbuff2, 0, 0, Width2, Height2, BltMode);
        }
        if (BarNotSelbuff3 != NULL) {
            AreaDraw(pDesBuf, bar3X, bar3Y, BarNotSelbuff3, 0, 0, Width3, Height3, BltMode);
        }
        if (BarSelbuff3 != NULL) {
            int corr1BarValue = - BarValue + 1000;
            double angle = corr1BarValue * 90 / 333 + 45;
            double radianAngle = (- angle + 90) * M_PI / 180;
            posY = OriginY + Radius * sin(radianAngle); //not for the circle 's sin and cos.
            posX = OriginX + Radius * cos(radianAngle);
            int heightY = posY - (OriginY - Length);
            AreaDraw(pDesBuf, bar3X, bar3Y, BarSelbuff3, 0, 0, Width3, heightY, BltMode);
        }
    }
    //Draw Icon
    int desX1 = posX - BarIconWidth / 2;
    int desY1 = posY - BarIconHeight / 2;

    if (pWidget->x <= desX1 && desX1 <= pWidget->x + BarWidth  &&  pWidget->y <= desY1 && desY1 <= pWidget->y + BarHeight) {
	if (BarIconbuff != NULL) {
	    AreaDraw(pDesBuf, desX1, desY1, BarIconbuff, 0, 0, BarIconWidth, BarIconHeight, ALPHA_STYLE);
	}
    }
    return 0;
}


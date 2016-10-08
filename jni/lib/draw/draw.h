#ifndef _DRAW_H_
#define _DRAW_H_

#include "flagdefine.h"
#include "ctlroot.h"

#define DRAWTHREADSTOP      0
#define DRAWTHREADONE      1
#define DRAWTHREADRUN       2

void CheckDrawThread(void);
int SetDrawThread(int StartStop, int effectmode);
void InitDrawThread(CtlRoot* proot);

#endif

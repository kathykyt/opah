#include "rkeyboard.h"
#include "draw.h"
#include "BmpDataArray.h"
#include "rbutton.h"
#define KEY_W	27
#define KEY_H	42

#define KEY_CAP_W	42
#define KEY_CAP_H	84

#define KEY_DEL_W	40
#define KEY_DEL_H	84

#define KEY_SPACE_W	190
#define KEY_SPACE_H	42

static char inputBuf[KB_MAX_BUF];
static int bufIndex = 0;
static bool caseState = true;

onTextChange onTextChangeListener;

void OnKeyClick(Event* pevent, void* data);
void ProcKeyCapClick(Event* pevent, void* data);
void onKeySpace(Event* pevent, void* data);
void onKeyDel(Event* pevent, void* data);


/* for search pictures in class MulImage */
static char	keyMapCapital[KB_MAX_ROW][KB_MAX_LINE] = {
	{ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' },
	{ 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P' },
	{ 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' },
	{ 'Z', 'X', 'C', 'V', 'B', 'N', 'M' },
	{ '!', '_', '$' }       /* corresponding to changeCase, space and delete */
};

static char	keyMapLowercase[KB_MAX_ROW][KB_MAX_LINE] = {
	{ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' },
	{ 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p' },
	{ 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' },
	{ 'z', 'x', 'c', 'v', 'b', 'n', 'm' },
	{ '!', '_', '$' }       /* corresponding to changeCase, space and delete */
};

static RButton* pButtonArray[KB_MAX_ROW][KB_MAX_LINE];

RKeyboard::RKeyboard( int x, int y, int w, int h , CtlObject* pRootWidget) : Image( x, y, w, h )
{
	caseState		= true; /* true represents Capital, false represents lowercase */
	memset(inputBuf, 0, sizeof(inputBuf));
	
	/* allocation memory for pInputStr */
	Color color = {0, 0, 0, 0};

	/* initialize pButtonArray */
	int id = 0;
	char keyText[2] = {0};
	for ( int countx = 0; countx <= firstLine; countx++ )
	{
		pButtonArray[0][countx] = new RButton( x + KEY_W * countx, y, KEY_W, KEY_H );
		keyText[0] = keyMapCapital[0][countx];
		pButtonArray[0][countx]->SetText(keyText);
		//INFO_X("key=%s", keyText);
		pButtonArray[0][countx]->SetId(id); id++;
		pButtonArray[0][countx]->SetTextSize(14);
		pButtonArray[0][countx]->SetTextColor(color);
		pButtonArray[0][countx]->SetOnClickListener(OnKeyClick);
		pRootWidget->AddChild(pButtonArray[0][countx]);
	}
	for ( int countx = 0; countx < secondLine; countx++ )
	{
		pButtonArray[1][countx] = new RButton( x + KEY_W * countx, y + KEY_H, KEY_W, KEY_H );
		keyText[0] = keyMapCapital[1][countx];
		pButtonArray[1][countx]->SetText(keyText);
		//INFO_X("key=%s", &keyMapCapital[1][countx]);
		pButtonArray[1][countx]->SetId(id); id++;
		pButtonArray[1][countx]->SetTextSize(14);
		pButtonArray[1][countx]->SetTextColor(color);
		pButtonArray[1][countx]->SetOnClickListener(OnKeyClick);
		pRootWidget->AddChild(pButtonArray[1][countx]);		
	}
	for ( int countx = 0; countx < thirdLine; countx++ )
	{
		pButtonArray[2][countx] = new RButton( x + KEY_W/2 + KEY_W * countx, y + KEY_H * 2, KEY_W, KEY_H );
		keyText[0] = keyMapCapital[2][countx];
		pButtonArray[2][countx]->SetText(keyText);
		//INFO_X("key=%s", &keyMapCapital[2][countx]);
		pButtonArray[2][countx]->SetId(id); id++;
		pButtonArray[2][countx]->SetTextSize(14);
		pButtonArray[2][countx]->SetTextColor(color);
		pButtonArray[2][countx]->SetOnClickListener(OnKeyClick);
		pRootWidget->AddChild(pButtonArray[2][countx]);			
	}
	for ( int countx = 0; countx < fourthLine; countx++ )
	{
		pButtonArray[3][countx] = new RButton( x + KEY_CAP_W + KEY_W * countx, y + KEY_H * 3, KEY_W, KEY_H );
		keyText[0] = keyMapCapital[3][countx];
		pButtonArray[3][countx]->SetText(keyText);
		//INFO_X("key=%s", &keyMapCapital[3][countx]);
		pButtonArray[3][countx]->SetId(id); id++;
		pButtonArray[3][countx]->SetTextSize(14);
		pButtonArray[3][countx]->SetTextColor(color);
		pButtonArray[3][countx]->SetOnClickListener(OnKeyClick);	
		pRootWidget->AddChild(pButtonArray[3][countx]);	
	}
	/* change case, space and delete in the fifth line. */
	//cap key
	pButtonArray[4][0]	= new RButton( x, y + KEY_H * 3, KEY_CAP_W, KEY_CAP_H);
	pButtonArray[4][0]->SetId(id); id++;
	pButtonArray[4][0]->SetOnClickListener(ProcKeyCapClick);
	pRootWidget->AddChild(pButtonArray[4][0]);

	//space key
	pButtonArray[4][1]	= new RButton( x + KEY_CAP_W, y + KEY_H * 4, KEY_SPACE_W, KEY_SPACE_H );
	pButtonArray[4][1]->SetId(id); id++;
	pButtonArray[4][1]->SetOnClickListener(onKeySpace);
	pRootWidget->AddChild(pButtonArray[4][1]);

	//del key
	pButtonArray[4][2]	= new RButton( x + KEY_CAP_W+KEY_SPACE_W, y + KEY_H * 3, KEY_DEL_W, KEY_DEL_H );
	pButtonArray[4][2]->SetId(id); id++;
	pButtonArray[4][2]->SetOnClickListener(onKeyDel);
	pRootWidget->AddChild(pButtonArray[4][2]);

	pWidget->pMyCtlObject = this;
}

void ProcKeyCapClick(Event* pevent, void* data){
	//INFO_X("ProcKeyCapClick");
	char keyText[2] = {0};
	
	if (caseState){
		for ( int countx = 0; countx < RKeyboard::firstLine; countx++ )
		{
			keyText[0] = keyMapLowercase[0][countx];
			pButtonArray[0][countx]->SetText(keyText);

		}
		for ( int countx = 0; countx < RKeyboard::secondLine; countx++ )
		{
			keyText[0] = keyMapLowercase[1][countx];
			pButtonArray[1][countx]->SetText(keyText);	
		}
		for ( int countx = 0; countx < RKeyboard::thirdLine; countx++ )
		{
			keyText[0] = keyMapLowercase[2][countx];
			pButtonArray[2][countx]->SetText(keyText);		
		}
		for ( int countx = 0; countx < RKeyboard::fourthLine; countx++ )
		{
			keyText[0] = keyMapLowercase[3][countx];
			pButtonArray[3][countx]->SetText(keyText);	
		}
		caseState =false;
		pButtonArray[4][0]->SetImagePic(pic_kb_ic_key_cap_lock_off_png);
	}
	else
	{
		
		for ( int countx = 0; countx < RKeyboard::firstLine; countx++ )
		{
			keyText[0] = keyMapCapital[0][countx];
			pButtonArray[0][countx]->SetText(keyText);

		}
		for ( int countx = 0; countx < RKeyboard::secondLine; countx++ )
		{
			keyText[0] = keyMapCapital[1][countx];
			pButtonArray[1][countx]->SetText(keyText);	
		}
		for ( int countx = 0; countx < RKeyboard::thirdLine; countx++ )
		{
			keyText[0] = keyMapCapital[2][countx];
			pButtonArray[2][countx]->SetText(keyText);		
		}
		for ( int countx = 0; countx < RKeyboard::fourthLine; countx++ )
		{
			keyText[0] = keyMapCapital[3][countx];
			pButtonArray[3][countx]->SetText(keyText);	
		}
		caseState = true;
		pButtonArray[4][0]->SetImagePic(pic_kb_ic_key_cap_lock_on_png);		
	}
		
}

void onKeyDel(Event* pevent, void* data){
	//INFO_X("onKeyDel = %s idx=%d", inputBuf, bufIndex);

	bufIndex --;
	if(bufIndex < 0)
	{
		bufIndex = 0;
		return;
	}

	//INFO_X("del char=%c", inputBuf[bufIndex]);
	inputBuf[bufIndex] = 0;
	
	if (onTextChangeListener != NULL)
	{
		onTextChangeListener(inputBuf);
	}
}

void onKeySpace(Event* pevent, void* data)
{
	//INFO_X("onKeySpace");
	
	if (bufIndex >= KB_MAX_BUF)
		return;

	inputBuf[bufIndex] = ' ';
	bufIndex ++;
	inputBuf[bufIndex] = 0;
	
	//SendKeyEvent(((RButton*)data)->GetId());
	if (onTextChangeListener != NULL)
	{
		onTextChangeListener(inputBuf);
	}
}

void OnKeyClick(Event* pevent, void* data)
{
	char *ptext = ((RButton*)data)->GetText();
	//INFO_X("on btn click text=%c", *ptext);

	if (bufIndex >= KB_MAX_BUF)
		return;	
	
	inputBuf[bufIndex] = *(ptext);
	bufIndex ++;
	inputBuf[bufIndex] = 0;
	
	if (onTextChangeListener != NULL)
	{
		onTextChangeListener(inputBuf);
	}
	//INFO_X("buf text=%s idx=%d", inputBuf, bufIndex);	
}

RKeyboard::~RKeyboard( void )
{

}

void RKeyboard::ResetKeyboard()
{
	inputBuf[0] = 0;
	bufIndex = 0;
}


void RKeyboard::InitKeyboardLayout(CtlObject* pParent)
{
	int r = 0, l = 0;
	for(r = 0;r < 2; r ++){
		for(l = 0;l < firstLine; l ++){
			//INFO_X("r=%d l=%d", r, l);
			//if (pButtonArray[r][l] != NULL){
			{
				pButtonArray[r][l]->LoadButtonPic(pic_kb_ic_key_pressed_png, pic_kb_ic_key_png);
				pButtonArray[r][l]->SetImageBltMode(ALPHA_STYLE);
			}
		}
	}


	for(l = 0;l < thirdLine; l ++){
		//INFO_X("r=%d l=%d", r, l);
		//if (pButtonArray[r][l] != NULL){
		{
			pButtonArray[2][l]->LoadButtonPic(pic_kb_ic_key_pressed_png, pic_kb_ic_key_png);
			pButtonArray[2][l]->SetImageBltMode(ALPHA_STYLE);
		}
	}

	for(l = 0;l < fourthLine; l ++){
		//INFO_X("r=%d l=%d", r, l);
		//if (pButtonArray[r][l] != NULL){
		{
			pButtonArray[3][l]->LoadButtonPic(pic_kb_ic_key_pressed_png, pic_kb_ic_key_png);
			pButtonArray[3][l]->SetImageBltMode(ALPHA_STYLE);
		}
	}
	//init special key
	//for cap key
	if(caseState)
		pButtonArray[4][0]->SetImagePic(pic_kb_ic_key_cap_lock_on_png);
	else
		pButtonArray[4][0]->SetImagePic(pic_kb_ic_key_cap_lock_off_png);
	pButtonArray[4][0]->SetImageBltMode(ALPHA_STYLE);
	//for space key
	pButtonArray[4][1]->LoadButtonPic(pic_kb_ic_key_space_bar_pressed_png, pic_kb_ic_key_space_bar_png);
	pButtonArray[4][1]->SetImageBltMode(ALPHA_STYLE);

	//for del key
	pButtonArray[4][2]->LoadButtonPic(pic_kb_ic_key_del_pressed_png, pic_kb_ic_key_del_png);
	pButtonArray[4][2]->SetImageBltMode(ALPHA_STYLE);
	
}

void RKeyboard::SetOnTextChange(onTextChange listener)
{

	onTextChangeListener = listener;
}


int RKeyboard::CtlFocus( int focus )
{
	return(0);
}

int RKeyboard::CtlEvent( Event* pEvent )
{
	INFO_X("ctlevent");
	return(0);
}

int RKeyboard::CtlShow( PIXEL* pDesBuf )
{
	MulImagePic* pPic;
	if ( Bgimagebuff != NULL )
	{
		AreaDraw( pDesBuf, pWidget->x + BgimageOffsetX, pWidget->y + BgimageOffsetY, Bgimagebuff, 0, 0,
			  DIFF( BgimageWidth, BgimageOffsetX, pWidget->width ), DIFF( BgimageHeight, BgimageOffsetY, pWidget->height ), BltMode );
	} else if ( BgColor != TRANSCOLOR )
	{
		AreaFill( pDesBuf, pWidget->x + BgimageOffsetX, pWidget->y + BgimageOffsetY, pWidget->width - BgimageOffsetX, pWidget->height - BgimageOffsetY, BgColor );
	}

	return(0);
}



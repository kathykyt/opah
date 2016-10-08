#include "interface2.h"
#include "BmpDataArray.h"
#include "freetype.h"

static CtlRoot* pRoot;
static Image* pBg;
static Keyboard* pKeyboard;
static Text* pText;
static FreeType2* pFreeType2;

static int keyboardCallback(Event* pEvent) {
    Any_Event *pany_event = NULL;
    printf("keyboardCallback\n");
    pany_event = &(pEvent->uEvent.any_event);
    if (pany_event->event_type == MsgTypeMouseUp) {
        const char* input;
        input = pKeyboard->GetInputStr();
        pBg->SetImageRefresh();
        pText->SetTextStr(input);
    }
    return 0;
}

static void LoadInterface2Bmp(void) {
    unsigned char* pic_textChangeCase_Delete[] = {
        pic_heater_work_time_1_png, pic_heater_xiaoshi, pic_heater_work_time_0_png,
    };
    unsigned char* pic_textblank[] = {
        pic_barcode_ppg_blank_png,
    };

    // call the freetype
    int count;
    int numOfWords;
    int width = 20;
    int height = 25;
    unsigned char* FT2Image = NULL;

    pFreeType2 = new FreeType2("/data/FreeSans.ttf", width, height);
    //a 1 A in Ox 61 30 41
    //Load a to z
    numOfWords = 26;
    unsigned char* pic_texta_z[numOfWords];
    for (count = 0; count < numOfWords; count++) {
        FT2Image = pFreeType2->getBmpData(0x061 + count);
        int length = FT2Image[4] > FT2Image[0] ? FT2Image[4] * FT2Image[4] : FT2Image[0] * FT2Image[0];
        pic_texta_z[count] = new unsigned char[length * 4];
        memset(pic_texta_z[count], 0, length * 4);
        memcpy(pic_texta_z[count], FT2Image, length * 4);
    }
    //Load 1 to 9
    numOfWords = 10;
    unsigned char* pic_text0_9[numOfWords];
    for (count = 0; count < numOfWords; count++) {
        FT2Image = pFreeType2->getBmpData(0x0030 + count);
        int length = FT2Image[4] > FT2Image[0] ? FT2Image[4] * FT2Image[4] : FT2Image[0] * FT2Image[0];
        pic_text0_9[count] = new unsigned char[length * 4];
        memset(pic_text0_9[count], 0, length * 4);
        memcpy(pic_text0_9[count], FT2Image, length * 4);
    }
    //Load A to Z
    numOfWords = 26;
    unsigned char* pic_textA_Z[numOfWords];
    for (count = 0; count < numOfWords; count++) {
        FT2Image = pFreeType2->getBmpData(0x0041 + count);
       int length = FT2Image[4] > FT2Image[0] ? FT2Image[4] * FT2Image[4] : FT2Image[0] * FT2Image[0];
        pic_textA_Z[count] = new unsigned char[length * 4];
        memset(pic_textA_Z[count], 0, length * 4);
        memcpy(pic_textA_Z[count], FT2Image, length * 4);
    }
    if (FT2Image) {
        delete [] FT2Image;
    }
    //freetype ends

    pBg->LoadImagePic(pic_tips_tips_8);
    unsigned int i;
    for (i = 0; i<sizeof (pic_text0_9) / sizeof (unsigned char*); i++)
        MulImageLoadPic(pKeyboard->pKeyboardPic, pic_text0_9[i], 0, 0, i);
    for (i = 0; i<sizeof (pic_textA_Z) / sizeof (unsigned char*); i++)
        MulImageLoadPic(pKeyboard->pKeyboardPic, pic_textA_Z[i], 0, 0, 10 + i);
    MulImageLoadPic(pKeyboard->pKeyboardPic, pic_textChangeCase_Delete[0], 0, 0, 36);
    MulImageLoadPic(pKeyboard->pKeyboardPic, pic_textChangeCase_Delete[1], 0, 0, 37);
    MulImageLoadPic(pKeyboard->pKeyboardPic, pic_textChangeCase_Delete[2], 0, 0, 38);
    for (i = 0; i<sizeof (pic_texta_z) / sizeof (unsigned char*); i++)
        MulImageLoadPic(pKeyboard->pKeyboardPic, pic_texta_z[i], 0, 0, 39 + i);
    pKeyboard->SetImageBltMode(ALPHA_STYLE);
    pKeyboard->CtlCallback(keyboardCallback);

    for (i = 0; i<sizeof (pic_text0_9) / sizeof (unsigned char*); i++)
        MulImageLoadPic(pText->pTextPic, pic_text0_9[i], 0, 0, '0' + i);
    for (i = 0; i<sizeof (pic_textA_Z) / sizeof (unsigned char*); i++)
        MulImageLoadPic(pText->pTextPic, pic_textA_Z[i], 0, 0, 'A' + i);
    for (i = 0; i<sizeof (pic_texta_z) / sizeof (unsigned char*); i++)
        MulImageLoadPic(pText->pTextPic, pic_texta_z[i], 0, 0, 'a' + i);
    for (i = 0; i<sizeof (pic_textblank) / sizeof (unsigned char*); i++)
        MulImageLoadPic(pText->pTextPic, pic_textblank[i], 0, 0, ' ' + i);

    pText->SetImageBltMode(ALPHA_STYLE);
}

void RegisterInterface2(CtlRoot* proot, const char* pInterfaceName) {
    pRoot = proot;
    pBg = new Image(0, 0, LCDW, LCDH);
    pKeyboard = new Keyboard(0, 0, 210, 125);
    pText = new Text(0, 150, 300, 40, 10, "aaaa");

    proot->CtlRootAddInterface(pBg, pInterfaceName);
    pBg->AddChild(pKeyboard);
    pBg->AddChild(pText);

    SendLoadBmpMsg(LoadInterface2Bmp, (void*) proot->CtlRootGetCtl(pInterfaceName));
}




#include "GUI.H" 
#include <stddef.h>           /* needed for definition of NULL */
#include "GUI_Private.h"
#include "GUI_Protected.h"

#include "RA8875.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       DB2SJIS
*/
static U16 DB2SJIS(U8 Byte0, U8 Byte1) {
  return Byte1 | (((U16)Byte0)<<8);
}

/*********************************************************************
*
*       _GetLineDistX_SJIS
*/
static int _GetLineDistX_SJIS(const char GUI_UNI_PTR *s, int Len) {
  int Dist =0;
  if (s) {
    U8 c0;
    while (((c0=*(const U8*)s) !=0) && Len >=0) {
      s++; Len--;
      if (c0 > 127) {
        U8  c1 = *(const U8*)s++;
        Len--;
        Dist += GUI_GetCharDistX(DB2SJIS(c0, c1));
      } else {
        Dist += GUI_GetCharDistX(c0);
      }
    }
  }
  return Dist;
}

/*********************************************************************
*
*       _GetLineLen_SJIS
* Purpose:
*   Returns the number of characters in a string.
*
* NOTE:
*   The return value can be used as offset into the
*   string, which means that double characters count double
*/
static int _GetLineLen_SJIS(const char GUI_UNI_PTR *s, int MaxLen) {
  int Len =0;
  U8 c0;
  while (((c0=*(const U8*)s) !=0) && Len < MaxLen) {
    s++;
    if (c0 > 127) {
      Len++; s++;
    } else {
      switch (c0) {
      case '\n': return Len;
      }
    }
    Len++;
  }
  return Len;
}

/*********************************************************************
*
*       _DispLine_SJIS
*/
static void _DispLine_SJIS(const char GUI_UNI_PTR *s, int Len)
{
	GUI_DRAWMODE DrawMode = GUI_Context.TextMode;
	GUI_DRAWMODE OldDrawMode;
	OldDrawMode  = LCD_SetDrawMode(DrawMode);
	LcdPrintStr((char *)s,GUI_Context.DispPosX, GUI_Context.DispPosY,BLACK,0,0);
	LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
}
/*********************************************************************
*
*       GUI_ENC_APIList_SJIS, API list
*/

const tGUI_ENC_APIList GT30L32S4W_GUI_ENC_APIList = {
  _GetLineDistX_SJIS,
  _GetLineLen_SJIS,
  _DispLine_SJIS
};


void GT30L32S4W_DispChar(U16P c) 
{
	FontSizeSpiflash(16);
	RA8875_LcdPrintWord(GUI_Context.DispPosX, GUI_Context.DispPosY,c);
	//GUI_Context.DispPosX +=10;
}

GUI_FLASH const GUI_CHARINFO GUI_FontHZbp_CharInfo[2] = 
{
	{ 8, 8, 1, (void GUI_FLASH *)0},
	{ 15, 15, 2, (void GUI_FLASH *)0},
};

GUI_FLASH const GUI_FONT_PROP GUI_FontHZbp_Propd6c6= {
      0xd6c6,
      0xd6c6,
      &GUI_FontHZbp_CharInfo[1],
      (void *)0 
};

GUI_CONST_STORAGE GUI_FONT GB2312_12x12 = {
  GT30L32S4W_DispChar,             
	GUIPROP_GetCharDistX,         \
	GUIPROP_GetFontInfo,          \
	GUIPROP_IsInFont,             \
  &GT30L32S4W_GUI_ENC_APIList /* type of font    */
  ,32                       /* height of font  */
  ,32                       /* space of font y */
  ,1                        /* magnification x */
  ,1                        /* magnification y */
  ,{(void*)0}
  ,25, 32, 32
};
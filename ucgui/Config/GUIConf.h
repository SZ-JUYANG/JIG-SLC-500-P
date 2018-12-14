#ifndef GUICONF_H
#define GUICONF_H

#define GUI_OS                    (1)  	/* Compile with multitasking support */
#define GUI_SUPPORT_TOUCH         (1)  	/* Support a touch screen (req. win-manager) */
#define GUI_SUPPORT_UNICODE       (1)  	/* Support mixed ASCII/UNICODE strings */
#define	GUI_SUPPORT_MEMDEV        (1)	/* Memory devices available */	
#define GUI_WINSUPPORT            (1)  	/* Window manager package available */
#define GUI_SUPPORT_AA            (0)  	/* Anti aliasing available */
#define GUI_ALLOC_SIZE            (1024*13)  /* Size of dynamic memory ... For WM and memory devices*/

//字库设置
//GUI_FontHZ_bp_15为宋体11的一些用到的字和ASCII
//去掉了不用的字库节约SRAM空间

	#if 1

		#define GUI_DEFAULT_FONT          &GUI_FontHZ16x16//GUI_Font6x8
		#define BUTTON_FONT_DEFAULT       &GUI_FontHZ16x16
		#define CHECKBOX_FONT_DEFAULT &GUI_FontHZ16x16
		#define DROPDOWN_FONT_DEFAULT &GUI_FontHZ16x16
		#define EDIT_FONT_DEFAULT &GUI_FontHZ16x16//&GUI_FontD32 //GUI_FontHZ_Times_New_Roman_32//&GUI_FontHZ_bp_15
		#define HEADER_FONT_DEFAULT &GUI_FontHZ16x16
		#define LISTBOX_FONT_DEFAULT &GUI_FontHZ16x16
		#define LISTVIEW_FONT_DEFAULT &GUI_FontHZ16x16
		#define MENU_FONT_DEFAULT &GUI_FontHZ16x16
		#define MULTIEDIT_FONT_DEFAULT &GUI_FontHZ16x16
		#define MULTIPAGE_FONT_DEFAULT &GUI_FontHZ16x16
		#define RADIO_FONT_DEFAULT &GUI_FontHZ16x16
		#define TEXT_FONT_DEFAULT &GUI_FontHZ16x16
		#define FRAMEWIN_DEFAULT_FONT &GUI_FontHZ16x16

	#else
	  
    #define GUI_DEFAULT_FONT          &GUI_FontHZ24x24//GUI_Font6x8
		#define BUTTON_FONT_DEFAULT       &GUI_FontHZ24x24
		#define CHECKBOX_FONT_DEFAULT &GUI_FontHZ24x24
		#define DROPDOWN_FONT_DEFAULT &GUI_FontHZ24x24
		#define EDIT_FONT_DEFAULT &GUI_FontHZ24x24//&GUI_FontD32 //GUI_FontHZ_Times_New_Roman_32//&GUI_FontHZ_bp_15
		#define HEADER_FONT_DEFAULT &GUI_FontHZ24x24
		#define LISTBOX_FONT_DEFAULT &GUI_FontHZ24x24
		#define LISTVIEW_FONT_DEFAULT &GUI_FontHZ24x24
		#define MENU_FONT_DEFAULT &GUI_FontHZ24x24
		#define MULTIEDIT_FONT_DEFAULT &GUI_FontHZ24x24
		#define MULTIPAGE_FONT_DEFAULT &GUI_FontHZ24x24
		#define RADIO_FONT_DEFAULT &GUI_FontHZ24x24
		#define TEXT_FONT_DEFAULT &GUI_FontHZ24x24
		#define FRAMEWIN_DEFAULT_FONT &GUI_FontHZ24x24
		
   #endif

#endif

#include <stddef.h>
#include <GUI.h>
#include <DIALOG.h>
#include <stm32f10x.h>

//�����ڻص�����
void OnMainDSClicked(WM_MESSAGE * pMsg);
void OnMainSJClicked(WM_MESSAGE * pMsg);
void OnMainLXClicked(WM_MESSAGE * pMsg);
void OnMainKZClicked(WM_MESSAGE * pMsg);
void OnMainFSClicked(WM_MESSAGE * pMsg);
void OnMainPowerClicked(WM_MESSAGE * pMsg);
//�����ڿؼ����ú���
void SetMainwindowDispValue(u16 * value);
void SetMainwindowPowerValue(u16 * value1);


//EventsFunctionList
void OnPowerSetClicked(WM_MESSAGE * pMsg);
//EndofEventsFunctionList
void ON_DISPPOWER_CLICKED();
void ON_DISPALL_CLICKED();

void BP_UCGUI_Task(void);


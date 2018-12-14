



#ifndef __BPTFT_H_
#define __BPTFT_H_

#include <include.h>
#include <bp_config.h>





void fsmc_init(void);
//void tft_gettouchpoint(int *touchx,int *touchy);
INT8U touch_calibration(int * x0,int * y0,int * x1,int *y1);


#endif /* __BPTFT_H */

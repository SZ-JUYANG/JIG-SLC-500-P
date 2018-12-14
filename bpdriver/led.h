#ifndef __BPLED_H
#define __BPLED_H

#include <include.h>
#include <bp_config.h>
#define led_on1 GPIO_SetBits(led_gpio,led1)
#define led_on2 GPIO_SetBits(led_gpio,led2)
#define led_on3 GPIO_SetBits(led_gpio,led3)
#define led_on4 GPIO_SetBits(led_gpio,led4)
#define led_off1 GPIO_ResetBits(led_gpio,led1)
#define led_off2 GPIO_ResetBits(led_gpio,led2)
#define led_off3 GPIO_ResetBits(led_gpio,led3)
#define led_off4 GPIO_ResetBits(led_gpio,led4)
#define led_turn1 GPIO_WriteBit(led_gpio, led1 , (BitAction)!GPIO_ReadOutputDataBit(led_gpio, led1)) 
#define led_turn2 GPIO_WriteBit(led_gpio, led2 , (BitAction)!GPIO_ReadOutputDataBit(led_gpio, led2))
#define led_turn3 GPIO_WriteBit(led_gpio, led3 , (BitAction)!GPIO_ReadOutputDataBit(led_gpio, led3))
#define led_turn4 GPIO_WriteBit(led_gpio, led4 , (BitAction)!GPIO_ReadOutputDataBit(led_gpio, led4))
void led_init(void);
void beep_run(u16);
void led_beep_test(void);


#endif /* __BPLED_H */

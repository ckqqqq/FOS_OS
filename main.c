/**
 * @file main.c
 * @author Sheeplin (sheeplin@bupt.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2020-November
 * 
 * @copyright Copyright (c) 2020  如来佛组
 * 
 */

#include "lib.h"
#include "printk.h"
#include "gate.h"
#include "trap.h"
#include "memory.h"


void Start_Kernel(void)
{
	int *addr = (int *)0xffff800000a00000;
	int i;

	Pos.XResolution = 1440;
	Pos.YResolution = 900;

	Pos.XPosition = 0;
	Pos.YPosition = 0;

	Pos.XCharSize = 8;
	Pos.YCharSize = 16;

	Pos.FB_addr = (int *)0xffff800000a00000;
	Pos.FB_length = (Pos.XResolution * Pos.YResolution * 4);
        init_memory();

	color_printk(YELLOW, BLACK, "Hello\t\t World!\n");
	color_printk(YELLOW, BLACK, "zhe shi yi ge tiao zhan\n");
	load_TR(8);

	set_tss64(0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00);
     
	sys_vector_init();

	memory_management_struct.start_code= (unsigned long)& _text;

	i = 1 / 0;
	//	i = *(int *)0xffff80000aa00000;

	while (1)
		;
}

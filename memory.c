

#include "memory.h"
#include "lib.h"

// tonight inatruct

void init_memory()
{
	int i,j;
	unsigned long TotalMem = 0 ;
	struct E820 *p = NULL;	
	
	color_printk(BLUE,BLACK,"SHOW Physical Address Allcation : Type ->1: RAM, ->2: ROM ,->3: ACPI Reclaim Memory, ->:4 ACPI NVS Memory \n");
	p = (struct E820 *)0xffff800000007e00;

	for(i = 0;i < 32;i++)
	{
		color_printk(GREEN,BLACK,"Address:%#018lx\tLength:%#018lx\tType:%#010x\n", p->address, p->length,p->type);
		unsigned long tmp = 0;
		if(p->type == 1)
			TotalMem +=  p->length;

		memory_management_struct.e820[i].address += p->address;
		memory_management_struct.e820[i].length	 += p->length;
		memory_management_struct.e820[i].type	 = p->type;
		memory_management_struct.e820_length = i;

		p++;
		if(p->type > 4 || p->length == 0 || p->type < 1)
			break;		
	}

	color_printk(YELLOW,BLACK,"Fuzu OS RAM:%#018lx\n",TotalMem);

	TotalMem = 0;

	for(i = 0; i <= memory_management_struct.e820_length; i++)
	{
		unsigned long start,end;
		if(memory_management_struct.e820[i].type != 1)
			continue;
        //align addr with 2m 
		start = PAGE_2M_ALIGN(memory_management_struct.e820[i].address);
        //cut low bits
		end   = ((memory_management_struct.e820[i].address + memory_management_struct.e820[i].length) >> PAGE_2M_SHIFT) << PAGE_2M_SHIFT;

		if(end <= start)
			continue;
        //end-start ->num of 2M page
		TotalMem += (end - start) >> PAGE_2M_SHIFT;
	}
	
	color_printk(INDIGO,BLACK,"OS Can Used Total 2M PAGEs:%#010x=%010d\n",TotalMem,TotalMem);
}
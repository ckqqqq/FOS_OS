

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "printk.h"
#include "lib.h"


//	8Bytes per cell
//fuzu os is a 64 bit os-> 8 bit each item & size of page is 4kb -> 512
#define PTRS_PER_PAGE	512

/*

*/
//page begin addr
#define PAGE_OFFSET	((unsigned long)0xffff800000000000)

// https://www.cnblogs.com/chenwb89/p/operating_system_003.html GDT:global descriptor
//power of 1GB 1<<39
#define PAGE_GDT_SHIFT	39
//1<<30
#define PAGE_1G_SHIFT	30
//1<<21 2mb page
#define PAGE_2M_SHIFT	21
//1<<12
#define PAGE_4K_SHIFT	12
//page size must be  power of 2
#define PAGE_2M_SIZE	(1UL << PAGE_2M_SHIFT)
#define PAGE_4K_SIZE	(1UL << PAGE_4K_SHIFT)
//PAGE_MASK获得在页中的offset ~-> "!"" by bit ->000001111->111110000 
#define PAGE_2M_MASK	(~ (PAGE_2M_SIZE - 1))
#define PAGE_4K_MASK	(~ (PAGE_4K_SIZE - 1))
//对齐 align phy addr by cut the low bit addr-> power of page size ('+'-> round up to an integer )
#define PAGE_2M_ALIGN(addr)	(((unsigned long)(addr) + PAGE_2M_SIZE - 1) & PAGE_2M_MASK)
#define PAGE_4K_ALIGN(addr)	(((unsigned long)(addr) + PAGE_4K_SIZE - 1) & PAGE_4K_MASK)
//as name 
#define Virt_To_Phy(addr)	((unsigned long)(addr) - PAGE_OFFSET)
//pointrt
#define Phy_To_Virt(addr)	((unsigned long *)((unsigned long)(addr) + PAGE_OFFSET))

#define Virt_To_2M_Page(kaddr)	(memory_management_struct.pages_struct + (Virt_To_Phy(kaddr) >> PAGE_2M_SHIFT))
#define Phy_to_2M_Page(kaddr)	(memory_management_struct.pages_struct + ((unsigned long)(kaddr) >> PAGE_2M_SHIFT))


////page table attribute shuxing

//对页各个位的定义
//	bit 63	Execution Disable:
#define PAGE_XD		(unsigned long)0x1000000000000000

//	bit 12	Page Attribute Table//?
#define	PAGE_PAT	(unsigned long)0x1000

//	bit 8	Global Page:1,global;0,part
#define	PAGE_Global	(unsigned long)0x0100

//	bit 7	Page Size:1,big page;0,small page;
#define	PAGE_PS		(unsigned long)0x0080

//	bit 6	Dirty:1,dirty;0,clean;
#define	PAGE_Dirty	(unsigned long)0x0040

//	bit 5	Accessed:1,visited;0,unvisited;
#define	PAGE_Accessed	(unsigned long)0x0020

//	bit 4	Page Level Cache Disable
#define PAGE_PCD	(unsigned long)0x0010

//	bit 3	Page Level Write Through
#define PAGE_PWT	(unsigned long)0x0008

//	bit 2	User Supervisor:1,user and supervisor;0,supervisor;
#define	PAGE_U_S	(unsigned long)0x0004

//	bit 1	Read Write:1,read and write;0,read;
#define	PAGE_R_W	(unsigned long)0x0002

//	bit 0	Present:1,present;0,no present;
#define	PAGE_Present	(unsigned long)0x0001

//1,0
#define PAGE_KERNEL_GDT		(PAGE_R_W | PAGE_Present)

//1,0	
#define PAGE_KERNEL_Dir		(PAGE_R_W | PAGE_Present)

//7,1,0
#define	PAGE_KERNEL_Page	(PAGE_PS  | PAGE_R_W | PAGE_Present)

//2,1,0
#define PAGE_USER_Dir		(PAGE_U_S | PAGE_R_W | PAGE_Present)

//7,2,1,0
#define	PAGE_USER_Page		(PAGE_PS  | PAGE_U_S | PAGE_R_W | PAGE_Present)
////


//help dqw???
typedef struct {unsigned long pml4t;} pml4t_t;
#define	mk_mpl4t(addr,attr)	((unsigned long)(addr) | (unsigned long)(attr))
#define set_mpl4t(mpl4tptr,mpl4tval)	(*(mpl4tptr) = (mpl4tval))


typedef struct {unsigned long pdpt;} pdpt_t;
#define mk_pdpt(addr,attr)	((unsigned long)(addr) | (unsigned long)(attr))
#define set_pdpt(pdptptr,pdptval)	(*(pdptptr) = (pdptval))


typedef struct {unsigned long pdt;} pdt_t;
#define mk_pdt(addr,attr)	((unsigned long)(addr) | (unsigned long)(attr))
#define set_pdt(pdtptr,pdtval)		(*(pdtptr) = (pdtval))


typedef struct {unsigned long pt;} pt_t;
#define mk_pt(addr,attr)	((unsigned long)(addr) | (unsigned long)(attr))
#define set_pt(ptptr,ptval)		(*(ptptr) = (ptval))

// addr struct
struct E820
{
	unsigned long address;
	unsigned long length;
	unsigned int	type;
}__attribute__((packed));

// GMD show blog
struct Global_Memory_Descriptor
{
	struct E820 e820[32];			//物理内存段结构数组
	unsigned long e820_length;		//物理内存段结构数组长度

	unsigned long * bits_map;		//物理地址空间页映射位图
	unsigned long bits_size;		//物理地址空间页数量
	unsigned long bits_length;		//物理地址空间页映射位图长度
	
	struct Page * pages_struct;		//指向全局struct page结构体数组的指针
	unsigned long pages_size;		//struct page结构体总数
	unsigned long pages_length;		//struct page结构体数组长度

	struct Zone * zones_struct;		//指向全局struct zone结构体数组的指针
	unsigned long zones_size;		//struct zone结构体数量
	unsigned long zones_length;		//struct zone结构体数组长度

	unsigned long start_code;		//内核程序的起始代码段地址
	unsigned long end_code;			//内核程序的结束代码段地址
	unsigned long end_data;			//内核程序的结束数据段地址
	unsigned long end_brk;			//内核程序的结束地址

	unsigned long end_of_struct;	//内存页管理结构的结尾地址
};


extern struct Global_Memory_Descriptor memory_management_struct;

void init_memory();

#endif

struct Page
{
	struct Zone * zone_struct;		//指向本页所属的的区域结构体
	unsigned long PHY_address;		//页的物理地址
	unsigned long attribute;		//页的属性

	unsigned long reference_count;	//描述的是该页的引用次数

	unsigned long age;				//描述的是该页的创建时间
};

struct Zone
{
	struct Page * pages_group;			//struct page结构体数组指针
	unsigned long pages_length;			//本区域包含的struct page结构体数量

	unsigned long zone_start_address;	//本区域的起始页对齐地址
	unsigned long zone_end_address;		//本区域的结束页对齐地址
	unsigned long zone_length;			//本区域经过页对齐后的地址长度
	unsigned long attribute;			//本区域空间的属性

	struct Global_Memory_Descriptor * GMD_struct;	//指向全局结构体struct Global_Memory_Descriptor

	unsigned long page_using_count;		//本区域已使用物理内存页数量
	unsigned long page_free_count;		//本区域空闲物理内存页数量

	unsigned long total_pages_link;		//本区域物理页被引用次数
};
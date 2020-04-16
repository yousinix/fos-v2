/* See COPYRIGHT for copyright information. */

#ifndef FOS_KERN_KCLOCK_H
#define FOS_KERN_KCLOCK_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

#define	IO_RTC		0x070		/* RTC port */

#define	MC_NVRAM_START	0xe	/* start of NVRAM: offset 14 */
#define	MC_NVRAM_SIZE	50	/* 50 bytes of NVRAM */

/* NVRAM bytes 7 & 8: base memory size */
#define NVRAM_BASELO	(MC_NVRAM_START + 7)	/* low byte; RTC off. 0x15 */
#define NVRAM_BASEHI	(MC_NVRAM_START + 8)	/* high byte; RTC off. 0x16 */

/* NVRAM bytes 9 & 10: extended memory size */
#define NVRAM_EXTLO	(MC_NVRAM_START + 9)	/* low byte; RTC off. 0x17 */
#define NVRAM_EXTHI	(MC_NVRAM_START + 10)	/* high byte; RTC off. 0x18 */

/* NVRAM bytes 34 and 35: extended memory POSTed size */
#define NVRAM_PEXTLO	(MC_NVRAM_START + 34)	/* low byte; RTC off. 0x30 */
#define NVRAM_PEXTHI	(MC_NVRAM_START + 35)	/* high byte; RTC off. 0x31 */

/* NVRAM byte 36: current century.  (please increment in Dec99!) */
#define NVRAM_CENTURY	(MC_NVRAM_START + 36)	/* RTC offset 0x32 */

unsigned mc146818_read(unsigned reg);
void mc146818_write(unsigned reg, unsigned datum);

void kclock_start(uint8 quantum_in_ms);
void kclock_stop(void);
void kclock_resume(void);
void kclock_start_counter(uint8 cnt0);

uint16 kclock_read_cnt0(void);
uint16 kclock_read_cnt0_latch(void);

//2017
void kclock_write_cnt0_LSB_first(uint16 val);

//2018
void kclock_set_quantum(uint8 quantum_in_ms);


extern uint32 virtualTime;

//__inline struct uint64 get_virtual_time() __attribute__((always_inline));
static inline __attribute__((always_inline)) struct uint64
get_virtual_time()
{

/*
 * 	uint16 cx;
	uint16 dx;
		__asm __volatile("int %3\n"
		: "=c" (cx), "=d" (dx)
		: "a" (0),
		  "i" (26)
		  //: "ax", "cx", "dx"
		);
		uint32 result = (cx<<16) | dx;
*/
	//	uint32 oldVal = rcr4();
	//	lcr4(0);


	struct uint64 result;

	__asm __volatile("rdtsc\n"
	: "=a" (result.low), "=d" (result.hi)
	);

	/*
	uint32 low;
	uint32 hi;
	uint32 cx,eaxp,ebxp,ecxp,edxp ;
	//; read APERF
	cpuid(6, &eaxp, &ebxp, &ecxp, &edxp);

//	__asm __volatile("movl 6, %eax\ncpuid\n"//bt %%ecx, 0\nmov %%ecx,%0"
//		//	:	"=c" (cx)
//			//: "=a" (low), "=d" (hi)
//		);

//	__asm __volatile("rdmsr"
//					: "=a" (low), "=d" (hi)
//			);


	//char* ptr=(char*)&ecxp;
	//ptr[3]=0;
	//cprintf("as str = %s\n", ptr);
	cprintf("ax = %x, bx = %x, cx = %x, dx = %x\n", eaxp,ebxp,ecxp,edxp);
	*/

	return result;
}

#endif	// !FOS_KERN_KCLOCK_H

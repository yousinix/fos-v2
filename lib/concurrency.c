#include <inc/lib.h>
#include <inc/timerreg.h>

void
env_sleep(uint32 approxMilliSeconds)
{
//	cprintf("%s go to sleep...\n", myEnv->prog_name);
	uint32 time_in_cycles=approxMilliSeconds*CYCLES_PER_MILLISEC;
	uint32 cycles_counter =0;

	struct uint64 baseTime = sys_get_virtual_time() ;
	while(cycles_counter<time_in_cycles)
	{
		struct uint64 currentTime = sys_get_virtual_time() ;

		// update the cycles_count
		#define M32 0xffffffff
		// subtract basetime from current time
		struct uint64 res;
		res.low = (currentTime.low - baseTime.low) & M32;
		res.hi = (currentTime.hi - baseTime.hi - (res.low > currentTime.low)) & M32;

		//update cycles_count with result
		cycles_counter = res.low;

//		cprintf("t=%u - %u, cur = %u- %u, sub = %u - %u, cc= %u\n"
//				,baseTime.hi, baseTime.low
//				,currentTime.hi, currentTime.low
//				,res.hi, res.low
//				,cycles_counter
//				);
	}
	//cprintf("%s [%d] wake up now!\n", myEnv->prog_name, myEnv->env_id);

}

//2017
uint32 busy_wait(uint32 loopMax)
{
	uint32 i = 0 ;
	while (i < loopMax) i++;
	return i;
}

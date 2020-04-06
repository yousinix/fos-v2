/*
 * Minimal PIO-based (non-interrupt-driven) IDE driver code.
 * For information about what all this IDE/ATA magic means,
 * see the materials available on the class references page.
 */

#include <inc/disk.h>
#include <inc/x86.h>

#define IDE_BSY		0x80
#define IDE_DRDY	0x40
#define IDE_DF		0x20
#define IDE_ERR		0x01

static int diskno = 0;

static int ide_wait_ready(bool check_error)
{
	int r;

	while (((r = inb(0x1F7)) & (IDE_BSY|IDE_DRDY)) != IDE_DRDY)
		/* do nothing */;


	if (check_error && (r & (IDE_DF|IDE_ERR)) != 0)
	{
 		LOG_STATMENT(cprintf("ERROR @ ide_wait_ready() = %x(%d)\n",r,r););
		return -1;
	}
	return 0;
}

int	ide_read(uint32 secno, void *dst, uint32 nsecs)
{
	int r;

	assert(nsecs <= 256);

	//TODO: This BUSY-WAIT should be replaced by Interrupt to allow the OS to schedule another process till the device become ready [el7 :)]
	ide_wait_ready(0);

	outb(0x1F2, nsecs);
	outb(0x1F3, secno & 0xFF);
	outb(0x1F4, (secno >> 8) & 0xFF);
	outb(0x1F5, (secno >> 16) & 0xFF);
	outb(0x1F6, 0xE0 | ((diskno&1)<<4) | ((secno>>24)&0x0F));
	outb(0x1F7, 0x20);	// CMD 0x20 means read sector

	for (; nsecs > 0; nsecs--, dst += SECTSIZE) {
		if ((r = ide_wait_ready(1)) < 0)
			return r;
		insl(0x1F0, dst, SECTSIZE/4);
	}

	return 0;
}

int ide_write(uint32 secno, const void *src, uint32 nsecs)
{
	int r;

	//LOG_STATMENT(cprintf("1 ==> nsecs = %d\n",nsecs);)
	assert(nsecs <= 256);

	//LOG_STATMENT(cprintf("2\n");)
	ide_wait_ready(0);

	//LOG_STATMENT(cprintf("3 ==> nsecs = %d\n",nsecs);)
	outb(0x1F2, nsecs);
	outb(0x1F3, secno & 0xFF);
	outb(0x1F4, (secno >> 8) & 0xFF);
	outb(0x1F5, (secno >> 16) & 0xFF);
	outb(0x1F6, 0xE0 | ((diskno&1)<<4) | ((secno>>24)&0x0F));
	outb(0x1F7, 0x30);	// CMD 0x30 means write sector


	for (; nsecs > 0; nsecs--, src += SECTSIZE) {
		if ((r = ide_wait_ready(1)) < 0)
		{
			LOG_STATMENT(cprintf("FAILURE to write %d sectors to disk\n",nsecs););
			return r;
		}
		else
		{
			outsl(0x1F0, src, SECTSIZE/4);
			//LOG_STATMENT(cprintf("written %d sectors to disk successfully\n",nsecs););
		}
	}
	//LOG_STATMENT(cprintf("5\n");)
	//cprintf("returning from ide_write \n");

	return 0;
}


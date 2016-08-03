#include <asm/types.h>
#include <asm/mtrr.h>
#include <asm/p2m.h>
#include <asm/hvm/vmx/vmx.h>
#include <asm/hvm/vmx/vvmx.h>
#include <asm/hvm/nestedhvm.h>



//#include <include/xen/kernel.h>


static unsigned int temp=0;
unsigned int mCR3=0;

void mget_cr()
{
	printk("my hypercall\n");

	if(temp != mCR3){
		printk("load CR3: %x\n",mCR3);
		temp = mCR3;
	}

}

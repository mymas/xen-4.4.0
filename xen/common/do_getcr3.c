#include <xen/hypercall.h>
#include <xen/lib.h>
#include <asm-x86/hvm/vmx/vvmx.h>
#include <xen/guest_access.h>


extern unsigned long guest_cr3;

#ifdef orig_miyama
uint64_t do_getCR3(void)
{
	printk("m_val : %lx\n",m_val);
	return m_val;
}
#endif

//extern void *hvm_map_guest_frame2(struct domain *d, unsigned long gfn,bool_t writable, bool_t permanent);

struct get_cr3_arg {
    unsigned long cr3;
};

long do_get_cr3(XEN_GUEST_HANDLE_PARAM(void) uarg)
{
    struct get_cr3_arg arg;

    if (guest_cr3 == 0)
        return -EFAULT;

    arg.cr3 = guest_cr3;

    if (copy_to_guest(uarg, &arg, 1))
        return -EFAULT;

    return 0;
}

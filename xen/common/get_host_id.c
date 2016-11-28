
#include <xen/hypercall.h>
#include <xen/lib.h>
#include <asm-x86/hvm/vmx/vvmx.h>
#include <xen/guest_access.h>

extern domid_t target_domid;

unsigned short do_get_host_id(void)
{
	return target_domid;
}

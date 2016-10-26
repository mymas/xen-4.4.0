#include <xen/hypercall.h>
#include <xen/lib.h>
#include <asm-x86/hvm/vmx/vvmx.h>
#include <xen/guest_access.h>

#define MIYAMA_VM_LIST

extern unsigned long guest_cr3;
#ifdef MIYAMA_VM_LIST
struct VM_LIST{
	int target_domain_id;
	unsigned long target_cr3;
	unsigned long target_ept;
	struct VM_LIST *next;
};

extern struct VM_LIST *VM_TOP;
#define VM_LIST_TAIL NULL
#endif

struct get_cr3_arg {
	int dom_id;
	unsigned long cr3;
};

long do_get_cr3(XEN_GUEST_HANDLE_PARAM(void) uarg)
{
	struct get_cr3_arg arg;
	struct VM_LIST *vm_list;
	int dom_id;

	if (guest_cr3 == 0)
		return -EFAULT;


	if (copy_from_guest(&arg, uarg, 1))
		return -EFAULT;

#ifdef MIYAMA_VM_LIST
	dom_id = arg.dom_id;

	vm_list = VM_list_search_by_domid(dom_id);
	if(vm_list != 0){
		printk("vm_list cr3 %lx \n", vm_list->target_cr3);
	}else{
		printk("error.....\n");
	}
#endif // MIYAMA_VM_LIST

#ifdef MIYAMA_VM_LIST
	arg.cr3 = vm_list->target_cr3;
#else
	arg.cr3 = guest_cr3;
#endif

	if (copy_to_guest(uarg, &arg, 1))
		return -EFAULT;

	return 0;
}

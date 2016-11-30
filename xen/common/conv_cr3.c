#include <xen/lib.h>
#include <asm-x86/hvm/vmx/vmx.h>
#include <asm-x86/hvm/hvm.h>
#include <asm/hvm/hvm.h>
#include <asm-x86/hvm/nestedhvm.h>
#include <xen/sched.h>
#include <xen/hypercall.h>
#include <xen/guest_access.h>
#include <asm-x86/hvm/vmx/vvmx.h>

#define MIYAMA_VM_LIST
extern domid_t target_domid;
extern uint64_t target_eptp;
extern int nept_translate(struct domain *d, uint64_t eptp, paddr_t l2ga, unsigned long *l1gfn);

struct conv_ept_arg {
	unsigned long dom_tag;
	unsigned long l2gfn;  /* IN */
	unsigned long l1gfn;  /* OUT */
};

#ifdef MIYAMA_VM_LIST
struct VM_LIST{
	unsigned long dom_tag;
	unsigned long target_cr3;
	unsigned long target_ept;
	struct VM_LIST *next;
};
extern struct VM_LIST *VM_TOP;
#define VM_LIST_TAIL NULL
#endif//MIYAMA_VM_LIST

unsigned long do_conv_ept(XEN_GUEST_HANDLE_PARAM(void) uarg)
{
#ifdef MIYAMA_VM_LIST
	struct VM_LIST *vm_list;
	unsigned long dom_tag;
#endif
	struct conv_ept_arg arg;
	unsigned long l2gfn, l1gfn;
	struct domain *d;
	int rc;

	if (target_domid == 0 || target_eptp == 0)
		return -EFAULT;

	if (copy_from_guest(&arg, uarg, 1))
		return -EFAULT;

	d = rcu_lock_domain_by_id(target_domid);
	if (d == NULL)
		return -ENOENT;

	l2gfn = arg.l2gfn;

#ifdef MIYAMA_VM_LIST
	dom_tag = arg.dom_tag;
	//ここでIDを使って、target_eptpを検索をする
	vm_list = VM_list_search_by_domtag(dom_tag);
	//printk("dom_id: %d, target_domid %d \n", dom_id, target_domid);
	
	if(vm_list != NULL){
		printk("dom_id:%lx, target_ept%lx\n", vm_list->dom_tag, vm_list->target_ept);
	}else{
		printk("conv_cr3 hypercall error....\n");
	}
#else
	printk("target_eptp %lx\n", target_eptp);
#endif
	rc = nept_translate(d, vm_list->target_ept, l2gfn << PAGE_SHIFT, &l1gfn);
	rcu_unlock_domain(d);

	if (rc)
		return -EINVAL;

	arg.l1gfn = l1gfn;

	if (copy_to_guest(uarg, &arg, 1))
		return -EFAULT;

	return 0;
}

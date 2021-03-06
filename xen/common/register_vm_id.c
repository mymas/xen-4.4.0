#include <xen/lib.h>
#include <asm-x86/hvm/vmx/vmx.h>
#include <asm-x86/hvm/hvm.h>
#include <asm/hvm/hvm.h>
#include <asm-x86/hvm/nestedhvm.h>
#include <xen/sched.h>
#include <xen/hypercall.h>
#include <xen/guest_access.h>

#define MIYAMA_VM_LIST

#ifdef MIYAMA_VM_LIST
struct VM_LIST{
	unsigned long dom_tag;
	unsigned long target_cr3;
	unsigned long target_ept;
	struct VM_LIST *next;
};
extern struct VM_LIST *VM_TOP;
#define VM_LIST_TAIL NULL
#define VM_ID_FLAG 0
struct VM_LIST * VM_list_update_id(unsigned long vm_tag, unsigned long target_eptp);
// IDが０の構造体を返す
struct VM_LIST * VM_list_update_id(unsigned long vm_tag, unsigned long target_eptp){
	struct VM_LIST *p;
	for(p = VM_TOP; p!= VM_LIST_TAIL; p=p->next){
		if(p->target_ept == target_eptp){
			p->dom_tag = vm_tag;
			printk("vm_tag %lx\n",p->dom_tag);
			return p;
		}
	}
	return 0;
}

int do_register_vm_id(unsigned long vm_tag, unsigned long target_eptp){
	struct VM_LIST *rc;
	rc = VM_list_update_id(vm_tag, target_eptp);

	if (rc)
		return -EINVAL;

	if(rc != NULL){
		return 1;
	}else{
		return 0;
	}
	//return rc;
}
#endif /*MIYAMA_VM_LIST*/

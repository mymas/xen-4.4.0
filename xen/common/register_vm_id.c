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
	int target_domain_id;
	unsigned long target_cr3;
	unsigned long target_ept;
	struct VM_LIST *next;
};
//struct VM_LIST *VM_TOP = NULL;
extern struct VM_LIST *VM_TOP;
#define VM_LIST_TAIL NULL
#define VM_ID_FLAG 0
struct VM_LIST * VM_list_update_id(int vm_id, unsigned long target_eptp);
// IDが０の構造体を返す
struct VM_LIST * VM_list_update_id(int vm_id,unsigned long target_eptp){
	struct VM_LIST *p;
	for(p = VM_TOP; p!= VM_LIST_TAIL; p=p->next){
		if(p->target_ept == target_eptp){
			p->target_domain_id = vm_id;
			return p;
		}
	}
	return 0;
}

#ifdef maybe_not_need
struct vm_id_arg {
	int vm_id;
};
#endif

int do_register_vm_id(int vm_id, unsigned long target_eptp){
	struct VM_LIST *rc;
	rc = VM_list_update_id(vm_id,target_eptp);
	printk("dom_id %d cr3 %lx ept %lx\n",rc->target_domain_id,rc->target_cr3,rc->target_ept);

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

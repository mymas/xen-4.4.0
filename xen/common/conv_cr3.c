#include <xen/lib.h>
//#include <asm-x86/hvm/vmx/vvmx.h>
#include <asm-x86/hvm/vmx/vmx.h>
#include <asm-x86/hvm/hvm.h>
#include <asm/hvm/hvm.h>
#include <asm-x86/hvm/nestedhvm.h>
#include <xen/sched.h>
#include <xen/hypercall.h>
#include <xen/guest_access.h>


//#define check
//#define MIYAMA
//#define ORIG

extern domid_t target_domid;
extern uint64_t target_eptp;
extern int nept_translate(struct domain *d, uint64_t eptp, paddr_t l2ga,
                          unsigned long *l1gfn);
#ifdef ORIG
extern uint64_t target_gpa;
uint64_t do_conv_cr3(uint64_t val) 
{
    struct domain *d;
    //struct vcpu *v = NULL;
    struct vcpu v;
    //struct nestedvcpu *nvcpu = &vcpu_nestedhvm(v);
    struct nestedvcpu *nvcpu = &vcpu_nestedhvm(&v);
#ifdef check
    uint8_t p2m_acc;
    uint64_t exit_qual;
    uint32_t exit_reason;
    unsigned int page_order;
    uint64_t ret_l1gfn;
#endif
    printk("do_conv_cr3\n");
    d = get_domain_by_id(target_domid);
    printk("do_conv_cr3\n");
    //v->domain = d;
    v.domain = d;
    nvcpu->nv_vvmcx = hvm_map_guest_frame2(d, target_gpa >> PAGE_SHIFT, 1, 1);
    printk("dom_id %d, target_gpa %08lx, target_domid %d, vvmcx %lx\n", d->domain_id, target_gpa,target_domid,(unsigned long)nvcpu->nv_vvmcx);
    //printk("conv %lu\n", ret_l1gfn);
    nept_translate_l2ga(&v, val, &m_page_order, 0, &m_l1gfn, &m_p2m_acc, &m_exit_qual, &m_exit_reason);
    //nept_translate_l2ga(v, val, &m_page_order, 0, &m_l1gfn, &p2m_acc, &exit_qual, &exit_reason);
    hvm_unmap_guest_frame(nvcpu->nv_vvmcx, 1);
    put_domain(d);
    //return ret_l1gfn;
    return m_l1gfn;
}
#endif



#ifdef MIYAMA
extern int nept_translate(struct domain *d, uint64_t eptp, paddr_t l2ga, unsigned long *l1gfn);
extern uint64_t target_eptp;
//uint64_t do_conv_cr3(uint64_t val)
int do_conv_cr3(uint64_t paddr, unsigned long l1gfn)
{
   // struct conv_ept_arg arg;
    unsigned long l2gfn;
    struct domain *d;
    int rc;

    if (target_domid == 0 || target_eptp == 0){
        return -EFAULT;
    }
    d = get_domain_by_id(target_domid);

    if (d == NULL){
        return -ENOENT;
    }

    // hvm_map_guest_frame2 は使わない
    l2gfn = paddr;
    //rc = nept_translate(d, target_eptp, l2gfn << PAGE_SHIFT, &l1gfn);
    rc = nept_translate(d, target_eptp, l2gfn, &l1gfn);
    // hvm_unmap_guest_frame は使わない
    put_domain(d);
    if (rc){
        return -EINVAL;
    }
    /*arg.l1gfn = l1gfn;

    if (copy_to_guest(uarg, &arg, 1)){
        return -EFAULT;
    }*/
    printk("l2gfn %lx, l1gfn %lx, target_eptp %lx\n", l2gfn, l1gfn, target_eptp);
    //return l1gfn;
    return rc;
}
#endif

struct conv_ept_arg {
    unsigned long l2gfn;  /* IN */
    unsigned long l1gfn;  /* OUT */
};

unsigned long do_conv_ept(XEN_GUEST_HANDLE_PARAM(void) uarg)
{
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

    rc = nept_translate(d, target_eptp, l2gfn << PAGE_SHIFT, &l1gfn);

    rcu_unlock_domain(d);

    if (rc)
        return -EINVAL;

    arg.l1gfn = l1gfn;

    if (copy_to_guest(uarg, &arg, 1))
        return -EFAULT;

    return 0;
}

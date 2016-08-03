#define MIYAMA

#ifdef MIYAMA
#include <xen/hypercall.h>
#include <xen/guest_access.h>

unsigned long guest_cr3;
domid_t target_domid;
unsigned long vmcs_gfn;  /* L2 VMCS gfn */

extern void *hvm_map_guest_frame2(struct domain *d, unsigned long gfn,bool_t writable, bool_t permanent);

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

struct conv_ept_arg {
    unsigned long l2gfn;
    unsigned long l1gfn;
};

unsigned long do_conv_ept(XEN_GUEST_HANDLE_PARAM(void) uarg)
{
    struct conv_ept_arg arg;
    unsigned long l2gfn, l1gfn;
    struct domain *d;
    struct vcpu v;
    struct nestedvcpu *nvcpu = &vcpu_nestedhvm(&v);
    unsigned int page_order;
    uint8_t p2m_acc;
    uint64_t exit_qual;
    uint32_t exit_reason;
    int rc;

    if (target_domid == 0 || vmcs_gfn == 0)
        return -EFAULT;

    //printk("domid: %u, vmcs: %lx\n", target_domid, vmcs_gfn);

    if (copy_from_guest(&arg, uarg, 1))
        return -EFAULT;

    d = get_domain_by_id(target_domid);
    if (d == NULL)
        return 0;

    /* map VMCS in a host VM */
    nvcpu->nv_vvmcx = hvm_map_guest_frame2(d, vmcs_gfn, 1, 1);

    //printk("eptp: %lx\n", nvmx_vcpu_eptp_base(&v));

    v.domain = d;
    l2gfn = arg.l2gfn;

    rc = nept_translate_l2ga(&v, l2gfn << PAGE_SHIFT, &page_order, 0, &l1gfn,
                             &p2m_acc, &exit_qual, &exit_reason);

    //printk("l2: %lx, l1: %lx\n", l2gfn, l2gfn);

    hvm_unmap_guest_frame(nvcpu->nv_vvmcx, 1);
    put_domain(d);

    if (rc == 0) {
        arg.l1gfn = l1gfn;

        if (copy_to_guest(uarg, &arg, 1))
            return -EFAULT;
    }

    return rc;
}
#endif /* MIYAMA */

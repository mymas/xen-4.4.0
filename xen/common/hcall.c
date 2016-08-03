#include <xen/hypercall.h>
#include <xen/guest_access.h>

extern int nept_translate(struct domain *d, uint64_t eptp, paddr_t l2ga,
                          unsigned long *l1gfn);

extern u64 guest_cr3;
extern domid_t target_domid;
extern uint64_t target_eptp;

struct get_cr3_arg {
    unsigned long cr3;  /* OUT */
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libxl.h"
#include "libxl_utils.h"

static char *rand_str(void)
{
    int i, sz = rand() % 32;
    char *s = malloc(sz+1);
    for (i=0; i<sz; i++)
        s[i] = 'a' + (rand() % 26);
    s[i] = '\0';
    return s;
}

static void rand_bytes(uint8_t *p, size_t sz)
{
    int i;
    for (i=0; i<sz; i++)
        p[i] = rand() % 256;
}

static void libxl_bitmap_rand_init(libxl_bitmap *bitmap)
{
    int i;
    bitmap->size = rand() % 16;
    bitmap->map = calloc(bitmap->size, sizeof(*bitmap->map));
    libxl_for_each_bit(i, *bitmap) {
        if (rand() % 2)
            libxl_bitmap_set(bitmap, i);
        else
            libxl_bitmap_reset(bitmap, i);
    }
}

static void libxl_key_value_list_rand_init(libxl_key_value_list *pkvl)
{
    int i, nr_kvp = rand() % 16;
    libxl_key_value_list kvl = calloc(nr_kvp+1, 2*sizeof(char *));

    for (i = 0; i<2*nr_kvp; i += 2) {
        kvl[i] = rand_str();
        if (rand() % 8)
            kvl[i+1] = rand_str();
        else
            kvl[i+1] = NULL;
    }
    kvl[i] = NULL;
    kvl[i+1] = NULL;
    *pkvl = kvl;
}

static void libxl_cpuid_policy_list_rand_init(libxl_cpuid_policy_list *pp)
{
    int i, nr_policies = rand() % 16;
    struct {
        const char *n;
        int w;
    } options[] = {
      /* A random selection from libxl_cpuid_parse_config */
        {"maxleaf",     32},
        {"family",       8},
        {"model",        8},
        {"stepping",     4},
        {"localapicid",  8},
        {"proccount",    8},
        {"clflush",      8},
        {"brandid",      8},
        {"f16c",         1},
        {"avx",          1},
        {"osxsave",      1},
        {"xsave",        1},
        {"aes",          1},
        {"popcnt",       1},
        {"movbe",        1},
        {"x2apic",       1},
        {"sse4.2",       1},
        {"sse4.1",       1},
        {"dca",          1},
        {"pdcm",         1},
        {"procpkg",      6},
    };
    const int nr_options = sizeof(options)/sizeof(options[0]);
    char buf[64];
    libxl_cpuid_policy_list p = NULL;

    for (i = 0; i < nr_policies; i++) {
        int opt = rand() % nr_options;
        int val = rand() % (1<<options[opt].w);
        snprintf(buf, 64, "%s=%#x", options[opt].n, val);
        libxl_cpuid_parse_config(&p, buf);
    }
    *pp = p;
}

static void libxl_string_list_rand_init(libxl_string_list *p)
{
    int i, nr = rand() % 16;
    libxl_string_list l = calloc(nr+1, sizeof(char *));

    for (i = 0; i<nr; i++) {
        l[i] = rand_str();
    }
    l[i] = NULL;
    *p = l;
}
static void libxl_defbool_rand_init(libxl_defbool *p);
static void libxl_defbool_rand_init(libxl_defbool *p)
{
    libxl_defbool_set(p, !!rand() % 1);
}

static void libxl_domid_rand_init(libxl_domid *p);
static void libxl_domid_rand_init(libxl_domid *p)
{
    *p = rand() % (sizeof(*p)*8);
}

static void libxl_devid_rand_init(libxl_devid *p);
static void libxl_devid_rand_init(libxl_devid *p)
{
    *p = rand() % (sizeof(*p)*8);
}

static void libxl_uuid_rand_init(libxl_uuid *p);
static void libxl_uuid_rand_init(libxl_uuid *p)
{
    rand_bytes((uint8_t *)p, sizeof(*p));
}

static void libxl_mac_rand_init(libxl_mac *p);
static void libxl_mac_rand_init(libxl_mac *p)
{
    rand_bytes((uint8_t *)p, sizeof(*p));
}

static void libxl_hwcap_rand_init(libxl_hwcap *p);
static void libxl_hwcap_rand_init(libxl_hwcap *p)
{
    rand_bytes((uint8_t *)p, sizeof(*p));
}

static void libxl_ev_link_rand_init(libxl_ev_link *p);
static void libxl_ev_link_rand_init(libxl_ev_link *p)
{
}

static void libxl_error_rand_init(libxl_error *p);
static void libxl_error_rand_init(libxl_error *p)
{
    *p = ERROR_UNKNOWN_CHILD;
}

static void libxl_domain_type_rand_init(libxl_domain_type *p);
static void libxl_domain_type_rand_init(libxl_domain_type *p)
{
    *p = LIBXL_DOMAIN_TYPE_PV;
}

static void libxl_device_model_version_rand_init(libxl_device_model_version *p);
static void libxl_device_model_version_rand_init(libxl_device_model_version *p)
{
    *p = LIBXL_DEVICE_MODEL_VERSION_UNKNOWN;
}

static void libxl_console_type_rand_init(libxl_console_type *p);
static void libxl_console_type_rand_init(libxl_console_type *p)
{
    *p = LIBXL_CONSOLE_TYPE_UNKNOWN;
}

static void libxl_disk_format_rand_init(libxl_disk_format *p);
static void libxl_disk_format_rand_init(libxl_disk_format *p)
{
    *p = LIBXL_DISK_FORMAT_QCOW2;
}

static void libxl_disk_backend_rand_init(libxl_disk_backend *p);
static void libxl_disk_backend_rand_init(libxl_disk_backend *p)
{
    *p = LIBXL_DISK_BACKEND_QDISK;
}

static void libxl_nic_type_rand_init(libxl_nic_type *p);
static void libxl_nic_type_rand_init(libxl_nic_type *p)
{
    *p = LIBXL_NIC_TYPE_VIF;
}

static void libxl_action_on_shutdown_rand_init(libxl_action_on_shutdown *p);
static void libxl_action_on_shutdown_rand_init(libxl_action_on_shutdown *p)
{
    *p = LIBXL_ACTION_ON_SHUTDOWN_RESTART;
}

static void libxl_trigger_rand_init(libxl_trigger *p);
static void libxl_trigger_rand_init(libxl_trigger *p)
{
    *p = LIBXL_TRIGGER_RESET;
}

static void libxl_tsc_mode_rand_init(libxl_tsc_mode *p);
static void libxl_tsc_mode_rand_init(libxl_tsc_mode *p)
{
    *p = LIBXL_TSC_MODE_NATIVE;
}

static void libxl_timer_mode_rand_init(libxl_timer_mode *p);
static void libxl_timer_mode_rand_init(libxl_timer_mode *p)
{
    *p = LIBXL_TIMER_MODE_NO_MISSED_TICKS_PENDING;
}

static void libxl_bios_type_rand_init(libxl_bios_type *p);
static void libxl_bios_type_rand_init(libxl_bios_type *p)
{
    *p = LIBXL_BIOS_TYPE_UNKNOWN;
}

static void libxl_scheduler_rand_init(libxl_scheduler *p);
static void libxl_scheduler_rand_init(libxl_scheduler *p)
{
    *p = LIBXL_SCHEDULER_ARINC653;
}

static void libxl_shutdown_reason_rand_init(libxl_shutdown_reason *p);
static void libxl_shutdown_reason_rand_init(libxl_shutdown_reason *p)
{
    *p = LIBXL_SHUTDOWN_REASON_WATCHDOG;
}

static void libxl_vga_interface_type_rand_init(libxl_vga_interface_type *p);
static void libxl_vga_interface_type_rand_init(libxl_vga_interface_type *p)
{
    *p = LIBXL_VGA_INTERFACE_TYPE_CIRRUS;
}

static void libxl_vendor_device_rand_init(libxl_vendor_device *p);
static void libxl_vendor_device_rand_init(libxl_vendor_device *p)
{
    *p = LIBXL_VENDOR_DEVICE_XENSERVER;
}

static void libxl_ioport_range_rand_init(libxl_ioport_range *p);
static void libxl_ioport_range_rand_init(libxl_ioport_range *p)
{
    p->first = rand() % (sizeof(p->first)*8);
    p->number = rand() % (sizeof(p->number)*8);
}

static void libxl_iomem_range_rand_init(libxl_iomem_range *p);
static void libxl_iomem_range_rand_init(libxl_iomem_range *p)
{
    p->start = rand() % (sizeof(p->start)*8);
    p->number = rand() % (sizeof(p->number)*8);
}

static void libxl_vga_interface_info_rand_init(libxl_vga_interface_info *p);
static void libxl_vga_interface_info_rand_init(libxl_vga_interface_info *p)
{
    p->kind = LIBXL_VGA_INTERFACE_TYPE_STD;
}

static void libxl_vnc_info_rand_init(libxl_vnc_info *p);
static void libxl_vnc_info_rand_init(libxl_vnc_info *p)
{
    libxl_defbool_rand_init(&p->enable);
    p->listen = rand_str();
    p->passwd = rand_str();
    p->display = rand() % (sizeof(p->display)*8);
    libxl_defbool_rand_init(&p->findunused);
}

static void libxl_spice_info_rand_init(libxl_spice_info *p);
static void libxl_spice_info_rand_init(libxl_spice_info *p)
{
    libxl_defbool_rand_init(&p->enable);
    p->port = rand() % (sizeof(p->port)*8);
    p->tls_port = rand() % (sizeof(p->tls_port)*8);
    p->host = rand_str();
    libxl_defbool_rand_init(&p->disable_ticketing);
    p->passwd = rand_str();
    libxl_defbool_rand_init(&p->agent_mouse);
    libxl_defbool_rand_init(&p->vdagent);
    libxl_defbool_rand_init(&p->clipboard_sharing);
    p->usbredirection = rand() % (sizeof(p->usbredirection)*8);
}

static void libxl_sdl_info_rand_init(libxl_sdl_info *p);
static void libxl_sdl_info_rand_init(libxl_sdl_info *p)
{
    libxl_defbool_rand_init(&p->enable);
    libxl_defbool_rand_init(&p->opengl);
    p->display = rand_str();
    p->xauthority = rand_str();
}

static void libxl_dominfo_rand_init(libxl_dominfo *p);
static void libxl_dominfo_rand_init(libxl_dominfo *p)
{
    libxl_uuid_rand_init(&p->uuid);
    libxl_domid_rand_init(&p->domid);
    p->ssidref = rand() % (sizeof(p->ssidref)*8);
    p->running = rand() % 2;
    p->blocked = rand() % 2;
    p->paused = rand() % 2;
    p->shutdown = rand() % 2;
    p->dying = rand() % 2;
    p->shutdown_reason = LIBXL_SHUTDOWN_REASON_WATCHDOG;
    p->outstanding_memkb = rand() % (sizeof(p->outstanding_memkb)*8);
    p->current_memkb = rand() % (sizeof(p->current_memkb)*8);
    p->shared_memkb = rand() % (sizeof(p->shared_memkb)*8);
    p->paged_memkb = rand() % (sizeof(p->paged_memkb)*8);
    p->max_memkb = rand() % (sizeof(p->max_memkb)*8);
    p->cpu_time = rand() % (sizeof(p->cpu_time)*8);
    p->vcpu_max_id = rand() % (sizeof(p->vcpu_max_id)*8);
    p->vcpu_online = rand() % (sizeof(p->vcpu_online)*8);
    p->cpupool = rand() % (sizeof(p->cpupool)*8);
    p->domain_type = LIBXL_DOMAIN_TYPE_HVM;
}

static void libxl_cpupoolinfo_rand_init(libxl_cpupoolinfo *p);
static void libxl_cpupoolinfo_rand_init(libxl_cpupoolinfo *p)
{
    p->poolid = rand() % (sizeof(p->poolid)*8);
    p->sched = LIBXL_SCHEDULER_ARINC653;
    p->n_dom = rand() % (sizeof(p->n_dom)*8);
    libxl_bitmap_rand_init(&p->cpumap);
}

static void libxl_vminfo_rand_init(libxl_vminfo *p);
static void libxl_vminfo_rand_init(libxl_vminfo *p)
{
    libxl_uuid_rand_init(&p->uuid);
    libxl_domid_rand_init(&p->domid);
}

static void libxl_version_info_rand_init(libxl_version_info *p);
static void libxl_version_info_rand_init(libxl_version_info *p)
{
    p->xen_version_major = rand() % (sizeof(p->xen_version_major)*8);
    p->xen_version_minor = rand() % (sizeof(p->xen_version_minor)*8);
    p->xen_version_extra = rand_str();
    p->compiler = rand_str();
    p->compile_by = rand_str();
    p->compile_domain = rand_str();
    p->compile_date = rand_str();
    p->capabilities = rand_str();
    p->changeset = rand_str();
    p->virt_start = rand() % (sizeof(p->virt_start)*8);
    p->pagesize = rand() % (sizeof(p->pagesize)*8);
    p->commandline = rand_str();
}

static void libxl_domain_create_info_rand_init(libxl_domain_create_info *p);
static void libxl_domain_create_info_rand_init(libxl_domain_create_info *p)
{
    p->type = LIBXL_DOMAIN_TYPE_HVM;
    libxl_defbool_rand_init(&p->hap);
    libxl_defbool_rand_init(&p->oos);
    p->ssidref = rand() % (sizeof(p->ssidref)*8);
    p->name = rand_str();
    libxl_uuid_rand_init(&p->uuid);
    libxl_key_value_list_rand_init(&p->xsdata);
    libxl_key_value_list_rand_init(&p->platformdata);
    p->poolid = rand() % (sizeof(p->poolid)*8);
    libxl_defbool_rand_init(&p->run_hotplug_scripts);
    libxl_defbool_rand_init(&p->pvh);
    libxl_defbool_rand_init(&p->driver_domain);
}

static void libxl_domain_restore_params_rand_init(libxl_domain_restore_params *p);
static void libxl_domain_restore_params_rand_init(libxl_domain_restore_params *p)
{
    p->checkpointed_stream = rand() % (sizeof(p->checkpointed_stream)*8);
}

static void libxl_domain_sched_params_rand_init(libxl_domain_sched_params *p);
static void libxl_domain_sched_params_rand_init(libxl_domain_sched_params *p)
{
    p->sched = LIBXL_SCHEDULER_SEDF;
    p->weight = rand() % (sizeof(p->weight)*8);
    p->cap = rand() % (sizeof(p->cap)*8);
    p->period = rand() % (sizeof(p->period)*8);
    p->slice = rand() % (sizeof(p->slice)*8);
    p->latency = rand() % (sizeof(p->latency)*8);
    p->extratime = rand() % (sizeof(p->extratime)*8);
}

static void libxl_domain_build_info_rand_init(libxl_domain_build_info *p);
static void libxl_domain_build_info_rand_init(libxl_domain_build_info *p)
{
    p->max_vcpus = rand() % (sizeof(p->max_vcpus)*8);
    libxl_bitmap_rand_init(&p->avail_vcpus);
    libxl_bitmap_rand_init(&p->cpumap);
    libxl_bitmap_rand_init(&p->nodemap);
    libxl_defbool_rand_init(&p->numa_placement);
    p->tsc_mode = LIBXL_TSC_MODE_NATIVE;
    p->max_memkb = rand() % (sizeof(p->max_memkb)*8);
    p->target_memkb = rand() % (sizeof(p->target_memkb)*8);
    p->video_memkb = rand() % (sizeof(p->video_memkb)*8);
    p->shadow_memkb = rand() % (sizeof(p->shadow_memkb)*8);
    p->rtc_timeoffset = rand() % (sizeof(p->rtc_timeoffset)*8);
    p->exec_ssidref = rand() % (sizeof(p->exec_ssidref)*8);
    libxl_defbool_rand_init(&p->localtime);
    libxl_defbool_rand_init(&p->disable_migrate);
    libxl_cpuid_policy_list_rand_init(&p->cpuid);
    p->blkdev_start = rand_str();
    p->device_model_version = LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN;
    libxl_defbool_rand_init(&p->device_model_stubdomain);
    p->device_model = rand_str();
    p->device_model_ssidref = rand() % (sizeof(p->device_model_ssidref)*8);
    libxl_string_list_rand_init(&p->extra);
    libxl_string_list_rand_init(&p->extra_pv);
    libxl_string_list_rand_init(&p->extra_hvm);
    libxl_domain_sched_params_rand_init(&p->sched_params);
    p->num_ioports = rand()%8;
    p->ioports = calloc(p->num_ioports, sizeof(*p->ioports));
    {
        int i;
        for (i=0; i<p->num_ioports; i++)
            libxl_ioport_range_rand_init(&p->ioports[i]);
    }
    p->num_irqs = rand()%8;
    p->irqs = calloc(p->num_irqs, sizeof(*p->irqs));
    {
        int i;
        for (i=0; i<p->num_irqs; i++)
            p->irqs[i] = rand() % (sizeof(p->irqs[i])*8);
    }
    p->num_iomem = rand()%8;
    p->iomem = calloc(p->num_iomem, sizeof(*p->iomem));
    {
        int i;
        for (i=0; i<p->num_iomem; i++)
            libxl_iomem_range_rand_init(&p->iomem[i]);
    }
    libxl_defbool_rand_init(&p->claim_mode);
    p->event_channels = rand() % (sizeof(p->event_channels)*8);
    p->type = LIBXL_DOMAIN_TYPE_PV;
    switch (p->type) {
    case LIBXL_DOMAIN_TYPE_HVM:
        p->u.hvm.firmware = rand_str();
        p->u.hvm.bios = LIBXL_BIOS_TYPE_SEABIOS;
        libxl_defbool_rand_init(&p->u.hvm.pae);
        libxl_defbool_rand_init(&p->u.hvm.apic);
        libxl_defbool_rand_init(&p->u.hvm.acpi);
        libxl_defbool_rand_init(&p->u.hvm.acpi_s3);
        libxl_defbool_rand_init(&p->u.hvm.acpi_s4);
        libxl_defbool_rand_init(&p->u.hvm.nx);
        libxl_defbool_rand_init(&p->u.hvm.viridian);
        p->u.hvm.timeoffset = rand_str();
        libxl_defbool_rand_init(&p->u.hvm.hpet);
        libxl_defbool_rand_init(&p->u.hvm.vpt_align);
        p->u.hvm.timer_mode = LIBXL_TIMER_MODE_NO_DELAY_FOR_MISSED_TICKS;
        libxl_defbool_rand_init(&p->u.hvm.nested_hvm);
        p->u.hvm.smbios_firmware = rand_str();
        p->u.hvm.acpi_firmware = rand_str();
        libxl_defbool_rand_init(&p->u.hvm.nographic);
        libxl_vga_interface_info_rand_init(&p->u.hvm.vga);
        libxl_vnc_info_rand_init(&p->u.hvm.vnc);
        p->u.hvm.keymap = rand_str();
        libxl_sdl_info_rand_init(&p->u.hvm.sdl);
        libxl_spice_info_rand_init(&p->u.hvm.spice);
        libxl_defbool_rand_init(&p->u.hvm.gfx_passthru);
        p->u.hvm.serial = rand_str();
        p->u.hvm.boot = rand_str();
        libxl_defbool_rand_init(&p->u.hvm.usb);
        p->u.hvm.usbversion = rand() % (sizeof(p->u.hvm.usbversion)*8);
        p->u.hvm.usbdevice = rand_str();
        p->u.hvm.soundhw = rand_str();
        libxl_defbool_rand_init(&p->u.hvm.xen_platform_pci);
        libxl_string_list_rand_init(&p->u.hvm.usbdevice_list);
        p->u.hvm.vendor_device = LIBXL_VENDOR_DEVICE_NONE;
        break;
    case LIBXL_DOMAIN_TYPE_PV:
        p->u.pv.kernel = rand_str();
        p->u.pv.slack_memkb = rand() % (sizeof(p->u.pv.slack_memkb)*8);
        p->u.pv.bootloader = rand_str();
        libxl_string_list_rand_init(&p->u.pv.bootloader_args);
        p->u.pv.cmdline = rand_str();
        p->u.pv.ramdisk = rand_str();
        libxl_defbool_rand_init(&p->u.pv.e820_host);
        break;
    case LIBXL_DOMAIN_TYPE_INVALID:
        break;
    }
}

static void libxl_device_vfb_rand_init(libxl_device_vfb *p);
static void libxl_device_vfb_rand_init(libxl_device_vfb *p)
{
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
    libxl_devid_rand_init(&p->devid);
    libxl_vnc_info_rand_init(&p->vnc);
    libxl_sdl_info_rand_init(&p->sdl);
    p->keymap = rand_str();
}

static void libxl_device_vkb_rand_init(libxl_device_vkb *p);
static void libxl_device_vkb_rand_init(libxl_device_vkb *p)
{
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
    libxl_devid_rand_init(&p->devid);
}

static void libxl_device_disk_rand_init(libxl_device_disk *p);
static void libxl_device_disk_rand_init(libxl_device_disk *p)
{
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
    p->pdev_path = rand_str();
    p->vdev = rand_str();
    p->backend = LIBXL_DISK_BACKEND_UNKNOWN;
    p->format = LIBXL_DISK_FORMAT_EMPTY;
    p->script = rand_str();
    p->removable = rand() % (sizeof(p->removable)*8);
    p->readwrite = rand() % (sizeof(p->readwrite)*8);
    p->is_cdrom = rand() % (sizeof(p->is_cdrom)*8);
}

static void libxl_device_nic_rand_init(libxl_device_nic *p);
static void libxl_device_nic_rand_init(libxl_device_nic *p)
{
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
    libxl_devid_rand_init(&p->devid);
    p->mtu = rand() % (sizeof(p->mtu)*8);
    p->model = rand_str();
    libxl_mac_rand_init(&p->mac);
    p->ip = rand_str();
    p->bridge = rand_str();
    p->ifname = rand_str();
    p->script = rand_str();
    p->nictype = LIBXL_NIC_TYPE_VIF_IOEMU;
    p->rate_bytes_per_interval = rand() % (sizeof(p->rate_bytes_per_interval)*8);
    p->rate_interval_usecs = rand() % (sizeof(p->rate_interval_usecs)*8);
    p->gatewaydev = rand_str();
}

static void libxl_device_pci_rand_init(libxl_device_pci *p);
static void libxl_device_pci_rand_init(libxl_device_pci *p)
{
    p->func = rand() % (sizeof(p->func)*8);
    p->dev = rand() % (sizeof(p->dev)*8);
    p->bus = rand() % (sizeof(p->bus)*8);
    p->domain = rand() % (sizeof(p->domain)*8);
    p->vdevfn = rand() % (sizeof(p->vdevfn)*8);
    p->vfunc_mask = rand() % (sizeof(p->vfunc_mask)*8);
    p->msitranslate = rand() % 2;
    p->power_mgmt = rand() % 2;
    p->permissive = rand() % 2;
}

static void libxl_device_vtpm_rand_init(libxl_device_vtpm *p);
static void libxl_device_vtpm_rand_init(libxl_device_vtpm *p)
{
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
    libxl_devid_rand_init(&p->devid);
    libxl_uuid_rand_init(&p->uuid);
}

static void libxl_domain_config_rand_init(libxl_domain_config *p);
static void libxl_domain_config_rand_init(libxl_domain_config *p)
{
    libxl_domain_create_info_rand_init(&p->c_info);
    libxl_domain_build_info_rand_init(&p->b_info);
    p->num_disks = rand()%8;
    p->disks = calloc(p->num_disks, sizeof(*p->disks));
    {
        int i;
        for (i=0; i<p->num_disks; i++)
            libxl_device_disk_rand_init(&p->disks[i]);
    }
    p->num_nics = rand()%8;
    p->nics = calloc(p->num_nics, sizeof(*p->nics));
    {
        int i;
        for (i=0; i<p->num_nics; i++)
            libxl_device_nic_rand_init(&p->nics[i]);
    }
    p->num_pcidevs = rand()%8;
    p->pcidevs = calloc(p->num_pcidevs, sizeof(*p->pcidevs));
    {
        int i;
        for (i=0; i<p->num_pcidevs; i++)
            libxl_device_pci_rand_init(&p->pcidevs[i]);
    }
    p->num_vfbs = rand()%8;
    p->vfbs = calloc(p->num_vfbs, sizeof(*p->vfbs));
    {
        int i;
        for (i=0; i<p->num_vfbs; i++)
            libxl_device_vfb_rand_init(&p->vfbs[i]);
    }
    p->num_vkbs = rand()%8;
    p->vkbs = calloc(p->num_vkbs, sizeof(*p->vkbs));
    {
        int i;
        for (i=0; i<p->num_vkbs; i++)
            libxl_device_vkb_rand_init(&p->vkbs[i]);
    }
    p->num_vtpms = rand()%8;
    p->vtpms = calloc(p->num_vtpms, sizeof(*p->vtpms));
    {
        int i;
        for (i=0; i<p->num_vtpms; i++)
            libxl_device_vtpm_rand_init(&p->vtpms[i]);
    }
    p->on_poweroff = LIBXL_ACTION_ON_SHUTDOWN_PRESERVE;
    p->on_reboot = LIBXL_ACTION_ON_SHUTDOWN_DESTROY;
    p->on_watchdog = LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY;
    p->on_crash = LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART;
}

static void libxl_diskinfo_rand_init(libxl_diskinfo *p);
static void libxl_diskinfo_rand_init(libxl_diskinfo *p)
{
    p->backend = rand_str();
    p->backend_id = rand() % (sizeof(p->backend_id)*8);
    p->frontend = rand_str();
    p->frontend_id = rand() % (sizeof(p->frontend_id)*8);
    libxl_devid_rand_init(&p->devid);
    p->state = rand() % (sizeof(p->state)*8);
    p->evtch = rand() % (sizeof(p->evtch)*8);
    p->rref = rand() % (sizeof(p->rref)*8);
}

static void libxl_nicinfo_rand_init(libxl_nicinfo *p);
static void libxl_nicinfo_rand_init(libxl_nicinfo *p)
{
    p->backend = rand_str();
    p->backend_id = rand() % (sizeof(p->backend_id)*8);
    p->frontend = rand_str();
    p->frontend_id = rand() % (sizeof(p->frontend_id)*8);
    libxl_devid_rand_init(&p->devid);
    p->state = rand() % (sizeof(p->state)*8);
    p->evtch = rand() % (sizeof(p->evtch)*8);
    p->rref_tx = rand() % (sizeof(p->rref_tx)*8);
    p->rref_rx = rand() % (sizeof(p->rref_rx)*8);
}

static void libxl_vtpminfo_rand_init(libxl_vtpminfo *p);
static void libxl_vtpminfo_rand_init(libxl_vtpminfo *p)
{
    p->backend = rand_str();
    p->backend_id = rand() % (sizeof(p->backend_id)*8);
    p->frontend = rand_str();
    p->frontend_id = rand() % (sizeof(p->frontend_id)*8);
    libxl_devid_rand_init(&p->devid);
    p->state = rand() % (sizeof(p->state)*8);
    p->evtch = rand() % (sizeof(p->evtch)*8);
    p->rref = rand() % (sizeof(p->rref)*8);
    libxl_uuid_rand_init(&p->uuid);
}

static void libxl_vcpuinfo_rand_init(libxl_vcpuinfo *p);
static void libxl_vcpuinfo_rand_init(libxl_vcpuinfo *p)
{
    p->vcpuid = rand() % (sizeof(p->vcpuid)*8);
    p->cpu = rand() % (sizeof(p->cpu)*8);
    p->online = rand() % 2;
    p->blocked = rand() % 2;
    p->running = rand() % 2;
    p->vcpu_time = rand() % (sizeof(p->vcpu_time)*8);
    libxl_bitmap_rand_init(&p->cpumap);
}

static void libxl_physinfo_rand_init(libxl_physinfo *p);
static void libxl_physinfo_rand_init(libxl_physinfo *p)
{
    p->threads_per_core = rand() % (sizeof(p->threads_per_core)*8);
    p->cores_per_socket = rand() % (sizeof(p->cores_per_socket)*8);
    p->max_cpu_id = rand() % (sizeof(p->max_cpu_id)*8);
    p->nr_cpus = rand() % (sizeof(p->nr_cpus)*8);
    p->cpu_khz = rand() % (sizeof(p->cpu_khz)*8);
    p->total_pages = rand() % (sizeof(p->total_pages)*8);
    p->free_pages = rand() % (sizeof(p->free_pages)*8);
    p->scrub_pages = rand() % (sizeof(p->scrub_pages)*8);
    p->outstanding_pages = rand() % (sizeof(p->outstanding_pages)*8);
    p->sharing_freed_pages = rand() % (sizeof(p->sharing_freed_pages)*8);
    p->sharing_used_frames = rand() % (sizeof(p->sharing_used_frames)*8);
    p->nr_nodes = rand() % (sizeof(p->nr_nodes)*8);
    libxl_hwcap_rand_init(&p->hw_cap);
    p->cap_hvm = rand() % 2;
    p->cap_hvm_directio = rand() % 2;
}

static void libxl_numainfo_rand_init(libxl_numainfo *p);
static void libxl_numainfo_rand_init(libxl_numainfo *p)
{
    p->size = rand() % (sizeof(p->size)*8);
    p->free = rand() % (sizeof(p->free)*8);
    p->num_dists = rand()%8;
    p->dists = calloc(p->num_dists, sizeof(*p->dists));
    {
        int i;
        for (i=0; i<p->num_dists; i++)
            p->dists[i] = rand() % (sizeof(p->dists[i])*8);
    }
}

static void libxl_cputopology_rand_init(libxl_cputopology *p);
static void libxl_cputopology_rand_init(libxl_cputopology *p)
{
    p->core = rand() % (sizeof(p->core)*8);
    p->socket = rand() % (sizeof(p->socket)*8);
    p->node = rand() % (sizeof(p->node)*8);
}

static void libxl_sched_credit_params_rand_init(libxl_sched_credit_params *p);
static void libxl_sched_credit_params_rand_init(libxl_sched_credit_params *p)
{
    p->tslice_ms = rand() % (sizeof(p->tslice_ms)*8);
    p->ratelimit_us = rand() % (sizeof(p->ratelimit_us)*8);
}

static void libxl_domain_remus_info_rand_init(libxl_domain_remus_info *p);
static void libxl_domain_remus_info_rand_init(libxl_domain_remus_info *p)
{
    p->interval = rand() % (sizeof(p->interval)*8);
    p->blackhole = rand() % 2;
    p->compression = rand() % 2;
}

static void libxl_event_type_rand_init(libxl_event_type *p);
static void libxl_event_type_rand_init(libxl_event_type *p)
{
    *p = LIBXL_EVENT_TYPE_OPERATION_COMPLETE;
}

static void libxl_event_rand_init(libxl_event *p);
static void libxl_event_rand_init(libxl_event *p)
{
    libxl_ev_link_rand_init(&p->link);
    libxl_domid_rand_init(&p->domid);
    libxl_uuid_rand_init(&p->domuuid);
    p->for_user = rand() % (sizeof(p->for_user)*8);
    p->type = LIBXL_EVENT_TYPE_DISK_EJECT;
    switch (p->type) {
    case LIBXL_EVENT_TYPE_DOMAIN_SHUTDOWN:
        p->u.domain_shutdown.shutdown_reason = rand() % (sizeof(p->u.domain_shutdown.shutdown_reason)*8);
        break;
    case LIBXL_EVENT_TYPE_DOMAIN_DEATH:
        break;
    case LIBXL_EVENT_TYPE_DISK_EJECT:
        p->u.disk_eject.vdev = rand_str();
        libxl_device_disk_rand_init(&p->u.disk_eject.disk);
        break;
    case LIBXL_EVENT_TYPE_OPERATION_COMPLETE:
        p->u.operation_complete.rc = rand() % (sizeof(p->u.operation_complete.rc)*8);
        break;
    case LIBXL_EVENT_TYPE_DOMAIN_CREATE_CONSOLE_AVAILABLE:
        break;
    }
}


int main(int argc, char **argv)
{
    libxl_error libxl_error_val;
    libxl_domain_type libxl_domain_type_val;
    libxl_device_model_version libxl_device_model_version_val;
    libxl_console_type libxl_console_type_val;
    libxl_disk_format libxl_disk_format_val;
    libxl_disk_backend libxl_disk_backend_val;
    libxl_nic_type libxl_nic_type_val;
    libxl_action_on_shutdown libxl_action_on_shutdown_val;
    libxl_trigger libxl_trigger_val;
    libxl_tsc_mode libxl_tsc_mode_val;
    libxl_timer_mode libxl_timer_mode_val;
    libxl_bios_type libxl_bios_type_val;
    libxl_scheduler libxl_scheduler_val;
    libxl_shutdown_reason libxl_shutdown_reason_val;
    libxl_vga_interface_type libxl_vga_interface_type_val;
    libxl_vendor_device libxl_vendor_device_val;
    libxl_ioport_range libxl_ioport_range_val;
    libxl_iomem_range libxl_iomem_range_val;
    libxl_vga_interface_info libxl_vga_interface_info_val;
    libxl_vnc_info libxl_vnc_info_val;
    libxl_spice_info libxl_spice_info_val;
    libxl_sdl_info libxl_sdl_info_val;
    libxl_dominfo libxl_dominfo_val;
    libxl_cpupoolinfo libxl_cpupoolinfo_val;
    libxl_vminfo libxl_vminfo_val;
    libxl_version_info libxl_version_info_val;
    libxl_domain_create_info libxl_domain_create_info_val;
    libxl_domain_restore_params libxl_domain_restore_params_val;
    libxl_domain_sched_params libxl_domain_sched_params_val;
    libxl_domain_build_info libxl_domain_build_info_val;
    libxl_device_vfb libxl_device_vfb_val;
    libxl_device_vkb libxl_device_vkb_val;
    libxl_device_disk libxl_device_disk_val;
    libxl_device_nic libxl_device_nic_val;
    libxl_device_pci libxl_device_pci_val;
    libxl_device_vtpm libxl_device_vtpm_val;
    libxl_domain_config libxl_domain_config_val;
    libxl_diskinfo libxl_diskinfo_val;
    libxl_nicinfo libxl_nicinfo_val;
    libxl_vtpminfo libxl_vtpminfo_val;
    libxl_vcpuinfo libxl_vcpuinfo_val;
    libxl_physinfo libxl_physinfo_val;
    libxl_numainfo libxl_numainfo_val;
    libxl_cputopology libxl_cputopology_val;
    libxl_sched_credit_params libxl_sched_credit_params_val;
    libxl_domain_remus_info libxl_domain_remus_info_val;
    libxl_event_type libxl_event_type_val;
    libxl_event libxl_event_val;

    int rc;
    char *s;
    xentoollog_logger_stdiostream *logger;
    libxl_ctx *ctx;

    logger = xtl_createlogger_stdiostream(stderr, XTL_DETAIL, 0);
    if (!logger) exit(1);

    if (libxl_ctx_alloc(&ctx, LIBXL_VERSION, 0, (xentoollog_logger*)logger)) {
        fprintf(stderr, "cannot init xl context\n");
        exit(1);
    }
    printf("Testing TYPE_to_json()\n");
    printf("----------------------\n");
    printf("\n");
    libxl_error_rand_init(&libxl_error_val);
    s = libxl_error_to_json(ctx, libxl_error_val);
    printf("%s: %s\n", "libxl_error", s);
    if (s == NULL) abort();
    free(s);

    libxl_domain_type_rand_init(&libxl_domain_type_val);
    s = libxl_domain_type_to_json(ctx, libxl_domain_type_val);
    printf("%s: %s\n", "libxl_domain_type", s);
    if (s == NULL) abort();
    free(s);

    libxl_device_model_version_rand_init(&libxl_device_model_version_val);
    s = libxl_device_model_version_to_json(ctx, libxl_device_model_version_val);
    printf("%s: %s\n", "libxl_device_model_version", s);
    if (s == NULL) abort();
    free(s);

    libxl_console_type_rand_init(&libxl_console_type_val);
    s = libxl_console_type_to_json(ctx, libxl_console_type_val);
    printf("%s: %s\n", "libxl_console_type", s);
    if (s == NULL) abort();
    free(s);

    libxl_disk_format_rand_init(&libxl_disk_format_val);
    s = libxl_disk_format_to_json(ctx, libxl_disk_format_val);
    printf("%s: %s\n", "libxl_disk_format", s);
    if (s == NULL) abort();
    free(s);

    libxl_disk_backend_rand_init(&libxl_disk_backend_val);
    s = libxl_disk_backend_to_json(ctx, libxl_disk_backend_val);
    printf("%s: %s\n", "libxl_disk_backend", s);
    if (s == NULL) abort();
    free(s);

    libxl_nic_type_rand_init(&libxl_nic_type_val);
    s = libxl_nic_type_to_json(ctx, libxl_nic_type_val);
    printf("%s: %s\n", "libxl_nic_type", s);
    if (s == NULL) abort();
    free(s);

    libxl_action_on_shutdown_rand_init(&libxl_action_on_shutdown_val);
    s = libxl_action_on_shutdown_to_json(ctx, libxl_action_on_shutdown_val);
    printf("%s: %s\n", "libxl_action_on_shutdown", s);
    if (s == NULL) abort();
    free(s);

    libxl_trigger_rand_init(&libxl_trigger_val);
    s = libxl_trigger_to_json(ctx, libxl_trigger_val);
    printf("%s: %s\n", "libxl_trigger", s);
    if (s == NULL) abort();
    free(s);

    libxl_tsc_mode_rand_init(&libxl_tsc_mode_val);
    s = libxl_tsc_mode_to_json(ctx, libxl_tsc_mode_val);
    printf("%s: %s\n", "libxl_tsc_mode", s);
    if (s == NULL) abort();
    free(s);

    libxl_timer_mode_rand_init(&libxl_timer_mode_val);
    s = libxl_timer_mode_to_json(ctx, libxl_timer_mode_val);
    printf("%s: %s\n", "libxl_timer_mode", s);
    if (s == NULL) abort();
    free(s);

    libxl_bios_type_rand_init(&libxl_bios_type_val);
    s = libxl_bios_type_to_json(ctx, libxl_bios_type_val);
    printf("%s: %s\n", "libxl_bios_type", s);
    if (s == NULL) abort();
    free(s);

    libxl_scheduler_rand_init(&libxl_scheduler_val);
    s = libxl_scheduler_to_json(ctx, libxl_scheduler_val);
    printf("%s: %s\n", "libxl_scheduler", s);
    if (s == NULL) abort();
    free(s);

    libxl_shutdown_reason_rand_init(&libxl_shutdown_reason_val);
    s = libxl_shutdown_reason_to_json(ctx, libxl_shutdown_reason_val);
    printf("%s: %s\n", "libxl_shutdown_reason", s);
    if (s == NULL) abort();
    free(s);

    libxl_vga_interface_type_rand_init(&libxl_vga_interface_type_val);
    s = libxl_vga_interface_type_to_json(ctx, libxl_vga_interface_type_val);
    printf("%s: %s\n", "libxl_vga_interface_type", s);
    if (s == NULL) abort();
    free(s);

    libxl_vendor_device_rand_init(&libxl_vendor_device_val);
    s = libxl_vendor_device_to_json(ctx, libxl_vendor_device_val);
    printf("%s: %s\n", "libxl_vendor_device", s);
    if (s == NULL) abort();
    free(s);

    libxl_ioport_range_rand_init(&libxl_ioport_range_val);
    s = libxl_ioport_range_to_json(ctx, &libxl_ioport_range_val);
    printf("%s: %s\n", "libxl_ioport_range", s);
    if (s == NULL) abort();
    free(s);
    libxl_ioport_range_dispose(&libxl_ioport_range_val);

    libxl_iomem_range_rand_init(&libxl_iomem_range_val);
    s = libxl_iomem_range_to_json(ctx, &libxl_iomem_range_val);
    printf("%s: %s\n", "libxl_iomem_range", s);
    if (s == NULL) abort();
    free(s);
    libxl_iomem_range_dispose(&libxl_iomem_range_val);

    libxl_vga_interface_info_rand_init(&libxl_vga_interface_info_val);
    s = libxl_vga_interface_info_to_json(ctx, &libxl_vga_interface_info_val);
    printf("%s: %s\n", "libxl_vga_interface_info", s);
    if (s == NULL) abort();
    free(s);
    libxl_vga_interface_info_dispose(&libxl_vga_interface_info_val);

    libxl_vnc_info_rand_init(&libxl_vnc_info_val);
    s = libxl_vnc_info_to_json(ctx, &libxl_vnc_info_val);
    printf("%s: %s\n", "libxl_vnc_info", s);
    if (s == NULL) abort();
    free(s);
    libxl_vnc_info_dispose(&libxl_vnc_info_val);

    libxl_spice_info_rand_init(&libxl_spice_info_val);
    s = libxl_spice_info_to_json(ctx, &libxl_spice_info_val);
    printf("%s: %s\n", "libxl_spice_info", s);
    if (s == NULL) abort();
    free(s);
    libxl_spice_info_dispose(&libxl_spice_info_val);

    libxl_sdl_info_rand_init(&libxl_sdl_info_val);
    s = libxl_sdl_info_to_json(ctx, &libxl_sdl_info_val);
    printf("%s: %s\n", "libxl_sdl_info", s);
    if (s == NULL) abort();
    free(s);
    libxl_sdl_info_dispose(&libxl_sdl_info_val);

    libxl_dominfo_rand_init(&libxl_dominfo_val);
    s = libxl_dominfo_to_json(ctx, &libxl_dominfo_val);
    printf("%s: %s\n", "libxl_dominfo", s);
    if (s == NULL) abort();
    free(s);
    libxl_dominfo_dispose(&libxl_dominfo_val);

    libxl_cpupoolinfo_rand_init(&libxl_cpupoolinfo_val);
    s = libxl_cpupoolinfo_to_json(ctx, &libxl_cpupoolinfo_val);
    printf("%s: %s\n", "libxl_cpupoolinfo", s);
    if (s == NULL) abort();
    free(s);
    libxl_cpupoolinfo_dispose(&libxl_cpupoolinfo_val);

    libxl_vminfo_rand_init(&libxl_vminfo_val);
    s = libxl_vminfo_to_json(ctx, &libxl_vminfo_val);
    printf("%s: %s\n", "libxl_vminfo", s);
    if (s == NULL) abort();
    free(s);
    libxl_vminfo_dispose(&libxl_vminfo_val);

    libxl_version_info_rand_init(&libxl_version_info_val);
    s = libxl_version_info_to_json(ctx, &libxl_version_info_val);
    printf("%s: %s\n", "libxl_version_info", s);
    if (s == NULL) abort();
    free(s);
    libxl_version_info_dispose(&libxl_version_info_val);

    libxl_domain_create_info_rand_init(&libxl_domain_create_info_val);
    s = libxl_domain_create_info_to_json(ctx, &libxl_domain_create_info_val);
    printf("%s: %s\n", "libxl_domain_create_info", s);
    if (s == NULL) abort();
    free(s);
    libxl_domain_create_info_dispose(&libxl_domain_create_info_val);

    libxl_domain_restore_params_rand_init(&libxl_domain_restore_params_val);
    s = libxl_domain_restore_params_to_json(ctx, &libxl_domain_restore_params_val);
    printf("%s: %s\n", "libxl_domain_restore_params", s);
    if (s == NULL) abort();
    free(s);
    libxl_domain_restore_params_dispose(&libxl_domain_restore_params_val);

    libxl_domain_sched_params_rand_init(&libxl_domain_sched_params_val);
    s = libxl_domain_sched_params_to_json(ctx, &libxl_domain_sched_params_val);
    printf("%s: %s\n", "libxl_domain_sched_params", s);
    if (s == NULL) abort();
    free(s);
    libxl_domain_sched_params_dispose(&libxl_domain_sched_params_val);

    libxl_domain_build_info_rand_init(&libxl_domain_build_info_val);
    s = libxl_domain_build_info_to_json(ctx, &libxl_domain_build_info_val);
    printf("%s: %s\n", "libxl_domain_build_info", s);
    if (s == NULL) abort();
    free(s);
    libxl_domain_build_info_dispose(&libxl_domain_build_info_val);

    libxl_device_vfb_rand_init(&libxl_device_vfb_val);
    s = libxl_device_vfb_to_json(ctx, &libxl_device_vfb_val);
    printf("%s: %s\n", "libxl_device_vfb", s);
    if (s == NULL) abort();
    free(s);
    libxl_device_vfb_dispose(&libxl_device_vfb_val);

    libxl_device_vkb_rand_init(&libxl_device_vkb_val);
    s = libxl_device_vkb_to_json(ctx, &libxl_device_vkb_val);
    printf("%s: %s\n", "libxl_device_vkb", s);
    if (s == NULL) abort();
    free(s);
    libxl_device_vkb_dispose(&libxl_device_vkb_val);

    libxl_device_disk_rand_init(&libxl_device_disk_val);
    s = libxl_device_disk_to_json(ctx, &libxl_device_disk_val);
    printf("%s: %s\n", "libxl_device_disk", s);
    if (s == NULL) abort();
    free(s);
    libxl_device_disk_dispose(&libxl_device_disk_val);

    libxl_device_nic_rand_init(&libxl_device_nic_val);
    s = libxl_device_nic_to_json(ctx, &libxl_device_nic_val);
    printf("%s: %s\n", "libxl_device_nic", s);
    if (s == NULL) abort();
    free(s);
    libxl_device_nic_dispose(&libxl_device_nic_val);

    libxl_device_pci_rand_init(&libxl_device_pci_val);
    s = libxl_device_pci_to_json(ctx, &libxl_device_pci_val);
    printf("%s: %s\n", "libxl_device_pci", s);
    if (s == NULL) abort();
    free(s);
    libxl_device_pci_dispose(&libxl_device_pci_val);

    libxl_device_vtpm_rand_init(&libxl_device_vtpm_val);
    s = libxl_device_vtpm_to_json(ctx, &libxl_device_vtpm_val);
    printf("%s: %s\n", "libxl_device_vtpm", s);
    if (s == NULL) abort();
    free(s);
    libxl_device_vtpm_dispose(&libxl_device_vtpm_val);

    libxl_domain_config_rand_init(&libxl_domain_config_val);
    s = libxl_domain_config_to_json(ctx, &libxl_domain_config_val);
    printf("%s: %s\n", "libxl_domain_config", s);
    if (s == NULL) abort();
    free(s);
    libxl_domain_config_dispose(&libxl_domain_config_val);

    libxl_diskinfo_rand_init(&libxl_diskinfo_val);
    s = libxl_diskinfo_to_json(ctx, &libxl_diskinfo_val);
    printf("%s: %s\n", "libxl_diskinfo", s);
    if (s == NULL) abort();
    free(s);
    libxl_diskinfo_dispose(&libxl_diskinfo_val);

    libxl_nicinfo_rand_init(&libxl_nicinfo_val);
    s = libxl_nicinfo_to_json(ctx, &libxl_nicinfo_val);
    printf("%s: %s\n", "libxl_nicinfo", s);
    if (s == NULL) abort();
    free(s);
    libxl_nicinfo_dispose(&libxl_nicinfo_val);

    libxl_vtpminfo_rand_init(&libxl_vtpminfo_val);
    s = libxl_vtpminfo_to_json(ctx, &libxl_vtpminfo_val);
    printf("%s: %s\n", "libxl_vtpminfo", s);
    if (s == NULL) abort();
    free(s);
    libxl_vtpminfo_dispose(&libxl_vtpminfo_val);

    libxl_vcpuinfo_rand_init(&libxl_vcpuinfo_val);
    s = libxl_vcpuinfo_to_json(ctx, &libxl_vcpuinfo_val);
    printf("%s: %s\n", "libxl_vcpuinfo", s);
    if (s == NULL) abort();
    free(s);
    libxl_vcpuinfo_dispose(&libxl_vcpuinfo_val);

    libxl_physinfo_rand_init(&libxl_physinfo_val);
    s = libxl_physinfo_to_json(ctx, &libxl_physinfo_val);
    printf("%s: %s\n", "libxl_physinfo", s);
    if (s == NULL) abort();
    free(s);
    libxl_physinfo_dispose(&libxl_physinfo_val);

    libxl_numainfo_rand_init(&libxl_numainfo_val);
    s = libxl_numainfo_to_json(ctx, &libxl_numainfo_val);
    printf("%s: %s\n", "libxl_numainfo", s);
    if (s == NULL) abort();
    free(s);
    libxl_numainfo_dispose(&libxl_numainfo_val);

    libxl_cputopology_rand_init(&libxl_cputopology_val);
    s = libxl_cputopology_to_json(ctx, &libxl_cputopology_val);
    printf("%s: %s\n", "libxl_cputopology", s);
    if (s == NULL) abort();
    free(s);
    libxl_cputopology_dispose(&libxl_cputopology_val);

    libxl_sched_credit_params_rand_init(&libxl_sched_credit_params_val);
    s = libxl_sched_credit_params_to_json(ctx, &libxl_sched_credit_params_val);
    printf("%s: %s\n", "libxl_sched_credit_params", s);
    if (s == NULL) abort();
    free(s);

    libxl_domain_remus_info_rand_init(&libxl_domain_remus_info_val);
    s = libxl_domain_remus_info_to_json(ctx, &libxl_domain_remus_info_val);
    printf("%s: %s\n", "libxl_domain_remus_info", s);
    if (s == NULL) abort();
    free(s);
    libxl_domain_remus_info_dispose(&libxl_domain_remus_info_val);

    libxl_event_type_rand_init(&libxl_event_type_val);
    s = libxl_event_type_to_json(ctx, libxl_event_type_val);
    printf("%s: %s\n", "libxl_event_type", s);
    if (s == NULL) abort();
    free(s);

    libxl_event_rand_init(&libxl_event_val);
    s = libxl_event_to_json(ctx, &libxl_event_val);
    printf("%s: %s\n", "libxl_event", s);
    if (s == NULL) abort();
    free(s);
    libxl_event_dispose(&libxl_event_val);

    printf("Testing Enumerations\n");
    printf("--------------------\n");
    printf("\n");
    printf("libxl_error -- to string:\n");
    printf("\tNONSPECIFIC = %d = \"%s\"\n", ERROR_NONSPECIFIC, libxl_error_to_string(ERROR_NONSPECIFIC));
    printf("\tVERSION = %d = \"%s\"\n", ERROR_VERSION, libxl_error_to_string(ERROR_VERSION));
    printf("\tFAIL = %d = \"%s\"\n", ERROR_FAIL, libxl_error_to_string(ERROR_FAIL));
    printf("\tNI = %d = \"%s\"\n", ERROR_NI, libxl_error_to_string(ERROR_NI));
    printf("\tNOMEM = %d = \"%s\"\n", ERROR_NOMEM, libxl_error_to_string(ERROR_NOMEM));
    printf("\tINVAL = %d = \"%s\"\n", ERROR_INVAL, libxl_error_to_string(ERROR_INVAL));
    printf("\tBADFAIL = %d = \"%s\"\n", ERROR_BADFAIL, libxl_error_to_string(ERROR_BADFAIL));
    printf("\tGUEST_TIMEDOUT = %d = \"%s\"\n", ERROR_GUEST_TIMEDOUT, libxl_error_to_string(ERROR_GUEST_TIMEDOUT));
    printf("\tTIMEDOUT = %d = \"%s\"\n", ERROR_TIMEDOUT, libxl_error_to_string(ERROR_TIMEDOUT));
    printf("\tNOPARAVIRT = %d = \"%s\"\n", ERROR_NOPARAVIRT, libxl_error_to_string(ERROR_NOPARAVIRT));
    printf("\tNOT_READY = %d = \"%s\"\n", ERROR_NOT_READY, libxl_error_to_string(ERROR_NOT_READY));
    printf("\tOSEVENT_REG_FAIL = %d = \"%s\"\n", ERROR_OSEVENT_REG_FAIL, libxl_error_to_string(ERROR_OSEVENT_REG_FAIL));
    printf("\tBUFFERFULL = %d = \"%s\"\n", ERROR_BUFFERFULL, libxl_error_to_string(ERROR_BUFFERFULL));
    printf("\tUNKNOWN_CHILD = %d = \"%s\"\n", ERROR_UNKNOWN_CHILD, libxl_error_to_string(ERROR_UNKNOWN_CHILD));

    printf("libxl_error -- to JSON:\n");
    printf("\tNONSPECIFIC = %d = %s", ERROR_NONSPECIFIC, libxl_error_to_json(ctx, ERROR_NONSPECIFIC));
    printf("\tVERSION = %d = %s", ERROR_VERSION, libxl_error_to_json(ctx, ERROR_VERSION));
    printf("\tFAIL = %d = %s", ERROR_FAIL, libxl_error_to_json(ctx, ERROR_FAIL));
    printf("\tNI = %d = %s", ERROR_NI, libxl_error_to_json(ctx, ERROR_NI));
    printf("\tNOMEM = %d = %s", ERROR_NOMEM, libxl_error_to_json(ctx, ERROR_NOMEM));
    printf("\tINVAL = %d = %s", ERROR_INVAL, libxl_error_to_json(ctx, ERROR_INVAL));
    printf("\tBADFAIL = %d = %s", ERROR_BADFAIL, libxl_error_to_json(ctx, ERROR_BADFAIL));
    printf("\tGUEST_TIMEDOUT = %d = %s", ERROR_GUEST_TIMEDOUT, libxl_error_to_json(ctx, ERROR_GUEST_TIMEDOUT));
    printf("\tTIMEDOUT = %d = %s", ERROR_TIMEDOUT, libxl_error_to_json(ctx, ERROR_TIMEDOUT));
    printf("\tNOPARAVIRT = %d = %s", ERROR_NOPARAVIRT, libxl_error_to_json(ctx, ERROR_NOPARAVIRT));
    printf("\tNOT_READY = %d = %s", ERROR_NOT_READY, libxl_error_to_json(ctx, ERROR_NOT_READY));
    printf("\tOSEVENT_REG_FAIL = %d = %s", ERROR_OSEVENT_REG_FAIL, libxl_error_to_json(ctx, ERROR_OSEVENT_REG_FAIL));
    printf("\tBUFFERFULL = %d = %s", ERROR_BUFFERFULL, libxl_error_to_json(ctx, ERROR_BUFFERFULL));
    printf("\tUNKNOWN_CHILD = %d = %s", ERROR_UNKNOWN_CHILD, libxl_error_to_json(ctx, ERROR_UNKNOWN_CHILD));

    printf("libxl_error -- from string:\n");
    libxl_error_val = -1;
    rc = libxl_error_from_string("NonsPecifiC", &libxl_error_val);
    printf("\tNONSPECIFIC = \"%s\" = %d (rc %d)\n", "NonsPecifiC", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("vERSION", &libxl_error_val);
    printf("\tVERSION = \"%s\" = %d (rc %d)\n", "vERSION", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("fail", &libxl_error_val);
    printf("\tFAIL = \"%s\" = %d (rc %d)\n", "fail", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("Ni", &libxl_error_val);
    printf("\tNI = \"%s\" = %d (rc %d)\n", "Ni", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("nOMeM", &libxl_error_val);
    printf("\tNOMEM = \"%s\" = %d (rc %d)\n", "nOMeM", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("invAl", &libxl_error_val);
    printf("\tINVAL = \"%s\" = %d (rc %d)\n", "invAl", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("BAdfAIL", &libxl_error_val);
    printf("\tBADFAIL = \"%s\" = %d (rc %d)\n", "BAdfAIL", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("GuEST_TimEDOuT", &libxl_error_val);
    printf("\tGUEST_TIMEDOUT = \"%s\" = %d (rc %d)\n", "GuEST_TimEDOuT", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("TIMeDOut", &libxl_error_val);
    printf("\tTIMEDOUT = \"%s\" = %d (rc %d)\n", "TIMeDOut", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("NoPARAvIrt", &libxl_error_val);
    printf("\tNOPARAVIRT = \"%s\" = %d (rc %d)\n", "NoPARAvIrt", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("not_rEady", &libxl_error_val);
    printf("\tNOT_READY = \"%s\" = %d (rc %d)\n", "not_rEady", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("oseVent_REG_FAiL", &libxl_error_val);
    printf("\tOSEVENT_REG_FAIL = \"%s\" = %d (rc %d)\n", "oseVent_REG_FAiL", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("buFFERFUll", &libxl_error_val);
    printf("\tBUFFERFULL = \"%s\" = %d (rc %d)\n", "buFFERFUll", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("unkNoWn_CHIld", &libxl_error_val);
    printf("\tUNKNOWN_CHILD = \"%s\" = %d (rc %d)\n", "unkNoWn_CHIld", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("an iNvALiD vaLue", &libxl_error_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "an iNvALiD vaLue", libxl_error_val, rc);

    printf("libxl_domain_type -- to string:\n");
    printf("\tINVALID = %d = \"%s\"\n", LIBXL_DOMAIN_TYPE_INVALID, libxl_domain_type_to_string(LIBXL_DOMAIN_TYPE_INVALID));
    printf("\tHVM = %d = \"%s\"\n", LIBXL_DOMAIN_TYPE_HVM, libxl_domain_type_to_string(LIBXL_DOMAIN_TYPE_HVM));
    printf("\tPV = %d = \"%s\"\n", LIBXL_DOMAIN_TYPE_PV, libxl_domain_type_to_string(LIBXL_DOMAIN_TYPE_PV));

    printf("libxl_domain_type -- to JSON:\n");
    printf("\tINVALID = %d = %s", LIBXL_DOMAIN_TYPE_INVALID, libxl_domain_type_to_json(ctx, LIBXL_DOMAIN_TYPE_INVALID));
    printf("\tHVM = %d = %s", LIBXL_DOMAIN_TYPE_HVM, libxl_domain_type_to_json(ctx, LIBXL_DOMAIN_TYPE_HVM));
    printf("\tPV = %d = %s", LIBXL_DOMAIN_TYPE_PV, libxl_domain_type_to_json(ctx, LIBXL_DOMAIN_TYPE_PV));

    printf("libxl_domain_type -- from string:\n");
    libxl_domain_type_val = -1;
    rc = libxl_domain_type_from_string("iNValiD", &libxl_domain_type_val);
    printf("\tINVALID = \"%s\" = %d (rc %d)\n", "iNValiD", libxl_domain_type_val, rc);
    libxl_domain_type_val = -1;
    rc = libxl_domain_type_from_string("HVm", &libxl_domain_type_val);
    printf("\tHVM = \"%s\" = %d (rc %d)\n", "HVm", libxl_domain_type_val, rc);
    libxl_domain_type_val = -1;
    rc = libxl_domain_type_from_string("Pv", &libxl_domain_type_val);
    printf("\tPV = \"%s\" = %d (rc %d)\n", "Pv", libxl_domain_type_val, rc);
    libxl_domain_type_val = -1;
    rc = libxl_domain_type_from_string("aN invaLID VaLUe", &libxl_domain_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "aN invaLID VaLUe", libxl_domain_type_val, rc);

    printf("libxl_device_model_version -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_DEVICE_MODEL_VERSION_UNKNOWN, libxl_device_model_version_to_string(LIBXL_DEVICE_MODEL_VERSION_UNKNOWN));
    printf("\tQEMU_XEN_TRADITIONAL = %d = \"%s\"\n", LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN_TRADITIONAL, libxl_device_model_version_to_string(LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN_TRADITIONAL));
    printf("\tQEMU_XEN = %d = \"%s\"\n", LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN, libxl_device_model_version_to_string(LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN));

    printf("libxl_device_model_version -- to JSON:\n");
    printf("\tUNKNOWN = %d = %s", LIBXL_DEVICE_MODEL_VERSION_UNKNOWN, libxl_device_model_version_to_json(ctx, LIBXL_DEVICE_MODEL_VERSION_UNKNOWN));
    printf("\tQEMU_XEN_TRADITIONAL = %d = %s", LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN_TRADITIONAL, libxl_device_model_version_to_json(ctx, LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN_TRADITIONAL));
    printf("\tQEMU_XEN = %d = %s", LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN, libxl_device_model_version_to_json(ctx, LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN));

    printf("libxl_device_model_version -- from string:\n");
    libxl_device_model_version_val = -1;
    rc = libxl_device_model_version_from_string("UNknoWn", &libxl_device_model_version_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "UNknoWn", libxl_device_model_version_val, rc);
    libxl_device_model_version_val = -1;
    rc = libxl_device_model_version_from_string("QeMu_xeN_TraDitIOnAL", &libxl_device_model_version_val);
    printf("\tQEMU_XEN_TRADITIONAL = \"%s\" = %d (rc %d)\n", "QeMu_xeN_TraDitIOnAL", libxl_device_model_version_val, rc);
    libxl_device_model_version_val = -1;
    rc = libxl_device_model_version_from_string("QEmU_xEn", &libxl_device_model_version_val);
    printf("\tQEMU_XEN = \"%s\" = %d (rc %d)\n", "QEmU_xEn", libxl_device_model_version_val, rc);
    libxl_device_model_version_val = -1;
    rc = libxl_device_model_version_from_string("aN InvaLId value", &libxl_device_model_version_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "aN InvaLId value", libxl_device_model_version_val, rc);

    printf("libxl_console_type -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_CONSOLE_TYPE_UNKNOWN, libxl_console_type_to_string(LIBXL_CONSOLE_TYPE_UNKNOWN));
    printf("\tSERIAL = %d = \"%s\"\n", LIBXL_CONSOLE_TYPE_SERIAL, libxl_console_type_to_string(LIBXL_CONSOLE_TYPE_SERIAL));
    printf("\tPV = %d = \"%s\"\n", LIBXL_CONSOLE_TYPE_PV, libxl_console_type_to_string(LIBXL_CONSOLE_TYPE_PV));

    printf("libxl_console_type -- to JSON:\n");
    printf("\tUNKNOWN = %d = %s", LIBXL_CONSOLE_TYPE_UNKNOWN, libxl_console_type_to_json(ctx, LIBXL_CONSOLE_TYPE_UNKNOWN));
    printf("\tSERIAL = %d = %s", LIBXL_CONSOLE_TYPE_SERIAL, libxl_console_type_to_json(ctx, LIBXL_CONSOLE_TYPE_SERIAL));
    printf("\tPV = %d = %s", LIBXL_CONSOLE_TYPE_PV, libxl_console_type_to_json(ctx, LIBXL_CONSOLE_TYPE_PV));

    printf("libxl_console_type -- from string:\n");
    libxl_console_type_val = -1;
    rc = libxl_console_type_from_string("unKnoWN", &libxl_console_type_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "unKnoWN", libxl_console_type_val, rc);
    libxl_console_type_val = -1;
    rc = libxl_console_type_from_string("sERIAl", &libxl_console_type_val);
    printf("\tSERIAL = \"%s\" = %d (rc %d)\n", "sERIAl", libxl_console_type_val, rc);
    libxl_console_type_val = -1;
    rc = libxl_console_type_from_string("pV", &libxl_console_type_val);
    printf("\tPV = \"%s\" = %d (rc %d)\n", "pV", libxl_console_type_val, rc);
    libxl_console_type_val = -1;
    rc = libxl_console_type_from_string("An INVaLId vaLue", &libxl_console_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "An INVaLId vaLue", libxl_console_type_val, rc);

    printf("libxl_disk_format -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_DISK_FORMAT_UNKNOWN, libxl_disk_format_to_string(LIBXL_DISK_FORMAT_UNKNOWN));
    printf("\tQCOW = %d = \"%s\"\n", LIBXL_DISK_FORMAT_QCOW, libxl_disk_format_to_string(LIBXL_DISK_FORMAT_QCOW));
    printf("\tQCOW2 = %d = \"%s\"\n", LIBXL_DISK_FORMAT_QCOW2, libxl_disk_format_to_string(LIBXL_DISK_FORMAT_QCOW2));
    printf("\tVHD = %d = \"%s\"\n", LIBXL_DISK_FORMAT_VHD, libxl_disk_format_to_string(LIBXL_DISK_FORMAT_VHD));
    printf("\tRAW = %d = \"%s\"\n", LIBXL_DISK_FORMAT_RAW, libxl_disk_format_to_string(LIBXL_DISK_FORMAT_RAW));
    printf("\tEMPTY = %d = \"%s\"\n", LIBXL_DISK_FORMAT_EMPTY, libxl_disk_format_to_string(LIBXL_DISK_FORMAT_EMPTY));

    printf("libxl_disk_format -- to JSON:\n");
    printf("\tUNKNOWN = %d = %s", LIBXL_DISK_FORMAT_UNKNOWN, libxl_disk_format_to_json(ctx, LIBXL_DISK_FORMAT_UNKNOWN));
    printf("\tQCOW = %d = %s", LIBXL_DISK_FORMAT_QCOW, libxl_disk_format_to_json(ctx, LIBXL_DISK_FORMAT_QCOW));
    printf("\tQCOW2 = %d = %s", LIBXL_DISK_FORMAT_QCOW2, libxl_disk_format_to_json(ctx, LIBXL_DISK_FORMAT_QCOW2));
    printf("\tVHD = %d = %s", LIBXL_DISK_FORMAT_VHD, libxl_disk_format_to_json(ctx, LIBXL_DISK_FORMAT_VHD));
    printf("\tRAW = %d = %s", LIBXL_DISK_FORMAT_RAW, libxl_disk_format_to_json(ctx, LIBXL_DISK_FORMAT_RAW));
    printf("\tEMPTY = %d = %s", LIBXL_DISK_FORMAT_EMPTY, libxl_disk_format_to_json(ctx, LIBXL_DISK_FORMAT_EMPTY));

    printf("libxl_disk_format -- from string:\n");
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("unKnOwN", &libxl_disk_format_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "unKnOwN", libxl_disk_format_val, rc);
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("QcOw", &libxl_disk_format_val);
    printf("\tQCOW = \"%s\" = %d (rc %d)\n", "QcOw", libxl_disk_format_val, rc);
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("qCoW2", &libxl_disk_format_val);
    printf("\tQCOW2 = \"%s\" = %d (rc %d)\n", "qCoW2", libxl_disk_format_val, rc);
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("VHd", &libxl_disk_format_val);
    printf("\tVHD = \"%s\" = %d (rc %d)\n", "VHd", libxl_disk_format_val, rc);
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("RAw", &libxl_disk_format_val);
    printf("\tRAW = \"%s\" = %d (rc %d)\n", "RAw", libxl_disk_format_val, rc);
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("EmPTy", &libxl_disk_format_val);
    printf("\tEMPTY = \"%s\" = %d (rc %d)\n", "EmPTy", libxl_disk_format_val, rc);
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("AN invALiD value", &libxl_disk_format_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN invALiD value", libxl_disk_format_val, rc);

    printf("libxl_disk_backend -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_DISK_BACKEND_UNKNOWN, libxl_disk_backend_to_string(LIBXL_DISK_BACKEND_UNKNOWN));
    printf("\tPHY = %d = \"%s\"\n", LIBXL_DISK_BACKEND_PHY, libxl_disk_backend_to_string(LIBXL_DISK_BACKEND_PHY));
    printf("\tTAP = %d = \"%s\"\n", LIBXL_DISK_BACKEND_TAP, libxl_disk_backend_to_string(LIBXL_DISK_BACKEND_TAP));
    printf("\tQDISK = %d = \"%s\"\n", LIBXL_DISK_BACKEND_QDISK, libxl_disk_backend_to_string(LIBXL_DISK_BACKEND_QDISK));

    printf("libxl_disk_backend -- to JSON:\n");
    printf("\tUNKNOWN = %d = %s", LIBXL_DISK_BACKEND_UNKNOWN, libxl_disk_backend_to_json(ctx, LIBXL_DISK_BACKEND_UNKNOWN));
    printf("\tPHY = %d = %s", LIBXL_DISK_BACKEND_PHY, libxl_disk_backend_to_json(ctx, LIBXL_DISK_BACKEND_PHY));
    printf("\tTAP = %d = %s", LIBXL_DISK_BACKEND_TAP, libxl_disk_backend_to_json(ctx, LIBXL_DISK_BACKEND_TAP));
    printf("\tQDISK = %d = %s", LIBXL_DISK_BACKEND_QDISK, libxl_disk_backend_to_json(ctx, LIBXL_DISK_BACKEND_QDISK));

    printf("libxl_disk_backend -- from string:\n");
    libxl_disk_backend_val = -1;
    rc = libxl_disk_backend_from_string("uNkNOWN", &libxl_disk_backend_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "uNkNOWN", libxl_disk_backend_val, rc);
    libxl_disk_backend_val = -1;
    rc = libxl_disk_backend_from_string("Phy", &libxl_disk_backend_val);
    printf("\tPHY = \"%s\" = %d (rc %d)\n", "Phy", libxl_disk_backend_val, rc);
    libxl_disk_backend_val = -1;
    rc = libxl_disk_backend_from_string("TAP", &libxl_disk_backend_val);
    printf("\tTAP = \"%s\" = %d (rc %d)\n", "TAP", libxl_disk_backend_val, rc);
    libxl_disk_backend_val = -1;
    rc = libxl_disk_backend_from_string("qdisk", &libxl_disk_backend_val);
    printf("\tQDISK = \"%s\" = %d (rc %d)\n", "qdisk", libxl_disk_backend_val, rc);
    libxl_disk_backend_val = -1;
    rc = libxl_disk_backend_from_string("an iNvALID VAlUe", &libxl_disk_backend_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "an iNvALID VAlUe", libxl_disk_backend_val, rc);

    printf("libxl_nic_type -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_NIC_TYPE_UNKNOWN, libxl_nic_type_to_string(LIBXL_NIC_TYPE_UNKNOWN));
    printf("\tVIF_IOEMU = %d = \"%s\"\n", LIBXL_NIC_TYPE_VIF_IOEMU, libxl_nic_type_to_string(LIBXL_NIC_TYPE_VIF_IOEMU));
    printf("\tVIF = %d = \"%s\"\n", LIBXL_NIC_TYPE_VIF, libxl_nic_type_to_string(LIBXL_NIC_TYPE_VIF));

    printf("libxl_nic_type -- to JSON:\n");
    printf("\tUNKNOWN = %d = %s", LIBXL_NIC_TYPE_UNKNOWN, libxl_nic_type_to_json(ctx, LIBXL_NIC_TYPE_UNKNOWN));
    printf("\tVIF_IOEMU = %d = %s", LIBXL_NIC_TYPE_VIF_IOEMU, libxl_nic_type_to_json(ctx, LIBXL_NIC_TYPE_VIF_IOEMU));
    printf("\tVIF = %d = %s", LIBXL_NIC_TYPE_VIF, libxl_nic_type_to_json(ctx, LIBXL_NIC_TYPE_VIF));

    printf("libxl_nic_type -- from string:\n");
    libxl_nic_type_val = -1;
    rc = libxl_nic_type_from_string("UnKnOwN", &libxl_nic_type_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "UnKnOwN", libxl_nic_type_val, rc);
    libxl_nic_type_val = -1;
    rc = libxl_nic_type_from_string("vif_IoEMu", &libxl_nic_type_val);
    printf("\tVIF_IOEMU = \"%s\" = %d (rc %d)\n", "vif_IoEMu", libxl_nic_type_val, rc);
    libxl_nic_type_val = -1;
    rc = libxl_nic_type_from_string("vIF", &libxl_nic_type_val);
    printf("\tVIF = \"%s\" = %d (rc %d)\n", "vIF", libxl_nic_type_val, rc);
    libxl_nic_type_val = -1;
    rc = libxl_nic_type_from_string("an iNVaLID vALUE", &libxl_nic_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "an iNVaLID vALUE", libxl_nic_type_val, rc);

    printf("libxl_action_on_shutdown -- to string:\n");
    printf("\tDESTROY = %d = \"%s\"\n", LIBXL_ACTION_ON_SHUTDOWN_DESTROY, libxl_action_on_shutdown_to_string(LIBXL_ACTION_ON_SHUTDOWN_DESTROY));
    printf("\tRESTART = %d = \"%s\"\n", LIBXL_ACTION_ON_SHUTDOWN_RESTART, libxl_action_on_shutdown_to_string(LIBXL_ACTION_ON_SHUTDOWN_RESTART));
    printf("\tRESTART_RENAME = %d = \"%s\"\n", LIBXL_ACTION_ON_SHUTDOWN_RESTART_RENAME, libxl_action_on_shutdown_to_string(LIBXL_ACTION_ON_SHUTDOWN_RESTART_RENAME));
    printf("\tPRESERVE = %d = \"%s\"\n", LIBXL_ACTION_ON_SHUTDOWN_PRESERVE, libxl_action_on_shutdown_to_string(LIBXL_ACTION_ON_SHUTDOWN_PRESERVE));
    printf("\tCOREDUMP_DESTROY = %d = \"%s\"\n", LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY, libxl_action_on_shutdown_to_string(LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY));
    printf("\tCOREDUMP_RESTART = %d = \"%s\"\n", LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART, libxl_action_on_shutdown_to_string(LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART));

    printf("libxl_action_on_shutdown -- to JSON:\n");
    printf("\tDESTROY = %d = %s", LIBXL_ACTION_ON_SHUTDOWN_DESTROY, libxl_action_on_shutdown_to_json(ctx, LIBXL_ACTION_ON_SHUTDOWN_DESTROY));
    printf("\tRESTART = %d = %s", LIBXL_ACTION_ON_SHUTDOWN_RESTART, libxl_action_on_shutdown_to_json(ctx, LIBXL_ACTION_ON_SHUTDOWN_RESTART));
    printf("\tRESTART_RENAME = %d = %s", LIBXL_ACTION_ON_SHUTDOWN_RESTART_RENAME, libxl_action_on_shutdown_to_json(ctx, LIBXL_ACTION_ON_SHUTDOWN_RESTART_RENAME));
    printf("\tPRESERVE = %d = %s", LIBXL_ACTION_ON_SHUTDOWN_PRESERVE, libxl_action_on_shutdown_to_json(ctx, LIBXL_ACTION_ON_SHUTDOWN_PRESERVE));
    printf("\tCOREDUMP_DESTROY = %d = %s", LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY, libxl_action_on_shutdown_to_json(ctx, LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY));
    printf("\tCOREDUMP_RESTART = %d = %s", LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART, libxl_action_on_shutdown_to_json(ctx, LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART));

    printf("libxl_action_on_shutdown -- from string:\n");
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("DestrOY", &libxl_action_on_shutdown_val);
    printf("\tDESTROY = \"%s\" = %d (rc %d)\n", "DestrOY", libxl_action_on_shutdown_val, rc);
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("ResTart", &libxl_action_on_shutdown_val);
    printf("\tRESTART = \"%s\" = %d (rc %d)\n", "ResTart", libxl_action_on_shutdown_val, rc);
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("rESTaRt_renAME", &libxl_action_on_shutdown_val);
    printf("\tRESTART_RENAME = \"%s\" = %d (rc %d)\n", "rESTaRt_renAME", libxl_action_on_shutdown_val, rc);
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("prESERve", &libxl_action_on_shutdown_val);
    printf("\tPRESERVE = \"%s\" = %d (rc %d)\n", "prESERve", libxl_action_on_shutdown_val, rc);
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("COREDUmP_DestRoY", &libxl_action_on_shutdown_val);
    printf("\tCOREDUMP_DESTROY = \"%s\" = %d (rc %d)\n", "COREDUmP_DestRoY", libxl_action_on_shutdown_val, rc);
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("cOreDump_rEstArT", &libxl_action_on_shutdown_val);
    printf("\tCOREDUMP_RESTART = \"%s\" = %d (rc %d)\n", "cOreDump_rEstArT", libxl_action_on_shutdown_val, rc);
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("AN InVaLId vaLuE", &libxl_action_on_shutdown_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN InVaLId vaLuE", libxl_action_on_shutdown_val, rc);

    printf("libxl_trigger -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_TRIGGER_UNKNOWN, libxl_trigger_to_string(LIBXL_TRIGGER_UNKNOWN));
    printf("\tPOWER = %d = \"%s\"\n", LIBXL_TRIGGER_POWER, libxl_trigger_to_string(LIBXL_TRIGGER_POWER));
    printf("\tSLEEP = %d = \"%s\"\n", LIBXL_TRIGGER_SLEEP, libxl_trigger_to_string(LIBXL_TRIGGER_SLEEP));
    printf("\tNMI = %d = \"%s\"\n", LIBXL_TRIGGER_NMI, libxl_trigger_to_string(LIBXL_TRIGGER_NMI));
    printf("\tINIT = %d = \"%s\"\n", LIBXL_TRIGGER_INIT, libxl_trigger_to_string(LIBXL_TRIGGER_INIT));
    printf("\tRESET = %d = \"%s\"\n", LIBXL_TRIGGER_RESET, libxl_trigger_to_string(LIBXL_TRIGGER_RESET));
    printf("\tS3RESUME = %d = \"%s\"\n", LIBXL_TRIGGER_S3RESUME, libxl_trigger_to_string(LIBXL_TRIGGER_S3RESUME));

    printf("libxl_trigger -- to JSON:\n");
    printf("\tUNKNOWN = %d = %s", LIBXL_TRIGGER_UNKNOWN, libxl_trigger_to_json(ctx, LIBXL_TRIGGER_UNKNOWN));
    printf("\tPOWER = %d = %s", LIBXL_TRIGGER_POWER, libxl_trigger_to_json(ctx, LIBXL_TRIGGER_POWER));
    printf("\tSLEEP = %d = %s", LIBXL_TRIGGER_SLEEP, libxl_trigger_to_json(ctx, LIBXL_TRIGGER_SLEEP));
    printf("\tNMI = %d = %s", LIBXL_TRIGGER_NMI, libxl_trigger_to_json(ctx, LIBXL_TRIGGER_NMI));
    printf("\tINIT = %d = %s", LIBXL_TRIGGER_INIT, libxl_trigger_to_json(ctx, LIBXL_TRIGGER_INIT));
    printf("\tRESET = %d = %s", LIBXL_TRIGGER_RESET, libxl_trigger_to_json(ctx, LIBXL_TRIGGER_RESET));
    printf("\tS3RESUME = %d = %s", LIBXL_TRIGGER_S3RESUME, libxl_trigger_to_json(ctx, LIBXL_TRIGGER_S3RESUME));

    printf("libxl_trigger -- from string:\n");
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("UNknOwn", &libxl_trigger_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "UNknOwn", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("poWeR", &libxl_trigger_val);
    printf("\tPOWER = \"%s\" = %d (rc %d)\n", "poWeR", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("SlEEP", &libxl_trigger_val);
    printf("\tSLEEP = \"%s\" = %d (rc %d)\n", "SlEEP", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("nMI", &libxl_trigger_val);
    printf("\tNMI = \"%s\" = %d (rc %d)\n", "nMI", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("iniT", &libxl_trigger_val);
    printf("\tINIT = \"%s\" = %d (rc %d)\n", "iniT", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("ReSet", &libxl_trigger_val);
    printf("\tRESET = \"%s\" = %d (rc %d)\n", "ReSet", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("S3rESuMe", &libxl_trigger_val);
    printf("\tS3RESUME = \"%s\" = %d (rc %d)\n", "S3rESuMe", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("an InvalId ValUe", &libxl_trigger_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "an InvalId ValUe", libxl_trigger_val, rc);

    printf("libxl_tsc_mode -- to string:\n");
    printf("\tDEFAULT = %d = \"%s\"\n", LIBXL_TSC_MODE_DEFAULT, libxl_tsc_mode_to_string(LIBXL_TSC_MODE_DEFAULT));
    printf("\tALWAYS_EMULATE = %d = \"%s\"\n", LIBXL_TSC_MODE_ALWAYS_EMULATE, libxl_tsc_mode_to_string(LIBXL_TSC_MODE_ALWAYS_EMULATE));
    printf("\tNATIVE = %d = \"%s\"\n", LIBXL_TSC_MODE_NATIVE, libxl_tsc_mode_to_string(LIBXL_TSC_MODE_NATIVE));
    printf("\tNATIVE_PARAVIRT = %d = \"%s\"\n", LIBXL_TSC_MODE_NATIVE_PARAVIRT, libxl_tsc_mode_to_string(LIBXL_TSC_MODE_NATIVE_PARAVIRT));

    printf("libxl_tsc_mode -- to JSON:\n");
    printf("\tDEFAULT = %d = %s", LIBXL_TSC_MODE_DEFAULT, libxl_tsc_mode_to_json(ctx, LIBXL_TSC_MODE_DEFAULT));
    printf("\tALWAYS_EMULATE = %d = %s", LIBXL_TSC_MODE_ALWAYS_EMULATE, libxl_tsc_mode_to_json(ctx, LIBXL_TSC_MODE_ALWAYS_EMULATE));
    printf("\tNATIVE = %d = %s", LIBXL_TSC_MODE_NATIVE, libxl_tsc_mode_to_json(ctx, LIBXL_TSC_MODE_NATIVE));
    printf("\tNATIVE_PARAVIRT = %d = %s", LIBXL_TSC_MODE_NATIVE_PARAVIRT, libxl_tsc_mode_to_json(ctx, LIBXL_TSC_MODE_NATIVE_PARAVIRT));

    printf("libxl_tsc_mode -- from string:\n");
    libxl_tsc_mode_val = -1;
    rc = libxl_tsc_mode_from_string("DEFAult", &libxl_tsc_mode_val);
    printf("\tDEFAULT = \"%s\" = %d (rc %d)\n", "DEFAult", libxl_tsc_mode_val, rc);
    libxl_tsc_mode_val = -1;
    rc = libxl_tsc_mode_from_string("Always_eMUlatE", &libxl_tsc_mode_val);
    printf("\tALWAYS_EMULATE = \"%s\" = %d (rc %d)\n", "Always_eMUlatE", libxl_tsc_mode_val, rc);
    libxl_tsc_mode_val = -1;
    rc = libxl_tsc_mode_from_string("nATIVe", &libxl_tsc_mode_val);
    printf("\tNATIVE = \"%s\" = %d (rc %d)\n", "nATIVe", libxl_tsc_mode_val, rc);
    libxl_tsc_mode_val = -1;
    rc = libxl_tsc_mode_from_string("native_PArAvIrT", &libxl_tsc_mode_val);
    printf("\tNATIVE_PARAVIRT = \"%s\" = %d (rc %d)\n", "native_PArAvIrT", libxl_tsc_mode_val, rc);
    libxl_tsc_mode_val = -1;
    rc = libxl_tsc_mode_from_string("aN INvalID VALue", &libxl_tsc_mode_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "aN INvalID VALue", libxl_tsc_mode_val, rc);

    printf("libxl_timer_mode -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_TIMER_MODE_UNKNOWN, libxl_timer_mode_to_string(LIBXL_TIMER_MODE_UNKNOWN));
    printf("\tDELAY_FOR_MISSED_TICKS = %d = \"%s\"\n", LIBXL_TIMER_MODE_DELAY_FOR_MISSED_TICKS, libxl_timer_mode_to_string(LIBXL_TIMER_MODE_DELAY_FOR_MISSED_TICKS));
    printf("\tNO_DELAY_FOR_MISSED_TICKS = %d = \"%s\"\n", LIBXL_TIMER_MODE_NO_DELAY_FOR_MISSED_TICKS, libxl_timer_mode_to_string(LIBXL_TIMER_MODE_NO_DELAY_FOR_MISSED_TICKS));
    printf("\tNO_MISSED_TICKS_PENDING = %d = \"%s\"\n", LIBXL_TIMER_MODE_NO_MISSED_TICKS_PENDING, libxl_timer_mode_to_string(LIBXL_TIMER_MODE_NO_MISSED_TICKS_PENDING));
    printf("\tONE_MISSED_TICK_PENDING = %d = \"%s\"\n", LIBXL_TIMER_MODE_ONE_MISSED_TICK_PENDING, libxl_timer_mode_to_string(LIBXL_TIMER_MODE_ONE_MISSED_TICK_PENDING));

    printf("libxl_timer_mode -- to JSON:\n");
    printf("\tUNKNOWN = %d = %s", LIBXL_TIMER_MODE_UNKNOWN, libxl_timer_mode_to_json(ctx, LIBXL_TIMER_MODE_UNKNOWN));
    printf("\tDELAY_FOR_MISSED_TICKS = %d = %s", LIBXL_TIMER_MODE_DELAY_FOR_MISSED_TICKS, libxl_timer_mode_to_json(ctx, LIBXL_TIMER_MODE_DELAY_FOR_MISSED_TICKS));
    printf("\tNO_DELAY_FOR_MISSED_TICKS = %d = %s", LIBXL_TIMER_MODE_NO_DELAY_FOR_MISSED_TICKS, libxl_timer_mode_to_json(ctx, LIBXL_TIMER_MODE_NO_DELAY_FOR_MISSED_TICKS));
    printf("\tNO_MISSED_TICKS_PENDING = %d = %s", LIBXL_TIMER_MODE_NO_MISSED_TICKS_PENDING, libxl_timer_mode_to_json(ctx, LIBXL_TIMER_MODE_NO_MISSED_TICKS_PENDING));
    printf("\tONE_MISSED_TICK_PENDING = %d = %s", LIBXL_TIMER_MODE_ONE_MISSED_TICK_PENDING, libxl_timer_mode_to_json(ctx, LIBXL_TIMER_MODE_ONE_MISSED_TICK_PENDING));

    printf("libxl_timer_mode -- from string:\n");
    libxl_timer_mode_val = -1;
    rc = libxl_timer_mode_from_string("uNknoWN", &libxl_timer_mode_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "uNknoWN", libxl_timer_mode_val, rc);
    libxl_timer_mode_val = -1;
    rc = libxl_timer_mode_from_string("DeLAY_For_mIsSEd_TiCKS", &libxl_timer_mode_val);
    printf("\tDELAY_FOR_MISSED_TICKS = \"%s\" = %d (rc %d)\n", "DeLAY_For_mIsSEd_TiCKS", libxl_timer_mode_val, rc);
    libxl_timer_mode_val = -1;
    rc = libxl_timer_mode_from_string("No_deLAy_for_mIssED_TickS", &libxl_timer_mode_val);
    printf("\tNO_DELAY_FOR_MISSED_TICKS = \"%s\" = %d (rc %d)\n", "No_deLAy_for_mIssED_TickS", libxl_timer_mode_val, rc);
    libxl_timer_mode_val = -1;
    rc = libxl_timer_mode_from_string("nO_mIsSed_TICkS_PeNDIng", &libxl_timer_mode_val);
    printf("\tNO_MISSED_TICKS_PENDING = \"%s\" = %d (rc %d)\n", "nO_mIsSed_TICkS_PeNDIng", libxl_timer_mode_val, rc);
    libxl_timer_mode_val = -1;
    rc = libxl_timer_mode_from_string("ONE_MIssEd_ticK_peNdINg", &libxl_timer_mode_val);
    printf("\tONE_MISSED_TICK_PENDING = \"%s\" = %d (rc %d)\n", "ONE_MIssEd_ticK_peNdINg", libxl_timer_mode_val, rc);
    libxl_timer_mode_val = -1;
    rc = libxl_timer_mode_from_string("aN invALID vALUe", &libxl_timer_mode_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "aN invALID vALUe", libxl_timer_mode_val, rc);

    printf("libxl_bios_type -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_BIOS_TYPE_UNKNOWN, libxl_bios_type_to_string(LIBXL_BIOS_TYPE_UNKNOWN));
    printf("\tROMBIOS = %d = \"%s\"\n", LIBXL_BIOS_TYPE_ROMBIOS, libxl_bios_type_to_string(LIBXL_BIOS_TYPE_ROMBIOS));
    printf("\tSEABIOS = %d = \"%s\"\n", LIBXL_BIOS_TYPE_SEABIOS, libxl_bios_type_to_string(LIBXL_BIOS_TYPE_SEABIOS));
    printf("\tOVMF = %d = \"%s\"\n", LIBXL_BIOS_TYPE_OVMF, libxl_bios_type_to_string(LIBXL_BIOS_TYPE_OVMF));

    printf("libxl_bios_type -- to JSON:\n");
    printf("\tUNKNOWN = %d = %s", LIBXL_BIOS_TYPE_UNKNOWN, libxl_bios_type_to_json(ctx, LIBXL_BIOS_TYPE_UNKNOWN));
    printf("\tROMBIOS = %d = %s", LIBXL_BIOS_TYPE_ROMBIOS, libxl_bios_type_to_json(ctx, LIBXL_BIOS_TYPE_ROMBIOS));
    printf("\tSEABIOS = %d = %s", LIBXL_BIOS_TYPE_SEABIOS, libxl_bios_type_to_json(ctx, LIBXL_BIOS_TYPE_SEABIOS));
    printf("\tOVMF = %d = %s", LIBXL_BIOS_TYPE_OVMF, libxl_bios_type_to_json(ctx, LIBXL_BIOS_TYPE_OVMF));

    printf("libxl_bios_type -- from string:\n");
    libxl_bios_type_val = -1;
    rc = libxl_bios_type_from_string("UNkNOWn", &libxl_bios_type_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "UNkNOWn", libxl_bios_type_val, rc);
    libxl_bios_type_val = -1;
    rc = libxl_bios_type_from_string("romBiOs", &libxl_bios_type_val);
    printf("\tROMBIOS = \"%s\" = %d (rc %d)\n", "romBiOs", libxl_bios_type_val, rc);
    libxl_bios_type_val = -1;
    rc = libxl_bios_type_from_string("sEaBIoS", &libxl_bios_type_val);
    printf("\tSEABIOS = \"%s\" = %d (rc %d)\n", "sEaBIoS", libxl_bios_type_val, rc);
    libxl_bios_type_val = -1;
    rc = libxl_bios_type_from_string("oVmf", &libxl_bios_type_val);
    printf("\tOVMF = \"%s\" = %d (rc %d)\n", "oVmf", libxl_bios_type_val, rc);
    libxl_bios_type_val = -1;
    rc = libxl_bios_type_from_string("An iNvalID ValUe", &libxl_bios_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "An iNvalID ValUe", libxl_bios_type_val, rc);

    printf("libxl_scheduler -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_SCHEDULER_UNKNOWN, libxl_scheduler_to_string(LIBXL_SCHEDULER_UNKNOWN));
    printf("\tSEDF = %d = \"%s\"\n", LIBXL_SCHEDULER_SEDF, libxl_scheduler_to_string(LIBXL_SCHEDULER_SEDF));
    printf("\tCREDIT = %d = \"%s\"\n", LIBXL_SCHEDULER_CREDIT, libxl_scheduler_to_string(LIBXL_SCHEDULER_CREDIT));
    printf("\tCREDIT2 = %d = \"%s\"\n", LIBXL_SCHEDULER_CREDIT2, libxl_scheduler_to_string(LIBXL_SCHEDULER_CREDIT2));
    printf("\tARINC653 = %d = \"%s\"\n", LIBXL_SCHEDULER_ARINC653, libxl_scheduler_to_string(LIBXL_SCHEDULER_ARINC653));

    printf("libxl_scheduler -- to JSON:\n");
    printf("\tUNKNOWN = %d = %s", LIBXL_SCHEDULER_UNKNOWN, libxl_scheduler_to_json(ctx, LIBXL_SCHEDULER_UNKNOWN));
    printf("\tSEDF = %d = %s", LIBXL_SCHEDULER_SEDF, libxl_scheduler_to_json(ctx, LIBXL_SCHEDULER_SEDF));
    printf("\tCREDIT = %d = %s", LIBXL_SCHEDULER_CREDIT, libxl_scheduler_to_json(ctx, LIBXL_SCHEDULER_CREDIT));
    printf("\tCREDIT2 = %d = %s", LIBXL_SCHEDULER_CREDIT2, libxl_scheduler_to_json(ctx, LIBXL_SCHEDULER_CREDIT2));
    printf("\tARINC653 = %d = %s", LIBXL_SCHEDULER_ARINC653, libxl_scheduler_to_json(ctx, LIBXL_SCHEDULER_ARINC653));

    printf("libxl_scheduler -- from string:\n");
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("UnKNowN", &libxl_scheduler_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "UnKNowN", libxl_scheduler_val, rc);
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("SeDF", &libxl_scheduler_val);
    printf("\tSEDF = \"%s\" = %d (rc %d)\n", "SeDF", libxl_scheduler_val, rc);
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("credit", &libxl_scheduler_val);
    printf("\tCREDIT = \"%s\" = %d (rc %d)\n", "credit", libxl_scheduler_val, rc);
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("crEdIT2", &libxl_scheduler_val);
    printf("\tCREDIT2 = \"%s\" = %d (rc %d)\n", "crEdIT2", libxl_scheduler_val, rc);
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("ArInc653", &libxl_scheduler_val);
    printf("\tARINC653 = \"%s\" = %d (rc %d)\n", "ArInc653", libxl_scheduler_val, rc);
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("An INvaLiD vALUe", &libxl_scheduler_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "An INvaLiD vALUe", libxl_scheduler_val, rc);

    printf("libxl_shutdown_reason -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_SHUTDOWN_REASON_UNKNOWN, libxl_shutdown_reason_to_string(LIBXL_SHUTDOWN_REASON_UNKNOWN));
    printf("\tPOWEROFF = %d = \"%s\"\n", LIBXL_SHUTDOWN_REASON_POWEROFF, libxl_shutdown_reason_to_string(LIBXL_SHUTDOWN_REASON_POWEROFF));
    printf("\tREBOOT = %d = \"%s\"\n", LIBXL_SHUTDOWN_REASON_REBOOT, libxl_shutdown_reason_to_string(LIBXL_SHUTDOWN_REASON_REBOOT));
    printf("\tSUSPEND = %d = \"%s\"\n", LIBXL_SHUTDOWN_REASON_SUSPEND, libxl_shutdown_reason_to_string(LIBXL_SHUTDOWN_REASON_SUSPEND));
    printf("\tCRASH = %d = \"%s\"\n", LIBXL_SHUTDOWN_REASON_CRASH, libxl_shutdown_reason_to_string(LIBXL_SHUTDOWN_REASON_CRASH));
    printf("\tWATCHDOG = %d = \"%s\"\n", LIBXL_SHUTDOWN_REASON_WATCHDOG, libxl_shutdown_reason_to_string(LIBXL_SHUTDOWN_REASON_WATCHDOG));

    printf("libxl_shutdown_reason -- to JSON:\n");
    printf("\tUNKNOWN = %d = %s", LIBXL_SHUTDOWN_REASON_UNKNOWN, libxl_shutdown_reason_to_json(ctx, LIBXL_SHUTDOWN_REASON_UNKNOWN));
    printf("\tPOWEROFF = %d = %s", LIBXL_SHUTDOWN_REASON_POWEROFF, libxl_shutdown_reason_to_json(ctx, LIBXL_SHUTDOWN_REASON_POWEROFF));
    printf("\tREBOOT = %d = %s", LIBXL_SHUTDOWN_REASON_REBOOT, libxl_shutdown_reason_to_json(ctx, LIBXL_SHUTDOWN_REASON_REBOOT));
    printf("\tSUSPEND = %d = %s", LIBXL_SHUTDOWN_REASON_SUSPEND, libxl_shutdown_reason_to_json(ctx, LIBXL_SHUTDOWN_REASON_SUSPEND));
    printf("\tCRASH = %d = %s", LIBXL_SHUTDOWN_REASON_CRASH, libxl_shutdown_reason_to_json(ctx, LIBXL_SHUTDOWN_REASON_CRASH));
    printf("\tWATCHDOG = %d = %s", LIBXL_SHUTDOWN_REASON_WATCHDOG, libxl_shutdown_reason_to_json(ctx, LIBXL_SHUTDOWN_REASON_WATCHDOG));

    printf("libxl_shutdown_reason -- from string:\n");
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("unKNown", &libxl_shutdown_reason_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "unKNown", libxl_shutdown_reason_val, rc);
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("poWeRofF", &libxl_shutdown_reason_val);
    printf("\tPOWEROFF = \"%s\" = %d (rc %d)\n", "poWeRofF", libxl_shutdown_reason_val, rc);
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("ReboOt", &libxl_shutdown_reason_val);
    printf("\tREBOOT = \"%s\" = %d (rc %d)\n", "ReboOt", libxl_shutdown_reason_val, rc);
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("sUsPENd", &libxl_shutdown_reason_val);
    printf("\tSUSPEND = \"%s\" = %d (rc %d)\n", "sUsPENd", libxl_shutdown_reason_val, rc);
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("CrASh", &libxl_shutdown_reason_val);
    printf("\tCRASH = \"%s\" = %d (rc %d)\n", "CrASh", libxl_shutdown_reason_val, rc);
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("WaTChdOg", &libxl_shutdown_reason_val);
    printf("\tWATCHDOG = \"%s\" = %d (rc %d)\n", "WaTChdOg", libxl_shutdown_reason_val, rc);
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("AN InVAliD vALuE", &libxl_shutdown_reason_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN InVAliD vALuE", libxl_shutdown_reason_val, rc);

    printf("libxl_vga_interface_type -- to string:\n");
    printf("\tCIRRUS = %d = \"%s\"\n", LIBXL_VGA_INTERFACE_TYPE_CIRRUS, libxl_vga_interface_type_to_string(LIBXL_VGA_INTERFACE_TYPE_CIRRUS));
    printf("\tSTD = %d = \"%s\"\n", LIBXL_VGA_INTERFACE_TYPE_STD, libxl_vga_interface_type_to_string(LIBXL_VGA_INTERFACE_TYPE_STD));

    printf("libxl_vga_interface_type -- to JSON:\n");
    printf("\tCIRRUS = %d = %s", LIBXL_VGA_INTERFACE_TYPE_CIRRUS, libxl_vga_interface_type_to_json(ctx, LIBXL_VGA_INTERFACE_TYPE_CIRRUS));
    printf("\tSTD = %d = %s", LIBXL_VGA_INTERFACE_TYPE_STD, libxl_vga_interface_type_to_json(ctx, LIBXL_VGA_INTERFACE_TYPE_STD));

    printf("libxl_vga_interface_type -- from string:\n");
    libxl_vga_interface_type_val = -1;
    rc = libxl_vga_interface_type_from_string("cIrrus", &libxl_vga_interface_type_val);
    printf("\tCIRRUS = \"%s\" = %d (rc %d)\n", "cIrrus", libxl_vga_interface_type_val, rc);
    libxl_vga_interface_type_val = -1;
    rc = libxl_vga_interface_type_from_string("std", &libxl_vga_interface_type_val);
    printf("\tSTD = \"%s\" = %d (rc %d)\n", "std", libxl_vga_interface_type_val, rc);
    libxl_vga_interface_type_val = -1;
    rc = libxl_vga_interface_type_from_string("AN iNvALID VALue", &libxl_vga_interface_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN iNvALID VALue", libxl_vga_interface_type_val, rc);

    printf("libxl_vendor_device -- to string:\n");
    printf("\tNONE = %d = \"%s\"\n", LIBXL_VENDOR_DEVICE_NONE, libxl_vendor_device_to_string(LIBXL_VENDOR_DEVICE_NONE));
    printf("\tXENSERVER = %d = \"%s\"\n", LIBXL_VENDOR_DEVICE_XENSERVER, libxl_vendor_device_to_string(LIBXL_VENDOR_DEVICE_XENSERVER));

    printf("libxl_vendor_device -- to JSON:\n");
    printf("\tNONE = %d = %s", LIBXL_VENDOR_DEVICE_NONE, libxl_vendor_device_to_json(ctx, LIBXL_VENDOR_DEVICE_NONE));
    printf("\tXENSERVER = %d = %s", LIBXL_VENDOR_DEVICE_XENSERVER, libxl_vendor_device_to_json(ctx, LIBXL_VENDOR_DEVICE_XENSERVER));

    printf("libxl_vendor_device -- from string:\n");
    libxl_vendor_device_val = -1;
    rc = libxl_vendor_device_from_string("NOnE", &libxl_vendor_device_val);
    printf("\tNONE = \"%s\" = %d (rc %d)\n", "NOnE", libxl_vendor_device_val, rc);
    libxl_vendor_device_val = -1;
    rc = libxl_vendor_device_from_string("xENsErVer", &libxl_vendor_device_val);
    printf("\tXENSERVER = \"%s\" = %d (rc %d)\n", "xENsErVer", libxl_vendor_device_val, rc);
    libxl_vendor_device_val = -1;
    rc = libxl_vendor_device_from_string("an INvaLiD VALUE", &libxl_vendor_device_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "an INvaLiD VALUE", libxl_vendor_device_val, rc);

    printf("libxl_event_type -- to string:\n");
    printf("\tDOMAIN_SHUTDOWN = %d = \"%s\"\n", LIBXL_EVENT_TYPE_DOMAIN_SHUTDOWN, libxl_event_type_to_string(LIBXL_EVENT_TYPE_DOMAIN_SHUTDOWN));
    printf("\tDOMAIN_DEATH = %d = \"%s\"\n", LIBXL_EVENT_TYPE_DOMAIN_DEATH, libxl_event_type_to_string(LIBXL_EVENT_TYPE_DOMAIN_DEATH));
    printf("\tDISK_EJECT = %d = \"%s\"\n", LIBXL_EVENT_TYPE_DISK_EJECT, libxl_event_type_to_string(LIBXL_EVENT_TYPE_DISK_EJECT));
    printf("\tOPERATION_COMPLETE = %d = \"%s\"\n", LIBXL_EVENT_TYPE_OPERATION_COMPLETE, libxl_event_type_to_string(LIBXL_EVENT_TYPE_OPERATION_COMPLETE));
    printf("\tDOMAIN_CREATE_CONSOLE_AVAILABLE = %d = \"%s\"\n", LIBXL_EVENT_TYPE_DOMAIN_CREATE_CONSOLE_AVAILABLE, libxl_event_type_to_string(LIBXL_EVENT_TYPE_DOMAIN_CREATE_CONSOLE_AVAILABLE));

    printf("libxl_event_type -- to JSON:\n");
    printf("\tDOMAIN_SHUTDOWN = %d = %s", LIBXL_EVENT_TYPE_DOMAIN_SHUTDOWN, libxl_event_type_to_json(ctx, LIBXL_EVENT_TYPE_DOMAIN_SHUTDOWN));
    printf("\tDOMAIN_DEATH = %d = %s", LIBXL_EVENT_TYPE_DOMAIN_DEATH, libxl_event_type_to_json(ctx, LIBXL_EVENT_TYPE_DOMAIN_DEATH));
    printf("\tDISK_EJECT = %d = %s", LIBXL_EVENT_TYPE_DISK_EJECT, libxl_event_type_to_json(ctx, LIBXL_EVENT_TYPE_DISK_EJECT));
    printf("\tOPERATION_COMPLETE = %d = %s", LIBXL_EVENT_TYPE_OPERATION_COMPLETE, libxl_event_type_to_json(ctx, LIBXL_EVENT_TYPE_OPERATION_COMPLETE));
    printf("\tDOMAIN_CREATE_CONSOLE_AVAILABLE = %d = %s", LIBXL_EVENT_TYPE_DOMAIN_CREATE_CONSOLE_AVAILABLE, libxl_event_type_to_json(ctx, LIBXL_EVENT_TYPE_DOMAIN_CREATE_CONSOLE_AVAILABLE));

    printf("libxl_event_type -- from string:\n");
    libxl_event_type_val = -1;
    rc = libxl_event_type_from_string("DOmAiN_sHutDOWN", &libxl_event_type_val);
    printf("\tDOMAIN_SHUTDOWN = \"%s\" = %d (rc %d)\n", "DOmAiN_sHutDOWN", libxl_event_type_val, rc);
    libxl_event_type_val = -1;
    rc = libxl_event_type_from_string("doMAiN_DeAth", &libxl_event_type_val);
    printf("\tDOMAIN_DEATH = \"%s\" = %d (rc %d)\n", "doMAiN_DeAth", libxl_event_type_val, rc);
    libxl_event_type_val = -1;
    rc = libxl_event_type_from_string("DISK_EJeCt", &libxl_event_type_val);
    printf("\tDISK_EJECT = \"%s\" = %d (rc %d)\n", "DISK_EJeCt", libxl_event_type_val, rc);
    libxl_event_type_val = -1;
    rc = libxl_event_type_from_string("oPERATiON_compLEte", &libxl_event_type_val);
    printf("\tOPERATION_COMPLETE = \"%s\" = %d (rc %d)\n", "oPERATiON_compLEte", libxl_event_type_val, rc);
    libxl_event_type_val = -1;
    rc = libxl_event_type_from_string("DoMaIn_creAte_cOnsOlE_AvaILaBle", &libxl_event_type_val);
    printf("\tDOMAIN_CREATE_CONSOLE_AVAILABLE = \"%s\" = %d (rc %d)\n", "DoMaIn_creAte_cOnsOlE_AvaILaBle", libxl_event_type_val, rc);
    libxl_event_type_val = -1;
    rc = libxl_event_type_from_string("An INValID VAlUe", &libxl_event_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "An INValID VAlUe", libxl_event_type_val, rc);



    libxl_ctx_free(ctx);
    xtl_logger_destroy((xentoollog_logger*)logger);

    return 0;
}

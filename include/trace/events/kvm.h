#if !defined(_TRACE_KVM_MAIN_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_KVM_MAIN_H

#include <linux/tracepoint.h>

#undef TRACE_SYSTEM
#define TRACE_SYSTEM kvm

#define ERSN(x) { KVM_EXIT_##x, "KVM_EXIT_" #x }

#define kvm_trace_exit_reason						\
	ERSN(UNKNOWN), ERSN(EXCEPTION), ERSN(IO), ERSN(HYPERCALL),	\
	ERSN(DEBUG), ERSN(HLT), ERSN(MMIO), ERSN(IRQ_WINDOW_OPEN),	\
	ERSN(SHUTDOWN), ERSN(FAIL_ENTRY), ERSN(INTR), ERSN(SET_TPR),	\
	ERSN(TPR_ACCESS), ERSN(S390_SIEIC), ERSN(S390_RESET), ERSN(DCR),\
	ERSN(NMI), ERSN(INTERNAL_ERROR), ERSN(OSI)

TRACE_EVENT(kvm_userspace_exit,
	    TP_PROTO(__u32 reason, int errno),
	    TP_ARGS(reason, errno),

	TP_STRUCT__entry(
		__field(	__u32,		reason		)
		__field(	int,		errno		)
	),

	TP_fast_assign(
		__entry->reason		= reason;
		__entry->errno		= errno;
	),

	TP_printk("reason %s (%d)",
		  __entry->errno < 0 ?
		  (__entry->errno == -EINTR ? "restart" : "error") :
		  __print_symbolic(__entry->reason, kvm_trace_exit_reason),
		  __entry->errno < 0 ? -__entry->errno : __entry->reason)
);

#if defined(__KVM_HAVE_IOAPIC)
TRACE_EVENT(kvm_set_irq,
	TP_PROTO(unsigned int gsi, int level, int irq_source_id),
	TP_ARGS(gsi, level, irq_source_id),

	TP_STRUCT__entry(
		__field(	unsigned int,	gsi		)
		__field(	int,		level		)
		__field(	int,		irq_source_id	)
	),

	TP_fast_assign(
		__entry->gsi		= gsi;
		__entry->level		= level;
		__entry->irq_source_id	= irq_source_id;
	),

	TP_printk("gsi %u level %d source %d",
		  __entry->gsi, __entry->level, __entry->irq_source_id)
);

#define kvm_deliver_mode		\
	{0x0, "Fixed"},			\
	{0x1, "LowPrio"},		\
	{0x2, "SMI"},			\
	{0x3, "Res3"},			\
	{0x4, "NMI"},			\
	{0x5, "INIT"},			\
	{0x6, "SIPI"},			\
	{0x7, "ExtINT"}

TRACE_EVENT(kvm_ioapic_set_irq,
	    TP_PROTO(__u64 e, int pin, bool coalesced),
	    TP_ARGS(e, pin, coalesced),

	TP_STRUCT__entry(
		__field(	__u64,		e		)
		__field(	int,		pin		)
		__field(	bool,		coalesced	)
	),

	TP_fast_assign(
		__entry->e		= e;
		__entry->pin		= pin;
		__entry->coalesced	= coalesced;
	),

	TP_printk("pin %u dst %x vec=%u (%s|%s|%s%s)%s",
		  __entry->pin, (u8)(__entry->e >> 56), (u8)__entry->e,
		  __print_symbolic((__entry->e >> 8 & 0x7), kvm_deliver_mode),
		  (__entry->e & (1<<11)) ? "logical" : "physical",
		  (__entry->e & (1<<15)) ? "level" : "edge",
		  (__entry->e & (1<<16)) ? "|masked" : "",
		  __entry->coalesced ? " (coalesced)" : "")
);

TRACE_EVENT(kvm_msi_set_irq,
	    TP_PROTO(__u64 address, __u64 data),
	    TP_ARGS(address, data),

	TP_STRUCT__entry(
		__field(	__u64,		address		)
		__field(	__u64,		data		)
	),

	TP_fast_assign(
		__entry->address	= address;
		__entry->data		= data;
	),

	TP_printk("dst %u vec %x (%s|%s|%s%s)",
		  (u8)(__entry->address >> 12), (u8)__entry->data,
		  __print_symbolic((__entry->data >> 8 & 0x7), kvm_deliver_mode),
		  (__entry->address & (1<<2)) ? "logical" : "physical",
		  (__entry->data & (1<<15)) ? "level" : "edge",
		  (__entry->address & (1<<3)) ? "|rh" : "")
);

#define kvm_irqchips						\
	{KVM_IRQCHIP_PIC_MASTER,	"PIC master"},		\
	{KVM_IRQCHIP_PIC_SLAVE,		"PIC slave"},		\
	{KVM_IRQCHIP_IOAPIC,		"IOAPIC"}

TRACE_EVENT(kvm_ack_irq,
	TP_PROTO(unsigned int irqchip, unsigned int pin),
	TP_ARGS(irqchip, pin),

	TP_STRUCT__entry(
		__field(	unsigned int,	irqchip		)
		__field(	unsigned int,	pin		)
	),

	TP_fast_assign(
		__entry->irqchip	= irqchip;
		__entry->pin		= pin;
	),

	TP_printk("irqchip %s pin %u",
		  __print_symbolic(__entry->irqchip, kvm_irqchips),
		 __entry->pin)
);



#endif /* defined(__KVM_HAVE_IOAPIC) */

#define KVM_TRACE_MMIO_READ_UNSATISFIED 0
#define KVM_TRACE_MMIO_READ 1
#define KVM_TRACE_MMIO_WRITE 2

#define kvm_trace_symbol_mmio \
	{ KVM_TRACE_MMIO_READ_UNSATISFIED, "unsatisfied-read" }, \
	{ KVM_TRACE_MMIO_READ, "read" }, \
	{ KVM_TRACE_MMIO_WRITE, "write" }

TRACE_EVENT(kvm_mmio,
	TP_PROTO(int type, int len, u64 gpa, u64 val),
	TP_ARGS(type, len, gpa, val),

	TP_STRUCT__entry(
		__field(	u32,	type		)
		__field(	u32,	len		)
		__field(	u64,	gpa		)
		__field(	u64,	val		)
	),

	TP_fast_assign(
		__entry->type		= type;
		__entry->len		= len;
		__entry->gpa		= gpa;
		__entry->val		= val;
	),

	TP_printk("mmio %s len %u gpa 0x%llx val 0x%llx",
		  __print_symbolic(__entry->type, kvm_trace_symbol_mmio),
		  __entry->len, __entry->gpa, __entry->val)
);

#define kvm_fpu_load_symbol	\
	{0, "unload"},		\
	{1, "load"}

TRACE_EVENT(kvm_fpu,
	TP_PROTO(int load),
	TP_ARGS(load),

	TP_STRUCT__entry(
		__field(	u32,	        load		)
	),

	TP_fast_assign(
		__entry->load		= load;
	),

	TP_printk("%s", __print_symbolic(__entry->load, kvm_fpu_load_symbol))
);

TRACE_EVENT(kvm_age_page,
	TP_PROTO(ulong hva, struct kvm_memory_slot *slot, int ref),
	TP_ARGS(hva, slot, ref),

	TP_STRUCT__entry(
		__field(	u64,	hva		)
		__field(	u64,	gfn		)
		__field(	u8,	referenced	)
	),

	TP_fast_assign(
		__entry->hva		= hva;
		__entry->gfn		=
		  slot->base_gfn + ((hva - slot->userspace_addr) >> PAGE_SHIFT);
		__entry->referenced	= ref;
	),

	TP_printk("hva %llx gfn %llx %s",
		  __entry->hva, __entry->gfn,
		  __entry->referenced ? "YOUNG" : "OLD")
);

#ifdef CONFIG_KVM_ASYNC_PF
DECLARE_EVENT_CLASS(kvm_async_get_page_class,

	TP_PROTO(u64 gva, u64 gfn),

	TP_ARGS(gva, gfn),

	TP_STRUCT__entry(
		__field(__u64, gva)
		__field(u64, gfn)
	),

	TP_fast_assign(
		__entry->gva = gva;
		__entry->gfn = gfn;
	),

	TP_printk("gva = %#llx, gfn = %#llx", __entry->gva, __entry->gfn)
);

DEFINE_EVENT(kvm_async_get_page_class, kvm_try_async_get_page,

	TP_PROTO(u64 gva, u64 gfn),

	TP_ARGS(gva, gfn)
);

DEFINE_EVENT(kvm_async_get_page_class, kvm_async_pf_doublefault,

	TP_PROTO(u64 gva, u64 gfn),

	TP_ARGS(gva, gfn)
);

DECLARE_EVENT_CLASS(kvm_async_pf_nopresent_ready,

	TP_PROTO(u64 token, u64 gva),

	TP_ARGS(token, gva),

	TP_STRUCT__entry(
		__field(__u64, token)
		__field(__u64, gva)
	),

	TP_fast_assign(
		__entry->token = token;
		__entry->gva = gva;
	),

	TP_printk("token %#llx gva %#llx", __entry->token, __entry->gva)

);

DEFINE_EVENT(kvm_async_pf_nopresent_ready, kvm_async_pf_not_present,

	TP_PROTO(u64 token, u64 gva),

	TP_ARGS(token, gva)
);

DEFINE_EVENT(kvm_async_pf_nopresent_ready, kvm_async_pf_ready,

	TP_PROTO(u64 token, u64 gva),

	TP_ARGS(token, gva)
);

TRACE_EVENT(
	kvm_async_pf_completed,
	TP_PROTO(unsigned long address, struct page *page, u64 gva),
	TP_ARGS(address, page, gva),

	TP_STRUCT__entry(
		__field(unsigned long, address)
		__field(pfn_t, pfn)
		__field(u64, gva)
		),

	TP_fast_assign(
		__entry->address = address;
		__entry->pfn = page ? page_to_pfn(page) : 0;
		__entry->gva = gva;
		),

	TP_printk("gva %#llx address %#lx pfn %#llx",  __entry->gva,
		  __entry->address, __entry->pfn)
);

#endif

#ifdef CONFIG_KVM_VDI
#include <linux/kvm_task_aware.h>
TRACE_EVENT(kvm_vcpu_switch,
	TP_PROTO(int op, struct kvm_vcpu *vcpu, struct task_struct *task, u64 schedstat),
	TP_ARGS(op, vcpu, task, schedstat),

	TP_STRUCT__entry(
		__field(	int,		op              )
		__field(	int,	        vcpu_id		)
		__field(	u64,		schedstat       )
		__field(	int,	        state           )
	),

	TP_fast_assign(
		__entry->op             = op;
		__entry->vcpu_id        = vcpu->vcpu_id;
		__entry->schedstat      = schedstat;
		__entry->state          = vcpu->state;
	),

	TP_printk("%s v%d %s=%llu state=%d", __entry->op ? "arrive" : "depart", 
		__entry->vcpu_id, __entry->op ? "run_delay" : "exec_time", 
		__entry->schedstat, __entry->state)
);
#define trace_kvm_vcpu_switch_arrive(vcpu, task, schedstat) \
        trace_kvm_vcpu_switch(1, vcpu, task, schedstat)
#define trace_kvm_vcpu_switch_depart(vcpu, task, schedstat) \
        trace_kvm_vcpu_switch(0, vcpu, task, schedstat)

TRACE_EVENT(kvm_gthread_switch,
	TP_PROTO(int op, struct kvm_vcpu *vcpu, unsigned int load_idx, u64 cpu_load, long long exec_time),
	TP_ARGS(op, vcpu, load_idx, cpu_load, exec_time),

	TP_STRUCT__entry(
		__field(	int,		op              )
		__field(	int,	        vcpu_id		)
		__field(	unsigned long,  guest_task_id   )
		__field(	unsigned int,   load_idx        )
		__field(	u64,		cpu_load        )
		__field(	unsigned int,   flags           )
	),

	TP_fast_assign(
		__entry->op             = op;
		__entry->vcpu_id        = vcpu->vcpu_id;
		__entry->guest_task_id  = vcpu->cur_guest_task ? vcpu->cur_guest_task->id : 0;
		__entry->load_idx       = load_idx;
		__entry->cpu_load       = cpu_load;
		__entry->flags          = vcpu->cur_guest_task ? vcpu->cur_guest_task->flags : 0;
	),

	TP_printk("%s gtid=%05lx v%d load_idx=%u cpu_load=%llu flags=%u", 
			__entry->op ? "arrive" : "depart", 
			__entry->guest_task_id, __entry->vcpu_id, __entry->load_idx, __entry->cpu_load, 
			__entry->flags)
);
#define trace_kvm_gthread_switch_arrive(vcpu, load_idx, cpu_load, exec_time) \
        trace_kvm_gthread_switch(1, vcpu, load_idx, cpu_load, exec_time)
#define trace_kvm_gthread_switch_depart(vcpu, load_idx, cpu_load, exec_time) \
        trace_kvm_gthread_switch(0, vcpu, load_idx, cpu_load, exec_time)

TRACE_EVENT(kvm_ui,
	TP_PROTO(struct kvm *kvm, int event_type, int event_info, unsigned int load_idx),
	TP_ARGS(kvm, event_type, event_info, load_idx),

	TP_STRUCT__entry(
		__field(	int,		vm_id           )
		__field(	int,		event_type      )
		__field(	int,	        event_info      )
		__field(	unsigned int,   load_idx        )
	),

	TP_fast_assign(
		__entry->vm_id          = kvm->vm_id;
		__entry->event_type     = event_type;
		__entry->event_info     = event_info;
		__entry->load_idx       = load_idx;
	),

	TP_printk("vm_id=%d type=%d info=%d load_idx=%u", 
                __entry->vm_id, __entry->event_type, __entry->event_info, __entry->load_idx)
);

TRACE_EVENT(kvm_load_check,
	TP_PROTO(int op, int vm_id, int nr_load_entries, unsigned int load_period_msec, u64 start_load_time, u64 end_load_time, int output),
	TP_ARGS(op, vm_id, nr_load_entries, load_period_msec, start_load_time, end_load_time, output),

	TP_STRUCT__entry(
		__field(	int,            op              )
		__field(	int,            vm_id           )
		__field(	int,            nr_load_entries )
		__field(	unsigned int,   load_period_msec)
		__field(	u64,            start_load_time )
		__field(	u64,            end_load_time   )
		__field(	int,            output		)
	),

	TP_fast_assign(
                __entry->op             = op;
                __entry->vm_id          = vm_id;
                __entry->nr_load_entries = nr_load_entries;
                __entry->load_period_msec = load_period_msec;
                __entry->start_load_time= start_load_time;
                __entry->end_load_time  = end_load_time;
                __entry->output = output;
	),

	TP_printk("%s vm%d nr_load_entries=%d load_period_msec=%u start=%llu end=%llu output=%d", __entry->op ? "entry" : "exit",
                        __entry->vm_id, __entry->nr_load_entries, __entry->load_period_msec, 
			__entry->start_load_time, __entry->end_load_time, __entry->output)
);
#define trace_kvm_load_check_entry(vm_id, load_period_msec, nr_load_entries, start_load_time, end_load_time, output)     \
        trace_kvm_load_check(1, vm_id, load_period_msec, nr_load_entries, start_load_time, end_load_time, output)
#define trace_kvm_load_check_exit(vm_id, load_period_msec, nr_load_entries, start_load_time, end_load_time, output)     \
        trace_kvm_load_check(0, vm_id, load_period_msec, nr_load_entries, start_load_time, end_load_time, output)

TRACE_EVENT(kvm_vcpu_stat,
	TP_PROTO(int vm_id, struct kvm_vcpu *vcpu, u64 run_delay),
	TP_ARGS(vm_id, vcpu, run_delay),

	TP_STRUCT__entry(
		__field(	int,            vm_id                   )
		__field(	int,            vcpu_id                 )
		__field(	u64,	        run_delay               )
	),

	TP_fast_assign(
                __entry->vm_id                  = vm_id;
                __entry->vcpu_id                = vcpu->vcpu_id;
		__entry->run_delay              = run_delay;
	),

	TP_printk("vm%d v%d run_delay=%llu", 
                        __entry->vm_id, __entry->vcpu_id, __entry->run_delay)
);

TRACE_EVENT(kvm_vcpu_load,
	TP_PROTO(int vm_id, int vcpu_id, unsigned int cur_load_idx, unsigned int load_idx, u64 cpu_load),
	TP_ARGS(vm_id, vcpu_id, cur_load_idx, load_idx, cpu_load),

	TP_STRUCT__entry(
		__field(	int,            vm_id           )
		__field(	int,            vcpu_id         )
		__field(	unsigned int,   cur_load_idx    )
		__field(	unsigned int,   load_idx        )
		__field(	u64,	        cpu_load        )
	),

	TP_fast_assign(
                __entry->vm_id          = vm_id;
                __entry->vcpu_id        = vcpu_id;
		__entry->cur_load_idx   = cur_load_idx;
		__entry->load_idx       = load_idx;
		__entry->cpu_load       = cpu_load;
	),

	TP_printk("vm%d v%d cur_load_idx=%u load_idx=%u cpu_load=%llu", __entry->vm_id,
                __entry->vcpu_id, __entry->cur_load_idx, __entry->load_idx, __entry->cpu_load)
);

TRACE_EVENT(kvm_gthread_load,
	TP_PROTO(int vm_id, unsigned long guest_task_id, int vcpu_id, unsigned int cur_load_idx, unsigned int load_idx, u64 cpu_load),
	TP_ARGS(vm_id, guest_task_id, vcpu_id, cur_load_idx, load_idx, cpu_load),

	TP_STRUCT__entry(
		__field(	int,            vm_id           )
		__field(	unsigned long,  guest_task_id   )
		__field(	int,            vcpu_id         )
		__field(	unsigned int,   cur_load_idx    )
		__field(	unsigned int,   load_idx        )
		__field(	u64,	        cpu_load        )
	),

	TP_fast_assign(
                __entry->vm_id          = vm_id;
                __entry->guest_task_id  = guest_task_id;
                __entry->vcpu_id        = vcpu_id;
		__entry->cur_load_idx   = cur_load_idx;
		__entry->load_idx       = load_idx;
		__entry->cpu_load       = cpu_load;
	),

	TP_printk("vm%d v%d gtid=%05lx cur_load_idx=%u load_idx=%u cpu_load=%llu", __entry->vm_id, __entry->vcpu_id, 
                        __entry->guest_task_id, __entry->cur_load_idx, __entry->load_idx, __entry->cpu_load)
);

TRACE_EVENT(kvm_load_info,
	TP_PROTO(struct kvm *kvm, unsigned int vm_load, unsigned int eff_vm_load, unsigned int reactive_gtask_load),
	TP_ARGS(kvm, vm_load, eff_vm_load, reactive_gtask_load),

	TP_STRUCT__entry(
		__field(	int,            vm_id                   )
		__field(	int,            interactive_phase       )
		__field(	unsigned int,   pre_monitor_load        )
		__field(	unsigned int,	vm_load                 )
		__field(	unsigned int,	eff_vm_load             )
		__field(	unsigned int,	reactive_gtask_load     )
	),

	TP_fast_assign(
                __entry->vm_id                  = kvm->vm_id;
                __entry->interactive_phase      = kvm->interactive_phase;
                __entry->pre_monitor_load       = kvm->pre_monitor_load;
                __entry->vm_load                = vm_load;
                __entry->eff_vm_load            = eff_vm_load;
                __entry->reactive_gtask_load    = reactive_gtask_load;
	),

	TP_printk("vm%d interactive_phase=%d pre_monitor_load=%u vm_load=%u eff_vm_load=%u reactive_gtask_load=%u",
                        __entry->vm_id, __entry->interactive_phase,
                        __entry->pre_monitor_load, __entry->vm_load, __entry->eff_vm_load, __entry->reactive_gtask_load)
);

TRACE_EVENT(kvm_gtask_stat,
	TP_PROTO(struct kvm *kvm, struct guest_task_struct *gtask, unsigned int bg_load_thresh_pct, unsigned int cpu_load),
	TP_ARGS(kvm, gtask, bg_load_thresh_pct, cpu_load),

	TP_STRUCT__entry(
		__field(	int,            vm_id                   )
		__field(	int,            interactive_phase       )
		__field(	unsigned long,  guest_task_id           )
		__field(	unsigned int,   cpu_load                )
		__field(	unsigned int,   flags                   )
	),

	TP_fast_assign(
                __entry->vm_id                  = kvm->vm_id;
                __entry->interactive_phase      = kvm->interactive_phase;
                __entry->guest_task_id          = gtask->id;
                __entry->cpu_load               = cpu_load;
                __entry->flags                  = gtask->flags;
	),

	TP_printk("vm%d interactive_phase=%d gtid=%05lx cpu_load=%u flags=%u", __entry->vm_id, 
                __entry->interactive_phase, __entry->guest_task_id, __entry->cpu_load, __entry->flags)
);

TRACE_EVENT(kvm_bg_vcpu,
	TP_PROTO(struct kvm_vcpu *vcpu, int bg_nice),
	TP_ARGS(vcpu, bg_nice),

	TP_STRUCT__entry(
		__field(	int,		vcpu_id         )
		__field(	u64,	        bg_exec_time    )
		__field(	u64,	        exec_time       )
		__field(	int,		bg_nice		)
	),

	TP_fast_assign(
		__entry->vcpu_id        = vcpu->vcpu_id;
		__entry->bg_exec_time   = vcpu->bg_exec_time;
		__entry->exec_time      = vcpu->exec_time;
		__entry->bg_nice	= bg_nice;
	),
	TP_printk("v%d bg_exec_time=%llu exec_time=%llu bg_nice=%d", __entry->vcpu_id, 
                __entry->bg_exec_time, __entry->exec_time, __entry->bg_nice)
);

TRACE_EVENT(kvm_system_task,
        TP_PROTO(int vcpu_id, unsigned long system_task_id, unsigned long cur_task_id),

        TP_ARGS(vcpu_id, system_task_id, cur_task_id),

        TP_STRUCT__entry(
                __field( int,   vcpu_id)
                __field( unsigned long, system_task_id)
                __field( unsigned long, cur_task_id)
        ),

        TP_fast_assign(
                __entry->vcpu_id        = vcpu_id;
                __entry->system_task_id = system_task_id;
                __entry->cur_task_id    = cur_task_id;
        ),

        TP_printk("vcpu_id=%d system_task_id=%lu cur_task_id=%lu",
                __entry->vcpu_id, __entry->system_task_id, __entry->cur_task_id)
);
TRACE_EVENT(kvm_ipi,
	TP_PROTO(struct kvm_vcpu *src_vcpu, struct kvm_vcpu *dst_vcpu, u32 vector),
	TP_ARGS(src_vcpu, dst_vcpu, vector),

	TP_STRUCT__entry(
		__field(        int,            src_vcpu_id     )
		__field(        int,            dst_vcpu_id     )
		__field(        u32,            vector          )
	),              

	TP_fast_assign(
		__entry->src_vcpu_id    = src_vcpu->vcpu_id;
		__entry->dst_vcpu_id    = dst_vcpu->vcpu_id;
		__entry->vector         = vector;
	),              

	TP_printk("src_vcpu_id=%d dst_vcpu_id=%d vector=%u\n",
			__entry->src_vcpu_id, __entry->dst_vcpu_id, __entry->vector)
);
TRACE_EVENT(kvm_audio_access,
	TP_PROTO(struct kvm_vcpu *vcpu, int waker_update),
	TP_ARGS(vcpu, waker_update),

	TP_STRUCT__entry(
		__field(        int,            vcpu_id		)
		__field(        int,            port		)
	),              

	TP_fast_assign(
		__entry->vcpu_id    = vcpu->vcpu_id;
		__entry->port	    = vcpu->arch.pio.port
	),              

	TP_printk("vcpu_id=%d port=%x\n", __entry->vcpu_id, __entry->port)
);

TRACE_EVENT(kvm_bg2fg,
	TP_PROTO(struct kvm_vcpu *vcpu, struct guest_task_struct *gtask),
	TP_ARGS(vcpu, gtask),

	TP_STRUCT__entry(
		__field(	int,            vcpu_id			)
		__field(	unsigned long,  gtask_id		)
	),

	TP_fast_assign(
                __entry->vcpu_id		= vcpu->vcpu_id;
                __entry->gtask_id		= gtask->id;
	),

	TP_printk("vcpu_id=%d gtask_id=%05lx", __entry->vcpu_id, __entry->gtask_id) 
);
#endif /* CONFIG_KVM_VDI */
#endif /* _TRACE_KVM_MAIN_H */

/* This part must be outside protection */
#include <trace/define_trace.h>

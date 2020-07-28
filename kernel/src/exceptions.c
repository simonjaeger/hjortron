#include "exceptions.h"
#include "cpu/irq.h"
#include "string.h"
#include "debug.h"

void exception_kernel_halt(string reason)
{
    debug("kernel halt, reason=%s", reason);
    asm("hlt");
}

void exception_handle_divide_by_zero_error()
{
    exception_kernel_halt("divide-by-zero error");
}

void exception_handle_bound_range_exceeded()
{
    exception_kernel_halt("bound range exceeded");
}

void exception_handle_invalid_opcode()
{
    exception_kernel_halt("invalid opcode");
}

void exception_handle_device_not_available()
{
    exception_kernel_halt("device not available");
}

void exception_handle_double_fault()
{
    exception_kernel_halt("double fault");
}

void exception_handle_invalid_tss()
{
    exception_kernel_halt("invalid TSS");
}

void exception_handle_segment_not_present()
{
    exception_kernel_halt("segment not present");
}

void exception_handle_stack_segment_fault()
{
    exception_kernel_halt("stack-segment fault");
}

void exception_handle_general_protection_fault()
{
    exception_kernel_halt("general protection fault");
}

void exception_handle_page_fault()
{
    exception_kernel_halt("page fault");
}

void exception_handle_x87_floating_point_exception()
{
    exception_kernel_halt("x87 floating-point exception");
}

void exception_handle_alignment_check()
{
    exception_kernel_halt("alignment check");
}

void exception_handle_machine_check()
{
    exception_kernel_halt("machine check");
}

void exception_handle_simd_floating_point_exception()
{
    exception_kernel_halt("SIMD floating-point exception");
}

void exception_handle_virtualization_exception()
{
    exception_kernel_halt("virtualization exception");
}

void exception_handle_security_exception()
{
    exception_kernel_halt("security exception");
}

void exceptions_init()
{
    irq_init_handler(0x00, exception_handle_divide_by_zero_error);
    irq_init_handler(0x05, exception_handle_bound_range_exceeded);
    irq_init_handler(0x06, exception_handle_invalid_opcode);
    irq_init_handler(0x07, exception_handle_device_not_available);
    irq_init_handler(0x08, exception_handle_double_fault);
    irq_init_handler(0x0A, exception_handle_invalid_tss);
    irq_init_handler(0x0B, exception_handle_segment_not_present);
    irq_init_handler(0x0C, exception_handle_stack_segment_fault);
    irq_init_handler(0x0D, exception_handle_general_protection_fault);
    irq_init_handler(0x0E, exception_handle_page_fault);
    irq_init_handler(0x10, exception_handle_x87_floating_point_exception);
    irq_init_handler(0x11, exception_handle_alignment_check);
    irq_init_handler(0x12, exception_handle_machine_check);
    irq_init_handler(0x13, exception_handle_simd_floating_point_exception);
    irq_init_handler(0x14, exception_handle_virtualization_exception);
    irq_init_handler(0x1E, exception_handle_security_exception);

    debug("%s", "initialized exceptions");
}
#include "exceptions.h"
#include "cpu/irq.h"
#include "string.h"
#include "debug.h"
#include "display/colors.h"
#include "display/display.h"

void exceptions_kernel_halt(string reason)
{
    printf("%fhalt, reason=%s", (text_attribute){COLOR_RED, COLOR_WHITE}, reason);
    error("halt, reason=%s", reason);
    asm("hlt");
}

void exceptions_handle_divide_by_zero_error()
{
    exceptions_kernel_halt("divide-by-zero error");
}

void exceptions_handle_bound_range_exceeded()
{
    exceptions_kernel_halt("bound range exceeded");
}

void exceptions_handle_invalid_opcode()
{
    exceptions_kernel_halt("invalid opcode");
}

void exceptions_handle_device_not_available()
{
    exceptions_kernel_halt("device not available");
}

void exceptions_handle_double_fault()
{
    exceptions_kernel_halt("double fault");
}

void exceptions_handle_invalid_tss()
{
    exceptions_kernel_halt("invalid TSS");
}

void exceptions_handle_segment_not_present()
{
    exceptions_kernel_halt("segment not present");
}

void exceptions_handle_stack_segment_fault()
{
    exceptions_kernel_halt("stack-segment fault");
}

void exceptions_handle_general_protection_fault()
{
    exceptions_kernel_halt("general protection fault");
}

void exceptions_handle_page_fault()
{
    exceptions_kernel_halt("page fault");
}

void exceptions_handle_x87_floating_point_exception()
{
    exceptions_kernel_halt("x87 floating-point exception");
}

void exceptions_handle_alignment_check()
{
    exceptions_kernel_halt("alignment check");
}

void exceptions_handle_machine_check()
{
    exceptions_kernel_halt("machine check");
}

void exceptions_handle_simd_floating_point_exception()
{
    exceptions_kernel_halt("SIMD floating-point exception");
}

void exceptions_handle_virtualization_exception()
{
    exceptions_kernel_halt("virtualization exception");
}

void exceptions_handle_security_exception()
{
    exceptions_kernel_halt("security exception");
}

void exceptions_init()
{
    irq_init_handler(0x00, exceptions_handle_divide_by_zero_error);
    irq_init_handler(0x05, exceptions_handle_bound_range_exceeded);
    irq_init_handler(0x06, exceptions_handle_invalid_opcode);
    irq_init_handler(0x07, exceptions_handle_device_not_available);
    irq_init_handler(0x08, exceptions_handle_double_fault);
    irq_init_handler(0x0A, exceptions_handle_invalid_tss);
    irq_init_handler(0x0B, exceptions_handle_segment_not_present);
    irq_init_handler(0x0C, exceptions_handle_stack_segment_fault);
    irq_init_handler(0x0D, exceptions_handle_general_protection_fault);
    irq_init_handler(0x0E, exceptions_handle_page_fault);
    irq_init_handler(0x10, exceptions_handle_x87_floating_point_exception);
    irq_init_handler(0x11, exceptions_handle_alignment_check);
    irq_init_handler(0x12, exceptions_handle_machine_check);
    irq_init_handler(0x13, exceptions_handle_simd_floating_point_exception);
    irq_init_handler(0x14, exceptions_handle_virtualization_exception);
    irq_init_handler(0x1E, exceptions_handle_security_exception);

    info("%s", "initialized");
}
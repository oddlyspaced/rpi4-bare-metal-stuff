#include "utils.h"
#include "printf.h"
#include "entry.h"
#include "peripherals/irq.h"
#include "peripherals/aux.h"
#include "mini_uart.h"
#include "timer.h"

const char entry_error_message[16][32] = {
    "SYNC_INVALID_EL1t",
    "IRQ_INVALID_EL1t",
    "F1Q_INVALID_EL1t",
    "ERROR_INVALID_EL1t",

    "SYNC_INVALID_EL1h",
    "IRQ_INVALID_EL1h",
    "FIQ_INVALID_EL1h",
    "ERROR_INVALID_EL1h",

    "SYNC_INVALID_EL0_64",
    "IRQ_INVALID_EL0_64",
    "FIQ_INVALID_EL0_64",
    "ERROR_INVALID_EL0_64",

    "SYNC_INVALID_EL0_32",
    "IRQ_INVALID_EL0_32",
    "FIQ_INVALID_EL0_32",
    "ERROR_INVALID_EL0_32"
};

void show_invalid_entry_message(u32 type, u64 esr, u64 address) {
    printf("ERROR CAUGHT: %s - %d, ESR: %X, Address: %X\n", entry_error_message[type], type, esr, address);
}

void enable_interrupt_controller() {
    REGS_IRQ->irq0_enable_0 = AUX_IRQ | SYS_TIMER_IRQ_1;
}

void handle_irq() {
    u32 irq;
    irq = REGS_IRQ->irq0_pending_0;
    while(irq) {
        if (irq & AUX_IRQ) {
            irq &= ~AUX_IRQ;

            while ((REGS_AUX->mu_iir & 4) == 4) {
                printf("UART RECV: ");
                uart_send(uart_recv());
                printf("\n");
            }
        }

        if (irq & SYS_TIMER_IRQ_1) {
            irq &= ~SYS_TIMER_IRQ_1;
            handle_timer_1();
        }
    }
}
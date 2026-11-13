#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "btstack.h"
#include "pico/btstack_cyw43.h"

static btstack_packet_callback_registration_t hci_cb;
static uint8_t adv[] = {2,0x01,0x06, 8,0x09,'W','y','e','T','e','s','t'};

static void on_hci(uint8_t t, uint16_t ch, uint8_t *p, uint16_t sz) {
    (void)ch; (void)sz;
    if (t != HCI_EVENT_PACKET || hci_event_packet_get_type(p) != BTSTACK_EVENT_STATE) return;
    if (btstack_event_state_get_state(p) != HCI_STATE_WORKING) return;
    bd_addr_t a; gap_local_bd_addr(a);
    printf("BLE up, addr %s\n", bd_addr_to_str(a));
    bd_addr_t null = {0};
    gap_advertisements_set_params(0x30, 0x30, 0, 0, null, 0x07, 0);
    gap_advertisements_set_data(sizeof(adv), adv);
    gap_advertisements_enable(1);
}

int main(void) {
    stdio_init_all();
    if (cyw43_arch_init()) { while (1) tight_loop_contents(); } /* plain Pico: no radio */
    l2cap_init(); sm_init();
    hci_cb.callback = &on_hci;
    hci_add_event_handler(&hci_cb);
    hci_power_control(HCI_POWER_ON);
    while (1) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1); sleep_ms(250);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0); sleep_ms(250);
    }
}

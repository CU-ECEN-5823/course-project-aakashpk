/*
 * letimer.h
 *
 *  Created on: Jan 29, 2019
 *      Author: aakash
 */

#ifndef SRC_LETIMER_H_
#define SRC_LETIMER_H_

#include <stdbool.h>

#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_letimer.h"
#include "em_chip.h"
#include "sleep.h"


#include "gpio.h"
#include "log.h"
#include "common_helper.h"

#define MAX_TICKS 0xFFFF // Set for 16 bit timer, do not change
/**
 * Use these values to hard set the prescaler
 * values , if required to use dynamic prescaler calculation
 * then uncomment the DYNAMIC_PRESCALER define
 */
//#define DYNAMIC_PRESCALER 1

/*
 *ULFRCO prescaler set to 1 to get atleast 1ms
 *ULFRCO resolution when using EM3
 *LFXO prescaler set to 4 to get time
 */
#define PRESCALER_PRESET_LFXO (2U)
#define PRESCALER_PRESET_ULFRCO (1U)

#define ULFRCO_FREQ (1000U)

void LETIMER_square_setup(uint32_t on_time_ms, uint32_t period_ms, SLEEP_EnergyMode_t sleep_mode);
void LETIMER_pulse_setup(uint32_t period_ms, SLEEP_EnergyMode_t sleep_mode);

void LETIMER_register_UFISR(void (*_isr_func)());
void LETIMER_register_COMP1ISR(void (*_isr_func)());

void timerWaitUs(uint32_t us_wait);

void timerSetEventInMs(uint32_t msToWait);

uint32_t timerGetRunTimeMilliseconds(void);

__STATIC_INLINE void LETIMER_start_intr(void) {
	NVIC_EnableIRQ(LETIMER0_IRQn);
}

__STATIC_INLINE void LETIMER_stop_intr(void){
	NVIC_DisableIRQ(LETIMER0_IRQn);;
}

__STATIC_INLINE void disable_delay_intr(void) {
	LETIMER_IntDisable(LETIMER0, LETIMER_IF_COMP1);
}

__STATIC_INLINE void timerWait_ms(uint32_t ms_wait){
	timerWaitUs((ms_wait*1000));
}

#endif /* SRC_LETIMER_H_ */

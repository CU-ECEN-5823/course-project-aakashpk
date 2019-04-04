/*
 * letimer.c
 *
 *  Created on: Jan 29, 2019
 *      Author: Aakash Kumar
 */

#include "letimer.h"

static uint32_t rollover_count = 0, ms_per_rollover = 1, freq_khz = 1;

void LETIMER_clock_enable(bool useLowPower)
{
	/**
	 * If required to go to EM3,
	 * use ULFRCO
	 */
	if(useLowPower)
	{
		CMU_OscillatorEnable(cmuOsc_ULFRCO ,true,true); // Enable ULFRCO
		/* Select ULFRCO for EM3 operation */
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
	}
	/**
	 * else use LFXO for operation upto EM2
	 */
	else
	{
		CMU_OscillatorEnable(cmuOsc_LFXO,true,true); // Enable LFXO
		/* Select LFXO for EM1/EM2 operation */
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	}

	CMU_ClockEnable(cmuClock_LETIMER0, true);//Enable Clock for LETIMER
}


/**
 * If using dynamic prescaler calculation then add a DYNAMIC_PRESCALER define
 * else value set with PRESCALER_PRESET is used as prescaler
 */
uint32_t calc_prescaler(uint32_t max_period_ms)
{
	uint32_t freq = CMU_ClockFreqGet(cmuClock_LETIMER0);

#ifdef DYNAMIC_PRESCALER

	uint32_t prescaler_val = 1;

	while((float)((1/((freq/1000.0)/prescaler_val))*MAX_TICKS) <= max_period_ms)
		prescaler_val = prescaler_val<<1;

	return prescaler_val;

#else

	if(freq > ULFRCO_FREQ) // If greater than ULFRCO frequency then use LFXO prescaler
		return PRESCALER_PRESET_LFXO;
	else
		return PRESCALER_PRESET_ULFRCO; // else use ULFRCO prescaler

#endif

}

uint32_t LETIMER_clock_setup(uint32_t period_ms)
{
	uint32_t prescaler_val = 0;
	prescaler_val = calc_prescaler(period_ms);
	CMU_ClockDivSet(cmuClock_LETIMER0, prescaler_val);
	return prescaler_val;
}

void LETIMER_setup(uint32_t period_ms, SLEEP_EnergyMode_t sleep_mode)
{
	bool em3use = false;

	if(sleep_mode > sleepEM3)
		em3use = true;

	LETIMER_clock_enable(em3use);

	LETIMER_clock_setup(period_ms);

  /* Set configurations for LETIMER 0 */
  const LETIMER_Init_TypeDef letimerInit =
  {
  .enable         = true,                   /* Start counting when init completed. */
  .debugRun       = false,                  /* Counter shall not keep running during debug halt. */
  .comp0Top       = true,                   /* Load COMP0 register into CNT when counter underflows. COMP0 is used as TOP */
  .bufTop         = false,                  /* Don't load COMP1 into COMP0 when REP0 reaches 0. */
  .out0Pol        = 0,                      /* Idle value for output 0. */
  .out1Pol        = 0,                      /* Idle value for output 1. */
  .ufoa0          = letimerUFOANone,        /* No output on output 0 */
  .ufoa1          = letimerUFOANone,       	/* No output on output 1*/
  .repMode        = letimerRepeatFree,      /* Count until stopped */
  };

	/* Initialize LETIMER */
	LETIMER_Init(LETIMER0, &letimerInit);
}


uint32_t calc_COMP0(uint32_t period_ms)
{
	uint32_t freq = CMU_ClockFreqGet(cmuClock_LETIMER0);
	uint32_t comp0_val;

	comp0_val = period_ms* freq / 1000;

	return comp0_val;
}

uint32_t calc_COMP1(uint32_t on_time_ms)
{
	uint32_t freq = CMU_ClockFreqGet(cmuClock_LETIMER0);
	uint32_t comp1_val;

	comp1_val = on_time_ms* freq / 1000;

	return comp1_val;
}

/*
 * To be called after setting comp0 value
 * during timer init.
 */
void set_ms_per_rollover_tick ()
{
	freq_khz = CMU_ClockFreqGet(cmuClock_LETIMER0) / 1000;
	ms_per_rollover = LETIMER_CompareGet(LETIMER0,0) / freq_khz;
}

void LETIMER_square_setup(uint32_t on_time_ms,
		uint32_t period_ms, SLEEP_EnergyMode_t sleep_mode)
{
	LETIMER_setup(period_ms,sleep_mode);

	//used in variable as it used in 2 places below
	uint32_t comp0_val = calc_COMP0(period_ms);

	LETIMER_CompareSet(LETIMER0, 0, comp0_val); // COMP0
	LETIMER_CompareSet(LETIMER0, 1, comp0_val - calc_COMP1(on_time_ms)); //COMP1
	set_ms_per_rollover_tick();

	/* Enable underflow interrupt */
	LETIMER_IntEnable(LETIMER0, LETIMER_IF_COMP0|LETIMER_IF_COMP1);

	/* Enable LETIMER0 interrupt vector in NVIC*/
	NVIC_EnableIRQ(LETIMER0_IRQn);
}


/**
 *
 */
void LETIMER_pulse_setup(uint32_t period_ms, SLEEP_EnergyMode_t sleep_mode)
{
	LETIMER_setup(period_ms,sleep_mode);

	LETIMER_CompareSet(LETIMER0, 0, calc_COMP0(period_ms)); // COMP0
	set_ms_per_rollover_tick();

	/* Enable underflow interrupt */
	LETIMER_IntEnable(LETIMER0, LETIMER_IF_COMP0);

	/* Enable LETIMER0 interrupt vector in NVIC*/
	LETIMER_start_intr();
}


/**
 * This will cannot have resolution less than 1000us when
 * ULFRCO is being used
 */
void timerWaitUs(uint32_t us_wait)
{
	uint32_t entering_tick = LETIMER_CounterGet(LETIMER0);
	uint32_t freq = CMU_ClockFreqGet(cmuClock_LETIMER0);
	uint32_t required_ticks = (us_wait*freq/1000000);
	int32_t compare_val = entering_tick - required_ticks;

	/**
	 * If compare value is negative then timer will roll over
	 * before the required delay, then run the counter to zero
	 * and remaining value once the counter rolls over
	 */
	if(compare_val < 0)
	{
		while(LETIMER_CounterGet(LETIMER0) > (uint32_t)(LETIMER_CompareGet(LETIMER0,0)+compare_val)){;}
	}
	else
	{
		while(LETIMER_CounterGet(LETIMER0) > (uint32_t)compare_val){	;}
	}
}


uint32_t timerGetRunTimeMilliseconds(void)
{
	uint32_t calc_millis;
	calc_millis = (rollover_count * ms_per_rollover)+ (LETIMER_CounterGet(LETIMER0) /freq_khz);
	return calc_millis ;
}

void timerSetEventInMs(uint32_t msToWait)
{
	uint32_t required_ticks = (msToWait * freq_khz);
	uint32_t entering_tick = LETIMER_CounterGet(LETIMER0);
	uint32_t comp1_val = 0;

	if(entering_tick < required_ticks)
	{
		// This actually subtracts
		comp1_val = LETIMER_CompareGet(LETIMER0,0) - (required_ticks - entering_tick);
	}
	else
	{
		comp1_val = entering_tick - required_ticks;
	}
	LETIMER_CompareSet(LETIMER0, 1, comp1_val);

	LETIMER_IntEnable(LETIMER0, LETIMER_IF_COMP1);
}

/*
 *  Interrupt handling functions
 */

static void(* timer_comp1_isr)() = unregistered;
static void(* timer_uf_isr)() = unregistered;


void LETIMER_register_UFISR(void (*_isr_func)())
{
	timer_uf_isr = _isr_func;
	LOG_DEBUG("Registered UF ISR");
}

void LETIMER_register_COMP1ISR(void (*_isr_func)())
{
	timer_comp1_isr = _isr_func;
	LOG_DEBUG("Registered COMP0 ISR");
}

void LETIMER0_IRQHandler(void)
{
	/*  Get timer state */
	uint32_t timer_state = LETIMER_IntGetEnabled(LETIMER0);
	/* Clear LETIMER0 interrupt flags */
	LETIMER_IntClear(LETIMER0, LETIMER_IF_UF | LETIMER_IF_COMP0 | LETIMER_IF_COMP1);

	if(timer_state & LETIMER_IF_COMP0)
	{
		rollover_count ++;
		timer_uf_isr();
	}

	if(timer_state & LETIMER_IF_COMP1)
	{
		timer_comp1_isr();
	}

}



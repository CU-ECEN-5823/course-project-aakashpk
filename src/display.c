/*
 * display.c
 *
 *  Created on: Jan 1, 2019
 *      Author: Dan Walkes
 *
 * @brief Contains functions used to control the LCD display on the Silicon Labs
 * blue gecko development board for ECEN 5823
 */

//#define INCLUDE_LOG_DEBUG 1
#include "graphics.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "glib.h"
#include "gpio.h"
#include "log.h"
#include "display.h"
#include "hardware/kit/common/drivers/display.h"
//#include "scheduler.h" // Add a reference to your module supporting scheduler events for display update
//#include "timer.h" // Add a reference to your module supporting configuration of underflow events here


#if ECEN5823_INCLUDE_DISPLAY_SUPPORT

/**
 * The number of characters per row
 */
#define DISPLAY_ROW_LEN   			 32
/**
 * The number of rows
 */
#define DISPLAY_ROW_NUMBER_OF_ROWS	 8

/**
 * A structure containing information about the data we want to display on a given
 * LCD display
 */
struct display_data {
	/**
	 * tracks the state of the extcomin pin for toggling purposes
	 */
	bool last_extcomin_state_high;
	/**
	 * GLIB_Context required for use with GLIB_ functions
	 */
	GLIB_Context_t context;
	/**
	 * The char content of each row, null terminated
	 */
	char row_data[DISPLAY_ROW_NUMBER_OF_ROWS][DISPLAY_ROW_LEN+1];
};

/**
 * We only support a single global display data structure and a
 * single display with this design
 */
static struct display_data global_display_data;

static struct display_data *displayGetData() {
	return &global_display_data;
}

/**
 * Necessary to prevent a compiler warning on GCC7, despite the fact strnlen is defined in
 * string.h.  I'm not sure why this is necessary
 */
extern size_t strnlen(const char *, size_t);

/**
 * Write the display data in the buffer represented by @param display to the device
 */
static void displayUpdateWriteBuffer(struct display_data *display)
{
	enum display_row row = DISPLAY_ROW_NAME;
	GLIB_Context_t *context = &display->context;
	EMSTATUS result = GLIB_clear(context);
	if( result != GLIB_OK ) {
		LOG_ERROR("GLIB_Clear failed with result %d",(int)result);
	} else {
		/**
		 * See example in graphics.c graphPrintCenter()
		 */
		for( row = DISPLAY_ROW_NAME; row < DISPLAY_ROW_MAX; row ++) {
			uint8_t row_len = strnlen(display->row_data[row],DISPLAY_ROW_LEN);
			uint8_t row_width = row_len * context->font.fontWidth;
			if( row_width > context->pDisplayGeometry->xSize ) {
				LOG_ERROR("Content of display row %d (%s) with length %d font width %d is too wide for display geometry size %d",
						row,&display->row_data[row][0],row_len,context->font.fontWidth,context->pDisplayGeometry->xSize);
			} else {
				uint8_t posX = (context->pDisplayGeometry->xSize - row_width) >> 1;
				uint8_t posY = ((context->font.lineSpacing + context->font.fontHeight) * row)
							   + context->font.lineSpacing;
				result = GLIB_drawString(context, &display->row_data[row][0], row_len, posX, posY, 0);
				if( result != GLIB_OK ) {
					if( result == GLIB_ERROR_NOTHING_TO_DRAW ) {
						/**
						 * This error happens if the content of the draw string did not change
						 */
						LOG_DEBUG("GLIB_drawString returned GLIB_ERROR_NOTHING_TO_DRAW for string %s len %d",&display->row_data[row][0],row_len);
						result = GLIB_OK;
					} else {
						LOG_ERROR("GLIB_drawString failed with result %d for content %s length %d at X=%d Y=%d",
								(int)result,&display->row_data[row][0],row_len,posX,posY);
					}
				}
			}
		}
	}
	result = DMD_updateDisplay();
	if( result != DMD_OK ) {
		LOG_ERROR("DMD_updateDisplay failed with result %d",(int)result);
	}
}

void displayPrintf(enum display_row row, const char *format, ... )
{
	struct display_data *display = displayGetData();
	if( row >= DISPLAY_ROW_MAX ) {
		LOG_WARN("Row %d exceeded max row, ignoring write request",row);
	} else {
		va_list args;
		va_start (args, format);
		int chars_written = vsnprintf(&display->row_data[row][0],DISPLAY_ROW_LEN,format,args);
		va_end(args);
		if( chars_written < 0 ) {
			LOG_WARN("Error encoding format string %s",format);
		}
		if( chars_written >= DISPLAY_ROW_LEN ) {
			LOG_WARN("Exceeded row buffer length for row %d with format string %s",row,format);
			chars_written = DISPLAY_ROW_LEN -1;
		}
		/**
		 * Ensure null terminator
		 */
		display->row_data[row][chars_written] = 0;
		LOG_DEBUG("Updating display row %d with content \"%s\"",row,&display->row_data[row][0]);
	}

	displayUpdateWriteBuffer(display);
}


/**
 * Based on example from graphInit() graphics.c
 */
static void displayGlibInit(GLIB_Context_t *context)
{
	EMSTATUS status;

	/* Initialize the display module. */
	status = DISPLAY_Init();
	if (DISPLAY_EMSTATUS_OK != status) {
		LOG_ERROR("Failed to init display, error was %d",(int)status);
	} else {
		/* Initialize the DMD module for the DISPLAY device driver. */
		status = DMD_init(0);
		if (DISPLAY_EMSTATUS_OK != status) {
			LOG_ERROR("Failed to init DMD, error was %d",(int)status);
		} else {
			status = GLIB_contextInit(context);
			if (DISPLAY_EMSTATUS_OK != status) {
				LOG_ERROR("Failed to init GLIB context, error was %d",(int)status);
			} else {
				context->backgroundColor = White;
				context->foregroundColor = Black;

				/* Use Narrow font */
				status = GLIB_setFont(context, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
				if( GLIB_OK != status ) {
					LOG_ERROR("Failed to set font to GLIB_FontNarrow6x8 in GLIB_setFont, error was %d",(int)status);
				}
			}
		}
	}
}

/**
 * Initialize the display.  Must call
 * @param header represents the content
 */
void displayInit()
{
	struct display_data *display = displayGetData();
	enum display_row row;
#if GPIO_DISPLAY_SUPPORT_IMPLEMENTED
	gpioEnableDisplay();
#else
#warning "gpioEnableDisplay is not implemented, please implement in order to use the display"
#endif
	memset(display,0,sizeof(struct display_data));
	display->last_extcomin_state_high = false;
	displayGlibInit(&display->context);
	for( row = DISPLAY_ROW_NAME; row < DISPLAY_ROW_MAX; row++ ) {
		displayPrintf(row,"%s"," ");
	}
#if SCHEDULER_SUPPORTS_DISPLAY_UPDATE_EVENT
#if TIMER_SUPPORTS_1HZ_TIMER_EVENT
	timerEnable1HzSchedulerEvent(Scheduler_DisplayUpdate);
#else
#warning "Timer does not support scheduling 1Hz event.  Please implement for full display support"
#endif
#else
#warning "Display Update event is not implemented in scheduler.  Please implement for display support"
#endif
}

/**
 * Call this function from your scheduler on periodic intervals in the seconds range
 * to prevent charge buildup within the Liquid Crystal Cells.
 * See details in https://www.silabs.com/documents/public/application-notes/AN0048.pdf
 * @return true when no calls are necessary and the function is ready to sleep
 */
bool displayUpdate()
{
	struct display_data *display = displayGetData();
	display->last_extcomin_state_high = !display->last_extcomin_state_high;
#if GPIO_SET_DISPLAY_EXT_COMIN_IMPLEMENTED
	gpioSetDisplayExtcomin(display->last_extcomin_state_high);
#else
#warning "gpioSetDisplayExtcomin is not implemented.  Please implement for display support"
#endif
	LOG_DEBUG("Display extcomin state is now %s",display->last_extcomin_state_high ? "high" : "low");
	return true;
}

#endif // ECEN5823_INCLUDE_DISPLAY_SUPPORT

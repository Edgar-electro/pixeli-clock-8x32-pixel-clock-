/* MIT License
 *
 * Copyright (c) 2019 - 2022 Andreas Merkle <web@blue-andi.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*******************************************************************************
    DESCRIPTION
*******************************************************************************/
/**
 * @brief  Main entry point
 * @author Andreas Merkle <web@blue-andi.de>
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <Arduino.h>
#include <Logging.h>
#include <LogSinkPrinter.h>
#include "LogSinkWebsocket.h"
#include <StateMachine.hpp>
#include <Board.h>

#include "InitState.h"
#include "TaskMon.h"
#include "MemMon.h"
#include "ResetMon.h"

/******************************************************************************
 * Macros
 *****************************************************************************/

#ifndef CONFIG_ESP_LOG_SEVERITY
#define CONFIG_ESP_LOG_SEVERITY (ESP_LOG_INFO)
#endif /* CONFIG_ESP_LOG_SEVERITY */

#ifndef CONFIG_LOG_SEVERITY
#define CONFIG_LOG_SEVERITY     (Logging::LOG_LEVEL_INFO)
#endif /* CONFIG_LOG_SEVERITY */

/******************************************************************************
 * Types and Classes
 *****************************************************************************/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

/******************************************************************************
 * Variables
 *****************************************************************************/

/** System state machine */
static StateMachine     gSysStateMachine(InitState::getInstance());

/** Serial log sink */
static LogSinkPrinter   gLogSinkSerial("Serial", &Serial);

/** Websocket log sink */
static LogSinkWebsocket gLogSinkWebsocket("Websocket", &WebSocketSrv::getInstance());

/** Serial interface baudrate. */
static const uint32_t   SERIAL_BAUDRATE     = 115200U;

/** Task period in ms of the loop() task. */
static const uint32_t   LOOP_TASK_PERIOD    = 40U;

#if ARDUINO_USB_MODE
#if ARDUINO_USB_CDC_ON_BOOT /* Serial used for USB CDC */

/**
 * Minimize the USB tx timeout (ms) to avoid too long blocking behaviour during
 * writing e.g. log messages to it. If the value is too high, it will influence
 * the display refresh bad.
 */
static const uint32_t   HWCDC_TX_TIMEOUT    = 4U;

#endif  /* ARDUINO_USB_CDC_ON_BOOT */
#endif  /* ARDUINO_USB_MODE */

/******************************************************************************
 * External functions
 *****************************************************************************/

/**
 * Setup the system.
 */
void setup()
{
    /* Start the reset monitor as early as possible to avoid loosing information. */
    ResetMon::getInstance().begin();

    /* Setup serial interface */
    Serial.begin(SERIAL_BAUDRATE);
    
    #if ARDUINO_USB_MODE
    #if ARDUINO_USB_CDC_ON_BOOT
    Serial.setTxTimeoutMs(HWCDC_TX_TIMEOUT);
    #endif  /* ARDUINO_USB_CDC_ON_BOOT */
    #endif  /* ARDUINO_USB_MODE */

    /* Set severity for esp logging system. */
    esp_log_level_set("*", CONFIG_ESP_LOG_SEVERITY);

    /* Register serial log sink and select it per default. */
    if (true == Logging::getInstance().registerSink(&gLogSinkSerial))
    {
        (void)Logging::getInstance().selectSink("Serial");
    }

    /* Register websocket log sink. */
    (void)Logging::getInstance().registerSink(&gLogSinkWebsocket);

    /* Set severity for Pixelix logging system. */
    Logging::getInstance().setLogLevel(CONFIG_LOG_SEVERITY);

    /* The setup routine shall handle only the initialization state.
     * All other states are handled in the loop routine.
     */
    do
    {
        gSysStateMachine.process();
    }
    while(static_cast<AbstractState*>(&InitState::getInstance()) == gSysStateMachine.getState());

    return;
}

/**
 * Main loop, which is called periodically.
 */
void loop()
{
    /* Reset monitor */
    ResetMon::getInstance().process();

    /* Process system state machine */
    gSysStateMachine.process();

    /* Task monitor */
    TaskMon::getInstance().process();

    /* Memory monitor */
    MemMon::getInstance().process();

    /* Schedule other tasks with same or lower priority. */
    delay(LOOP_TASK_PERIOD);

    return;
}

/******************************************************************************
 * Local functions
 *****************************************************************************/

/*
 * Copyright (C) 2020 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MATTHIAS RINGWALD AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 *  main.c
 *
 *  Minimal main application that initializes BTstack, prepares the example and enters BTstack's Run Loop.
 *
 *  If needed, you can create other threads. Please note that BTstack's API is not thread-safe and can only be
 *  called from BTstack timers or in response to its callbacks, e.g. packet handlers.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "btstack_port_esp32.h"
#include "btstack_run_loop.h"
#include "hci_dump.h"

#include "BluetoothManager.hpp"
#include "SpiManager.hpp"

#include "CircularQueue.hpp"

#include <cstdio>
#include <iostream>
#include <stddef.h>

void runSpiManager(void*)
{
    if (!AmpedUp::SpiManager::run(GPIO_NUM_23, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_22, GPIO_NUM_33))
    {
        printf("Error occured in SPI manager\n");
    }
}

TaskHandle_t spiTaskHandle = 0;

extern "C" int app_main(void){


    // Start SPI manager
    xTaskCreate(runSpiManager, "SpiManagerTask", 10240, nullptr, tskIDLE_PRIORITY, &spiTaskHandle);
    // Start Bluetooth manager
    AmpedUp::BluetoothManager::run();

    return 0;
}

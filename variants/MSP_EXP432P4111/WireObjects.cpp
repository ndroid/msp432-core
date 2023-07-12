/*
 * Copyright (c) 2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ti/runtime/wiring/msp432/I2CSlave.h>
#include <ti/runtime/wiring/Wire.h>
void I2CSlaveReadCallback(I2CSlave_Handle i2cSlave)
{
    Wire.readCallback(i2cSlave);
}

void I2CSlaveReadCallback1(I2CSlave_Handle i2cSlave)
{
    Wire1.readCallback(i2cSlave);
}

void I2CSlaveWriteCallback(I2CSlave_Handle i2cSlave, size_t count)
{
    Wire.writeCallback(i2cSlave, count);
}

void I2CSlaveWriteCallback1(I2CSlave_Handle i2cSlave, size_t count)
{
    Wire1.writeCallback(i2cSlave, count);
}

/*
 * Pre-Initialize Wire instances
 */
TwoWire Wire(0, I2CSlaveReadCallback, I2CSlaveWriteCallback);
TwoWire Wire1(1, I2CSlaveReadCallback1, I2CSlaveWriteCallback1);

/**
 * @file IRData.h
 *
 * @brief Structure of IR Data for transmit and receive.
 * 
 * Defines IRData struct used for both transmit and receive operations. See
 * IRreceiver and IRsender.
 *
 *  Copyright (C) 2023  chris miller
 *  miller4@rose-hulman.edu
 *
 *  TinyIRremote is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#ifndef IR_DATA_H
#define IR_DATA_H

#include <stdint.h>

/** \addtogroup IRDataStruct IR Data structure
 * @{
 *  Structure of IR Data for transmit and receive. See IRreceiver::decodeIR()
 *  and IRsender::write().
 */

/** 
 * An enum consisting of all formats supported by IRRemote library.
 * Included for consistancy with existing libraries.
 * TineIRremote library only implements NEC.
 */
typedef enum {
    UNKNOWN = 0,
    PULSE_DISTANCE,
    PULSE_WIDTH,
    DENON,
    DISH,
    JVC,
    LG,
    LG2,
    NEC,
    PANASONIC,
    KASEIKYO,
    KASEIKYO_JVC,
    KASEIKYO_DENON,
    KASEIKYO_SHARP,
    KASEIKYO_MITSUBISHI,
    RC5,
    RC6,
    SAMSUNG,
    SHARP,
    SONY,
    ONKYO,
    APPLE,
    BOSEWAVE,
    LEGO_PF,
    MAGIQUEST,
    WHYNTER,
} decode_type_t;

/**
 * Data structure for the user application, adapted from IRRemote library.
 * Minimal struct to only include necessary data.
 */
typedef struct IRDataStruct {
    //! defines protocol for IR encoding (assumes NEC)
    decode_type_t protocol;
    //! decoded address 
    uint16_t address; 
    //! decoded command
    uint16_t command; 
    //! true if last received message specifies repeat code
    bool isRepeat;
} IRData;

/** @}*/

#endif // IR_DATA_H

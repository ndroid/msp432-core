/*
 *  TinyIRremote.h
 *
 *
 *  Copyright (C) 2022  chris miller
 *  miller4@rose-hulman.edu
 *
 *  Modified 2022 chris miller to support MSP432 on Energia
 *  Modified 2020-2021  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
 *
 *  This file is part of IRMP https://github.com/ukw100/IRMP.
 *  This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 *  TinyIR is free software: you can redistribute it and/or modify
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

#ifndef TINY_IR_REMOTE_H
#define TINY_IR_REMOTE_H

#include <Energia.h>

#include "LongUnion.h"


/*
 * Uncomment the following line to exclude repeats for available IR results. 
 *  Otherwise, repeats are included and isRepeat is set. Must be defined before including header
 */
//#define EXCLUDE_REPEATS

/*
 * Uncommenting following line saves 12 bytes and reduces ISR handling time
 */
//#define DO_NOT_USE_FEEDBACK_LED 

/*
 * Uncomment the following line (or define HANDLE_IR_EVENT in user code prior 
 *  to include) in order to define handler for IR event in user code
 */
//#define HANDLE_IR_EVENT

#if !defined(IR_FEEDBACK_LED_PIN)
#define IR_FEEDBACK_LED_PIN     YELLOW_LED
#endif
#if !defined(LED_BUILTIN)
#define LED_BUILTIN             LED1
#endif

#if defined(__MSP432P401R__) || defined(__MSP432P4111__)
#include "IRFeedbackLED.hpp"
#include "TinyNECRX.hpp"
#include "TinyNECTX.hpp"

#else
#error TinyIRremote receiver only defined for MSP432 platform on Energia
#endif



/** @}*/

#endif // TINY_IR_REMOTE_H

#pragma once

/**
 * @file TinyIRremote.h
 *
 * @brief Main header file for TinyIRremote library.
 * 
 * Main header file for TinyIRremote library, only file necessary to be 
 * included by user sketches. Provides reference to all included IR transmit 
 * and receive resources. See IRreceiver, IRsender, IRData, and 
 * FeedbackLEDFunctions for details on classes and methods.
 *
 *
 *  Copyright (C) 2022  chris miller
 *  miller4@rose-hulman.edu
 *
 *  Modified 2022 chris miller to support MSP432 on Energia
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

#ifndef TINY_IR_REMOTE_H
#define TINY_IR_REMOTE_H

#include <Energia.h>

#include "TinyNECRX.h"
#include "TinyNECTX.h"
#include "IRData.h"
#include "IRFeedbackLED.h"
#include "LongUnion.h"


/* @}*/

#endif // TINY_IR_REMOTE_H

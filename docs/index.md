{#mainpage}
=================

# TI MSP432 Core

This page provides references for the MSP432 Core, supporting TI MSP432P4xx 
Launchpads on Arduino. It includes documentation for included libraries and 
for the Launchpad boards. This board package is intended to match Arduino 10600
specification. For core package reference, refer to 
[Arduino Reference](https://www.arduino.cc/reference/).

See the [MSP432-core](https://github.com/ndroid/msp432-core) repo for additional 
information, including source files and README.

<br><hr>

## TI MSP432 Core Libraries 
References for libraries included with the TI MSP432 Core for Arduino.

<dl>
  <dt>Educational BoosterPack MKII</dt>
    <dd>Provides a set of classes and methods to simplify display to the Educational 
    BoosterPack Mark II LCD screen. See the product page at 
    [https://www.ti.com/tool/BOOSTXL-EDUMKII](https://www.ti.com/tool/BOOSTXL-EDUMKII) 
    or the user guide at [BOOSTXL-EDUMKII User's Guide](https://www.ti.com/lit/pdf/slau599).<br></dd>
    <dd><b>[LCD screen library reference](EduBPMKII_Screen/LCD_screen-Reference_Manual.pdf)</b></dd>
  <dt>Servo Library ([Reference](Servo/index.html))</dt>
    <dd>This library is based on the Arduino Servo library, see 
    [https://www.arduino.cc/reference/en/libraries/servo/](https://www.arduino.cc/reference/en/libraries/servo/).</dd>
  <dt>TinyIRremote Library ([Reference](TinyIRremote/index.html))</dt>
    <dd>MSP432 optimized implementation of IR receive and transmit library for NEC protocol.</dd>
  <dt>TI RSLK Library ([Reference](RSLK/index.html))</dt>
    <dd>Library provides a set of classes and functions to make using the 
    [Texas Instrument's Robotic System Learning Kit](http://www.ti.com/rslk) 
    simple. <br>Go to [http://www.ti.com/rslk](http://www.ti.com/rslk) to see the 
    user guides, view curriculum and find the link to order.</dd>
</dl>


<br><hr>

### Board reference and pin maps

**Arduino Pin Maps for MSP432P4xx**
<table border-style="none" padding-left="30px">
  <tr >
    <td>   </td>
    <td ><b>[Pin Map](pins/msp432p4xx_1pg_pin_map.pdf)</b></td>
    <td>   </td>
    <td>1-page summary pin map</td>
  </tr>
  <tr >
    <td>   </td>
    <td ><b>[Pin Table (Breakout)](pins/msp432p4xx_hdr_pin_table.pdf)</b></td>
    <td>   </td>
    <td>pin map table ordered by breakout board header order</td>
  </tr>
  <tr>
    <td>   </td>
    <td ><b>[Pin Table (BoosterPack)](pins/msp432p4xx_booster_pin_table.pdf)</b></td>
    <td>   </td>
    <td>pin map table ordered by Arduino pin number order</td>
  </tr>
</table>


**MSP432P4xx pin tables** <br> These provide improved summaries of pin attributes from tables provided in 
Section 4 of the data sheet. 
<table border-style="none" padding-left="30px">
  <tr >
    <td>   </td>
    <td ><b>[Pin functions](pins/msp432p4xx_pin_functions.htm)</b></td>
    <td>   </td>
    <td>table of pin assignments organized by peripheral function</td>
  </tr>
  <tr >
    <td>   </td>
    <td ><b>[Pin attributes](pins/msp432p4xx_pin_attributes-compact.htm) 
  [(pdf)](pins/msp432p4xx_pin_attributes-compact.pdf)</b></td>
    <td>   </td>
    <td>pin attributes table ordered by port pin value (compact version)</td>
  </tr>
  <tr>
    <td>   </td>
    <td ><b>[Pin attributes](pins/msp432p4xx_pin_attributes-full.htm) 
  [(pdf)](pins/msp432p4xx_pin_attributes-full.pdf)</b></td>
    <td>   </td>
    <td>complete attributes table including summary descriptions</td>
  </tr>
</table>

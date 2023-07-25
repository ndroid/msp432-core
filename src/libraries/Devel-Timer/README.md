# Devel-Timer

Developer library to assist in understanding timer resource allocation in Energia at runtime.
Prints configuration information from timer peripheral registers, as well as status
data from TI Timer driver and TI TimerMSP432 driver.

Note: getTimerAReloadDiff() is an attempt to roughly estimate period, but does not preceisely capture the period. It only observes the difference between the timer count R and the CCR0 register.

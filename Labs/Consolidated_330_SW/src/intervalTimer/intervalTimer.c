#include "intervalTimer.h"
#include "../switchesAndButtons/buttons.h"
#include <stdio.h>
#include "xparameters.h"
#include "xil_io.h"

#define ZERO_INIT_VALUE_SMALL 0x000 // for initializing first 12 bits of a reg to 0
#define ZERO_INIT_VALUE_LARGE 0x00000000 // for initializing first 32 bits of a reg to 0
#define CASC_INIT_VALUE 0x800 // for setting the CASC bit in the TCSR1 reg to 1

#define TCSR0_OFFSET 0 // offset from the base address of a timer to the TCSR0 reg
#define TLR0_OFFSET 0x04 // offset from the base address of a timer to the TLR0 reg
#define TCR0_OFFSET 0x08 // offset from the base address of a timer to the TCSR0 reg
#define TCSR1_OFFSET 0x10 // offset from the base address of a timer to the TCSR1 reg
#define TLR1_OFFSET 0x14 // offset from the base address of a timer to the TLR1 reg
#define TCR1_OFFSET 0x18 // offset from the base address of a timer to the TCR1 reg

#define ENT_MASK 0x080 // mask value that toggles the ENT bit in the TCSR reg
#define LOAD_MASK 0x020 // mask value that toggles the LOAD bit in the TCSR reg
#define UDT_MASK 0x002 // mask value that toggles the UDT bit in the TCSR reg

// Number of bits to shift a 32 bit value by to be able to concatanate
// it with another 32 bit value
#define READ_SHIFT 32

// helper function that determintes the base address of a timer
// given its specified timer number
uint32_t getBaseAddress(uint32_t timerNumber) {
    // this variable representing the base address of the timer number
    // will be assigned in the switch statement matching timer numbers
    // to base addressess
    uint32_t baseAddr;

    // switch based off of the given timer number
    switch(timerNumber) {

        case INTERVAL_TIMER_TIMER_0:
            // for timer number 0, assign the appropriate base address
            baseAddr = XPAR_AXI_TIMER_0_BASEADDR;
            break;
        case INTERVAL_TIMER_TIMER_1:
            // for timer number 1, assign the appropriate base address
            baseAddr = XPAR_AXI_TIMER_1_BASEADDR;
            break;
        case INTERVAL_TIMER_TIMER_2:
            // for timer number 2, assign the appropriate base address
            baseAddr = XPAR_AXI_TIMER_2_BASEADDR;
            break;
    }

    return baseAddr;
}

// helper function that encapsulates the lower level call to the board read function
// accepts as a param the base address of the timer and the offset to the
// the appropriate register
uint32_t readTimerRegister(uint32_t baseAddr, uint32_t offset) {
    return Xil_In32(baseAddr + offset);
}

// helper function that encapsulates the lower level call to the board write function
// accepts as a param the base address of the timer and the offset to the
// the appropriate register
void writeTimerRegister(uint32_t baseAddr, uint32_t offset, uint32_t writeValue) {
    Xil_Out32(baseAddr + offset, writeValue);
}

// converts the timer reg values to a decimal seconds value
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
    // get the base address of the correct timer
    uint32_t baseAddr = getBaseAddress(timerNumber);

    // read the value of the tcr1 reg, which is the current value, of the correct timer
    uint64_t tcr1Val = readTimerRegister(baseAddr, TCR1_OFFSET);
    // read the value of the tcr0 reg, which is the current value, of the correct timer
    uint32_t tcr0Val = readTimerRegister(baseAddr, TCR0_OFFSET);
    // per the board documentation, reread the value of the tcr1 reg
    uint64_t tcr1Val_temp = readTimerRegister(baseAddr, TCR1_OFFSET);

    // per the board documentation, if the tcr1 2nd read value of the tcr1 reg does not
    // match the first, accept the 2nd read value as te correct value, and reread the
    // tcr0 value
    if(tcr1Val_temp != tcr1Val) {
        tcr1Val = tcr1Val_temp;
        tcr0Val = readTimerRegister(baseAddr, TCR0_OFFSET);
    }

    // concatenate the tcr0 and tcr1 values by shifting the tcr1 value by 32
    // and filling in the first 32 bits of the result with the bits of the tcr0 reg
    uint64_t totalValue = (tcr1Val << READ_SHIFT) | tcr0Val;

    // calculate the elapsed time of the timer by:
    //      1) finding the period of the timer
    //      2) multiplying the period of the timer by the value in the timer
    double elapsedTime = (1.0 / XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ) * totalValue;

    // return the calculated elapsed time
    return elapsedTime;
}

// this function is in charge of following documented protocal to initialize a single timer
intervalTimer_status_t intervalTimer_init(uint32_t timerNumber) {

    // find the base address of the correct timer
    uint32_t baseAddr = getBaseAddress(timerNumber);

    // zero out the first 12 bits (all of the unreserved bits) of the TCSR0 register
    writeTimerRegister(baseAddr, TCSR0_OFFSET, ZERO_INIT_VALUE_SMALL);
    // zero out the first 12 bits (all of the unreserved bits) of the TCSR1 register
    writeTimerRegister(baseAddr, TCSR1_OFFSET, ZERO_INIT_VALUE_SMALL);
    // set the CASC bit to 1 on the TCSR0 register of the correct timer
    writeTimerRegister(baseAddr, TCSR0_OFFSET, CASC_INIT_VALUE);

    // return a successful initialization status
    return INTERVAL_TIMER_STATUS_OK;
}


// makes a previously stopped timer start incrementing
void intervalTimer_start(uint32_t timerNumber) {
    // get the base address of the correct timer
    uint32_t baseAddr = getBaseAddress(timerNumber);
    // read the value currently stored in the TCSR0 reg of the correct timer
    uint32_t tcsr0Val = readTimerRegister(baseAddr, TCSR0_OFFSET);
    // mask the current value of te TCSR0 reg with the ENT mask, which sets
    // the ENT bit to to 1.  This makes the timer start incrementing
    writeTimerRegister(baseAddr, TCSR0_OFFSET, tcsr0Val | ENT_MASK);
}

// makes a previously incrementing timer stop incrementing
void intervalTimer_stop(uint32_t timerNumber) {
    // get the base address of the correct timer
    uint32_t baseAddr = getBaseAddress(timerNumber);
    // read the value currently stored in the TCSR0 reg of the correct timer
    uint32_t tcsr0Val = readTimerRegister(baseAddr, TCSR0_OFFSET);
    // mask the current value of te TCSR0 reg with the inverse ENT mask, which sets
    // the ENT bit to to 0.  This makes the timer stop incrementing
    writeTimerRegister(baseAddr, TCSR0_OFFSET, tcsr0Val & (~ENT_MASK));
}


void intervalTimer_reset(uint32_t timerNumber) {
    // get the base address of the correct timer
    uint32_t baseAddr = getBaseAddress(timerNumber);

    // write a 0 to TLR0 register
    writeTimerRegister(baseAddr, TLR0_OFFSET, ZERO_INIT_VALUE_LARGE);

    // write a 1 into the LOAD0 bit in the TCSR0
    uint32_t tcsr0Val = readTimerRegister(baseAddr, TCSR0_OFFSET);
    writeTimerRegister(baseAddr, TCSR0_OFFSET, tcsr0Val | LOAD_MASK);

    //write a 0 to TLR1 register
    writeTimerRegister(baseAddr, TLR1_OFFSET, ZERO_INIT_VALUE_LARGE);

    // write a 1 into the LOAD1 bit in the TCSR1
    uint32_t tcsr1Val = readTimerRegister(baseAddr, TCSR1_OFFSET);
    writeTimerRegister(baseAddr, TCSR1_OFFSET, tcsr1Val | LOAD_MASK);

    // after all of the correct bits are set, reinitialize the correct timer
    intervalTimer_init(timerNumber);
}

// convenience function in charge of initializing all of the timers
intervalTimer_status_t intervalTimer_initAll() {
    // initialize timer 0 and store the resulting initialization status
    intervalTimer_status_t status0 = intervalTimer_init(INTERVAL_TIMER_TIMER_0);
    // initialize timer 1 and store the resulting initialization status
    intervalTimer_status_t status1 = intervalTimer_init(INTERVAL_TIMER_TIMER_1);
    // initialize timer 2 and store the resulting initialization status
    intervalTimer_status_t status2 = intervalTimer_init(INTERVAL_TIMER_TIMER_2);

    // if all of the return statuses were successfull, return a successfull status,
    // otherwise return a fail status
    return status0 && status1 && status2 ? INTERVAL_TIMER_STATUS_OK  : INTERVAL_TIMER_STATUS_FAIL;
}

// convenience function in charge of resetting all of the timers
void intervalTimer_resetAll() {
    // reset timer 0
    intervalTimer_reset(INTERVAL_TIMER_TIMER_0);
    // reset timer 1
    intervalTimer_reset(INTERVAL_TIMER_TIMER_1);
    // reset timer 2
    intervalTimer_reset(INTERVAL_TIMER_TIMER_2);
}

// tests aspects of the interval timer (init and reset to 0) and gives
// examples of how to call the public api
intervalTimer_status_t intervalTimer_test(uint32_t timerNumber) {
    // get the base address of the correct timer
    uint32_t baseAddr = getBaseAddress(timerNumber);

    // init the correct timer and save its init status
    intervalTimer_status_t initStatus = intervalTimer_init(timerNumber);
    // keep track of correct functioning of the timer in the runStatus variable
    intervalTimer_status_t runStatus = INTERVAL_TIMER_STATUS_OK;

    // make sure the init call sets the reg to 0, and store the result in the
    // isClearAfterInit status code
    intervalTimer_status_t isClearAfterInit;
    isClearAfterInit = !readTimerRegister(baseAddr, TCR0_OFFSET);

    // example on how to start and stop the timer
    intervalTimer_start(timerNumber);
    intervalTimer_stop(timerNumber);

    // example of how to reset a timer
    intervalTimer_reset(timerNumber);

    // make sure the reset function reinitializes the reg to 0
    runStatus = !readTimerRegister(baseAddr, TCR0_OFFSET);

    // return ok if the init status and run status don't have errors, return false otherwise
    if(initStatus && runStatus && isClearAfterInit) {
        return INTERVAL_TIMER_STATUS_OK;
    }
    else {
        INTERVAL_TIMER_STATUS_FAIL;
    }
}

// convenience function that tests all timers
intervalTimer_status_t intervalTimer_testAll() {
    // test timer 0
    intervalTimer_status_t status0 = intervalTimer_test(INTERVAL_TIMER_TIMER_0);
    // test timer 1
    intervalTimer_status_t status1 = intervalTimer_test(INTERVAL_TIMER_TIMER_1);
    // test timer 2
    intervalTimer_status_t status2 = intervalTimer_test(INTERVAL_TIMER_TIMER_2);
    // if all of the test statuses were successfull, return a successfull status,
        // otherwise return a fail status
    return status0 && status1 && status2 ? INTERVAL_TIMER_STATUS_OK  : INTERVAL_TIMER_STATUS_FAIL;
}



#include "taskmanager.h"
#include "util/atomic.h"
#include "serial.h"
#include "config.h"
#include "utils.h"

namespace taskManager {


typedef struct _task
{
    uint32_t interval;
	uint32_t nextCallTime;
	uint8_t error;
	uint8_t currentError;
	uint8_t singleShot:1;
    uint8_t inUse:1;

	void (*func)(void);
} task;


volatile task tasks[TASKMANAGER_MAX_TASKS];
volatile uint16_t time_ovf;
volatile uint8_t tasksInUse;
volatile uint8_t time32bitOvf;


void initialize ()
{
    // Reset all internal states, tasks and RTC
    time_ovf = 0;
    tasksInUse = 0;

    for (uint8_t i = 0; i < TASKMANAGER_MAX_TASKS; i++){
        tasks[i].inUse = 0;
    }

    while(RTC.STATUS & RTC_SYNCBUSY_bm);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        RTC.CNT = 0;
    }

    // Enable 32.768 kHz clock and select it
    CLK.RTCCTRL = CLK_RTCEN_bm | 0b00001100;//CLK_RTCSRC_RCOSC32_bm;

    // Clock is divided by 16 so timer will cout 2048 steps per second
    RTC.CTRL = RTC_PRESCALER_DIV16_gc;

    // Enable only overflow interrupt. Compare interrut isn't needed because we don't have any tasks
    RTC.INTCTRL = RTC_OVFINTLVL_LO_gc | RTC_COMPINTLVL_OFF_gc;

}



inline uint16_t getRTCTime()
{
    uint16_t rtc;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        rtc = RTC.CNT;
    }
    return rtc;
}


uint32_t getTime()
{
    uint16_t rtc = getRTCTime();
    // Add upper values to rtc value to get 32 bit time
    return ((uint32_t)time_ovf << 16) + (uint32_t)rtc;
}

uint32_t getTimeMs()
{
  // Scale RTC value to milliseconds  time_ms = rts * (1000/2048)

  uint32_t r;
  return muldiv(getTime(), 1000, 2048, r);
}

void setNextInterrupt()
{
    // Find task that needs to be called next
    uint32_t nextInterrupt = 0xFFFFFFFF;
    for (uint8_t i = 0; i < TASKMANAGER_MAX_TASKS; i++)
    {
        if (tasks[i].inUse)
        {
            if(tasks[i].nextCallTime < nextInterrupt) nextInterrupt = tasks[i].nextCallTime;
        }
    }

    // Let's make sure we can write to rtc registers befor we disable interrupts
    while(RTC.STATUS & RTC_SYNCBUSY_bm);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {

        // If we are already past call time, we set next interrupt 2 ms from this moment.
        // One milli second could be enough, but to be sure that we don't skip it we set it to 2 milli seconds.
        uint32_t time = getTime();
        if(nextInterrupt <= time)
        {
            nextInterrupt = time + 2;
        }

        // Set next interrupt
        RTC.COMP = (uint16_t)nextInterrupt;
    }

}


int8_t registerTask(void (*function)(void), const uint32_t interval, const bool onlyOnce)
{

    // if maximum number of task is already in use we don't register function
    if(tasksInUse == TASKMANAGER_MAX_TASKS) return -1;

    // find function that isn't in use
    uint8_t i;
    for (i = 0; i < TASKMANAGER_MAX_TASKS; i++)
    {
        if (!tasks[i].inUse)
            break;
    }

    // Scale time to RTC's interval of 2048
    uint32_t roundingError;
    uint32_t intervalScaled = muldiv(interval, 2048, 1000, roundingError );

    // register it
    tasks[i].interval = intervalScaled;
    tasks[i].func = function;
    tasks[i].error = roundingError;
    tasks[i].currentError = 0;
    if (onlyOnce) tasks[i].singleShot = 1;
    else tasks[i].singleShot = 0;
    tasks[i].nextCallTime = getTime() + intervalScaled;
    tasks[i].inUse = 1;

    // Next interrupt must be updated, becouse this task can be next to be called.
    setNextInterrupt();

    // Enable compare interrupt
    RTC.INTCTRL = RTC_OVFINTLVL_LO_gc | RTC_COMPINTLVL_LO_gc;

    tasksInUse++;

    return i;
}

void removeTask(const uint8_t index)
{
    // Not a valid index. Do nothing
    if (index < 0 || index >= TASKMANAGER_MAX_TASKS)
        return;

    if (tasks[index].inUse)
    {
        tasks[index].inUse = 0;
        tasksInUse--;
    }

    // if all tasks are removed disable compare interrupt
    if(!tasksInUse)
        RTC.INTCTRL = RTC_OVFINTLVL_LO_gc | RTC_COMPINTLVL_OFF_gc;
}


ISR(RTC_COMP_vect)
{
    uint32_t time = getTime();

    // Loop all tasks and call those functions.
    // This can take some time depending on ammount of functions and how long they are.
    // So main loop is going to be on hold, but medium and high level interrupts will interrupt those functions.

    for (uint8_t i = 0; i < TASKMANAGER_MAX_TASKS; i++)
    {
        if (tasks[i].inUse)
        {
            if (tasks[i].nextCallTime <= time)
            {
                // If task needed calling only once disable it
                if (tasks[i].singleShot)
                {
                    tasks[i].inUse = 0;
                    removeTask(i);
                }
                else
                {
                    // Calculate when task need to be called next
                    tasks[i].nextCallTime += tasks[i].interval;

                    // Compenste error from scaling milli seconds to RTC time
                    tasks[i].currentError += tasks[i].error;
                    if(tasks[i].currentError > 100)
                    {
                        tasks[i].nextCallTime++;
                        tasks[i].currentError -= 100;
                    }
                }
                // call function
                tasks[i].func();
            }
        }
    }

    setNextInterrupt();
}

uint8_t checkTimeOvf()
{
    if (time32bitOvf)
    {
        time32bitOvf  = 0;
        return 1;
    }
    return 0;
}


ISR(RTC_OVF_vect)
{
    // RTC has 16 bit counter, so OVF vector happens every 32000 ms
    // count overflows so we can get 32 bit couter -> about 24.2 days
    if (time_ovf == 0xFFFF) time32bitOvf = 1;
    time_ovf++;
}


// end namespace
}

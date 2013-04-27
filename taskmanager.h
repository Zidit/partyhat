#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <avr/io.h>
#include <avr/interrupt.h>


#define TASKMANAGER_MAX_TASKS 8 // Max 127


/// THIS LIBRARY WILL OVERFLOW AFTER 24.2 DAYS AND IT WILL PORBABLY FAIL


namespace taskManager {
    /// All times are in milliseconds

    /// This needs to be called before using any functions. Calling it again removes all tasks and resests RTC.
    void initialize ();


    /// Register new task for task manager. Remember not to register more functions than defined in TASKMANAGER_MAX_TASKS
    /// \param function Pointer to function that task calls.
    /// \param interval Defines how often task is called.
    /// \param onlyOnce If onlyOnce isn't 0 task will be called only once. When time set in interval has ellapsed function
    /// is called and then task is removed.
    /// \return Returns task's index for removal if task is registered. If task can't be registered returns -1

    int8_t registerTask(void (*function)(void), const uint32_t interval, const bool onlyOnce);


    /// Removes task by index. If index isn't valid function won't do anything.
    /// \param index Index given by registerTask function.
    void removeTask(const uint8_t index);


    /// Returns time ellapsed from last initialize call. Will overflow about every 24.2 days
    /// \return Returns time ellapsed in milliseconds
    uint32_t getTimeMs();

    /// This function can be used to check if 32 bit timer has overflown. Calling this function resets overflow flag.
    /// \return Returns 1 if timer has overflown else returns 0.
    uint8_t checkTimeOvf();


}





#endif // TASKMANAGER_H

#pragma once

#include <stdbool.h>
#include <signal.h>
/**
* @brief Get number of milliseconds passed since start of the program.
*
* @return Number of milliseconds passed since start of the program.
*/
unsigned long millis();

/**
* @brief Stop current thread for given number of milliseconds.
*
* @param t How many milliseconds to wait.
*/
void waitMs(unsigned long t);

/// <summary>
/// Exit codes for this application. These are used for the
/// application exit code. They must all be between zero and 255,
/// where zero is reserved for successful termination.
/// </summary>
typedef enum {
    ExitCode_Success = 0,
    ExitCode_TermHandler_SigTerm = 1,
    ExitCode_SendMessage_Write = 2,
    ExitCode_ButtonTimer_Consume = 3,
    ExitCode_ButtonTimer_GetValue = 4,
    ExitCode_UartEvent_Read = 5,
    ExitCode_Init_EventLoop = 6,
    ExitCode_Init_UartOpen = 7,
    ExitCode_Init_RegisterIo = 8,
    ExitCode_Init_OpenButton = 9,
    ExitCode_Init_ButtonPollTimer = 10,
    ExitCode_Main_EventLoopFail = 11,
    ExitCode_TimerHandler_Consume = 12,
    ExitCode_Init_Timer = 13

} ExitCode;

// Termination state
static volatile sig_atomic_t exitCode = ExitCode_Success;
#include <stdio.h>
#include <pthread.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Local include for console functionality. */
#include "console.h"

/* Task priorities defined relative to the idle priority. */
#define mainTask1_Priority    ( tskIDLE_PRIORITY + 4 )
#define mainTask2_Priority    ( tskIDLE_PRIORITY + 3 )
#define mainTask3_Priority    ( tskIDLE_PRIORITY + 2 )
#define mainTask4_Priority    ( tskIDLE_PRIORITY + 1 )
#define mainTask5_Priority    ( tskIDLE_PRIORITY )

/* Task frequencies converted from milliseconds to scheduler ticks. */
#define mainTASK_SEND_FREQUENCY_MS         pdMS_TO_TICKS(200UL)
#define mainTIMER_SEND_FREQUENCY_MS        pdMS_TO_TICKS(2000UL)

/* Length of the queue used by tasks. */
#define mainQUEUE_LENGTH                   ( 2 )

/* Values that tasks and timers will send to the queue. */
#define mainVALUE_SENT_FROM_TASK           ( 100UL )
#define mainVALUE_SENT_FROM_TIMER          ( 200UL )

/* Binary search setup: Array size and target value for searching. */
#define ARRAY_SIZE 50
static int array[ARRAY_SIZE] = { /* initialize with values */ };
static int target = /* target value to search for */;

/* Callback function for the software timer expiration. */
static void prvQueueSendTimerCallback(TimerHandle_t xTimerHandle);

/* Queue used by tasks. */
static QueueHandle_t xQueue = NULL;

/* Software timer declaration. */
static TimerHandle_t xTimer = NULL;

/* Scheduler initialization and task creation function. */
void ipsa_sched(void) {
    const TickType_t xTimerPeriod = mainTIMER_SEND_FREQUENCY_MS;

    /* Create a queue to hold uint32_t values. */
    xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(uint32_t));

    if (xQueue != NULL) {
        /* Create tasks with defined priorities. */
        xTaskCreate(vTask1_PrintWorking, "Task1", configMINIMAL_STACK_SIZE, NULL, mainTask1_Priority, NULL);
        xTaskCreate(vTask2_ConvertTemperature, "Task2", configMINIMAL_STACK_SIZE, NULL, mainTask2_Priority, NULL);
        xTaskCreate(vTask3_MultiplyBigNumbers, "Task3", configMINIMAL_STACK_SIZE, NULL, mainTask3_Priority, NULL);
        xTaskCreate(vTask4_BinarySearch, "Task4", configMINIMAL_STACK_SIZE, NULL, mainTask4_Priority, NULL);
        xTaskCreate(vTask5_Aperiodic, "TaskAperi", configMINIMAL_STACK_SIZE, NULL, mainTask5_Priority, NULL);

        /* Create the software timer, but don't start it yet. */
        xTimer = xTimerCreate("Timer", xTimerPeriod, pdTRUE, NULL, prvQueueSendTimerCallback);

        if (xTimer != NULL) {
            /* Start the timer with no delay. */
            xTimerStart(xTimer, 0);
        }

        /* Start the scheduler to run the tasks. */
        vTaskStartScheduler();
    }

    /* Infinite loop in case of insufficient memory to create the idle task or timer. */
    for (;;) {
    }
}

/* Task 1: Periodically prints "Working..." and tracks execution time. */
static void vTask1_PrintWorking(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xDelay = pdMS_TO_TICKS(500);
    TickType_t startTick, endTick;
    uint32_t executionTime, maxExecutionTime = 0;
    uint32_t count = 0;

    for (;;) {
        startTick = xTaskGetTickCount();

        /* Simulated work */
        printf("Working...\n");

        endTick = xTaskGetTickCount();
        executionTime = endTick - startTick;
        if (executionTime > maxExecutionTime) {
            maxExecutionTime = executionTime;
        }

        /* Log the worst case execution time after 500 iterations. */
        count++;
        if (count >= 500) {
            printf("Worst Case Execution Time of Task 1: %u ticks.\n", maxExecutionTime);
            maxExecutionTime = 0; // Reset the maximum execution time.
            count = 0; // Reset the count.
        }

        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}

/* Task 2: Periodically converts temperatures and logs execution time. */
static void vTask2_ConvertTemperature(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xDelay = pdMS_TO_TICKS(1000); // 1 second
    TickType_t startTick, endTick;
    uint32_t executionTime, maxExecutionTime = 0;
    uint32_t count = 0;

    for (;;) {
        startTick = xTaskGetTickCount();

        float fahrenheit = 68.0; // Example temperature
        float celsius = (fahrenheit - 32) * 5 / 9; // Convert F to C
        printf("Temperature: %.2f F = %.2f C\n", fahrenheit, celsius);

        endTick = xTaskGetTickCount();
        executionTime = endTick - startTick;
        if (executionTime > maxExecutionTime) {
            maxExecutionTime = executionTime;
        }

        /* Log the worst case execution time after 500 iterations. */
        count++;
        if (count >= 500) {
            printf("Task 2 WCET: %u ticks.\n", maxExecutionTime);
            maxExecutionTime = 0; // Reset the maximum execution time.
            count = 0; // Reset the count.
        }

        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}

/* Task 3: Performs large number multiplications and tracks execution time. */
static void vTask3_MultiplyBigNumbers(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xDelay = pdMS_TO_TICKS(1500); // 1.5 seconds
    TickType_t startTick, endTick;
    uint32_t executionTime, maxExecutionTime = 0;
    uint32_t count = 0;

    for (;;) {
        startTick = xTaskGetTickCount();

        // Simulated heavy computation
        long int num1 = 123456789;
        long int num2 = 987654321;
        long int result = num1 * num2;  // Multiplication of large numbers
        printf("Multiplication: %ld * %ld = %ld\n", num1, num2, result);

        endTick = xTaskGetTickCount();
        executionTime = endTick - startTick;
        if (executionTime > maxExecutionTime) {
            maxExecutionTime = executionTime;
        }

        // Log the worst case execution time after 500 iterations.
        count++;
        if (count >= 500) {
            printf("Task 3 WCET: %u ticks.\n", maxExecutionTime);
            maxExecutionTime = 0;  // Reset the maximum execution time.
            count = 0;  // Reset the count.
        }

        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}

/* Task 4: Performs a binary search on a static array and logs execution time. */
static void vTask4_BinarySearch(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xDelay = pdMS_TO_TICKS(2000); // 2 seconds
    TickType_t startTick, endTick;
    uint32_t executionTime, maxExecutionTime = 0;
    uint32_t count = 0;

    for (;;) {
        startTick = xTaskGetTickCount();

        // Perform binary search on predefined array
        int result = binarySearch(array, 0, ARRAY_SIZE - 1, target);
        if (result != -1)
            printf("Element found at index: %d\n", result);
        else
            printf("Element not found\n");

        endTick = xTaskGetTickCount();
        executionTime = endTick - startTick;
        if (executionTime > maxExecutionTime) {
            maxExecutionTime = executionTime;
        }

        // Log the worst case execution time after 500 iterations.
        count++;
        if (count >= 500) {
            printf("Task 4 WCET: %u ticks.\n", maxExecutionTime);
            maxExecutionTime = 0;  // Reset the maximum execution time.
            count = 0;  // Reset the count.
        }

        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}

/* Task 5: Aperiodic task that simulates event-driven execution, logs execution time. */
static void vTask5_Aperiodic(void *pvParameters) {
    TickType_t startTick, endTick;

    for (;;) {
        startTick = xTaskGetTickCount();

        // Simulate an aperiodic event handling, might be triggered externally
        vTaskDelay(pdMS_TO_TICKS(100));  // Simulating event handling time

        endTick = xTaskGetTickCount();
        printf("Execution Time of Task 5: %u ticks.\n", (unsigned)(endTick - startTick));

        // Idle until the next event occurs
    }
}

int binarySearch(int arr[], int l, int r, int x) {
    while (l <= r) {
        int m = l + (r - l) / 2; // Calculate the mid point to avoid overflow

        // Check if the middle element is the one we are looking for
        if (arr[m] == x) {
            printf("\n%d found at index %d", x, m);
            return m; // Return the index if the element is found
        }

        // Decide to either go to the left half or the right half
        if (arr[m] < x) {
            l = m + 1; // Ignore the left half
        } else {
            r = m - 1; // Ignore the right half
        }
    }

    // Element is not present in the array
    printf("\n%d not found in the array.", x);
    return -1; // Return -1 to indicate not found
}
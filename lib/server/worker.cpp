#include <freertos/FreeRTOS.h>
#include <ESPAsyncWebServer.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include <worker.h>

TaskHandle_t notificationHandler = NULL;
QueueHandle_t queueHandler;
EventGroupHandle_t eventGroup;
xSemaphoreHandle semaphoreHandler;
xSemaphoreHandle mutexHandler;

TaskHandle_t blinkTaskHandle;

int gevt1 = BIT0;
int gevt2 = BIT1;
int gevt3 = BIT2;

bool groupResult = false;
int sharedVar = 0;

LEDParams ledParams = {
    .led = LED_BUILTIN,
    .time = 3000};

void writeShared(int i)
{
    sharedVar = i;
    Serial.println("do smth by shared semaphore: " + sharedVar);
}

void waitNotificationTask(void *pvParameter)
{
    uint32_t notificationValue;
    Serial.println("notification task started");
    for (;;)
    {
        if (xTaskNotifyWait(0, 0, &notificationValue, portMAX_DELAY))
        {
            NotificationMessage *l = (NotificationMessage *)notificationValue;
            Serial.print("notification message: ");
            Serial.println(l->body);
            break;
        }
        vTaskDelay(2000 / portTICK_RATE_MS);
    }
    Serial.println("notification task deleted");
    vTaskDelete(NULL);
}

void waitGroupEventsTask(void *pvParameter)
{
    Serial.println("group events task started");
    const EventBits_t xBitsToWaitFor = (gevt1 | gevt2 | gevt3);
    EventBits_t xEventGroupValue;
    for (;;)
    {
        xEventGroupValue = xEventGroupWaitBits(eventGroup,
                                               xBitsToWaitFor,
                                               pdTRUE,
                                               pdTRUE,
                                               portMAX_DELAY);
        Serial.println("occured all group:" + String(xEventGroupValue));
        vTaskDelay(2000 / portTICK_RATE_MS);
    }
}

void startQueueTask(void *pvParameter)
{
    QueueMessage msg;
    Serial.println("queue task started");
    for (;;)
    {
        if (xQueueReceive(queueHandler, (void *)&msg, portMAX_DELAY) == pdPASS)
        {
            Serial.print("Queue mesage: ");
            Serial.println(msg.message);
        }
        vTaskDelay(2000 / portTICK_RATE_MS);
    }
}

void waitSemaphoreTask(void *pvParameter)
{
    ;
    Serial.println("semaphore task started");
    for (;;)
    {
        if (xSemaphoreTake(semaphoreHandler, 2000 / portTICK_PERIOD_MS))
        {
            Serial.println("Semaphored: LED is ON");
            xTaskCreate(&blinkTask, "blink_task", configMINIMAL_STACK_SIZE, &ledParams, tskIDLE_PRIORITY, NULL);
        }
    }
}

void blinkTask(void *pvParameter)
{
    blinkTaskHandle = xTaskGetCurrentTaskHandle();
    LEDParams *l = (LEDParams *)pvParameter;
    Serial.println("blink task started");
    for (;;)
    {
        vTaskDelay(l->time / portTICK_RATE_MS);
        digitalWrite(l->led, HIGH);
        vTaskDelay(10 / portTICK_RATE_MS);
        digitalWrite(l->led, LOW);
    }
}

void writeSharedValue(int value)
{
    if (xSemaphoreTake(mutexHandler, 1000 / portTICK_PERIOD_MS))
    {
        writeShared(value);
        xSemaphoreGive(mutexHandler);
    }
}

void deleteTask(TaskHandle_t task)
{
    if (task != NULL)
    {
        Serial.print("before:");
        Serial.println(xPortGetFreeHeapSize());
        vTaskDelete(task);
        task = NULL;
        Serial.print("after:");
        Serial.println(xPortGetFreeHeapSize());
    }
    else
    {
        Serial.print("task is NULL");
    }
}

void setupOnBoardLed()
{
    pinMode(LED_BUILTIN, OUTPUT);
}

void setupWorker()
{
    queueHandler = xQueueCreate(1, sizeof(QueueMessage));
    semaphoreHandler = xSemaphoreCreateBinary();
    mutexHandler = xSemaphoreCreateMutex();
    eventGroup = xEventGroupCreate();
}
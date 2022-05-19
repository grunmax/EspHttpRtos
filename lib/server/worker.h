typedef struct NotificationMessage
{
    char body[20];
    int count;
    bool status;
} NotificationMessage;

typedef struct QueueMessage
{
    char message[20];
    int amount;
    bool status;
} QueueMessage;

typedef struct
{
    int led;
    int time;
} LEDParams;

extern LEDParams ledParams;

extern TaskHandle_t notificationHandler;
extern QueueHandle_t queueHandler;
extern EventGroupHandle_t eventGroup;
extern xSemaphoreHandle semaphoreHandler;
extern xSemaphoreHandle mutexHandler;

extern TaskHandle_t blinkTaskHandle;

extern int gevt1;
extern int gevt2;
extern int gevt3;

void setupWorker();
void setupOnBoardLed();
void waitNotificationTask(void *pvParameter);
void waitGroupEventsTask(void *pvParameter);
void startQueueTask(void *pvParameter);
void waitSemaphoreTask(void *pvParameter);
void writeSharedValue(int value);
void blinkTask(void *pvParameter);
void deleteTask(TaskHandle_t task);
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include "worker.h"
#include "blesrv.h"

AsyncWebServer server(80);

NotificationMessage notificationMessage;
QueueMessage queueMessage;

const char *JSONFMT = "text/json";
const char *HTMLFMT = "text/html";

const int SC_OK = 200;
const int SC_NOT_FOUND = 404;
const int SC_FORBIDDEN = 403;
const int SC_NOT_ACCEPTABLE = 406;

const char index_403[] PROGMEM = "<p>Forbidden</p>";
const char index_404[] PROGMEM = "<p>Not found</p>";

String lastStatus = "undefined";

String getStatusResponse(String status_)
{
    lastStatus = status_;
    return "{\"status\":\"" + status_ + "\"}";
}

void printFreeMem()
{
    Serial.print(":");
    Serial.println(xPortGetFreeHeapSize());
}

void notFoundHandler(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *response = request->beginResponse_P(SC_NOT_FOUND, HTMLFMT, index_404);
    response->addHeader("Server", "ESP Async Web Server");
    request->send(response);
}

void rootHandler(AsyncWebServerRequest *request)
{
    request->send_P(SC_FORBIDDEN, PSTR(HTMLFMT), index_403);
}

void infoHandler(AsyncWebServerRequest *request)
{
    String json;
    DynamicJsonDocument doc(1024);
    doc["memory"]["heap"] = ESP.getFreeHeap();
    doc["raw"] = serialized("[1,2,3]");
    doc["ssid"] = WiFi.SSID();
    serializeJson(doc, json);
    doc.clear();
    request->send(SC_OK, JSONFMT, json);
    printFreeMem();
}

void notificationSendHandler(AsyncWebServerRequest *request)
{
    int count = (request->pathArg(0)).toInt();
    String body = request->pathArg(1);
    notificationMessage.count = count;
    strcpy(notificationMessage.body, body.c_str());
    notificationMessage.status = true;
    xTaskNotify(notificationHandler, (uint32_t)&notificationMessage, eSetValueWithOverwrite);
    request->send(SC_OK, JSONFMT, getStatusResponse("send"));
}

void notificationGetDataHandler(AsyncWebServerRequest *request)
{
    String json;
    DynamicJsonDocument doc(1024);
    doc["notification"]["count"] = notificationMessage.count;
    doc["notification"]["body"] = notificationMessage.body;
    doc["status"] = notificationMessage.status;
    serializeJson(doc, json);
    doc.clear();
    request->send(SC_OK, JSONFMT, json);
    printFreeMem();
}

void bleGetDataHandler(AsyncWebServerRequest *request)
{
    String json;
    DynamicJsonDocument doc(1024);
    doc["ble"]["value"] = bleValue;
    serializeJson(doc, json);
    doc.clear();
    request->send(SC_OK, JSONFMT, json);
    printFreeMem();
}

void queueSendHandler(AsyncWebServerRequest *request)
{
    int amount = (request->pathArg(0)).toInt();
    String message = request->pathArg(1);
    queueMessage.amount = amount;
    strcpy(queueMessage.message, message.c_str());
    queueMessage.status = true;
    xQueueSend(queueHandler, (void *)&queueMessage, portMAX_DELAY);
    request->send(SC_OK, JSONFMT, getStatusResponse("send"));
}

void queueGetDataHandler(AsyncWebServerRequest *request)
{
    String json;
    DynamicJsonDocument doc(1024);
    doc["queue"]["amount"] = queueMessage.amount;
    doc["queue"]["message"] = queueMessage.message;
    doc["status"] = queueMessage.status;
    serializeJson(doc, json);
    doc.clear();
    request->send(SC_OK, JSONFMT, json);
    printFreeMem();
}

void groupSendHandler(AsyncWebServerRequest *request)
{
    int event = (request->pathArg(0)).toInt();
    xEventGroupSetBits(eventGroup, event);
    request->send(SC_OK, JSONFMT, getStatusResponse("send"));
}

void semaphoreGiveHandler(AsyncWebServerRequest *request)
{
    xSemaphoreGive(semaphoreHandler);
    request->send(SC_OK, JSONFMT, getStatusResponse("give"));
}

void mutexShareHandler(AsyncWebServerRequest *request)
{
    int value = (request->pathArg(0)).toInt();
    writeSharedValue(value);
    request->send(SC_OK, JSONFMT, getStatusResponse("share updated"));
}

void killHandler(AsyncWebServerRequest *request)
{
    char t = (request->pathArg(0))[0];
    if (t == 'b')
    {
        deleteTask(blinkTaskHandle);
    }
    request->send(SC_OK, JSONFMT, getStatusResponse("killed"));
}

void restServerRouting()
{
    server.onNotFound(notFoundHandler);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { rootHandler(request); });

    server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request)
              { infoHandler(request); });
    // once?
    server.on("^\\/notification\\/([0-9]+)\\/body\\/([a-zA-Z0-9]+)$", HTTP_GET, [](AsyncWebServerRequest *request)
              { notificationSendHandler(request); });

    server.on("/notification/get", HTTP_GET, [](AsyncWebServerRequest *request)
              { notificationGetDataHandler(request); });

        server.on("/ble/get", HTTP_GET, [](AsyncWebServerRequest *request)
              { bleGetDataHandler(request); });          

    server.on("^\\/queue\\/([0-9]+)\\/message\\/([a-zA-Z0-9]+)$", HTTP_GET, [](AsyncWebServerRequest *request)
              { queueSendHandler(request); });

    server.on("/queue/get", HTTP_GET, [](AsyncWebServerRequest *request)
              { queueGetDataHandler(request); });

    server.on("^\\/group\\/([1|2|4])$", HTTP_GET, [](AsyncWebServerRequest *request)
              { groupSendHandler(request); });

    server.on("/semaphore/give", HTTP_GET, [](AsyncWebServerRequest *request)
              { semaphoreGiveHandler(request); });

    server.on("^\\/mutex\\/([0-9]+)$", HTTP_GET, [](AsyncWebServerRequest *request)
              { mutexShareHandler(request); });

    server.on("^\\/kill\\/([b|n|q|g|s])$", HTTP_GET, [](AsyncWebServerRequest *request)
              { killHandler(request); });
}

void setupServer()
{
    xTaskCreate(&waitNotificationTask, "notification_task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &notificationHandler);
    xTaskCreate(&startQueueTask, "queue_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    xTaskCreate(&waitGroupEventsTask, "events_task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(&waitSemaphoreTask, "semaphore_task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(&blinkTask, "blink_task", configMINIMAL_STACK_SIZE, &ledParams, tskIDLE_PRIORITY, NULL);

    restServerRouting();
    server.begin();
    printFreeMem();
}

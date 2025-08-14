#include "mbed.h"
#include "rtos/ThisThread.h"
#include "NTPClient.h"

#include "certs.h"
#include "iothub.h"
#include "iothub_client_options.h"
#include "iothub_device_client.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/xlogging.h"

#include "iothubtransportmqtt.h"
#include "azure_cloud_credentials.h"

// Include all sensor headers
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_gyro.h"

// Global symbol referenced by the Azure SDK's port for Mbed OS
NetworkInterface *_defaultSystemNetwork;

static bool message_received = false;

static void on_connection_status(IOTHUB_CLIENT_CONNECTION_STATUS result,
                                 IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason,
                                 void* user_context)
{
    if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED) {
        LogInfo("Connected to IoT Hub");
    } else {
        LogError("Connection failed, reason: %s",
                 MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONNECTION_STATUS_REASON, reason));
    }
}

static IOTHUBMESSAGE_DISPOSITION_RESULT on_message_received(IOTHUB_MESSAGE_HANDLE message,
                                                            void* user_context)
{
    LogInfo("Message received from IoT Hub");

    const unsigned char *data_ptr;
    size_t len;
    if (IoTHubMessage_GetByteArray(message, &data_ptr, &len) != IOTHUB_MESSAGE_OK) {
        LogError("Failed to extract message data");
        return IOTHUBMESSAGE_ABANDONED;
    }

    message_received = true;
    LogInfo("Message body: %.*s", len, data_ptr);
    return IOTHUBMESSAGE_ACCEPTED;
}

static void on_message_sent(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    if (result == IOTHUB_CLIENT_CONFIRMATION_OK) {
        LogInfo("Message sent successfully");
    } else {
        LogInfo("Failed to send message, error: %s",
            MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
    }
}

void demo() {
    bool trace_on = MBED_CONF_APP_IOTHUB_CLIENT_TRACE;
    tickcounter_ms_t interval = 100;
    IOTHUB_CLIENT_RESULT res;

    LogInfo("Initializing IoT Hub client");
    IoTHub_Init();

    IOTHUB_DEVICE_CLIENT_HANDLE client_handle = IoTHubDeviceClient_CreateFromConnectionString(
        azure_cloud::credentials::iothub_connection_string,
        MQTT_Protocol
    );
    if (client_handle == nullptr) {
        LogError("Failed to create IoT Hub client handle");
        goto cleanup;
    }

    res = IoTHubDeviceClient_SetOption(client_handle, OPTION_LOG_TRACE, &trace_on);
    res = IoTHubDeviceClient_SetOption(client_handle, OPTION_TRUSTED_CERT, certificates);
    res = IoTHubDeviceClient_SetOption(client_handle, OPTION_DO_WORK_FREQUENCY_IN_MS, &interval);

    IoTHubDeviceClient_SetMessageCallback(client_handle, on_message_received, nullptr);
    IoTHubDeviceClient_SetConnectionStatusCallback(client_handle, on_connection_status, nullptr);

    // --- Initialize all sensors ---
    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();
    BSP_PSENSOR_Init();
    BSP_MAGNETO_Init();
    BSP_ACCELERO_Init();
    BSP_GYRO_Init();

    IOTHUB_MESSAGE_HANDLE message_handle;
    char message[768];

    int16_t magneto[3];
    float gyro[3];
    int16_t accelero[3];

    while (!message_received) {
        float temperature = BSP_TSENSOR_ReadTemp();
        float humidity = BSP_HSENSOR_ReadHumidity();
        float pressure = BSP_PSENSOR_ReadPressure();

        BSP_MAGNETO_GetXYZ(magneto);
        BSP_GYRO_GetXYZ(gyro);
        BSP_ACCELERO_AccGetXYZ(accelero);

        // Create timestamp string
        time_t raw_time = time(NULL);
        struct tm *timeinfo = localtime(&raw_time);
        char time_str[32];
        strftime(time_str, sizeof(time_str), "%m/%d/%Y - %H:%M:%S", timeinfo);

        sprintf(message,
            "{"
            "\"DeviceId\": \"DISCO-id1\","
            "\"timestamp\": \"%s\","
            "\"temperature(C)\": %.2f,"
            "\"humidity(%%)\": %.2f,"
            "\"pressure(hPa)\": %.2f,"
            "\"magnetometer_x(mG)\": %d,"
            "\"magnetometer_y(mG)\": %d,"
            "\"magnetometer_z(mG)\": %d,"
            "\"gyroscope_x(dps)\": %.2f,"
            "\"gyroscope_y(dps)\": %.2f,"
            "\"gyroscope_z(dps)\": %.2f,"
            "\"accelerometer_x(mg)\": %d,"
            "\"accelerometer_y(mg)\": %d,"
            "\"accelerometer_z(mg)\": %d"
            "}",
            time_str,
            temperature, humidity, pressure,
            magneto[0], magneto[1], magneto[2],
            gyro[0], gyro[1], gyro[2],
            accelero[0], accelero[1], accelero[2]
        );

        LogInfo("Sending sensor data: %s", message);

        message_handle = IoTHubMessage_CreateFromString(message);
        if (message_handle == nullptr) {
            LogError("Failed to create message");
            goto cleanup;
        }

        IoTHubMessage_SetContentTypeSystemProperty(message_handle, "application/json");
        IoTHubMessage_SetContentEncodingSystemProperty(message_handle, "utf-8");

        res = IoTHubDeviceClient_SendEventAsync(client_handle, message_handle, on_message_sent, nullptr);
        IoTHubMessage_Destroy(message_handle);

        if (res != IOTHUB_CLIENT_OK) {
            LogError("Failed to send message event, error: %d", res);
            goto cleanup;
        }

        ThisThread::sleep_for(5s);
    }

cleanup:
    IoTHubDeviceClient_Destroy(client_handle);
    IoTHub_Deinit();
}

int main() {
    LogInfo("Connecting to the network");

    _defaultSystemNetwork = NetworkInterface::get_default_instance();
    if (_defaultSystemNetwork == nullptr) {
        LogError("No network interface found");
        return -1;
    }

    int ret = _defaultSystemNetwork->connect();
    if (ret != 0) {
        LogError("Connection error: %d", ret);
        return -1;
    }
    LogInfo("Connection success, MAC: %s", _defaultSystemNetwork->get_mac_address());

    LogInfo("Getting time from the NTP server");
    NTPClient ntp(_defaultSystemNetwork);
    ntp.set_server("time.google.com", 123);
    time_t timestamp = ntp.get_timestamp();
    if (timestamp < 0) {
        LogError("Failed to get the current time, error: %ld", timestamp);
        return -1;
    }
    LogInfo("Time: %s", ctime(&timestamp));
    set_time(timestamp);

    LogInfo("Starting the Demo");
    demo();
    LogInfo("The demo has ended");

    return 0;
}

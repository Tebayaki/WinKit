#pragma comment(lib, "Bthprops.lib")

#include <stdio.h>
#include <wchar.h>
#include <Windows.h>
#include <bluetoothapis.h>

int wmain(int argc, wchar_t *argv[]) {
    if (argc != 2) {
        printf("Need a device name\n");
        return 0;
    }
    BLUETOOTH_DEVICE_SEARCH_PARAMS deviceSearchParams = {sizeof(deviceSearchParams)};
    deviceSearchParams.fReturnRemembered = TRUE;
    deviceSearchParams.cTimeoutMultiplier = 1;
    BLUETOOTH_DEVICE_INFO_STRUCT deviceInfo = {sizeof(deviceInfo)};
    HBLUETOOTH_DEVICE_FIND hDeviceFind = BluetoothFindFirstDevice(&deviceSearchParams, &deviceInfo);
    if (NULL == hDeviceFind) {
        printf("Device not found!\n");
        return 0;
    }

    do {
        if (0 == wcscmp(deviceInfo.szName, argv[1])) {
            printf("Connecting to %ls\n", deviceInfo.szName);
            if (ERROR_SERVICE_DOES_NOT_EXIST == BluetoothSetServiceState(NULL, &deviceInfo, &AVRemoteControlServiceClass_UUID, BLUETOOTH_SERVICE_DISABLE)) {
                printf("Service is not supported!\n");
                goto lRet;
            }
            if (ERROR_SUCCESS == BluetoothSetServiceState(NULL, &deviceInfo, &AVRemoteControlServiceClass_UUID, BLUETOOTH_SERVICE_ENABLE)) {
                printf("Connected\n");
            }
            else {
                printf("Failed to connect!\n");
            }
            goto lRet;
        }
    } while (BluetoothFindNextDevice(hDeviceFind, &deviceInfo));

    printf("Device not found!\n");

lRet:
    BluetoothFindDeviceClose(hDeviceFind);
    return 0;
}

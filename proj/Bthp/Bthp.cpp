#pragma comment(lib, "Bthprops.lib")

#include <stdio.h>
#include <wchar.h>
#include <Windows.h>
#include <bluetoothapis.h>

BOOL FindRememberedDeviceByName(const WCHAR *deviceName, BLUETOOTH_DEVICE_INFO_STRUCT *pDeviceInfo);
BOOL IsDeviceConnected(const BLUETOOTH_ADDRESS address);
BOOL ReEnableAVRemoteControlService(BLUETOOTH_DEVICE_INFO_STRUCT *pDeviceInfo);

int wmain(int argc, wchar_t *argv[]) {
    if (argc < 2) {
        printf("Need a device name\n");
        return 0;
    }

    BLUETOOTH_DEVICE_INFO_STRUCT deviceInfo;
    deviceInfo.dwSize = sizeof(deviceInfo);
    if (!FindRememberedDeviceByName(argv[1], &deviceInfo)) {
        printf("Device not found\n");
        return 1;
    }
    printf("%ls is found, trying to connect...\n", deviceInfo.szName);
    if (deviceInfo.fConnected) {
        printf("The device is already connected\n");
        return 2;
    }
    if (!ReEnableAVRemoteControlService(&deviceInfo)) {
        printf("Unable to reenable AVRemoteControlService\n");
        return 3;
    }
    if (!IsDeviceConnected(deviceInfo.Address)) {
        printf("Unable to connect to the device\n");
        return 4;
    }
	printf("Connected!\n");
    return 0;
}

BOOL FindRememberedDeviceByName(const WCHAR *deviceName, BLUETOOTH_DEVICE_INFO_STRUCT *pDeviceInfo) {
    BOOL res = FALSE;
    BLUETOOTH_DEVICE_SEARCH_PARAMS deviceSearchParams = {};
    deviceSearchParams.dwSize = sizeof(deviceSearchParams);
    deviceSearchParams.fReturnRemembered = TRUE;
    deviceSearchParams.cTimeoutMultiplier = 1;
    HBLUETOOTH_DEVICE_FIND hDeviceFind = BluetoothFindFirstDevice(&deviceSearchParams, pDeviceInfo);
    if (NULL != hDeviceFind) {
		do {
			if (0 == wcscmp(pDeviceInfo->szName, deviceName)) {
                res = TRUE;
                break;
			}
		} while (BluetoothFindNextDevice(hDeviceFind, pDeviceInfo));
		BluetoothFindDeviceClose(hDeviceFind);
    }
    return res;
}

BOOL IsDeviceConnected(const BLUETOOTH_ADDRESS address) {
    BOOL res = FALSE;
    BLUETOOTH_DEVICE_SEARCH_PARAMS deviceSearchParams = {};
    deviceSearchParams.dwSize = sizeof(deviceSearchParams);
    deviceSearchParams.fReturnConnected = TRUE;
    deviceSearchParams.cTimeoutMultiplier = 1;
    BLUETOOTH_DEVICE_INFO_STRUCT deviceInfo;
    deviceInfo.dwSize = sizeof(deviceInfo);
    HBLUETOOTH_DEVICE_FIND hDeviceFind = BluetoothFindFirstDevice(&deviceSearchParams, &deviceInfo);
    if (NULL != hDeviceFind) {
		do {
            if (deviceInfo.Address.ullLong == address.ullLong) {
                res = TRUE;
                break;
            }
		} while (BluetoothFindNextDevice(hDeviceFind, &deviceInfo));
		BluetoothFindDeviceClose(hDeviceFind);
    }
    return res;
}

BOOL ReEnableAVRemoteControlService(BLUETOOTH_DEVICE_INFO_STRUCT *pDeviceInfo) {
    if (ERROR_SERVICE_DOES_NOT_EXIST != BluetoothSetServiceState(NULL, pDeviceInfo, &AVRemoteControlServiceClass_UUID, BLUETOOTH_SERVICE_DISABLE)) {
        if (ERROR_SUCCESS == BluetoothSetServiceState(NULL, pDeviceInfo, &AVRemoteControlServiceClass_UUID, BLUETOOTH_SERVICE_ENABLE)) {
            return TRUE;
		}
	}
    return FALSE;
}

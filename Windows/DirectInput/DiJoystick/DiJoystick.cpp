#include <windows.h>
#include <stdio.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Ole32.lib")
 
#include <stdio.h>
#include <bitset>
#include <chrono>
#include <thread>

#define DEBUG_JOY 0

void clear() {
    COORD topLeft  = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(console, topLeft);
}


LPDIRECTINPUT lpDi=0;

void update(const CHAR tszProductName[], LPDIRECTINPUTDEVICE8 did, DWORD dwButtons, DWORD dwAxes, DWORD dwPOVs) {

	clear();

	if(FAILED(did->Poll())) {
		HRESULT hr = did->Acquire();
		while(hr == DIERR_INPUTLOST) {
			hr = did->Acquire();
		}
	} else {
		// https://learn.microsoft.com/en-us/previous-versions/windows/desktop/ee416628(v=vs.85)
		DIJOYSTATE2 joystate;
		if(SUCCEEDED(did->GetDeviceState(sizeof(DIJOYSTATE2), &joystate))){

			wprintf(L"\nProductName : %S\n\n", tszProductName);

			for(DWORD i = 0; i < dwPOVs; ++i){
				int pov = joystate.rgdwPOV[i]; //hat
				if(pov < 0) {
					pov = -1;
				} else {
					pov /= 100;
				}

				wprintf(L"\that[%d]=%3d \n", i+1, pov);
			}			

			int btn = 0;
			for(DWORD i = 0; i < dwButtons; ++i) {
				if(joystate.rgbButtons[i]) {
					btn |= 1 << i;
				}
			}
			wprintf(L"\tbutton=0x%08x (%S) \n", btn, std::bitset< 16 >( btn ).to_string().c_str());

			wprintf(L"\trglSlider[1]=%d rglSlider[2]=%d \n", joystate.rglSlider[0], joystate.rglSlider[1]);	
			wprintf(L"\trglVSlider[1]=%d rglVSlider[2]=%d \n", joystate.rglVSlider[0], joystate.rglVSlider[1]);
			wprintf(L"\trglASlider[1]=%d rglASlider[2]=%d \n", joystate.rglASlider[0], joystate.rglASlider[1]);
			wprintf(L"\trglFSlider[1]=%d rglFSlider[2]=%d \n", joystate.rglFSlider[0], joystate.rglFSlider[1]);

			wprintf(L"\tlX=%d lY=%d lZ=%d \n"      , joystate.lX, joystate.lY, joystate.lZ);
			wprintf(L"\tlRx=%d lRy=%d lRz=%d \n"   , joystate.lRx, joystate.lRy, joystate.lRz);
			wprintf(L"\tlVX=%d lVY=%d lVZ=%d \n"   , joystate.lVX, joystate.lVY, joystate.lVZ);
			wprintf(L"\tlVRx=%d lVRy=%d lVRz=%d \n", joystate.lVRx, joystate.lVRy, joystate.lVRz);
			wprintf(L"\tlAX=%d lAY=%d lAZ=%d \n"   , joystate.lAX, joystate.lAY, joystate.lAZ);
			wprintf(L"\tlARx=%d lARy=%d lARz=%d \n", joystate.lARx, joystate.lARy, joystate.lARz);
			wprintf(L"\tlFX=%d lFY=%d lFZ=%d \n"   , joystate.lFX, joystate.lFY, joystate.lFZ);
			wprintf(L"\tlFRx=%d lFRy=%d lFRz=%d \n", joystate.lFRx, joystate.lFRy, joystate.lFRz);

		}else{
			wprintf(L"GetDeviceState FAILED\n");
		}		
	}
	
}

static BOOL CALLBACK DIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef) {

	OLECHAR* guidProduct;
	
	wprintf(L"\n\tProductName    : %S\n", lpddi->tszProductName);
	wprintf(L"\tInstanceName   : %S\n", lpddi->tszInstanceName);
	if(SUCCEEDED(StringFromCLSID(lpddi->guidProduct, &guidProduct))){
		wprintf(L"\tguidProduct    : %s\n", guidProduct);
		// ensure memory is freed
		::CoTaskMemFree(guidProduct);
	}
	wprintf(L"\tSize           : %d\n", lpddi->dwSize);
	wprintf(L"\tDevType        : %d (0x%08x)\n", lpddi->dwDevType, lpddi->dwDevType);
	wprintf(L"\tUsage          : %d\n", lpddi->wUsage);
	wprintf(L"\tUsagePage      : %d\n\n", lpddi->wUsagePage);

	LPDIRECTINPUTDEVICE8 did = 0;

	if(SUCCEEDED(lpDi->CreateDevice(lpddi->guidInstance, (LPDIRECTINPUTDEVICE*) &did, 0))) {

		switch (lpddi->dwDevType)
		{
			case ((DIDEVTYPE_HID | DI8DEVTYPE_JOYSTICK) | (DI8DEVTYPEJOYSTICK_LIMITED << 8)):{
				
				wprintf(L"\tJOYSTICK/HID/LIMITED\n");
				if(SUCCEEDED(did->SetDataFormat(&c_dfDIJoystick2))) {

					DIDEVCAPS diDevCaps;
					diDevCaps.dwSize = sizeof(DIDEVCAPS);
					
					if(SUCCEEDED(did->GetCapabilities(&diDevCaps))) {
						wprintf(L"\t\tButtons              : %d\n", diDevCaps.dwButtons);
						wprintf(L"\t\tAxes                 : %d\n", diDevCaps.dwAxes);
						wprintf(L"\t\tPOVs                 : %d\n", diDevCaps.dwPOVs);
						wprintf(L"\t\tDevType              : %d\n", diDevCaps.dwDevType);
						wprintf(L"\t\tSize                 : %d\n", diDevCaps.dwSize);						
						wprintf(L"\t\tFlags                : %d (0x%08x)\n", diDevCaps.dwFlags, diDevCaps.dwFlags);
						wprintf(L"\t\tFFSamplePeriod       : %d\n", diDevCaps.dwFFSamplePeriod);
						wprintf(L"\t\tFFDriverVersion      : %d\n", diDevCaps.dwFFDriverVersion);
						wprintf(L"\t\tFFMinTimeResolution  : %d\n", diDevCaps.dwFFMinTimeResolution);
						wprintf(L"\t\tFirmwareRevision     : %d\n", diDevCaps.dwFirmwareRevision);
						wprintf(L"\t\tHardwareRevision     : %d\n", diDevCaps.dwHardwareRevision);	
						wprintf(L"\n");

					}else{
						wprintf(L"GetCapabilities FAILED\n");
					}
					
					if(DEBUG_JOY){
						for(;;) {
							update(lpddi->tszProductName, did, diDevCaps.dwButtons, diDevCaps.dwAxes, diDevCaps.dwPOVs);
							std::this_thread::sleep_for(std::chrono::milliseconds(2));
						}
					}

				}else{
					wprintf(L"SetDataFormat FAILED\n");
				}
				break;
			}
			default:{
				break;
			}
		}

	}else{
		wprintf(L"CreateDevice FAILED\n");
	}

	return DIENUM_CONTINUE;
};




int main(int, const char*) {
	
	if(SUCCEEDED(DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &lpDi, 0))) {

		if(lpDi != NULL){

			LPVOID pvRef = nullptr;
			if(SUCCEEDED(lpDi->EnumDevices(DI8DEVTYPE_JOYSTICK, DIEnumDevicesCallback, pvRef, DIEDFL_ATTACHEDONLY))){												
				
			}else{
				printf("EnumDevices FAILED\n");
			}

		}else{
			printf("lpDi is NULL\n");
		}

	}else{
		printf("DirectInput8Create FAILED\n");
	}

	return 0;
}

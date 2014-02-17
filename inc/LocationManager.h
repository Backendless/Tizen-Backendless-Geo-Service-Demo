//
// Tizen Native SDK
// Copyright (c) 2012 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.1 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef _LOCATION_MANAGER_H_
#define _LOCATION_MANAGER_H_

#include <FApp.h>
#include <FBase.h>
#include <FSystem.h>
#include <FUi.h>

class LocationManagerApp
	: public Tizen::App::UiApp
	, public Tizen::System::IScreenEventListener
{
public:
	static Tizen::App::UiApp* CreateInstance(void);

public:
	LocationManagerApp(void);
	virtual~LocationManagerApp(void);

	bool OnAppInitializing(Tizen::App::AppRegistry& appRegistry);
	bool OnAppWillTerminate(void); 
	bool OnAppTerminating(Tizen::App::AppRegistry& appRegistry, bool forcedTermination = false);
	void OnForeground(void);
	void OnBackground(void);
	void OnLowMemory(void);
	void OnBatteryLevelChanged(Tizen::System::BatteryLevel batteryLevel);
	void OnScreenOn(void);
	void OnScreenOff(void);

private:
	int __actionId;
};

#endif //  _LOCATION_MANAGER_H_

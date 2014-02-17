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

#include "LocationManager.h"
#include "LocationManagerMainForm.h"
#include "SceneRegister.h"

using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::System;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;

LocationManagerApp::LocationManagerApp(void)
{
}

LocationManagerApp::~LocationManagerApp(void)
{
}

UiApp*
LocationManagerApp::CreateInstance(void)
{
	return new LocationManagerApp();
}

bool
LocationManagerApp::OnAppInitializing(AppRegistry& appRegistry)
{
	Frame* pAppFrame = new Frame();
	pAppFrame->Construct();
	AddFrame(*pAppFrame);

	// Prepare Scene management.
	SceneRegister::RegisterAllScenes();
	SceneManager* pSceneManager = SceneManager::GetInstance();
	pSceneManager->GoForward(ForwardSceneTransition(SCENE_MAIN_FORM));

	return true;
}

bool
LocationManagerApp::OnAppWillTerminate(void)
{
	return true;
}

bool
LocationManagerApp::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
	return true;
}

void
LocationManagerApp::OnForeground(void)
{
}

void
LocationManagerApp::OnBackground(void)
{
}

void
LocationManagerApp::OnLowMemory(void)
{
}

void
LocationManagerApp::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
}

void
LocationManagerApp::OnScreenOn(void)
{
}

void
LocationManagerApp::OnScreenOff(void)
{
}

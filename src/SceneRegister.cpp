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

#include "SceneRegister.h"
#include "FormFactory.h"
#include <FUi.h>

using namespace Tizen::Ui::Scenes;

const wchar_t* SCENE_MAIN_FORM = L"Scene_MainForm";
const wchar_t* SCENE_REGION_MONITOR_FORM = L"Scene_RegionMonitorForm";

SceneRegister::SceneRegister(void)
{
}

SceneRegister::~SceneRegister(void)
{
}

void
SceneRegister::RegisterAllScenes(void)
{
	static const wchar_t* PANEL_BLANK = L"";
	static FormFactory formFactory;

	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);
	pSceneManager->RegisterFormFactory(formFactory);

	pSceneManager->RegisterScene(SCENE_MAIN_FORM, FORM_MAIN, PANEL_BLANK);
}

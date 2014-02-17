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

#include "FormFactory.h"
#include "LocationManagerMainForm.h"

using namespace Tizen::Ui::Scenes;

const wchar_t* FORM_MAIN = L"Form_Main";

FormFactory::FormFactory(void)
{
}

FormFactory::~FormFactory(void)
{
}

Tizen::Ui::Controls::Form*
FormFactory::CreateFormN(const Tizen::Base::String& formId, const Tizen::Ui::Scenes::SceneId& sceneId)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);
	Tizen::Ui::Controls::Form* pNewForm = null;

	if (formId == FORM_MAIN)
	{
		LocationManagerMainForm* pForm = new LocationManagerMainForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}

	return pNewForm;
}

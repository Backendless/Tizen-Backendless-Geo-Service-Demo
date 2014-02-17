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

#ifndef _LOCATION_MANAGER_MAIN_FORM_H_
#define _LOCATION_MANAGER_MAIN_FORM_H_

#include <FApp.h>
#include <FBase.h>
#include <FLocations.h>
#include <FUi.h>

class LocationManagerThread;

class LocationManagerMainForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::IActionEventListener
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::IPropagatedKeyEventListener
 	, public Tizen::Ui::Scenes::ISceneEventListener
 	, public Tizen::Locations::ILocationProviderListener
 	, public Tizen::App::IAppControlResponseListener
{
public:
	LocationManagerMainForm(void);
	virtual ~LocationManagerMainForm(void);
	bool Initialize(void);

	virtual result OnInitializing(void);
	virtual result OnTerminating(void);
	virtual void OnActionPerformed(const Tizen::Ui::Control& source, int actionId);
	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);
	virtual void OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId& previousSceneId,
								   const Tizen::Ui::Scenes::SceneId& currentSceneId, Tizen::Base::Collection::IList* pArgs);
	virtual void OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId& currentSceneId,
									const Tizen::Ui::Scenes::SceneId& nextSceneId);

	virtual bool OnKeyPressed(Control& source, const Tizen::Ui::KeyEventInfo& keyEventInfo);
	virtual bool OnKeyReleased(Control& source, const Tizen::Ui::KeyEventInfo& keyEventInfo);
	virtual bool OnPreviewKeyPressed(Control& source, const Tizen::Ui::KeyEventInfo& keyEventInfo);
	virtual bool OnPreviewKeyReleased(Control& source, const Tizen::Ui::KeyEventInfo& keyEventInfo);
	virtual bool TranslateKeyEventInfo(Control& source, Tizen::Ui::KeyEventInfo& keyEventInfo);

	virtual void OnLocationUpdated(const Tizen::Locations::Location& location);
	virtual void OnLocationUpdateStatusChanged(Tizen::Locations::LocationServiceStatus status);
	virtual void OnRegionEntered(Tizen::Locations::RegionId regionId);
	virtual void OnRegionLeft(Tizen::Locations::RegionId regionId);
	virtual void OnRegionMonitoringStatusChanged(Tizen::Locations::LocationServiceStatus status);
	virtual void OnAccuracyChanged(Tizen::Locations::LocationAccuracy accuracy);
	virtual void OnUserEventReceivedN(RequestId requestId, Tizen::Base::Collection::IList* pArgs);

	virtual void OnAppControlCompleteResponseReceived(const Tizen::App::AppId &appId, const Tizen::Base::String &operationId, Tizen::App::AppCtrlResult appControlResult, const Tizen::Base::Collection::IMap *pExtraData){}


	void DrawLocationInformation(const Tizen::Locations::Location& location);
	void SetLocationUpdateView();
	void SetRegionMonitorView();
	void ShowPopUp();
	void AddMonitoringRegion();
	void UpdateFooterItem(int item);
	void ShowMessageBox(const Tizen::Base::String& title, const Tizen::Base::String& message);
	void LogLocationInfo(const Tizen::Locations::Location location);
	bool CheckLocationSetting(void);

private:
	void LaunchLocationSettings(void);

public:
	static const RequestId LOC_MGR_DRAW_SYNC_LOC_UPDATE = 101;
	static const RequestId LOC_MGR_NOTIFY_ERROR = 102;

private:
	enum ViewType
	{
		VIEW_TYPE_NONE,
		VIEW_TYPE_LOCTION_UPDATE,
		VIEW_TYPE_REGION_MONITORING,
	}__viewType;

	static const int ID_HEADER_TAB_MONITORING_VIEW = 201;
	static const int ID_HEADER_TAB_UPDATE_VIEW = 202;

	static const int ID_FOOTER_BUTTON_UPDATE = 203;
	static const int ID_FOOTER_BUTTON_GET = 204;
	static const int ID_FOOTER_BUTTTON_CANCEL = 205;
	static const int ID_FOOTER_ADD_MONITORING_REGION = 206;
	static const int ID_FOOTER_REMOVE_MONITORING_REGION = 207;

	static const int ID_CONTEXT_UPDATE_INTERVAL = 208;
	static const int ID_CONTEXT_UPDATE_DISTANCE = 209;

	static const int ID_CONTEXT_GET_LAST = 210;
	static const int ID_CONTEXT_GET_CURRENT = 211;

	static const int ID_POPUP_REGION_OK = 212;
	static const int ID_POPUP_REGION_CANCEL = 213;

	Tizen::Ui::Controls::TextBox* __pTextBoxStatus;
	Tizen::Ui::Controls::TextBox* __pTextBoxAccuracy;
	Tizen::Ui::Controls::TextBox* __pTextBoxInfo;

	Tizen::Ui::Controls::Popup* __pRegionAddPopup;
	Tizen::Ui::Controls::EditField* __pRegionCenterLatitude;
	Tizen::Ui::Controls::EditField* __pRegionCenterLongitude;
	Tizen::Ui::Controls::EditField* __pRegionRadius;
	Tizen::Ui::Controls::Button* __pButtonCancel;
	Tizen::Ui::Controls::Button* __pButtonOk;

	Tizen::Ui::Controls::ContextMenu* __pContextMenu;

	Tizen::Locations::LocationProvider* __pLocProvider;
	LocationManagerThread* __pLocationManagerThread;
	Tizen::Locations::RegionId __regionId;
	Tizen::Locations::Coordinates __regionCenter;
	double __regionRadius;
	bool __isUpdateInProgress;
};

#endif	//_LOCATION_MANAGER_MAIN_FORM_H_

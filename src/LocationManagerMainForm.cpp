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

#include <new>
#include <FGraphics.h>
#include <FLocales.h>
#include <FLocCoordinates.h>
#include <FSysSettingInfo.h>
#include <FSysSystemTime.h>
#include "LocationManagerMainForm.h"
#include "LocationManagerThread.h"
#include "Backendless.h"

using namespace std;
using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Base::Utility;
using namespace Tizen::Graphics;
using namespace Tizen::Locales;
using namespace Tizen::Locations;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;

LocationManagerMainForm::LocationManagerMainForm(void)
	: __viewType(VIEW_TYPE_NONE)
	, __pTextBoxStatus(null)
	, __pTextBoxAccuracy(null)
	, __pTextBoxInfo(null)
	, __pRegionAddPopup(null)
	, __pRegionCenterLatitude(null)
	, __pRegionCenterLongitude(null)
	, __pRegionRadius(null)
	, __pButtonCancel(null)
	, __pButtonOk(null)
	, __pContextMenu(null)
	, __pLocProvider(null)
	, __pLocationManagerThread(null)
	, __regionId(-1)
	, __isUpdateInProgress(false)
{
}

LocationManagerMainForm::~LocationManagerMainForm(void)
{
}

bool
LocationManagerMainForm::Initialize(void)
{
	result r = Form::Construct(FORM_STYLE_INDICATOR|FORM_STYLE_FOOTER|FORM_STYLE_HEADER);
	if (IsFailed(r))
	{
		return false;
	}

	return true;
}

result
LocationManagerMainForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	const int TEXT_SIZE = 30;

	Rectangle clientBounds = GetClientAreaBounds();

	Header *pHeader = GetHeader();
	pHeader->SetStyle(HEADER_STYLE_TAB);

	HeaderItem updateView;
	updateView.Construct(ID_HEADER_TAB_UPDATE_VIEW);
	updateView.SetText(L"Location Update");
	pHeader->AddItem(updateView);
	pHeader->AddActionEventListener(*this);

	HeaderItem monitorView;
	monitorView.Construct(ID_HEADER_TAB_MONITORING_VIEW);
	monitorView.SetText(L"Region Monitoring");
	pHeader->AddItem(monitorView);
	pHeader->AddActionEventListener(*this);

	Footer* pFooter = GetFooter();
	pFooter->SetStyle(FOOTER_STYLE_BUTTON_TEXT);
	SetFormBackEventListener(this);

	const int STATUS_TEXT_BOX_HEIGHT = 112;
	const int TEXT_BOX_HEIGHT = (int) (clientBounds.height) - 2*STATUS_TEXT_BOX_HEIGHT;

	int x = 0;
	int y = 0;

	__pTextBoxStatus = new (std::nothrow) TextBox();
	__pTextBoxInfo = new (std::nothrow) TextBox();
	__pTextBoxAccuracy = new (std::nothrow) TextBox();

	if (__pTextBoxStatus != null)
	{
		__pTextBoxStatus->Construct(Rectangle(x, y, clientBounds.width, STATUS_TEXT_BOX_HEIGHT));
		__pTextBoxStatus->SetTextSize(TEXT_SIZE);
		AddControl(*__pTextBoxStatus);
	}

	y = y + STATUS_TEXT_BOX_HEIGHT;

	if (__pTextBoxAccuracy != null)
	{
		__pTextBoxAccuracy->Construct(Rectangle(x, y, clientBounds.width, STATUS_TEXT_BOX_HEIGHT));
		__pTextBoxAccuracy->SetTextSize(TEXT_SIZE);
		AddControl(*__pTextBoxAccuracy);
	}

	y = y + STATUS_TEXT_BOX_HEIGHT;

	if (__pTextBoxInfo != null)
	{
		__pTextBoxInfo->Construct(Rectangle(x, y, clientBounds.width, TEXT_BOX_HEIGHT));
		__pTextBoxInfo->SetTextSize(TEXT_SIZE);
		AddControl(*__pTextBoxInfo);
	}

	SetLocationUpdateView();

	__pContextMenu = new (std::nothrow) ContextMenu();
	__pContextMenu->Construct(Point(0,0), CONTEXT_MENU_STYLE_LIST);
	__pContextMenu->SetShowState(false);
	__pContextMenu->SetFocusable(true);

	// Create the Pop up for region addition

	const int POPUP_HEIGHT = (clientBounds.height/10)*6 ;
	const int POPUP_WIDTH = (clientBounds.width/10)*9;

	__pRegionAddPopup = new (std::nothrow) Popup();
	__pRegionAddPopup->Construct(true, Dimension(POPUP_WIDTH, POPUP_HEIGHT));
	__pRegionAddPopup->SetTitleText(L"Enter Region Details");

	Rectangle popupClientArea = __pRegionAddPopup->GetClientAreaBounds();

	const int wMargin = 10;
	const int hMargin = (int) popupClientArea.height/6 * 0.20;
	const int fieldHeight = 112;
	const int fieldWidth = (int) popupClientArea.width - 2*wMargin;
	const int buttonWidth = (int) popupClientArea.width/2 - 2*wMargin;

	x = popupClientArea.x;
	y = popupClientArea.y;

	y = hMargin;

	__pRegionCenterLatitude = new (std::nothrow) EditField();
	__pRegionCenterLatitude->Construct(Rectangle(wMargin, y, fieldWidth, fieldHeight), EDIT_FIELD_STYLE_NORMAL, INPUT_STYLE_FULLSCREEN, true);
	__pRegionCenterLatitude->SetTitleText(L"Center Latitude");

	y = y + fieldHeight + hMargin;

	__pRegionCenterLongitude = new (std::nothrow) EditField();
	__pRegionCenterLongitude->Construct(Rectangle(wMargin, y, fieldWidth, fieldHeight), EDIT_FIELD_STYLE_NORMAL, INPUT_STYLE_FULLSCREEN, true);
	__pRegionCenterLongitude->SetTitleText(L"Center Longitude");

	y = y + fieldHeight + hMargin;

	__pRegionRadius = new (std::nothrow) EditField();
	__pRegionRadius->Construct(Rectangle(wMargin, y, fieldWidth, fieldHeight), EDIT_FIELD_STYLE_NORMAL, INPUT_STYLE_FULLSCREEN, true);
	__pRegionRadius->SetTitleText(L"Region Radius");

	y = y + fieldHeight + hMargin;

	const int buttonHeight = 74;

	__pButtonCancel = new (std::nothrow) Button();
	__pButtonCancel->Construct(Rectangle(wMargin, y, buttonWidth, buttonHeight), L"Cancel");
	__pButtonCancel->SetActionId(ID_POPUP_REGION_CANCEL);
	__pButtonCancel->AddActionEventListener(*this);

	__pButtonOk = new (std::nothrow) Button();
	__pButtonOk->Construct(Rectangle((int)popupClientArea.width/2 + wMargin, y, buttonWidth, buttonHeight), L"OK");
	__pButtonOk->SetActionId(ID_POPUP_REGION_OK);
	__pButtonOk->AddActionEventListener(*this);

	__pRegionAddPopup->AddControl(*__pButtonOk);
	__pRegionAddPopup->AddControl(*__pButtonCancel);
	__pRegionAddPopup->AddControl(*__pRegionCenterLatitude);
	__pRegionAddPopup->AddControl(*__pRegionCenterLongitude);
	__pRegionAddPopup->AddControl(*__pRegionRadius);

	__pRegionAddPopup->SetShowState(false);
	__pRegionAddPopup->SetPropagatedKeyEventListener(this);

	__pLocationManagerThread =  new (std::nothrow) LocationManagerThread();
	r = __pLocationManagerThread->Construct(*this);
	if (IsFailed(r))
	{
		AppLog("Thread Construct failed.");
		return r;
	}

	LocationCriteria criteria;
	criteria.SetAccuracy(LOC_ACCURACY_HUNDRED_METERS);

	__pLocProvider = new (std::nothrow) LocationProvider();
	r = __pLocProvider->Construct(criteria, *this);
	if (r != E_SUCCESS)
	{
		AppLog("Location Provider construct failed.");
	}

	return r;
}

result
LocationManagerMainForm::OnTerminating(void)
{
	delete __pContextMenu;
	delete __pRegionAddPopup;
	delete __pLocProvider;
	__pLocationManagerThread->Join();
	delete __pLocationManagerThread;

	return E_SUCCESS;
}

void
LocationManagerMainForm::OnActionPerformed(const Tizen::Ui::Control& source, int actionId)
{
	switch(actionId)
		{
		case ID_HEADER_TAB_MONITORING_VIEW:
		{
			AppLog("ID_HEADER_TAB_MONITORING_VIEW");
			SetRegionMonitorView();
		}
		break;

		case ID_HEADER_TAB_UPDATE_VIEW:
		{
			AppLog("ID_HEADER_TAB_UPDATE_VIEW");
			SetLocationUpdateView();
		}
		break;

		case ID_FOOTER_BUTTON_UPDATE:
		{
			AppLog("ID_FOOTER_BUTTON_UPDATE");
			Footer* pFooter = GetFooter();
			Rectangle footerBounds = pFooter->GetBounds();

			Point anchorPos;
			anchorPos.x = footerBounds.x + (int)footerBounds.width/4;
			anchorPos.y = footerBounds.y;

			__pContextMenu->RemoveAllItems();

			__pContextMenu->SetAnchorPosition(anchorPos);
			__pContextMenu->AddItem(L"Update By Interval", ID_CONTEXT_UPDATE_INTERVAL);
			__pContextMenu->AddItem(L"Update By Distance", ID_CONTEXT_UPDATE_DISTANCE);
			__pContextMenu->AddActionEventListener(*this);
			__pContextMenu->SetShowState(true);
			__pContextMenu->Show();
		}
		break;

		case ID_FOOTER_BUTTON_GET:
		{
			AppLog("ID_FOOTER_BUTTON_GET");
			Footer* pFooter = GetFooter();
			Rectangle footerBounds = pFooter->GetBounds();

			Point anchorPos;
			anchorPos.x = footerBounds.x + (int)footerBounds.width* 3/4;
			anchorPos.y = footerBounds.y;

			__pContextMenu->RemoveAllItems();

			__pContextMenu->SetAnchorPosition(anchorPos);
			__pContextMenu->AddItem(L"Get Last Location", ID_CONTEXT_GET_LAST);
			__pContextMenu->AddItem(L"Get Current Location", ID_CONTEXT_GET_CURRENT);
			__pContextMenu->AddActionEventListener(*this);
			__pContextMenu->SetShowState(true);
			__pContextMenu->Show();

		}
		break;

		case ID_FOOTER_BUTTTON_CANCEL:
		{
			AppLog("ID_FOOTER_BUTTTON_CANCEL");
			result r = __pLocProvider->StopLocationUpdates();
			if (r == E_SUCCESS)
			{
				__pTextBoxInfo->Clear();
				__pTextBoxInfo->AppendText(L"- No Information -");
				__pTextBoxInfo->Draw();

				__pTextBoxAccuracy->Clear();
				__pTextBoxAccuracy->AppendText(L"Location Accuracy: Invalid");
				__pTextBoxAccuracy->Draw();

				__pTextBoxStatus->Clear();
				__pTextBoxStatus->AppendText(L"Update Status: Idle");
				__pTextBoxStatus->Draw();
			}
			else
			{
				ShowMessageBox(L"Invalid operation", L"Location updates cannot be stopped.");
			}

			UpdateFooterItem(ID_CONTEXT_UPDATE_INTERVAL);
			__isUpdateInProgress = false;
		}
		break;

		case ID_FOOTER_ADD_MONITORING_REGION:
		{
			AppLog("ID_FOOTER_ADD_MONITORING_REGION");
			ShowPopUp();
		}
		break;

		case ID_FOOTER_REMOVE_MONITORING_REGION:
		{
			AppLog("ID_FOOTER_REMOVE_MONITORING_REGION");

			result r = __pLocProvider->RemoveMonitoringRegion(__regionId);
			if (r == E_INVALID_ARG)
			{
				ShowMessageBox(L"Invalid Operation", L"No region added for monitoring.");
			}
			else
			{
				__pTextBoxInfo->AppendText(L"Monitoring region with region Id " + String(Integer::ToString(__regionId)) + L" removed\n");
				__pTextBoxInfo->Draw();
				__regionId = -1;

				__pTextBoxAccuracy->Clear();
				__pTextBoxAccuracy->AppendText(L"Location Accuracy: Invalid");
				__pTextBoxAccuracy->Draw();

				__pTextBoxStatus->Clear();
				__pTextBoxStatus->AppendText(L"Monitoring Status: Idle");
				__pTextBoxStatus->Draw();
			}
		}
		break;

		case ID_CONTEXT_UPDATE_INTERVAL:
		{
			AppLog("ID_CONTEXT_UPDATE_INTERVAL");

			result res = __pLocProvider->StartLocationUpdatesByInterval(3);

			if (res == E_USER_NOT_CONSENTED)
			{
				bool isSettingEnabled = CheckLocationSetting();
				if (!isSettingEnabled)
				{
					LaunchLocationSettings();
				}
				else
				{
					ShowMessageBox(L"Privacy protection", L"Please allow the application to use your location. You can change settings at Settings->Privacy.");
				}
			}
			else
			{
				UpdateFooterItem(ID_FOOTER_BUTTTON_CANCEL);
				__isUpdateInProgress = true;
			}
		}
		break;

		case ID_CONTEXT_UPDATE_DISTANCE:
		{
			AppLog("ID_CONTEXT_UPDATE_DISTANCE");

			result res = __pLocProvider->StartLocationUpdatesByDistance(100);

			if (res == E_USER_NOT_CONSENTED)
			{
				bool isSettingEnabled = CheckLocationSetting();
				if (!isSettingEnabled)
				{
					LaunchLocationSettings();
				}
				else
				{
					ShowMessageBox(L"Privacy protection", L"Please allow the application to use your location. You can change settings at Settings->Privacy.");
				}
			}
			else
			{
				UpdateFooterItem(ID_FOOTER_BUTTTON_CANCEL);
				__isUpdateInProgress = true;
			}
		}
		break;

		case ID_CONTEXT_GET_LAST:
		{
			AppLog("ID_CONTEXT_GET_LAST");

			Location location = LocationProvider::GetLastKnownLocation();
			result lastResult = GetLastResult();

			if (lastResult == E_USER_NOT_CONSENTED)
			{
				bool isSettingEnabled = CheckLocationSetting();
				if (!isSettingEnabled)
				{
					LaunchLocationSettings();
				}
				else
				{
					ShowMessageBox(L"Privacy protection", L"Please allow the application to use your location. You can change settings at Settings->Privacy.");
				}

				return;
			}
			if (location.IsValid())
			{
				DrawLocationInformation(location);
				AppLog("The last location is obtained successfully.");
				LogLocationInfo(location);
			}
			else
			{
				__pTextBoxInfo->Clear();
				__pTextBoxInfo->AppendText(L"Failed to fetch the last location");
				__pTextBoxInfo->Draw();
			}
		}
		break;

		case ID_CONTEXT_GET_CURRENT:
		{
			AppLog("ID_CONTEXT_GET_CURRENT");

			result r = __pLocationManagerThread->Start();

			if (IsFailed(r))
			{
				__pTextBoxInfo->Clear();
				__pTextBoxInfo->AppendText(L"Failed to fetch the current location");
				__pTextBoxInfo->Draw();
			}
		}
		break;

		case ID_POPUP_REGION_OK:
		{
			AppLog("ID_POPUP_REGION_OK");
			__pRegionAddPopup->SetShowState(false);
			AddMonitoringRegion();
		}
		break;

		case ID_POPUP_REGION_CANCEL:
		{
			AppLog("ID_POPUP_REGION_CANCEL");
			__pRegionCenterLatitude->Clear();
			__pRegionCenterLongitude->Clear();
			__pRegionRadius->Clear();
			__pRegionAddPopup->SetShowState(false);
		}
		break;

		default:
			break;
		}
}

void
LocationManagerMainForm::OnFormBackRequested(Tizen::Ui::Controls::Form& source)
{
	AppLog("OnFormBackRequested");
	UiApp* pApp = UiApp::GetInstance();
	AppAssert(pApp);
	pApp->Terminate();
}

bool
LocationManagerMainForm::OnKeyPressed(Control& source, const KeyEventInfo& keyEventInfo)
{
	AppLog("OnKeyPressed");
	return false;
}

bool
LocationManagerMainForm::OnKeyReleased(Control& source, const KeyEventInfo& keyEventInfo)
{
	AppLog("OnKeyReleased");
	if((keyEventInfo.GetKeyCode() == KEY_ESC || keyEventInfo.GetKeyCode() == KEY_BACK) && source.GetShowState() == true)
	{
		if(source.Equals(*__pRegionAddPopup) || source.Equals(*__pRegionCenterLatitude)
			|| source.Equals(*__pRegionCenterLongitude) || source.Equals(*__pRegionRadius)
			|| source.Equals(*__pButtonCancel) || source.Equals(*__pButtonOk))
		{
			__pRegionAddPopup->SetShowState(false);
		}
	}
	return false;
}

bool
LocationManagerMainForm::OnPreviewKeyPressed(Control& source, const KeyEventInfo& keyEventInfo)
{
	AppLog("OnPreviewKeyPressed");
	return false;
}

bool
LocationManagerMainForm::OnPreviewKeyReleased(Control& source, const KeyEventInfo& keyEventInfo)
{
	AppLog("OnPreviewKeyReleased");
	return false;
}

bool
LocationManagerMainForm::TranslateKeyEventInfo(Control& source, KeyEventInfo& keyEventInfo)
{
	AppLog("TranslateKeyEventInfo");
	return false;
}

void
LocationManagerMainForm::OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId& previousSceneId,
										  const Tizen::Ui::Scenes::SceneId& currentSceneId, Tizen::Base::Collection::IList* pArgs)
{
	AppLog("OnSceneActivatedN");
}

void
LocationManagerMainForm::OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId& currentSceneId,
										   const Tizen::Ui::Scenes::SceneId& nextSceneId)
{
	AppLog("OnSceneDeactivated");
}

void
LocationManagerMainForm::OnLocationUpdated(const Tizen::Locations::Location& location)
{
	AppLog("Location update obtained for application.");

	LogLocationInfo(location);

	if (__pTextBoxInfo != null)
	{
		DrawLocationInformation(location);
	}
	else
	{
		ShowMessageBox(L"Location updated", L"Location information obtained.");
	}
}

void
LocationManagerMainForm::OnLocationUpdateStatusChanged(LocationServiceStatus status)
{
	AppLog("Service Status is (%d)", status);

	__pTextBoxStatus->Clear();

	switch(status)
	{
	case LOC_SVC_STATUS_RUNNING:
		__pTextBoxStatus->AppendText(L"Update Status: Running");
		break;

	case LOC_SVC_STATUS_PAUSED:
		__pTextBoxStatus->AppendText(L"Update Status: Paused");
		break;

	case LOC_SVC_STATUS_DENIED:
		__pTextBoxStatus->AppendText(L"Update Status: Denied");
		break;

	case LOC_SVC_STATUS_NOT_FIXED:
		__pTextBoxStatus->AppendText(L"Update Status: Not Fixed");
		break;

	case LOC_SVC_STATUS_IDLE:
		__pTextBoxStatus->AppendText(L"Update Status: Idle");
		break;
	}

	__pTextBoxStatus->Draw();
}

void
LocationManagerMainForm::OnRegionEntered(RegionId regionId)
{
	AppLog("Region Entered for Id (%d)", regionId);
	__pTextBoxInfo->AppendText(L"Region entered for region Id " + String(Integer::ToString(__regionId)) + L"\n");
	__pTextBoxInfo->Draw();

}

void
LocationManagerMainForm::OnRegionLeft(RegionId regionId)
{
	AppLog("Region Left for Id (%d)", regionId);
	__pTextBoxInfo->AppendText(L"Region left for region Id " + String(Integer::ToString(__regionId)) + L"\n");
	__pTextBoxInfo->Draw();
}

void
LocationManagerMainForm::OnRegionMonitoringStatusChanged(LocationServiceStatus status)
{
	AppLog("Service Status is (%d)", status);

	__pTextBoxStatus->Clear();

	switch(status)
	{
	case LOC_SVC_STATUS_RUNNING:
		__pTextBoxStatus->AppendText(L"Monitoring Status: Running");
		break;

	case LOC_SVC_STATUS_PAUSED:
		__pTextBoxStatus->AppendText(L"Monitoring Status: Paused");
		break;

	case LOC_SVC_STATUS_DENIED:
		__pTextBoxStatus->AppendText(L"Monitoring Status: Denied");
		break;

	case LOC_SVC_STATUS_NOT_FIXED:
		__pTextBoxStatus->AppendText(L"Monitoring Status: Not Fixed");
		break;

	case LOC_SVC_STATUS_IDLE:
		__pTextBoxStatus->AppendText(L"Monitoring Status: Idle");
		break;
	}

	__pTextBoxStatus->Draw();
}

void
LocationManagerMainForm::OnAccuracyChanged(LocationAccuracy accuracy)
{
	AppLog("Accuracy changed to (%d)", accuracy);
	__pTextBoxAccuracy->Clear();

	switch(accuracy)
	{
	case LOC_ACCURACY_FINEST:
		__pTextBoxAccuracy->AppendText(L"Location Accuracy: Finest");
		break;

	case LOC_ACCURACY_TEN_METERS:
		__pTextBoxAccuracy->AppendText(L"Location Accuracy: Ten meters");
		break;

	case LOC_ACCURACY_HUNDRED_METERS:
		__pTextBoxAccuracy->AppendText(L"Location Accuracy: Hundred meters");
		break;

	case LOC_ACCURACY_ONE_KILOMETER:
		__pTextBoxAccuracy->AppendText(L"Location Accuracy: One Kilometer");
		break;

	case LOC_ACCURACY_ANY:
		__pTextBoxAccuracy->AppendText(L"Location Accuracy: Any");
		break;
	case LOC_ACCURACY_INVALID:
		__pTextBoxAccuracy->AppendText(L"Location Accuracy: Invalid");
		break;
	}

	__pTextBoxAccuracy->Draw();
}

void
LocationManagerMainForm::DrawLocationInformation(const Tizen::Locations::Location& location)
{
	__pTextBoxInfo->Clear();

	Coordinates coord = location.GetCoordinates();

	__pTextBoxInfo->AppendText(L"Latitude: " + Double::ToString(coord.GetLatitude()) + L" " +
						 L"Longitude: " + Double::ToString(coord.GetLongitude()) + L"\n" +
						 L"Altitude: " + Double::ToString(coord.GetAltitude()) + L"\n");

	__pTextBoxInfo->AppendText(L"Speed: "+ Double::ToString(location.GetSpeed()) + L"\n");
	__pTextBoxInfo->AppendText(L"Course: "+ Double::ToString(location.GetCourse()) + L"\n");
	__pTextBoxInfo->AppendText(L"Vertical Accuracy: "+ Double::ToString(location.GetVerticalAccuracy()) + L"\n");
	__pTextBoxInfo->AppendText(L"Horizontal Accuracy: " + Double::ToString(location.GetHorizontalAccuracy()) + L"\n");


	DateTime datetime = location.GetTimestamp();
	LocaleManager lclMgr;
	result r = lclMgr.Construct();
	if (r == E_SUCCESS)
	{
		int offset = lclMgr.GetSystemTimeZone().GetRawOffset();
		datetime.AddMinutes(offset);
	}

	__pTextBoxInfo->AppendText(L"Timestamp: " + datetime.ToString() + L"\n");


	String satelliteInfoKey = L"satellite";
	String methodName = L"location_method";

	__pTextBoxInfo->AppendText(L"Location Method: ");
	__pTextBoxInfo->AppendText(location.GetExtraInfo(methodName).GetPointer());
	__pTextBoxInfo->AppendText(L"\n");

	__pTextBoxInfo->AppendText(L"Satellite Information: ");
	__pTextBoxInfo->AppendText(L"\n");

	String satelliteInfo = location.GetExtraInfo(satelliteInfoKey);
	StringTokenizer satInfotoken(satelliteInfo, L";");
	int count = satInfotoken.GetTokenCount();

	for (int i = 0; i < count; i ++)
	{
		String element;
		satInfotoken.GetNextToken(element);
		StringTokenizer satElements(element, L",");

		String prn;
		String elevation;
		String azimuth;
		String snr;
		satElements.GetNextToken(prn);
		satElements.GetNextToken(elevation);
		satElements.GetNextToken(azimuth);
		satElements.GetNextToken(snr);

		prn.Trim();
		elevation.Trim();
		azimuth.Trim();
		snr.Trim();

		__pTextBoxInfo->AppendText(Integer::ToString(i + 1) + L")" + L" Id: " + prn + L" Elevation: " + elevation + L" Azimuth: " + azimuth + L" SNR: " + snr + L"\n");
	}

	__pTextBoxInfo->Draw();

	// **** BACKENDLESS CREATE (location)
	r = Backendless::GetInstance()->RequestSaveLocation(location);
	AppLog("[%s] backendless location CREATE.", GetErrorMessage(r));
	// ******************************************************************************

}

void
LocationManagerMainForm::SetLocationUpdateView(void)
{

	if (__viewType == VIEW_TYPE_LOCTION_UPDATE)
	{
		return;
	}

	__viewType = VIEW_TYPE_LOCTION_UPDATE;

	if (__isUpdateInProgress ==  true)
	{
		if(__regionId != -1)
		{
			__pLocProvider->RemoveMonitoringRegion(__regionId);
			__regionId = -1;
		}
		__isUpdateInProgress = false;
	}

	Footer* pFooter = GetFooter();

	pFooter->RemoveAllItems();

	FooterItem update;
	update.Construct(ID_FOOTER_BUTTON_UPDATE);
	update.SetText(L"Update");
	pFooter->AddItem(update);
	pFooter->AddActionEventListener(*this);

	FooterItem get;
	get.Construct(ID_FOOTER_BUTTON_GET);
	get.SetText(L"Get");
	pFooter->AddItem(get);
	pFooter->AddActionEventListener(*this);
	pFooter->Invalidate(true);

	__pTextBoxStatus->Clear();
	__pTextBoxStatus->AppendText(L"Update Status: Idle");
	__pTextBoxStatus->Draw();

	__pTextBoxAccuracy->Clear();
	__pTextBoxAccuracy->AppendText(L"Location Accuracy: Invalid");
	__pTextBoxAccuracy->Draw();

	__pTextBoxInfo->Clear();
	__pTextBoxInfo->AppendText(L"- No Information -");
	__pTextBoxInfo->Draw();
}

void
LocationManagerMainForm::SetRegionMonitorView(void)
{
	if (__viewType == VIEW_TYPE_REGION_MONITORING)
	{
		return;
	}

	__viewType = VIEW_TYPE_REGION_MONITORING;

	if (__isUpdateInProgress ==  true)
	{
		__pLocProvider->StopLocationUpdates();
		__isUpdateInProgress = false;
	}

	Footer* pFooter = GetFooter();

	pFooter->RemoveAllItems();

	FooterItem addRegion;
	addRegion.Construct(ID_FOOTER_ADD_MONITORING_REGION);
	addRegion.SetText(L"Add Region");
	pFooter->AddItem(addRegion);
	pFooter->AddActionEventListener(*this);

	FooterItem removeRegion;
	removeRegion.Construct(ID_FOOTER_REMOVE_MONITORING_REGION);
	removeRegion.SetText(L"Remove");
	pFooter->AddItem(removeRegion);
	pFooter->AddActionEventListener(*this);
	pFooter->Invalidate(true);

	__pTextBoxStatus->Clear();
	__pTextBoxStatus->AppendText(L"Monitoring Status: Idle");
	__pTextBoxStatus->Draw();

	__pTextBoxAccuracy->Clear();
	__pTextBoxAccuracy->AppendText(L"Location Accuracy: Invalid");
	__pTextBoxAccuracy->Draw();

	__pTextBoxInfo->Clear();
	__pTextBoxInfo->AppendText(L"- START -\n");
	__pTextBoxInfo->Draw();
}

void
LocationManagerMainForm::ShowPopUp(void)
{
	__pRegionAddPopup->SetShowState(true);
	__pRegionAddPopup->Show();
}

void
LocationManagerMainForm::AddMonitoringRegion(void)
{
	double latitude = 0.0;
	double longitude = 0.0;
	double radius = 0.0;
	bool success = true;

	AppLog("Now the regions will be added.");

	result r = Double::Parse(__pRegionCenterLatitude->GetText(), latitude);
	if (r != E_SUCCESS)
	{
		success = false;
	}

	r = Double::Parse(__pRegionCenterLongitude->GetText(), longitude);
	if (r != E_SUCCESS)
	{
		success = false;
	}

	r = Double::Parse(__pRegionRadius->GetText(), radius);
	if (r != E_SUCCESS)
	{
		success = false;
	}

	if (success == false)
	{
		ShowMessageBox(L"Invalid Input", L"The region info provided is not proper.");
	}
	else
	{
		__regionCenter.Set(latitude, longitude, 0.0);
		__regionRadius = radius;

		if (__regionId != -1)
		{
			__pLocProvider->RemoveMonitoringRegion(__regionId);
			__pTextBoxInfo->AppendText(L"Monitoring region with region Id " + String(Integer::ToString(__regionId)) + L" removed\n");
			__pTextBoxInfo->Draw();
			__regionId = -1;

			__pTextBoxAccuracy->Clear();
			__pTextBoxAccuracy->AppendText(L"Location Accuracy: Invalid");
			__pTextBoxAccuracy->Draw();

			__pTextBoxStatus->Clear();
			__pTextBoxStatus->AppendText(L"Monitoring Status: Idle");
			__pTextBoxStatus->Draw();
		}
		result r = __pLocProvider->AddMonitoringRegion(__regionCenter, __regionRadius, __regionId);
		if (r == E_USER_NOT_CONSENTED)
		{
			bool isSettingEnabled = CheckLocationSetting();
			if (!isSettingEnabled)
			{
				LaunchLocationSettings();
			}
			else
			{
				ShowMessageBox(L"Privacy protection", L"Please allow the application to use your location. You can change settings at Settings->Privacy.");
			}
			return;
		}
		else if (IsFailed(r))
		{
			ShowMessageBox(L"Invalid Input", L"The region info provided is not proper.");
			return;

		}
		__isUpdateInProgress = true;
		__pTextBoxInfo->AppendText(L"Monitoring region with region Id " + String(Integer::ToString(__regionId)) + L" added\n");
		__pTextBoxInfo->Draw();

	}
}

void
LocationManagerMainForm::UpdateFooterItem(int item)
{
	Footer* pFooter = GetFooter();
	pFooter->RemoveItemAt(0);

	if (item == ID_CONTEXT_UPDATE_INTERVAL)
	{
		FooterItem update;
		update.Construct(ID_FOOTER_BUTTON_UPDATE);
		update.SetText(L"Update");
		pFooter->InsertItemAt(0, update);
		pFooter->AddActionEventListener(*this);
	}

	else if (item == ID_FOOTER_BUTTTON_CANCEL)
	{
		FooterItem cancel;
		cancel.Construct(ID_FOOTER_BUTTTON_CANCEL);
		cancel.SetText(L"Cancel");
		pFooter->InsertItemAt(0, cancel);
		pFooter->AddActionEventListener(*this);
	}
	pFooter->Invalidate(true);
}

void
LocationManagerMainForm::ShowMessageBox(const String& title, const String& message)
{
	MessageBox messageBox;

	messageBox.Construct(title, message, MSGBOX_STYLE_OK);

	int ModalResult;
	messageBox.ShowAndWait(ModalResult);
}

void
LocationManagerMainForm::LogLocationInfo(const Tizen::Locations::Location location)
{
	String timeStamp = location.GetTimestamp().ToString();
	String extraInfo = L"satellite";

	Coordinates coord = location.GetCoordinates();
	AppLog("Location Information: Latitude = %lf, Longitude = %lf, Altitude = %lf", coord.GetLatitude(), coord.GetLongitude(), coord.GetAltitude());
	AppLog("Location Information: Horizontal Accuracy = %f, Vertical Accuracy = %f", location.GetHorizontalAccuracy(), location.GetVerticalAccuracy());
	AppLog("Location Information: Direction = %f, Speed = %f", location.GetCourse(), location.GetSpeed());
	AppLog("Location Information: SatelliteInfo = %ls", location.GetExtraInfo(extraInfo).GetPointer());
	AppLog("Location Information: TimeStamp = %ls",timeStamp.GetPointer());
}

void
LocationManagerMainForm::OnUserEventReceivedN(RequestId requestId, Tizen::Base::Collection::IList* pArgs)
{
	if (requestId == LOC_MGR_DRAW_SYNC_LOC_UPDATE)
	{
		if(__viewType == VIEW_TYPE_LOCTION_UPDATE)
		{
			Location* pLocation = static_cast<Location*> (pArgs->GetAt(0));

			if (pLocation->IsValid())
			{
				DrawLocationInformation(*pLocation);
			}
			else
			{
				__pTextBoxInfo->Clear();
				__pTextBoxInfo->AppendText(L"Failed to fetch the current location");
				__pTextBoxInfo->Draw();
			}
		}
	}
	else if(requestId == LOC_MGR_NOTIFY_ERROR)
	{
		bool isSettingEnabled = CheckLocationSetting();
		if (!isSettingEnabled)
		{
			LaunchLocationSettings();
		}
		else
		{
			ShowMessageBox(L"Privacy protection", L"Please allow the application to use your location. You can change settings at Settings->Privacy.");
		}
	}

	if(pArgs)
	{
		pArgs->RemoveAll(true);
		delete pArgs;
	}
}

void
LocationManagerMainForm::LaunchLocationSettings(void)
{
	int res;

	MessageBox messageBox;
	messageBox.Construct(L"Information", L"Location services are disabled. Enable them in location settings?", MSGBOX_STYLE_YESNO);
	messageBox.ShowAndWait(res);

	if (res == MSGBOX_RESULT_YES)
	{
		HashMap extraData;
		extraData.Construct();
		String categoryKey = L"category";
		String categoryVal = L"Location";
		extraData.Add(&categoryKey, &categoryVal);

		AppControl* pAc = AppManager::FindAppControlN(L"tizen.settings", L"http://tizen.org/appcontrol/operation/configure");

		if (pAc)
		{
			pAc->Start(null, null, &extraData, this);
			delete pAc;
		}
	}
	return;
}

bool
LocationManagerMainForm::CheckLocationSetting(void)
{
	bool hasPrivilege = false;
	bool gpsEnabled = true;
	bool wpsEnabled = true;

	result gps = Tizen::System::SettingInfo::GetValue(L"http://tizen.org/setting/location.gps", gpsEnabled);
	result wps = Tizen::System::SettingInfo::GetValue(L"http://tizen.org/setting/location.wps", wpsEnabled);

	hasPrivilege = gpsEnabled | wpsEnabled;
	if (gps != E_SUCCESS || wps != E_SUCCESS || hasPrivilege == false)
	{
		return false;
	}

	return true;
}

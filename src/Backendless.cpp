/*
 * Backendless.cpp
 */
/*
 * *********************************************************************************************************************
 *
 *  BACKENDLESS.COM CONFIDENTIAL
 *
 *  ********************************************************************************************************************
 *
 *  Copyright 2014 BACKENDLESS.COM. All Rights Reserved.
 *
 *  NOTICE: All information contained herein is, and remains the property of Backendless.com and its suppliers,
 *  if any. The intellectual and technical concepts contained herein are proprietary to Backendless.com and its
 *  suppliers and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret
 *  or copyright law. Dissemination of this information or reproduction of this material is strictly forbidden
 *  unless prior written permission is obtained from Backendless.com.
 *
 *  ********************************************************************************************************************
 */

#include <new>
#include <string>
#include "Backendless.h"

using namespace std;
using namespace Tizen::Base;
using namespace Tizen::Locations;
using namespace Tizen::Text;
using namespace Tizen::Net::Http;
using namespace Tizen::Web::Json;
using namespace Tizen::Base::Collection;


const static wchar_t* HTTP_CLIENT_HOST_ADDRESS = L"http://api.backendless.com";
const static wchar_t* BACKENDLESS_APPLICATION_ID = L"****** REPLACE THIS WITH YOUR APP ID ***********";
const static wchar_t* BACKENDLESS_SECRET_KEY = L"****** REPLACE THIS WITH YOUR REST SECREY KEY ***********";

const static wchar_t* JSON_TYPES[] = {L"JSON_TYPE_STRING", L"JSON_TYPE_NUMBER", L"JSON_TYPE_OBJECT", L"JSON_TYPE_ARRAY", L"JSON_TYPE_BOOL", L"JSON_TYPE_NULL"};


enum OperationRequest
{
	OPERATION_REQUEST_NONE,		/**< No operation	*/
	OPERATION_REQUEST_GET_LOCATIONSLIST, /**< Get Locations List	*/
	OPERATION_REQUEST_CREATE_LOCATION, /**< Create Location	*/
};


Backendless* Backendless::pBackendlessSinglton = null;

Backendless*
Backendless::GetInstance()
{
	if (!pBackendlessSinglton)
		pBackendlessSinglton = new Backendless();

	return pBackendlessSinglton;
}

Backendless::Backendless(void)
	: __pHttpSession(null)
	, __pLocation(null)
	, __pLocationsList(null)
	, __OperationRequest(OPERATION_REQUEST_NONE)
{

	__pLocationsList = new ArrayList();
	if (__pLocationsList != null)
	{
		__pLocationsList->Construct();
	}
	else
	{
		AppLogException("Backendless:: Failed to instantiate locations list.");
	}

}

Backendless::~Backendless(void)
{
	if (__pHttpSession != null)
	{
		delete __pHttpSession;
		__pHttpSession = null;
	}

	if (__pLocation != null)
	{
		delete __pLocation;
		__pLocation = null;
	}

	if (__pLocationsList != null)
	{
		__pLocationsList->RemoveAll(true);
		delete __pLocationsList;
		__pLocationsList = null;
	}
}

result
Backendless::CreateHttpSession(void)
{
	result r = E_SUCCESS;

	if (__pHttpSession == null)
	{
		//__pHttpSession = new (std::nothrow) HttpSession();
		__pHttpSession = new HttpSession();
		r = __pHttpSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, HTTP_CLIENT_HOST_ADDRESS, null);
		if (IsFailed(r)) {
			delete __pHttpSession;
			__pHttpSession = null;
			AppLogException("Backendless:: Failed to create the HttpSession.");
		}
	}

	return r;
}

result
Backendless::CreateHttpTransaction(NetHttpMethod method, String& uri, String* entity)
{
	AppLog("Backendless::CreateHttpTransaction");

	result r = E_SUCCESS;

	HttpTransaction* pHttpTransaction = null;
	HttpRequest* pHttpRequest = null;
	HttpHeader* pHeader = null;
	HttpStringEntity* pHttpStringEntity = null;

	r = CreateHttpSession();
	TryCatch(r == E_SUCCESS, , "Failed to create the HttpSession.");

	r = __pHttpSession->SetAutoRedirectionEnabled(true);
	TryCatch(r == E_SUCCESS, , "Failed to set the redirection automatically.");

	pHttpTransaction = __pHttpSession->OpenTransactionN();
	r = GetLastResult();
	TryCatch(pHttpTransaction != null, , "Failed to open the HttpTransaction.");

	r = pHttpTransaction->AddHttpTransactionListener(*this);
	TryCatch(r == E_SUCCESS, , "Failed to add the HttpTransactionListener.");

	pHttpRequest = const_cast< HttpRequest* >(pHttpTransaction->GetRequest());

	r = pHttpRequest->SetUri(uri);
	TryCatch(r == E_SUCCESS, , "Failed to set the uri.");

	r = pHttpRequest->SetMethod(method);
	TryCatch(r == E_SUCCESS, , "Failed to set the method.");

	// set headers
	pHeader = pHttpRequest->GetHeader();
	pHeader->AddField(L"application-id", BACKENDLESS_APPLICATION_ID);
	pHeader->AddField(L"secret-key", BACKENDLESS_SECRET_KEY);
	pHeader->AddField(L"application-type", L"REST");

	if (entity == null)
	{
		pHeader->AddField(L"Content-Type", L"text/plane");
	}
	else
	{
		//set body as json
		pHttpStringEntity = new HttpStringEntity();
		Encoding* pEnc = Encoding::GetEncodingN(L"ISO-8859-1");
	    pHttpStringEntity->Construct(*entity, L"application/json", L"ISO-8859-1", *pEnc);
	    r = pHttpRequest->SetEntity(*pHttpStringEntity);
	    delete pHttpStringEntity;
		TryCatch(r == E_SUCCESS, , "Failed to set the body.");
	}

	r = pHttpTransaction->Submit();
	TryCatch(r == E_SUCCESS, , "Failed to submit the HttpTransaction.");

	return r;

CATCH:

	delete pHttpTransaction;
	pHttpTransaction = null;

	AppLog("CreateHttpTransaction() failed. (%s)", GetErrorMessage(r));

	return r;
}

result
Backendless::RequestLoadLocationsList(Tizen::Base::Collection::IList* pLocationsList)
{
	//AppLog("Backendless::RequestLoadLocationsList");

	if (__pLocationsList != null)
		__pLocationsList->RemoveAll(true);

	__OperationRequest = OPERATION_REQUEST_GET_LOCATIONSLIST;
	String uri = L"http://api.backendless.com/v1/geo/points?categories=Tizen";
	return CreateHttpTransaction(NET_HTTP_METHOD_GET, uri, null);
}


result
Backendless::RequestSaveLocation(const Tizen::Locations::Location& location)
{
	Coordinates coord = location.GetCoordinates();

#if 1 // with metadata

	// create json object
	JsonObject *pJsonObject = new JsonObject();
    pJsonObject->Construct();

	// construct json object
	String *pAltitudeKey = new String(L"Altitude");
	JsonNumber *pAltitude = new JsonNumber(coord.GetAltitude());
	pJsonObject->Add(pAltitudeKey, pAltitude);

	String *pSpeedKey = new String(L"Speed");
	JsonNumber *pSpeed = new JsonNumber(location.GetSpeed());
	pJsonObject->Add(pSpeedKey, pSpeed);

	String *pCourseKey = new String(L"Course");
	JsonNumber *pCourse = new JsonNumber(location.GetCourse());
	pJsonObject->Add(pCourseKey, pCourse);

	String *pVerticalAccuracyKey = new String(L"VerticalAccuracy");
	JsonNumber *pVerticalAccuracy = new JsonNumber(location.GetVerticalAccuracy());
	pJsonObject->Add(pVerticalAccuracyKey, pVerticalAccuracy);

	String *pHorizontalAccuracyKey = new String(L"HorizontalAccuracy");
	JsonNumber *pHorizontalAccuracy = new JsonNumber(location.GetHorizontalAccuracy());
	pJsonObject->Add(pHorizontalAccuracyKey, pHorizontalAccuracy);

	String *pTimestampKey = new String(L"Timestamp");
	JsonString *pTimestamp = new JsonString(location.GetTimestamp().ToString());
	pJsonObject->Add(pTimestampKey, pTimestamp);

	String *pLocationMethodKey = new String(L"LocationMethod");
	JsonString *pLocationMethod = new JsonString(location.GetExtraInfo(L"location_method").GetPointer());
	pJsonObject->Add(pLocationMethodKey, pLocationMethod);

	// composes to ByteBuffer
    ByteBuffer *pBuffer = new ByteBuffer();
    result r = JsonWriter::Compose(pJsonObject, *pBuffer);

    // release json object
    pJsonObject->RemoveAll(true);
    delete pJsonObject;

    if (IsFailed(r))
    {
    	delete pBuffer;
		return r;
    }

	String json((char*)pBuffer->GetPointer());
	ComposeEscapedJsonUri(json);
	String uri = L"http://api.backendless.com/v1/geo/points?lat=" + Double::ToString(coord.GetLatitude()) + "&lon="+ Double::ToString(coord.GetLongitude()) +"&categories=Tizen&metadata="+json;

    delete pBuffer;

#else // without metadata
 	String uri = L"http://api.backendless.com/v1/geo/points?lat=" + Double::ToString(coord.GetLatitude()) + "&lon="+ Double::ToString(coord.GetLongitude()) +"&categories=Tizen";
#endif

 	AppLog("RequestSaveLocation URI: <%ls>", uri.GetPointer());

    // send HTTP request
	__OperationRequest = OPERATION_REQUEST_CREATE_LOCATION;
 	return CreateHttpTransaction(NET_HTTP_METHOD_PUT, uri, null);
}

void
Backendless::ComposeEscapedJsonUri(String& uri)
{
 	AppLog("ComposeEscapedJsonUri JSON URI (UNESCAPED): <%ls>", uri.GetPointer());

 	uri.Replace(L"\"", L"%22");
 	uri.Replace(L"{", L"%7B");
 	uri.Replace(L"}", L"%7D");
 	uri.Replace(L"/", L"%2F");
 	uri.Replace(L" ", L"%20");
 	uri.Replace(L"\\", L"");

 	AppLog("ComposeEscapedJsonUri JSON URI (ESCAPED): <%ls>", uri.GetPointer());
}

HashMap*
Backendless::ParseLocationMetadata(const JsonObject* pJsonObj)
{
	HashMap* pHashMap = new HashMap();
	if (pHashMap == null)
		return null;

	return pHashMap;
}

void
Backendless::ParseJsonBackendlessObject(IJsonValue* pJsonValue)
{
	if ( pJsonValue == null )
	{
		AppLog("ParseJsonBackendlessObject (FAULT) pJsonValue: <%s>", GetLastResult());
		return;
	}

	// Uses the pJsonValue to know what is the type
	JsonType jsonType = pJsonValue->GetType();
	switch (jsonType) {

		case JSON_TYPE_OBJECT:
		{

			JsonObject* pJsonObj = static_cast<JsonObject*>(pJsonValue); //use pJsonObj to access key-value pairs

			// get 'objectId' field (must be in backendless object)
			String objectId = L"";
			if ( (jsonType = GetJsonObjectFieldAsString(pJsonObj, L"objectId", &objectId)) != JSON_TYPE_STRING)
			{
				AppLog("ParseJsonBackendlessObject (OPTION) <NOT BACKENDLESS OBJECT - MAY BE A BACKENDLESS COLLECTION?>");

				IJsonValue* pValue = null;
				if ( (pValue = GetJsonObjectFieldValue(pJsonObj, L"data", JSON_TYPE_ARRAY)) != null )
				{
					JsonArray* pJsonArray = static_cast<JsonArray*>(pValue); //use pJsonArray to access values

					AppLog("ParseJsonBackendlessObject (SUCCESS) COLLECTION count = %d", pJsonArray->GetCount());

			        // Uses enumerator to access elements in the JsonArray
			        IEnumeratorT<IJsonValue*>* pEnum = pJsonArray->GetEnumeratorN();
			        if(pEnum)
			        {
		                IJsonValue* pNextValue = null;
			            while( pEnum->MoveNext() == E_SUCCESS )
			            {
			                pEnum->GetCurrent(pNextValue);
			                ParseJsonBackendlessObject(pNextValue);
			            }

			            delete pEnum;
			        }

			        AppLog("ParseJsonBackendlessObject (SUCCESS) LOCATIONS LIST count = %d", __pLocationsList->GetCount());

			        pJsonArray->RemoveAll(true);
				}
			}
			else
			{
				AppLog("ParseJsonBackendlessObject (SUCCESS) objectId = %ls", objectId.GetPointer());

				// construct Location metadata
				ParseLocationMetadata(pJsonObj);
			}

			pJsonObj->RemoveAll(true);

			break;
		}

		default:
		{
			AppLog("ParseJsonBackendlessObject <UNPROCESSED TYPE %ls>", JSON_TYPES[jsonType]);
			break;
		}
	}
}

IJsonValue*
Backendless::GetJsonObjectFieldValue(const JsonObject* pJsonObj, const String& FieldKey, const JsonType jsonType)
{

	IJsonValue* pValue = null;
	pJsonObj->GetValue(&FieldKey, pValue);
	if (pValue == null || pValue->GetType() != jsonType)
	{
		AppLog("GetJsonObjectFieldValue (ERROR) <FIELD '%ls' OF TYPE %ls IS ABSENT>", FieldKey.GetPointer(), JSON_TYPES[jsonType]);
		return null;
	}

	return pValue;
}

JsonType
Backendless::GetJsonObjectFieldAsString(const JsonObject* pJsonObj, const String& FieldKey, String* pFieldValue)
{
	if (pFieldValue == null)
	{
		return JSON_TYPE_NULL;
	}

	IJsonValue* pValue = null;
	pJsonObj->GetValue(&FieldKey, pValue);
	if (pValue == null)
	{
		return JSON_TYPE_NULL;
	}

	JsonType jsonType = pValue->GetType();
	if (jsonType != JSON_TYPE_STRING)
	{
		return jsonType;
	}

	JsonString* pString = static_cast<JsonString*>(pValue);
	String str(pString->GetPointer());
	*pFieldValue = str;

	//AppLog("GetJsonObjectFieldAsString (SUCCESS)  '%ls'", pFieldValue->GetPointer());

	return JSON_TYPE_STRING;
}

JsonType
Backendless::GetJsonObjectFieldAsNumber(const JsonObject* pJsonObj, const String& FieldKey, JsonNumber* pFieldValue)
{
	if (pFieldValue == null)
	{
		return JSON_TYPE_NULL;
	}

	IJsonValue* pValue = null;
	pJsonObj->GetValue(&FieldKey, pValue);
	if (pValue == null)
	{
		return JSON_TYPE_NULL;
	}

	JsonType jsonType = pValue->GetType();
	if (jsonType != JSON_TYPE_NUMBER)
	{
		return jsonType;
	}

	JsonNumber* pNumber = static_cast<JsonNumber*>(pValue);
	*pFieldValue = *pNumber;

	return JSON_TYPE_NUMBER;
}

void
Backendless::OnTransactionReadyToRead(HttpSession& httpSession, HttpTransaction& httpTransaction, int availableBodyLen)
{

	HttpResponse* pHttpResponse = httpTransaction.GetResponse();
	int status = pHttpResponse->GetHttpStatusCode();

	AppLog("OnTransactionReadyToRead [STATUS: %d, BODY LENGTH: %d]", status, availableBodyLen);

	if (status == HTTP_STATUS_OK)
	{
		HttpHeader* pHttpHeader = pHttpResponse->GetHeader();
		if (pHttpHeader != null)
		{
			String* tempHeaderString = pHttpHeader->GetRawHeaderN();

			if (availableBodyLen > 0)
			{
				ByteBuffer* pBuffer = pHttpResponse->ReadBodyN();

				// swow the response
				String text((char *)(pBuffer->GetPointer()));
				int tail = text.GetLength() - availableBodyLen;
				if (tail > 0) {
					text.Remove(availableBodyLen, tail);
				}

				AppLog("OnTransactionReadyToRead LENGTH: %d, [TEXT: %ls]", text.GetLength(), text.GetPointer());

				text = *tempHeaderString + text;

				// parse json response
				IJsonValue* pJsonValue = JsonParser::ParseN(*pBuffer);
				ParseJsonBackendlessObject(pJsonValue);
				delete pJsonValue;

				delete pBuffer;
			}

			delete tempHeaderString;
		}
		else
		{
			AppLog("OnTransactionReadyToRead [ERROR: No Headers]");
		}
	}
	else
	{
		String text(L"HTTP Status Code: ");
		text.Append(status);
		AppLog("OnTransactionReadyToRead [ERROR: %ls]", text.GetPointer());
	}
}

void
Backendless::OnTransactionAborted(HttpSession& httpSession, HttpTransaction& httpTransaction, result r)
{
	AppLog("OnTransactionAborted(%s)", GetErrorMessage(r));

	delete &httpTransaction;
}

void
Backendless::OnTransactionReadyToWrite(HttpSession& httpSession, HttpTransaction& httpTransaction, int recommendedChunkSize)
{
	AppLog("OnTransactionReadyToWrite");
}

void
Backendless::OnTransactionHeaderCompleted(HttpSession& httpSession, HttpTransaction& httpTransaction, int headerLen, bool authRequired)
{
	AppLog("OnTransactionHeaderCompleted");
}

void
Backendless::OnTransactionCompleted(HttpSession& httpSession, HttpTransaction& httpTransaction)
{
	AppLog("OnTransactionCompleted");

	delete &httpTransaction;
}

void
Backendless::OnTransactionCertVerificationRequiredN(HttpSession& httpSession, HttpTransaction& httpTransaction, Tizen::Base::String* pCert)
{
	AppLog("OnTransactionCertVerificationRequiredN");

	httpTransaction.Resume();

	delete pCert;
}







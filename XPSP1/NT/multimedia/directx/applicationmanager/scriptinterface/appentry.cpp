//////////////////////////////////////////////////////////////////////////////////////////////
//
// AppEntry.cpp
// 
// Copyright (C) 1998, 1999 Microsoft Corporation. All rights reserved.
//
// Abstract :
//
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <comdef.h>
#include "stdafx.h"
#include "AppManDispatch.h"
#include "AppEntry.h"
#include "AppMan.h"
#include "AppManAdmin.h"
#include "AppManDebug.h"

//To flag as DBG_APPMANDP
#ifdef DBG_MODULE
#undef DBG_MODULE
#endif

#define DBG_MODULE  DBG_APPMANDP

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define JANUARY     1
#define FEBRUARY    2
#define MARCH       3
#define APRIL       4
#define MAY         5
#define JUNE        6
#define JULY        7
#define AUGUST      8
#define SEPTEMBER   9
#define OCTOBER     10
#define NOVEMBER    11
#define DECEMBER    12

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

void  SystemTimeToDate(SYSTEMTIME * lpSystemTime, DATE * lpDate)
{
  FUNCTION(" SystemTimeToDate()");

  DOUBLE  dDouble;
  DWORD   dwIndex;

  //
  // How many days have elapsed between 1900 and January 1 of this year.
  //

  dDouble = 1.0;
  dwIndex = 1900;
  while (dwIndex < lpSystemTime->wYear)
  {
    if (!(dwIndex % 4))
    {
      dDouble += 366.0;
    }
    else
    {
      dDouble += 365.0;
    }
    dwIndex++;
  }

  //
  // Is this a bicentile year
  //

  if ((!(lpSystemTime->wYear % 4))&&(MARCH <= lpSystemTime->wMonth))
  {
    dDouble += 1.0;
  }

  //
  // How many days have elapsed since the beginning of the year
  //

  switch(lpSystemTime->wMonth)
  {
    case 1
    : dDouble += 0.0;
      break;
    case 2
    : dDouble += 31.0;
      break;
    case 3
    : dDouble += 59.0;
      break;
    case 4
    : dDouble += 90.0;
      break;
    case 5
    : dDouble += 120.0;
      break;
    case 6
    : dDouble += 151.0;
      break;
    case 7
    : dDouble += 181.0;
      break;
    case 8
    : dDouble += 212.0;
      break;
    case 9
    : dDouble += 243.0;
      break;
    case 10
    : dDouble += 273.0;
      break;
    case 11
    : dDouble += 304.0;
      break;
    case 12
    : dDouble += 334.0;
      break;
  }

  //
  // How many days have elapsed since the beginning of the month
  //

  dDouble += ((DOUBLE)lpSystemTime->wDay) - 1.0;

  //
  // How many hours have elapsed since the beginning of the day
  //

  dDouble += ((DOUBLE) lpSystemTime->wHour) / 24;

  //
  // Save the dDouble value
  //

  *lpDate = dDouble;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

DWORD StrLen(LPCWSTR wszStr)
{
  FUNCTION(" StrLen(LPCWSTR wszStr)");

  DWORD dwIndex;

  dwIndex = 0;
  while (wszStr[dwIndex] != 0)
  {
    dwIndex++;
  }

  return dwIndex;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

DWORD StrLen(BSTR strStr)
{
  FUNCTION(" StrLen(BSTR strStr)");

  return SysStringLen(strStr);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

CAppEntry::CAppEntry(void)
{
  DPFCONSTRUCTOR("CAppEntry()");

  m_fInitialized = FALSE;

  if (SUCCEEDED(CoInitialize(NULL)))
	{
		if (SUCCEEDED(CoCreateInstance(CLSID_ApplicationManager, NULL, CLSCTX_INPROC_SERVER, IID_ApplicationManager, (LPVOID *) &m_IApplicationManager)))
    {
      if (SUCCEEDED(m_IApplicationManager->CreateApplicationEntry(&m_IApplicationEntry)))
      {
        m_fInitialized = TRUE;
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

CAppEntry::~CAppEntry(void)
{
  DPFDESTRUCTOR("~CAppEntry()");

  m_IApplicationEntry->Release();
  m_IApplicationManager->Release();
  m_fInitialized = FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::SetPropertyGUID(DWORD dwProperty, BSTR strGuid)
{
  FUNCTION("CAppEntry::SetPropertyGUID()");

  HRESULT hResult = APPMAN_E_NOTINITIALIZED;
  GUID    sGuid;

  //
  // Make sure the IApplicationManager interface was successfully instantiated
  //

  if (TRUE == m_fInitialized)
  {
    hResult = CLSIDFromString(strGuid, &sGuid);
    if (SUCCEEDED(hResult))
    {
      hResult = m_IApplicationEntry->SetProperty(dwProperty, &sGuid, sizeof(sGuid));
    }
  }

	return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::GetPropertyGUID(DWORD dwProperty, BSTR * lpstrGuid)
{
  FUNCTION("CAppEntry::GetPropertyGUID()");

  HRESULT   hResult = APPMAN_E_NOTINITIALIZED;
  OLECHAR * lpszString;
  GUID      sGuid;

  //
  // Make sure the IApplicationManager interface was successfully instantiated
  //

  if (TRUE == m_fInitialized)
  {
    hResult = m_IApplicationEntry->GetProperty(dwProperty, &sGuid, sizeof(sGuid));
    if (SUCCEEDED(hResult))
    {
      hResult = StringFromCLSID(sGuid, &lpszString);
      if (SUCCEEDED(hResult))
      {
        *lpstrGuid = SysAllocString(lpszString);
        CoTaskMemFree((LPVOID) lpszString);

        //
        // Make sure *lpstrGuid is not NULL
        //

        if (NULL == *lpstrGuid)
        {
          hResult = E_OUTOFMEMORY;
        }
      }
    }
  }

	return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::SetPropertyDWORD(DWORD dwProperty, long lDword)
{
  FUNCTION("CAppEntry::SetPropertyDWORD()");

  HRESULT hResult = APPMAN_E_NOTINITIALIZED;

  //
  // Make sure the IApplicationManager interface was successfully instantiated
  //

  if (TRUE == m_fInitialized)
  {
    hResult = m_IApplicationEntry->SetProperty(dwProperty, &lDword, sizeof(lDword));
  }

  return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::GetPropertyDWORD(DWORD dwProperty, long * lplDword)
{
  FUNCTION("CAppEntry::GetPropertyDWORD()");

  HRESULT hResult = APPMAN_E_NOTINITIALIZED;

  //
  // Make sure the IApplicationManager interface was successfully instantiated
  //

  if (TRUE == m_fInitialized)
  {
    hResult = m_IApplicationEntry->GetProperty(dwProperty, lplDword, sizeof(long));
  }

  return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::SetPropertyWSTR(DWORD dwProperty, BSTR strString)
{
  FUNCTION("CAppEntry::SetPropertyWSTR()");

  HRESULT hResult = APPMAN_E_NOTINITIALIZED;

  //
  // Make sure the IApplicationManager interface was successfully instantiated
  //

  if (TRUE == m_fInitialized)
  {
    hResult = m_IApplicationEntry->SetProperty(dwProperty | APP_PROPERTY_STR_UNICODE, strString, StrLen(strString) * 2);
  }

	return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::GetPropertyWSTR(DWORD dwProperty, BSTR * lpstrString)
{
  FUNCTION("CAppEntry::GetPropertyWSTR()");

  HRESULT hResult = APPMAN_E_NOTINITIALIZED;
  OLECHAR lpszString[MAX_PATH+1];

  //
  // Make sure the IApplicationManager interface was successfully instantiated
  //

  if (TRUE == m_fInitialized)
  {
    ZeroMemory(lpszString, sizeof(lpszString));
    hResult = m_IApplicationEntry->GetProperty(dwProperty | APP_PROPERTY_STR_UNICODE, lpszString, sizeof(lpszString));
    if (SUCCEEDED(hResult))
    {
      *lpstrString = SysAllocString(lpszString);
    }
  }

	return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_Guid(BSTR * lpstrGuid)
{
  FUNCTION("CAppEntry::get_Guid()");

  return GetPropertyGUID(APP_PROPERTY_GUID, lpstrGuid);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_Guid(BSTR strGuid)
{
  FUNCTION("CAppEntry::put_Guid()");

  return SetPropertyGUID(APP_PROPERTY_GUID, strGuid);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_CompanyName(BSTR * lpstrCompanyName)
{
  FUNCTION("CAppEntry::get_CompanyName()");

  return GetPropertyWSTR(APP_PROPERTY_COMPANYNAME, lpstrCompanyName);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_CompanyName(BSTR strCompanyName)
{
  FUNCTION("CAppEntry::put_CompanyName()");

  return SetPropertyWSTR(APP_PROPERTY_COMPANYNAME, strCompanyName);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_Signature(BSTR * lpstrSignature)
{
  FUNCTION("CAppEntry::get_Signature()");

  return GetPropertyWSTR(APP_PROPERTY_SIGNATURE, lpstrSignature);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_Signature(BSTR strSignature)
{
  FUNCTION("CAppEntry::put_Signature()");

  return SetPropertyWSTR(APP_PROPERTY_SIGNATURE, strSignature);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_VersionString(BSTR * lpstrVersionString)
{
  FUNCTION("CAppEntry::get_VersionString()");

  return GetPropertyWSTR(APP_PROPERTY_VERSIONSTRING, lpstrVersionString);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_VersionString(BSTR strVersionString)
{
  FUNCTION("CAppEntry::put_VersionString()");

  return SetPropertyWSTR(APP_PROPERTY_VERSIONSTRING, strVersionString);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_LastUsedDate(DATE * lpDate)
{
  FUNCTION("CAppEntry::get_LastUsedDate()");

  SYSTEMTIME  sSystemTime;
  HRESULT     hResult;

  hResult = m_IApplicationEntry->GetProperty(APP_PROPERTY_LASTUSEDDATE, &sSystemTime, sizeof(sSystemTime));
  if (SUCCEEDED(hResult))
  {
    SystemTimeToDate(&sSystemTime, lpDate);
  }

  return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_InstallDate(DATE * lpDate)
{
  FUNCTION("CAppEntry::get_InstallDate()");

  SYSTEMTIME  sSystemTime;
  HRESULT     hResult;

  hResult = m_IApplicationEntry->GetProperty(APP_PROPERTY_INSTALLDATE, &sSystemTime, sizeof(sSystemTime));
  if (SUCCEEDED(hResult))
  {
    SystemTimeToDate(&sSystemTime, lpDate);
  }

  return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_Category(long * lplCategory)
{
  FUNCTION("CAppEntry::get_Category()");

  return GetPropertyDWORD(APP_PROPERTY_CATEGORY, lplCategory);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_Category(long lCategory)
{
  FUNCTION("CAppEntry::put_Category()");

  return SetPropertyDWORD(APP_PROPERTY_CATEGORY, lCategory);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_State(long * lplState)
{
  FUNCTION("CAppEntry::get_State()");

  return GetPropertyDWORD(APP_PROPERTY_STATE, lplState);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_State(long lState)
{
  FUNCTION("CAppEntry::put_State()");

  return SetPropertyDWORD(APP_PROPERTY_STATE, lState);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_SetupRootPath(BSTR * lpstrSetupRootPath)
{
  FUNCTION("CAppEntry::get_SetupRootPath()");

  return GetPropertyWSTR(APP_PROPERTY_SETUPROOTPATH, lpstrSetupRootPath);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_ApplicationRootPath(BSTR * lpstrApplicationRootPath)
{
  FUNCTION("CAppEntry::get_ApplicationRootPath()");

  return GetPropertyWSTR(APP_PROPERTY_ROOTPATH, lpstrApplicationRootPath);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_ApplicationRootPath(BSTR strApplicationRootPath)
{
  FUNCTION("CAppEntry::put_ApplicationRootPath()");

  return SetPropertyWSTR(APP_PROPERTY_ROOTPATH, strApplicationRootPath);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_EstimatedInstallKilobytes(long * lplKilobytes)
{
  FUNCTION("CAppEntry::get_EstimatedInstallKilobytes()");

  return GetPropertyDWORD(APP_PROPERTY_ESTIMATEDINSTALLKILOBYTES, lplKilobytes);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_EstimatedInstallKilobytes(long lKilobytes)
{
  FUNCTION("CAppEntry::put_EstimatedInstallKilobytes()");

  return SetPropertyDWORD(APP_PROPERTY_ESTIMATEDINSTALLKILOBYTES, lKilobytes);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_ExecuteCmdLine(BSTR * lpstrExecuteCmdLine)
{
  FUNCTION("CAppEntry::get_ExecuteCmdLine()");

  return GetPropertyWSTR(APP_PROPERTY_EXECUTECMDLINE, lpstrExecuteCmdLine);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_ExecuteCmdLine(BSTR strExecuteCmdLine)
{
  FUNCTION("CAppEntry::put_ExecuteCmdLine()");

  return SetPropertyWSTR(APP_PROPERTY_EXECUTECMDLINE, strExecuteCmdLine);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_DefaultSetupExeCmdLine(BSTR * lpstrDefaultSetupExeCmdLine)
{
  FUNCTION("CAppEntry::get_DefaultSetupExeCmdLine()");

  return GetPropertyWSTR(APP_PROPERTY_DEFAULTSETUPEXECMDLINE, lpstrDefaultSetupExeCmdLine);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_DefaultSetupExeCmdLine(BSTR strDefaultSetupExeCmdLine)
{
  FUNCTION("CAppEntry::put_DefaultSetupExeCmdLine()");

  return SetPropertyWSTR(APP_PROPERTY_DEFAULTSETUPEXECMDLINE, strDefaultSetupExeCmdLine);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_DownsizeCmdLine(BSTR * lpstrDownsizeCmdLine)
{
  FUNCTION("CAppEntry::get_DownsizeCmdLine()");

  return GetPropertyWSTR(APP_PROPERTY_DOWNSIZECMDLINE, lpstrDownsizeCmdLine);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_DownsizeCmdLine(BSTR strDownsizeCmdLine)
{
  FUNCTION("CAppEntry::put_DownsizeCmdLine()");

  return SetPropertyWSTR(APP_PROPERTY_DOWNSIZECMDLINE, strDownsizeCmdLine);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_ReInstallCmdLine(BSTR * lpstrReInstallCmdLine)
{
  FUNCTION("CAppEntry::get_ReInstallCmdLine()");

  return GetPropertyWSTR(APP_PROPERTY_REINSTALLCMDLINE, lpstrReInstallCmdLine);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_ReInstallCmdLine(BSTR strReInstallCmdLine)
{
  FUNCTION("CAppEntry::put_ReInstallCmdLine()");

  return SetPropertyWSTR(APP_PROPERTY_REINSTALLCMDLINE, strReInstallCmdLine);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_UnInstallCmdLine(BSTR * lpstrUnInstallCmdLine)
{
  FUNCTION("CAppEntry::get_UnInstallCmdLine()");

  return GetPropertyWSTR(APP_PROPERTY_UNINSTALLCMDLINE, lpstrUnInstallCmdLine);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_UnInstallCmdLine(BSTR strUnInstallCmdLine)
{
  FUNCTION("CAppEntry::put_UnInstallCmdLine()");

  return SetPropertyWSTR(APP_PROPERTY_UNINSTALLCMDLINE, strUnInstallCmdLine);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_SelfTestCmdLine(BSTR * lpstrSelfTestCmdLine)
{
  FUNCTION("CAppEntry::get_SelfTestCmdLine()");

  return GetPropertyWSTR(APP_PROPERTY_SELFTESTCMDLINE, lpstrSelfTestCmdLine);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_SelfTestCmdLine(BSTR strSelfTestCmdLine)
{
  FUNCTION("CAppEntry::put_SelfTestCmdLine()");

  return SetPropertyWSTR(APP_PROPERTY_SELFTESTCMDLINE, strSelfTestCmdLine);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_TitleURL(BSTR * lpstrTitleURL)
{
  FUNCTION("CAppEntry::get_TitleURL()");

  return GetPropertyWSTR(APP_PROPERTY_TITLEURL, lpstrTitleURL);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_TitleURL(BSTR strTitleURL)
{
  FUNCTION("CAppEntry::put_TitleURL()");

  return SetPropertyWSTR(APP_PROPERTY_TITLEURL, strTitleURL);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_DeveloperURL(BSTR * lpstrDeveloperURL)
{
  FUNCTION("CAppEntry::get_DeveloperURL()");

  return GetPropertyWSTR(APP_PROPERTY_DEVELOPERURL, lpstrDeveloperURL);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_DeveloperURL(BSTR strDeveloperURL)
{
  FUNCTION("CAppEntry::put_DeveloperURL()");

  return SetPropertyWSTR(APP_PROPERTY_DEVELOPERURL, strDeveloperURL);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_PublisherURL(BSTR * lpstrPublisherURL)
{
  FUNCTION("CAppEntry::get_PublisherURL()");

  return GetPropertyWSTR(APP_PROPERTY_PUBLISHERURL, lpstrPublisherURL);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_PublisherURL(BSTR strPublisherURL)
{
  FUNCTION("CAppEntry::put_PublisherURL()");

  return SetPropertyWSTR(APP_PROPERTY_PUBLISHERURL, strPublisherURL);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_XMLInfoFile(BSTR * lpstrXMLInfoFile)
{
  FUNCTION("CAppEntry::get_XMLInfoFile()");

  return GetPropertyWSTR(APP_PROPERTY_XMLINFOFILE, lpstrXMLInfoFile);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_XMLInfoFile(BSTR strXMLInfoFile)
{
  FUNCTION("CAppEntry::put_XMLInfoFile()");

  return SetPropertyWSTR(APP_PROPERTY_XMLINFOFILE, strXMLInfoFile);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::Clear()
{
  FUNCTION("CAppEntry::Clear()");

  return m_IApplicationEntry->Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::InitializeInstall(void)
{
  FUNCTION("CAppEntry::InitializeInstall()");

  return m_IApplicationEntry->InitializeInstall();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::FinalizeInstall(void)
{
  FUNCTION("CAppEntry::FinalizeInstall()");

  return m_IApplicationEntry->FinalizeInstall();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::InitializeDownsize(void)
{
  FUNCTION("CAppEntry::InitializeDownsize()");

  return m_IApplicationEntry->InitializeDownsize();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::FinalizeDownsize(void)
{
  FUNCTION("CAppEntry::FinalizeDownsize()");

  return m_IApplicationEntry->FinalizeDownsize();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::InitializeReInstall(void)
{
  FUNCTION("CAppEntry::InitializeReInstall()");

  return m_IApplicationEntry->InitializeReInstall();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::FinalizeReInstall(void)
{
  FUNCTION("CAppEntry::FinalizeReInstall()");

  return m_IApplicationEntry->FinalizeReInstall();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::InitializeUnInstall(void)
{
  FUNCTION("CAppEntry::InitializeUnInstall()");

  return m_IApplicationEntry->InitializeUnInstall();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::FinalizeUnInstall(void)
{
  FUNCTION("CAppEntry::FinalizeUnUnInstall()");

  return m_IApplicationEntry->FinalizeUnInstall();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::InitializeSelfTest(void)
{
  FUNCTION("CAppEntry::InitializeSelfTest()");

  return m_IApplicationEntry->InitializeSelfTest();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::FinalizeSelfTest(void)
{
  FUNCTION("CAppEntry::FinalizeSelfTest()");

  return m_IApplicationEntry->FinalizeSelfTest();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::Abort(void)
{
  FUNCTION("CAppEntry::Abort()");

  return m_IApplicationEntry->Abort();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::Run(long lRunFlags, BSTR strCmdLineParameters)
{
  FUNCTION("CAppEntry::Run()");

  return m_IApplicationEntry->Run((const DWORD) lRunFlags, APP_PROPERTY_STR_UNICODE, (LPVOID) strCmdLineParameters, StrLen(strCmdLineParameters));
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::AddAssociation(long lAssociationType, IAppEntry * lpAppEntry)
{
  FUNCTION("CAppEntry::AddAssociation()");

  IApplicationEntry * lpApplicationEntry;
  HRESULT             hResult = APPMAN_E_NOTINITIALIZED;

  if (TRUE == m_fInitialized)
  {
    ((CAppEntry *)lpAppEntry)->GetApplicationEntryPtr(&lpApplicationEntry);
    hResult = m_IApplicationEntry->AddAssociation(lAssociationType, lpApplicationEntry);
  }

  return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::RemoveAssociation(long lAssociationType, IAppEntry * lpAppEntry)
{
  FUNCTION("CAppEntry::RemoveAssociation()");

  IApplicationEntry * lpApplicationEntry;
  HRESULT             hResult = APPMAN_E_NOTINITIALIZED;

  if (TRUE == m_fInitialized)
  {
    ((CAppEntry *)lpAppEntry)->GetApplicationEntryPtr(&lpApplicationEntry);
    hResult = m_IApplicationEntry->RemoveAssociation(lAssociationType, lpApplicationEntry);
  }

  return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::EnumAssociationTypes(long lAssociationIndex, long * lAssociationType)
{
  FUNCTION("CAppEntry::EnumAssociationTypes()");

  IApplicationEntry * lpApplicationEntry;
  HRESULT             hResult = APPMAN_E_NOTINITIALIZED;

  if (TRUE == m_fInitialized)
  {
    hResult = m_IApplicationManager->CreateApplicationEntry(&lpApplicationEntry);
    if (SUCCEEDED(hResult))
    {
      hResult = m_IApplicationEntry->EnumAssociations(lAssociationIndex, (LPDWORD) lAssociationType, lpApplicationEntry);
      lpApplicationEntry->Release();
    }
  }

  return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::EnumAssociationObjects(long lAssociationIndex, IAppEntry * lpAppEntry)
{
  FUNCTION("CAppEntry::EnumAssociationObjects()");

  IApplicationEntry * lpApplicationEntry;
  HRESULT             hResult = APPMAN_E_NOTINITIALIZED;
  DWORD               dwAssociationType;

  if (TRUE == m_fInitialized)
  {
    ((CAppEntry *)lpAppEntry)->GetApplicationEntryPtr(&lpApplicationEntry);
    hResult = m_IApplicationEntry->EnumAssociations(lAssociationIndex, &dwAssociationType, lpApplicationEntry);
  }

  return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::GetTemporarySpace(long lKilobytesRequired, BSTR * lpstrRootPath)
{
  FUNCTION("CAppEntry::GetTemporarySpace()");

  HRESULT   hResult = E_FAIL;
  OLECHAR   szString[MAX_PATH+1];

  //
  // Make sure the IApplicationManager interface was successfully instantiated
  //

  if (TRUE == m_fInitialized)
  {
    ZeroMemory(szString, sizeof(szString));
    hResult = m_IApplicationEntry->GetTemporarySpace(lKilobytesRequired, APP_PROPERTY_STR_UNICODE, (LPVOID) szString, sizeof(szString));
    if (SUCCEEDED(hResult))
    {
      *lpstrRootPath = SysAllocString(szString);
    }
  }

  return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::RemoveTemporarySpace(BSTR strRootPath)
{
  FUNCTION("CAppEntry::RemoveTemporarySpace()");

  HRESULT   hResult = APPMAN_E_NOTINITIALIZED;

  if (TRUE == m_fInitialized)
  {
    hResult = m_IApplicationEntry->RemoveTemporarySpace(APP_PROPERTY_STR_UNICODE, (LPVOID) strRootPath, StrLen(strRootPath));
  }

  return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::EnumTemporarySpacePaths(long lTempSpaceIndex, BSTR * lpstrRootPath)
{
  FUNCTION("CAppEntry::EnumTemporarySpacePaths()");

  HRESULT   hResult = APPMAN_E_NOTINITIALIZED;
  OLECHAR   szString[MAX_PATH+1];
  DWORD     dwKilobytes;

  if (TRUE == m_fInitialized)
  {
    ZeroMemory(szString, sizeof(szString));
    hResult = m_IApplicationEntry->EnumTemporarySpaces(lTempSpaceIndex, &dwKilobytes, APP_PROPERTY_STR_UNICODE, (LPVOID) szString, sizeof(szString));
    if (SUCCEEDED(hResult))
    {
      *lpstrRootPath = SysAllocString(szString);
    }
  }

  return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::EnumTemporarySpaceAllocations(long lTempSpaceIndex, long * lTempSpaceKilobytes)
{
  FUNCTION("CAppEntry::EnumTemporarySpaceAllocations()");

  HRESULT   hResult = APPMAN_E_NOTINITIALIZED;
  OLECHAR   szString[MAX_PATH+1];

  if (TRUE == m_fInitialized)
  {
    hResult = m_IApplicationEntry->EnumTemporarySpaces(lTempSpaceIndex, (LPDWORD) lTempSpaceKilobytes, APP_PROPERTY_STR_UNICODE, (LPVOID) szString, sizeof(szString));
  }

  return hResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_RemovableKilobytes(long * lplKilobytes)
{
  FUNCTION("CAppEntry::get_RemovableKilobytes()");

  return GetPropertyDWORD(APP_PROPERTY_REMOVABLEKILOBYTES, lplKilobytes);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_RemovableKilobytes(long lKilobytes)
{
  FUNCTION("CAppEntry::put_RemovableKilobytes()");

  return SetPropertyDWORD(APP_PROPERTY_REMOVABLEKILOBYTES, lKilobytes);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::get_NonRemovableKilobytes(long * lplKilobytes)
{
  FUNCTION("CAppEntry::get_NonRemovableKilobytes()");

  return GetPropertyDWORD(APP_PROPERTY_NONREMOVABLEKILOBYTES, lplKilobytes);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::put_NonRemovableKilobytes(long lKilobytes)
{
  FUNCTION("CAppEntry::put_NonRemovableKilobytes()");

  return SetPropertyDWORD(APP_PROPERTY_NONREMOVABLEKILOBYTES, lKilobytes);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::Initialize(void)
{
  FUNCTION("CAppEntry::Initialize()");

  if (m_fInitialized)
  {
    return S_OK;
  }

  return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAppEntry::GetApplicationEntryPtr(IApplicationEntry ** lppApplicationEntry)
{
  FUNCTION("CAppEntry::GetApplicationEntryPtr()");

  if (!m_fInitialized)
  {
    return E_FAIL;
  }

  *lppApplicationEntry = m_IApplicationEntry;
  
  return S_OK;
}
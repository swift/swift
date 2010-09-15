// About:
//   Snarl C++ interface implementation
//   To understand what the different functions do and what they return, please
//   have a look at the API on http://www.fullphat.net/dev/api.htm.
//   Also have a look at mSnarl_i.bas found in the CVS/SVN repository for Snarl.
//
//   The functions in SnarlInterface both have ANSI(UTF8) and UNICODE versions.
//   If the LPCWSTR (unicode) version of the functions are called, the strings
//   are converted to UTF8 by SnarlInterface before sent to Snarl. So using the
//   ANSI/UTF8/LPCSTR versions of the functions are faster!
//   
//
// Difference to VB implementation:
//   Please note that string functions return NULL when they fail and not an
//   empty string. So check for NULL...
//   Function names doesn't have the pre "sn".
//
// 
// Authors:
//   Written and maintained by Toke Noer Nøttrup
//   Original C++ version by "Whitman"
//
// License etc. :
//   Feel free to use this code and class as you see fit.
//   If you improve the code, it would be nice of you to take contact to the
//   authors, so all can get the benifit.
//
//   There is no guarantee that the code is correct or functional.
//   USE AT YOUR OWN RISK
//-----------------------------------------------------------------------------

// History
//  2008/12/31 : Implemented V39 API
//             : Moved SnarlInterface into new Snarl namespace and moved enums etc. out of class
//             : Added WCHAR overloads for all functions
//  2008/08/27 : Fixed return value of IsMessageVisible and HideMessage (returns false on failure now)
//             : Fixed critical error in the new overloaded UpdateMessage() function
//  2008/08/27 : x64 compiler fix
//  2008/08/24 : Renamed all functions to not have prepended "sn".
//             : Memory allocation functions added. (Use FreeString to free strings returned by Snarl)
//             : Added m_nLastMessageId to the class. (Use GetLastMessageId() to get it)
//             : Overloaded a few functions, so one don't have include the message id. (UpdateMessage f.i.)

//  2008/06/20 : Fixed snShowMessageEx so it actually sends the extended version - oops
//             : Fixed compiler warning C4800: forcing value to bool 'true' or 'false' (performance warning)

//  2008/05/19 : uSend and uSendEx would always set return value to M_OK on successfull call
//  2008/04/14 : Updated to follow (what should be) the final Snarl 2.0 API
//  2008/03/28 : Few fixes for Snarl 2.0
//  2007/05/23 : snGetGlobalMsg & snGetSnarlWindow made static
//  2007/03/25 : 1.6 RC1 fixup
//  2007/03/04 : Added - snGetAppPath, snGetIconsPath, snGetVersionEx, 
//                       snSetTimeout, uSendEx


#include "SnarlInterface.h"

using namespace Snarl;


//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------
SnarlInterface::SnarlInterface()
: m_hwndFrom(NULL), m_nLastMessageId(0)
{

}

SnarlInterface::~SnarlInterface()
{

}


//-----------------------------------------------------------------------------
// snShowMessage()

/// Displays a message with Title and Text. Timeout controls how long the
/// message is displayed for (in seconds) (omitting this value means the message
/// is displayed indefinately). IconPath specifies the location of a PNG image
/// which will be displayed alongside the message text.
/// <returns>Message Id on success or M_RESULT on failure</returns>

LONG32 SnarlInterface::ShowMessage(LPCSTR szTitle, LPCSTR szText, LONG32 timeout, LPCSTR szIconPath, HWND hWndReply, WPARAM uReplyMsg)
{
	SNARLSTRUCT ss;
	ZeroMemory((void*)&ss, sizeof(ss));

	ss.Cmd = SNARL_SHOW;
	StringCbCopyA((LPSTR)&ss.Title, SNARL_STRING_LENGTH, szTitle);
	StringCbCopyA((LPSTR)&ss.Text,  SNARL_STRING_LENGTH, szText);
	StringCbCopyA((LPSTR)&ss.Icon,  SNARL_STRING_LENGTH, szIconPath);
	ss.Timeout = timeout;

	ss.LngData2 = reinterpret_cast<LONG32>(hWndReply);
	ss.Id = static_cast<LONG32>(uReplyMsg);

	m_nLastMessageId = Send(ss);
	return m_nLastMessageId;
}

LONG32 SnarlInterface::ShowMessage(LPCWSTR szTitle, LPCWSTR szText, LONG32 timeout, LPCWSTR szIconPath, HWND hWndReply, WPARAM uReplyMsg)
{
	LPSTR szUTF8Title = WideToUTF8(szTitle);
	LPSTR szUTF8Text  = WideToUTF8(szText);
	LPSTR szUFT8IconPath = WideToUTF8(szIconPath);
	
	LONG32 result = ShowMessage(szUTF8Title, szUTF8Text, timeout, szUFT8IconPath, hWndReply, uReplyMsg);
	
	delete [] szUTF8Title;
	delete [] szUTF8Text;
	delete [] szUFT8IconPath;
	
	return result;
}
	
//-----------------------------------------------------------------------------
// snShowMessageEx()

/// Displays a notification. This function is identical to snShowMessage()
/// except that Class specifies an alert previously registered with
/// snRegisterAlert() and SoundFile can optionally specify a WAV sound to play
/// when the notification is displayed on screen.

/// <returns>Message Id on success or M_RESULT on failure</returns>

LONG32 SnarlInterface::ShowMessageEx(LPCSTR szClass, LPCSTR szTitle, LPCSTR szText, LONG32 timeout, LPCSTR szIconPath, HWND hWndReply, WPARAM uReplyMsg, LPCSTR szSoundFile)
{
	SNARLSTRUCTEX ssex;
	ZeroMemory((void*)&ssex, sizeof(ssex));

	ssex.Cmd = SNARL_EX_SHOW;
	ssex.Timeout = timeout;
	ssex.LngData2 = reinterpret_cast<LONG32>(hWndReply);
	ssex.Id = static_cast<LONG32>(uReplyMsg);

	StringCbCopyA((LPSTR)&ssex.Class, SNARL_STRING_LENGTH, szClass);
	StringCbCopyA((LPSTR)&ssex.Title, SNARL_STRING_LENGTH, szTitle);
	StringCbCopyA((LPSTR)&ssex.Text,  SNARL_STRING_LENGTH, szText);
	StringCbCopyA((LPSTR)&ssex.Icon,  SNARL_STRING_LENGTH, szIconPath);
	StringCbCopyA((LPSTR)&ssex.Extra, SNARL_STRING_LENGTH, szSoundFile);

	m_nLastMessageId = Send(ssex);
	return m_nLastMessageId;
}

LONG32 SnarlInterface::ShowMessageEx(LPCWSTR szClass, LPCWSTR szTitle, LPCWSTR szText, LONG32 timeout, LPCWSTR szIconPath, HWND hWndReply, WPARAM uReplyMsg, LPCWSTR szSoundFile)
{
	LPSTR szUTF8Class = WideToUTF8(szClass);
	LPSTR szUTF8Title = WideToUTF8(szTitle);
	LPSTR szUTF8Text  = WideToUTF8(szText);
	LPSTR szUFT8IconPath = WideToUTF8(szIconPath);
	LPSTR szUFT8SoundFile = WideToUTF8(szSoundFile);
	
	LONG32 result = ShowMessageEx(szUTF8Class, szUTF8Title, szUTF8Text, timeout, szUFT8IconPath, hWndReply, uReplyMsg, szUFT8SoundFile);
	
	delete [] szUTF8Class;
	delete [] szUTF8Title;
	delete [] szUTF8Text;
	delete [] szUFT8IconPath;
	delete [] szUFT8SoundFile;
	
	return result;
}

//-----------------------------------------------------------------------------
// snHideMessage()

/// Hides the notification specified by Id. Id is the value returned by
/// snShowMessage() or snShowMessageEx() when the notification was initially
/// created. This function returns True if the notification was successfully
/// hidden or False otherwise (for example, the notification may no longer exist).

BOOL SnarlInterface::HideMessage(LONG32 Id)
{
	SNARLSTRUCT ss;
	ss.Cmd = SNARL_HIDE;
	ss.Id = Id;

	LONG32 n = Send(ss);
	return (n == -1 || n == 1) ? TRUE : FALSE;
}

BOOL SnarlInterface::HideMessage()
{
	return HideMessage(m_nLastMessageId);
}

//-----------------------------------------------------------------------------
// snIsMessageVisible()

/// Returns True if the notification specified by Id is still visible, or
/// False if not. Id is the value returned by snShowMessage() or
/// snShowMessageEx() when the notification was initially created.

BOOL SnarlInterface::IsMessageVisible(LONG32 Id)
{
	SNARLSTRUCT ss;
	ss.Cmd = SNARL_IS_VISIBLE;
	ss.Id = Id;

	// We are getting -1 when true, checking for 1 just in case. We don't want to return true for the other M_RESULT returns
	LONG32 n = Send(ss);	
	return (n == -1 || n == 1) ? TRUE : FALSE;
}

BOOL SnarlInterface::IsMessageVisible()
{
	if (m_nLastMessageId == 0)
		return FALSE;

	return IsMessageVisible(m_nLastMessageId);
}

//-----------------------------------------------------------------------------
// snUpdateMessage()

/// Changes the title and text in the message specified by Id to the values
/// specified by Title and Text respectively. Id is the value returned by 
/// snShowMessage() or snShowMessageEx() when the notification was originally
/// created. To change the timeout parameter of a notification, use snSetTimeout()

M_RESULT SnarlInterface::UpdateMessage(LONG32 id, LPCSTR szTitle, LPCSTR szText, LPCSTR szIconPath)
{
	SNARLSTRUCT ss;
	ZeroMemory((void*)&ss, sizeof(ss));

	ss.Cmd = SNARL_UPDATE;
	ss.Id = id;
	
	StringCbCopyA((LPSTR)&ss.Title, SNARL_STRING_LENGTH, szTitle);
	StringCbCopyA((LPSTR)&ss.Text,  SNARL_STRING_LENGTH, szText);
	StringCbCopyA((LPSTR)&ss.Icon,  SNARL_STRING_LENGTH, szIconPath);

	return static_cast<M_RESULT>(Send(ss));
}

M_RESULT SnarlInterface::UpdateMessage(LONG32 id, LPCWSTR szTitle, LPCWSTR szText, LPCWSTR szIconPath)
{
	LPSTR szParam1 = WideToUTF8(szTitle);
	LPSTR szParam2 = WideToUTF8(szText);
	LPSTR szParam3 = WideToUTF8(szIconPath);
	
	M_RESULT result = UpdateMessage(id, szParam1, szParam2, szParam3);
	
	delete [] szParam1;
	delete [] szParam2;
	delete [] szParam3;
	
	return result;
}

M_RESULT SnarlInterface::UpdateMessage(LPCSTR szTitle, LPCSTR szText, LPCSTR szIconPath)
{
	return UpdateMessage(m_nLastMessageId, szTitle, szText, szIconPath);
}

M_RESULT SnarlInterface::UpdateMessage(LPCWSTR szTitle, LPCWSTR szText, LPCWSTR szIconPath)
{
	return UpdateMessage(m_nLastMessageId, szTitle, szText, szIconPath);
}

//-----------------------------------------------------------------------------
// snRegisterConfig

/// Registers an application's configuration interface with Snarl.
/// AppName is the text that's displayed in the Applications list so it should
/// be people friendly ("My cool app" rather than "my_cool_app").

M_RESULT SnarlInterface::RegisterConfig(HWND hWnd, LPCSTR szAppName, LONG32 replyMsg)
{
	return RegisterConfig2(hWnd, szAppName, replyMsg, "");
}

M_RESULT SnarlInterface::RegisterConfig(HWND hWnd, LPCWSTR szAppName, LONG32 replyMsg)
{
	return RegisterConfig2(hWnd, szAppName, replyMsg, L"");
}

//-----------------------------------------------------------------------------
// snRegisterConfig2

/// Registers an application's configuration interface with Snarl.
/// This function is identical to snRegisterConfig() except that Icon can be
/// used to specify a PNG image which will be displayed against the
/// application's entry in Snarl's Preferences panel.

M_RESULT SnarlInterface::RegisterConfig2(HWND hWnd, LPCSTR szAppName, LONG32 replyMsg, LPCSTR szIcon)
{
	if (!szAppName || !szIcon)
		return M_BAD_POINTER;

	SNARLSTRUCT ss;

	m_hwndFrom = hWnd;

	ss.Cmd = SNARL_REGISTER_CONFIG_WINDOW_2;
	ss.LngData2 = reinterpret_cast<LONG32>(hWnd);
	ss.Id = replyMsg;
	StringCbCopyA((LPSTR)&ss.Title, SNARL_STRING_LENGTH, szAppName);
	StringCbCopyA((LPSTR)&ss.Icon, SNARL_STRING_LENGTH, szIcon);

	return static_cast<M_RESULT>(Send(ss));
}

M_RESULT SnarlInterface::RegisterConfig2(HWND hWnd, LPCWSTR szAppName, LONG32 replyMsg, LPCWSTR szIcon)
{
	LPSTR szParam1 = WideToUTF8(szAppName);
	LPSTR szParam2 = WideToUTF8(szIcon);
	
	M_RESULT result = RegisterConfig2(hWnd, szParam1, replyMsg, szParam2);
	
	delete [] szParam1;
	delete [] szParam2;
	
	return result;
}


//-----------------------------------------------------------------------------
// snRevokeConfig

/// Removes the application previously registered using snRegisterConfig() or
/// snRegisterConfig2(). hWnd should be the same as that used during registration.

M_RESULT SnarlInterface::RevokeConfig(HWND hWnd)
{
	SNARLSTRUCT ss;
	
	m_hwndFrom = NULL;

	ss.Cmd = SNARL_REVOKE_CONFIG_WINDOW;
	ss.LngData2 = reinterpret_cast<LONG32>(hWnd);

	return static_cast<M_RESULT>(Send(ss));
}


//-----------------------------------------------------------------------------
// snGetVersion()

/// Checks if Snarl is currently running and, if it is, retrieves the major and
/// minor release version numbers in Major and Minor respectively.
/// Returns True if Snarl is running, False otherwise.

BOOL SnarlInterface::GetVersion(WORD* Major, WORD* Minor)
{
	SNARLSTRUCT ss;
	ss.Cmd = SNARL_GET_VERSION;
	LONG32 versionInfo = Send(ss);
	if (versionInfo > 0 && versionInfo != M_FAILED && versionInfo != M_TIMED_OUT) {
		*Major = HIWORD(versionInfo);
		*Minor = LOWORD(versionInfo);
		return TRUE;
	}
	return FALSE;
}


//-----------------------------------------------------------------------------
// snGetVersionEx

/// Returns the Snarl system version number. This is an integer value which
/// represents the system build number and can be used to identify the specific
/// version of Snarl running

LONG32 SnarlInterface::GetVersionEx()
{
	SNARLSTRUCT ss;
	ss.Cmd = SNARL_GET_VERSION_EX;
	return Send(ss);
}


//-----------------------------------------------------------------------------
// snSetTimeout()

/// Sets the timeout of existing notification Id to Timeout seconds. Id is the
/// value returned by snShowMessage() or snShowMessageEx() when the notification
/// was first created. 

M_RESULT SnarlInterface::SetTimeout(LONG32 Id, LONG32 Timeout)
{
	SNARLSTRUCT ss;
	ss.Cmd = SNARL_SET_TIMEOUT;
	ss.Id = Id;
	ss.LngData2 = Timeout;

	return static_cast<M_RESULT>(Send(ss));
}

M_RESULT SnarlInterface::SetTimeout(LONG32 Timeout)
{
	return SetTimeout(m_nLastMessageId, Timeout);
}

//-----------------------------------------------------------------------------
// snRegisterAlert()

/// Registers an alert of Class for application AppName which must have previously
/// been registered with either snRegisterConfig() or snRegisterConfig2().

M_RESULT SnarlInterface::RegisterAlert(LPCSTR szAppName, LPCSTR szClass)
{
	SNARLSTRUCT ss;
	ss.Cmd = SNARL_REGISTER_ALERT;
	StringCbCopyA((LPSTR)&ss.Title, SNARL_STRING_LENGTH, szAppName);
	StringCbCopyA((LPSTR)&ss.Text, SNARL_STRING_LENGTH, szClass);

	return static_cast<M_RESULT>(Send(ss));
}

M_RESULT SnarlInterface::RegisterAlert(LPCWSTR szAppName, LPCWSTR szClass)
{
	LPSTR szParam1 = WideToUTF8(szAppName);
	LPSTR szParam2 = WideToUTF8(szClass);
	
	M_RESULT result = RegisterAlert(szParam1, szParam2);
	
	delete [] szParam1;
	delete [] szParam2;
	
	return result;
}

//-----------------------------------------------------------------------------
// snGetGlobalMsg()

/// Returns the atom that corresponds to the "SnarlGlobalEvent" registered
/// Windows message. This message is sent by Snarl when it is first starts and
/// when it shuts down.

LONG32 SnarlInterface::GetGlobalMsg()
{
	return RegisterWindowMessage(SNARL_GLOBAL_MSG);
}


//-----------------------------------------------------------------------------
// snGetSnarlWindow

HWND SnarlInterface::GetSnarlWindow()
{
	return FindWindow(NULL, _T("Snarl"));
}


//-----------------------------------------------------------------------------
// snGetAppPath()

/// Returns a pointer to the path.
/// ** Remember to call FreeString

LPCTSTR SnarlInterface::GetAppPath()
{
	HWND hWnd = GetSnarlWindow();
	if (hWnd)
	{
		HWND hWndPath = FindWindowEx(hWnd, 0, _T("static"), NULL);
		if (hWndPath)
		{
			TCHAR strTmp[MAX_PATH] = {0};
			int nReturn = GetWindowText(hWndPath, strTmp, MAX_PATH);
			if (nReturn > 0) {
				TCHAR* strReturn = AllocateString(nReturn + 1);
				StringCchCopy(strReturn, nReturn + 1, strTmp);
				return strReturn;
			}
		}
	}
	return NULL;
}


//-----------------------------------------------------------------------------
// snGetIconsPath()

/// Returns a pointer to the iconpath.
/// ** Remember to call FreeString when done with the string

LPCTSTR SnarlInterface::GetIconsPath()
{
	TCHAR* szIconPath = NULL;
	LPCTSTR szPath = GetAppPath();
	if (!szPath)
		return NULL;

	size_t nLen = 0;
	if (SUCCEEDED(StringCbLength(szPath, MAX_PATH, &nLen)))
	{
		nLen += 10 + 1; // etc\\icons\\ + NULL
		szIconPath = AllocateString(nLen);

		StringCbCopy(szIconPath, nLen * sizeof(TCHAR), szPath);
		StringCbCat(szIconPath, nLen * sizeof(TCHAR), _T("etc\\icons\\"));
	}
	
	FreeString(szPath);

	return szIconPath;
}


//-----------------------------------------------------------------------------
// snSetAsSnarlApp()

/// Identifies an application as a Snarl App.  (V39)

void SnarlInterface::SetAsSnarlApp(HWND hWndOwner, SNARL_APP_FLAGS Flags)
{
	if (IsWindow(hWndOwner)) {
		SetProp(hWndOwner, _T("snarl_app"), reinterpret_cast<HANDLE>(1));
		SetProp(hWndOwner, _T("snarl_app_flags"), reinterpret_cast<HANDLE>(Flags));
	}
}


//-----------------------------------------------------------------------------
// snGetAppMsg()

/// Returns the global Snarl Application message  (V39)

LONG32 SnarlInterface::GetAppMsg()
{
	return RegisterWindowMessage(SNARL_APP_MSG);
}


//-----------------------------------------------------------------------------
// snRegisterApp()

/// Registers an application with Snarl  (V39)

M_RESULT SnarlInterface::RegisterApp(LPCSTR Application, LPCSTR SmallIcon, LPCSTR LargeIcon, HWND hWnd, LONG32 ReplyMsg)
{
	m_hwndFrom = hWnd;
	  
    SNARLSTRUCT ss;
	ss.Cmd = SNARL_REGISTER_APP;
	
	StringCbCopyA((LPSTR)&ss.Title, SNARL_STRING_LENGTH, Application);
	StringCbCopyA((LPSTR)&ss.Icon,  SNARL_STRING_LENGTH, SmallIcon);
	StringCbCopyA((LPSTR)&ss.Text,  SNARL_STRING_LENGTH, LargeIcon);

	ss.LngData2 = reinterpret_cast<LONG32>(hWnd);
	ss.Id = ReplyMsg;
	ss.Timeout = GetCurrentProcessId();

	return static_cast<M_RESULT>(Send(ss));
}

M_RESULT SnarlInterface::RegisterApp(LPCWSTR Application, LPCWSTR SmallIcon, LPCWSTR LargeIcon, HWND hWnd, LONG32 ReplyMsg)
{
	LPSTR szParam1 = WideToUTF8(Application);
	LPSTR szParam2 = WideToUTF8(SmallIcon);
	LPSTR szParam3 = WideToUTF8(LargeIcon);
	
	M_RESULT result = RegisterApp(szParam1, szParam2, szParam3, hWnd, ReplyMsg);
	
	delete [] szParam1;
	delete [] szParam2;
	delete [] szParam3;
	
	return result;
}


//-----------------------------------------------------------------------------
// snUnregisterApp()

/// Unregisters an application with Snarl  (V39)

M_RESULT SnarlInterface::UnregisterApp()
{
	SNARLSTRUCT ss;
	ss.Cmd = SNARL_UNREGISTER_APP;
	ss.LngData2 = GetCurrentProcessId();
	
	m_hwndFrom = NULL;
	
	return static_cast<M_RESULT>(Send(ss));
}


//-----------------------------------------------------------------------------
// snShowNotification()

/// Displays a Snarl notification using registered class  (V39)
/// <returns>Message Id on success or M_RESULT on failure</returns>

LONG32 SnarlInterface::ShowNotification(LPCSTR Class, LPCSTR Title, LPCSTR Text, LONG32 Timeout, LPCSTR Icon, HWND hWndReply, LONG32 uReplyMsg, LPCSTR Sound)
{
	SNARLSTRUCTEX ssex;
	ssex.Cmd = SNARL_SHOW_NOTIFICATION;
	
	StringCbCopyExA((LPSTR)&ssex.Title, SNARL_STRING_LENGTH, Title, NULL, NULL, STRSAFE_IGNORE_NULLS);
	StringCbCopyExA((LPSTR)&ssex.Text,  SNARL_STRING_LENGTH, Text,  NULL, NULL, STRSAFE_IGNORE_NULLS);
	StringCbCopyExA((LPSTR)&ssex.Icon,  SNARL_STRING_LENGTH, Icon,  NULL, NULL, STRSAFE_IGNORE_NULLS);

	ssex.Timeout = Timeout;
	ssex.LngData2 = reinterpret_cast<LONG32>(hWndReply);
	ssex.Id = uReplyMsg;

	StringCbCopyExA((LPSTR)&ssex.Extra, SNARL_STRING_LENGTH, Sound, NULL, NULL, STRSAFE_IGNORE_NULLS);
	StringCbCopyA((LPSTR)&ssex.Class,  SNARL_STRING_LENGTH, Class);
    
	ssex.Reserved1 = GetCurrentProcessId();
	
	m_nLastMessageId = Send(ssex);
	return m_nLastMessageId;
}

LONG32 SnarlInterface::ShowNotification(LPCWSTR Class, LPCWSTR Title, LPCWSTR Text, LONG32 Timeout, LPCWSTR Icon, HWND hWndReply, LONG32 uReplyMsg, LPCWSTR Sound)
{
	LPSTR szParam1 = WideToUTF8(Class);
	LPSTR szParam2 = WideToUTF8(Title);
	LPSTR szParam3 = WideToUTF8(Text);
	LPSTR szParam4 = WideToUTF8(Icon);
	LPSTR szParam5 = WideToUTF8(Sound);
	
	LONG32 result = ShowNotification(szParam1, szParam2, szParam3, Timeout, szParam4, hWndReply, uReplyMsg, szParam5);
	
	delete [] szParam1;
	delete [] szParam2;
	delete [] szParam3;
	delete [] szParam4;
	delete [] szParam5;
	
	return result;
}


//-----------------------------------------------------------------------------
// snChangeAttribute()

/// (V39)

M_RESULT SnarlInterface::ChangeAttribute(LONG32 Id, SNARL_ATTRIBUTES Attr, LPCSTR Value)
{
	SNARLSTRUCT ss;
	ss.Cmd = SNARL_CHANGE_ATTR;
	ss.Id = Id;
	ss.LngData2 = Attr;
	
	StringCbCopyExA((LPSTR)&ss.Text, SNARL_STRING_LENGTH, Value, NULL, NULL, STRSAFE_IGNORE_NULLS);
	
	return static_cast<M_RESULT>(Send(ss));
}

M_RESULT SnarlInterface::ChangeAttribute(LONG32 Id, SNARL_ATTRIBUTES Attr, LPCWSTR Value)
{
	LPSTR szParam1 = WideToUTF8(Value);
	
	M_RESULT result = ChangeAttribute(Id, Attr, szParam1);
	
	delete [] szParam1;
	
	return result;
}

M_RESULT SnarlInterface::ChangeAttribute(SNARL_ATTRIBUTES Attr, LPCSTR Value)
{
	return ChangeAttribute(m_nLastMessageId, Attr, Value);
}

M_RESULT SnarlInterface::ChangeAttribute(SNARL_ATTRIBUTES Attr, LPCWSTR Value)
{
	return ChangeAttribute(m_nLastMessageId, Attr, Value);
}


//-----------------------------------------------------------------------------
// snSetClassDefault()

/// Sets the default value for an alert class  (V39)

M_RESULT SnarlInterface::SetClassDefault(LPCSTR Class, SNARL_ATTRIBUTES Attr, LPCSTR Value)
{
	SNARLSTRUCT ss;
	ss.Cmd = SNARL_SET_CLASS_DEFAULT;
	ss.LngData2 = Attr;
	ss.Timeout = GetCurrentProcessId();
	
	StringCbCopyExA((LPSTR)&ss.Text, SNARL_STRING_LENGTH, Class, NULL, NULL, STRSAFE_IGNORE_NULLS);
	StringCbCopyExA((LPSTR)&ss.Icon, SNARL_STRING_LENGTH, Value, NULL, NULL, STRSAFE_IGNORE_NULLS);

	return static_cast<M_RESULT>(Send(ss));
}

M_RESULT SnarlInterface::SetClassDefault(LPCWSTR Class, SNARL_ATTRIBUTES Attr, LPCWSTR Value)
{
	LPSTR szParam1 = WideToUTF8(Class);
	LPSTR szParam2 = WideToUTF8(Value);
	
	M_RESULT result = SetClassDefault(szParam1, Attr, szParam2);
	
	delete [] szParam1;
	delete [] szParam2;
	
	return result;
}


//-----------------------------------------------------------------------------
// snGetRevision()

/// Gets the current Snarl revision (build) number  (V39)
/// Returns the build version number, or M_RESULT on failure.

LONG32 SnarlInterface::GetRevision()
{
	SNARLSTRUCT ss;
	ss.Cmd = SNARL_GET_REVISION;
	ss.LngData2 = 0xFFFE;

	return Send(ss);
}


//-----------------------------------------------------------------------------
// snAddClass()

/// (V39)

M_RESULT SnarlInterface::AddClass(LPCSTR Class, LPCSTR Description, SNARL_CLASS_FLAGS Flags, LPCSTR DefaultTitle, LPCSTR DefaultIcon, LONG32 DefaultTimeout)
{
	SNARLSTRUCT ss;
	ss.Cmd = SNARL_ADD_CLASS;
	ss.LngData2 = Flags;
	ss.Timeout = GetCurrentProcessId();
	
	StringCbCopyExA((LPSTR)&ss.Text,  SNARL_STRING_LENGTH, Class, NULL, NULL, STRSAFE_IGNORE_NULLS);
	StringCbCopyExA((LPSTR)&ss.Title, SNARL_STRING_LENGTH, Description, NULL, NULL, STRSAFE_IGNORE_NULLS);

	LONG32 result = Send(ss);

	if (static_cast<M_RESULT>(result) == M_OK)
	{
		SetClassDefault(Class, SNARL_ATTRIBUTE_TITLE, DefaultTitle);
		SetClassDefault(Class, SNARL_ATTRIBUTE_ICON, DefaultIcon);
		if (DefaultTimeout > 0) {
			char str[64] = {0};
			StringCbPrintfA((LPSTR)&str, sizeof(str), "%d", DefaultTimeout);
			SetClassDefault(Class, SNARL_ATTRIBUTE_TIMEOUT, str);
		}
		
		return M_OK;
	}
	else
		return M_FAILED;
}

M_RESULT SnarlInterface::AddClass(LPCWSTR Class, LPCWSTR Description, SNARL_CLASS_FLAGS Flags, LPCWSTR DefaultTitle, LPCWSTR DefaultIcon, LONG32 DefaultTimeout)
{
	LPCSTR szClass        = WideToUTF8(Class);
	LPCSTR szDescription  = WideToUTF8(Description);
	LPCSTR szDefaultTitle = WideToUTF8(DefaultTitle);
	LPCSTR szDefaultIcon  = WideToUTF8(DefaultIcon);
	
	M_RESULT result = AddClass(szClass, szDescription, Flags, szDefaultTitle, szDefaultIcon, DefaultTimeout);
	
	delete [] szClass;
	delete [] szDescription;
	delete [] szDefaultTitle;
	delete [] szDefaultIcon;
	
	return result;
}

//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

template <class T>
LONG32 SnarlInterface::Send(T ss)
{
	DWORD_PTR nReturn = M_FAILED;

	HWND hWnd = GetSnarlWindow();
	if (IsWindow(hWnd))
	{
		COPYDATASTRUCT cds;
		cds.dwData = 2;
		cds.cbData = sizeof(ss);
		cds.lpData = &ss;

		if (SendMessageTimeout(hWnd, WM_COPYDATA, (WPARAM)m_hwndFrom, (LPARAM)&cds, SMTO_ABORTIFHUNG | SMTO_NOTIMEOUTIFNOTHUNG, 1000, &nReturn) == 0)
		{
			if (GetLastError() == ERROR_TIMEOUT)
				nReturn = M_TIMED_OUT;
		}
	}

	return static_cast<LONG32>(nReturn);
}

//-----------------------------------------------------------------------------

// Remember to : delete [] returned string

LPSTR SnarlInterface::WideToUTF8(LPCWSTR szWideStr)
{
	if (szWideStr == NULL)
		return NULL;

    int nSize = WideCharToMultiByte(CP_UTF8, 0, szWideStr, -1, NULL, 0, NULL, NULL);
    LPSTR szUTF8 = new char[nSize];
    WideCharToMultiByte(CP_UTF8, 0, szWideStr, -1, szUTF8, nSize, NULL, NULL);
    
    return szUTF8;
}
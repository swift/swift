#ifndef SNARL_INTERFACE_V41
#define SNARL_INTERFACE_V41

#include <tchar.h>
#include <windows.h>
#include <cstdio>

#ifndef SMTO_NOTIMEOUTIFNOTHUNG
	#define SMTO_NOTIMEOUTIFNOTHUNG 8
#endif


namespace Snarl {
	namespace V41 {

	static const LPCTSTR SnarlWindowClass = _T("w>Snarl");
	static const LPCTSTR SnarlWindowTitle = _T("Snarl");

	static const LPCTSTR SnarlGlobalMsg = _T("SnarlGlobalEvent");
	static const LPCTSTR SnarlAppMsg = _T("SnarlAppMessage");

	static const int SnarlPacketDataSize = 4096;

	// Enums put in own namespace, because ANSI C++ doesn't decorate enums with tagname :(
	namespace SnarlEnums {

		/// <summary>
		/// Global event identifiers.
		/// Identifiers marked with a '*' are sent by Snarl in two ways:
		///   1. As a broadcast message (uMsg = 'SNARL_GLOBAL_MSG')
		///   2. To the window registered in snRegisterConfig() or snRegisterConfig2()
		///      (uMsg = reply message specified at the time of registering)
		/// In both cases these values appear in wParam.
		///   
		/// Identifiers not marked are not broadcast; they are simply sent to the application's registered window.
		/// </summary>
		enum GlobalEvent
		{
			SnarlLaunched = 1,      // Snarl has just started running*
			SnarlQuit = 2,          // Snarl is about to stop running*
			SnarlAskAppletVer = 3,  // (R1.5) Reserved for future use
			SnarlShowAppUi = 4      // (R1.6) Application should show its UI
		};

		/// <summary>
		/// Message event identifiers.
		/// These are sent by Snarl to the window specified in RegisterApp() when the
		/// Snarl Notification raised times out or the user clicks on it.
		/// </summary>
		enum MessageEvent
		{
			NotificationClicked = 32,      // Notification was right-clicked by user
			NotificationCancelled = 32,    // Added in V37 (R1.6) -- same value, just improved the meaning of it
			NotificationTimedOut = 33,     // 
			NotificationAck = 34,          // Notification was left-clicked by user
			NotificationMenu = 35,         // Menu item selected (V39)
			NotificationMiddleButton = 36, // Notification middle-clicked by user (V39)
			NotificationClosed = 37        // User clicked the close gadget (V39)
		};

		/// <summary>
		/// Error values returned by calls to GetLastError().
		/// </summary>
		enum SnarlStatus
		{
			Success = 0,

			ErrorFailed = 101,        // miscellaneous failure
			ErrorUnknownCommand,      // specified command not recognised
			ErrorTimedOut,            // Snarl took too long to respond

			ErrorArgMissing = 109,    // required argument missing
			ErrorSystem,              // internal system error

			ErrorNotRunning = 201,    // Snarl handling window not found
			ErrorNotRegistered,       // 
			ErrorAlreadyRegistered,   // not used yet; RegisterApp() returns existing token
			ErrorClassAlreadyExists,  // not used yet; AddClass() returns existing token
			ErrorClassBlocked,
			ErrorClassNotFound,
			ErrorNotificationNotFound
		};

		/// <summary>
		/// Application flags - features this app supports.
		/// </summary>
		enum AppFlags
		{
			AppDefault = 0,
			AppHasPrefs = 1,
			AppHasAbout = 2,
			AppIsWindowless = 0x8000
		};

		enum SnarlCommand
		{
			RegisterApp = 1,
			UnregisterApp,
			UpdateApp,
			SetCallback,
			AddClass,
			RemoveClass,
			Notify,
			UpdateNotification,
			HideNotification,
			IsNotificationVisible,
			LastError                  // deprecated but retained for backwards compatability
		};
	}

	struct SnarlMessage
	{
		SnarlEnums::SnarlCommand Command;
		LONG32 Token;
		BYTE PacketData[SnarlPacketDataSize];
	};

	static const DWORD WM_SNARLTEST = WM_USER + 237;

	
	// ------------------------------------------------------------------------
	/// SnarlInterface class definition
	// ------------------------------------------------------------------------
	class SnarlInterface {
		public:
			SnarlInterface();
			~SnarlInterface();
			
			LPTSTR AllocateString(size_t n) { return new TCHAR[n]; }
			void FreeString(LPTSTR str)     { delete [] str; str = NULL; }
			void FreeString(LPCTSTR str)    { delete [] str; }

			/// <summary>Register application with Snarl.</summary>
			/// <returns>The application token or 0 on failure.</returns>
			/// <remarks>The application token is saved in SnarlInterface member variable, so just use return value to check for error.</remarks>
			LONG32 RegisterApp(LPCSTR signature, LPCSTR title, LPCSTR icon, HWND hWndReply = NULL, LONG32 msgReply = 0, SnarlEnums::AppFlags flags = SnarlEnums::AppDefault);
			LONG32 RegisterApp(LPCWSTR  signature, LPCWSTR title, LPCWSTR icon, HWND hWndReply = NULL, LONG32 msgReply = 0, SnarlEnums::AppFlags flags = SnarlEnums::AppDefault);

			/// <summary>Unregister application with Snarl when application is closing.</summary>
			/// <returns>0 on failure.</returns>
			LONG32 UnregisterApp();

			/// <summary>Update information provided when calling RegisterApp.</summary>
			/// <returns>0 on failure.</returns>
			LONG32 UpdateApp(LPCSTR title = NULL, LPCSTR icon = NULL);
			LONG32 UpdateApp(LPCWSTR title = NULL, LPCWSTR icon = NULL);

			/// <summary>Add a notification class to Snarl.</summary>
			/// <returns>0 on failure.</returns>
			LONG32 AddClass(LPCSTR className, LPCSTR description, bool enabled = true);
			LONG32 AddClass(LPCWSTR className, LPCWSTR description, bool enabled = true);
			
			/// <summary>Remove a notification class added with AddClass().</summary>
			/// <returns>0 on failure.</returns>
			LONG32 RemoveClass(LPCSTR className, bool forgetSettings = false);
			LONG32 RemoveClass(LPCWSTR className, bool forgetSettings = false);

			/// <summary>Remove all notification classes in one call.</summary>
			/// <returns>0 on failure.</returns>
			LONG32 RemoveAllClasses(bool forgetSettings = false);
			
			/// <summary>Show a Snarl notification.</summary>
			/// <returns>Returns the notification token or 0 on failure.</returns>
			/// <remarks>You can use <see cref="GetLastMsgToken()" /> to get the last token.</remarks>
			LONG32 EZNotify(LPCSTR className, LPCSTR title, LPCSTR text, LONG32 timeout = -1, LPCSTR icon = NULL, LONG32 priority = 0, LPCSTR acknowledge = NULL, LPCSTR value = NULL);
			LONG32 EZNotify(LPCWSTR className, LPCWSTR title, LPCWSTR text, LONG32 timeout = -1, LPCWSTR icon = NULL, LONG32 priority = 0, LPCWSTR acknowledge = NULL, LPCWSTR value = NULL);

			/// <summary>
			///     Show a Snarl notification.
			///     This function requires that you write your own packet data.
			/// </summary>
			/// <returns>Returns the notification token or 0 on failure.</returns>
			/// <remarks>You can use <see cref="GetLastMsgToken()" /> to get the last token.</remarks>
			LONG32 Notify(LPCSTR className, LPCSTR packetData);
			LONG32 Notify(LPCWSTR className, LPCWSTR packetData);

			/// <summary>Update the text or other parameters of a visible Snarl notification.</summary>
			/// <returns>0 on failure.</returns>
			LONG32 EZUpdate(LONG32 msgToken, LPCSTR title = NULL, LPCSTR text = NULL, LONG32 timeout = -1, LPCSTR icon = NULL);
			LONG32 EZUpdate(LONG32 msgToken, LPCWSTR title = NULL, LPCWSTR text = NULL, LONG32 timeout = -1, LPCWSTR icon = NULL);
			
			/// <summary>
			///     Update the text or other parameters of a visible Snarl notification.
			///     This function requires that you write your own packet data.
			/// </summary>
			/// <returns>0 on failure.</returns>
			LONG32 Update(LONG32 msgToken, LPCSTR packetData);
			LONG32 Update(LONG32 msgToken, LPCWSTR packetData);

			/// <summary>Hide a Snarl notification.</summary>
			/// <returns>0 on failure.</returns>
			LONG32 Hide(LONG32 msgToken);

			/// <summary>Test if a Snarl notification is visible.</summary>
			/// <returns>Returns -1 if message is visible. 0 if not visible or if an error occured.</returns>
			LONG32 IsVisible(LONG32 msgToken);

			/// <summary>Get the last error from Snarl. Call after other functions return 0 to know why it failed.</summary>
			/// <returns>Returns one of the SnarlEnums::SnarlStatus values.</returns>
			SnarlEnums::SnarlStatus GetLastError();

			/// <summary>Get Snarl version, if it is running.</summary>
			/// <returns>Returns a number indicating Snarl version.</returns>
			LONG32 GetVersion();

			/// <summary>
			///     Get the path to where Snarl is installed.
			///     ** Remember to call <see cref="FreeString(LPCTSTR)" /> on the returned string !!!
			/// </summary>
			/// <returns>Returns the path to where Snarl is installed.</returns>
			/// <remarks>This is a V39 API method.</remarks>
			LPCTSTR  GetAppPath();

			/// <summary>
			///     Get the path to where the default Snarl icons are located.
			///     <para>** Remember to call <see cref="FreeString(LPCTSTR)" /> on the returned string !!!</para>
			/// </summary>
			/// <returns>Returns the path to where the default Snarl icons are located.</returns>
			/// <remarks>This is a V39 API method.</remarks>
			LPCTSTR  GetIconsPath();

			/// <summary>GetLastMsgToken() returns token of the last message sent to Snarl.</summary>
			/// <returns>Returns message token of last message.</returns>
			/// <remarks>This function is not in the official API!</remarks>
			LONG32 GetLastMsgToken() const;
			
			/// <summary>Check whether Snarl is running</summary>
			/// <returns>Returns true if Snarl system was found running.</returns>
			static BOOL IsSnarlRunning();

			/// <summary>
			///     Returns the value of Snarl's global registered message.
			///     Notes:
			///       Snarl registers SNARL_GLOBAL_MSG during startup which it then uses to communicate
			///       with all running applications through a Windows broadcast message. This function can
			///       only fail if for some reason the Windows RegisterWindowMessage() function fails
			///       - given this, this function *cannnot* be used to test for the presence of Snarl.
			/// </summary>
			/// <returns>A 16-bit value (translated to 32-bit) which is the registered Windows message for Snarl.</returns>
			static UINT Broadcast();

			/// <summary>Returns the global Snarl Application message  (V39)</summary>
			/// <returns>Returns Snarl application registered message.</returns>
			static UINT AppMsg();

			/// <summary>Returns a handle to the Snarl Dispatcher window  (V37)</summary>
			/// <returns>Returns handle to Snarl Dispatcher window, or zero if it's not found.</returns>
			/// <remarks>This is now the preferred way to test if Snarl is actually running.</remarks>
			static HWND GetSnarlWindow();
		
		private:
			/// <summary>Send message to Snarl.</summary>
			/// <returns>Return zero on failure.</returns>
			LONG32 Send(SnarlMessage msg);

			/// <summary>Convert a unicode string to UTF8</summary>
			/// <returns>Returns pointer to the new string - Remember to delete [] returned string !</returns>
			/// <remarks>Remember to delete [] returned string !!!</remarks>
			LPSTR  WideToUTF8(LPCWSTR szWideStr);

			/// <summary>Pack data into the PackedData member field.</summary>
			/// <param name="data">Should always be a pointer to the PackedData field</param>
			/// <param name="format">The format string. Can be NULL or "" to just zero PackedData!</param>
			/// <param name="...">Variable number of objects to convert</param>
			void   PackData(BYTE* data, LPCSTR format, ...);

			LONG32 appToken;
			LONG32 lastMsgToken;
			SnarlEnums::SnarlStatus localError;

	}; // class

	} // namespace V41
} // namespace Snarl

#endif // SNARL_INTERFACE_V41

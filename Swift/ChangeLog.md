4.0-rc2 ( 2017-05-22 )
----------------------
- Fix regression in chat window titles for chat rooms

4.0-rc1 ( 2017-05-17 )
----------------------
- Fix UI layout issue for translations that require right-to-left (RTL) layout
- macOS releases are now code-signed with a key from Apple, so they can be run without Gatekeeper trust warnings
- Handle sessions being closed by the server
- Fix Last Message Correction in multi client scenarios
- Fix display of default avatar on Windows
- Support for automatic software updates on macOS
- Redesigned keyword highlighing
- Support for unicode emojis on macOS
- Improvements to font size handling in the chat theme
- Fix UX issues in trellis mode
- Improve date formatting
- And assorted smaller features and usability enhancements

4.0-beta2 ( 2016-07-20 )
------------------------
- Fix Swift bug introduced in 4.0-beta1 that results in the UI sometimes getting stuck during login

4.0-beta1 ( 2016-07-15 )
------------------------
- Support for message carbons (XEP-0280)
- Improved spell checker support on Linux
- Enabled trellis mode as a default feature, allowing several tiled chats windows to be shown at once
- New chat theme including a new font
- And assorted smaller features and usability enhancements

3.0 ( 2016-02-24 )
------------------
- File transfer and Mac Notification Center issues fixed
- Fix connection to servers with invalid or untrusted certificates on OS/X
- Support for the Notification Center on OS X
- Users can now authenticate using certificates (and smart cards on Windows) when using the 'BOSH' connection type.
- Encryption on OS X now uses the platform's native 'Secure Transport' mechanisms.
- Emoticons menu in chat dialogs
- Bookmark for rooms can now be edited directly from the ‘Recent Chats’ list
- Adds option to workaround servers that don’t interoperate well with Windows (schannel) encryption
- Rooms entered while offline will now get entered on reconnect
- Chats can now be seamlessly upgraded to multi-person chats by either inviting someone via the ‘cog’ menu, or dragging them from the roster. This relies on server-side support with an appropriate chatroom (MUC) service.
- Highlighting of keywords and messages from particular users can now be configured (Keyword Highlighting Blog post).
- Full profile vcards (contact information etc.) are now supported and can be configured for the user and queried for contacts.
- Simple Communication Blocking is now supported (subject to server support) to allow the blocking of nuisance users.
- Swift can now transfer files via the ‘Jingle File Transfer’ protocol.
- The status setter will now remember previously set statuses and will allow quick access to these when the user types part of a recently used status.
- Tooltips for contacts in the roster now show their vcard profile information
- The top of the roster window now shows a tooltip with your vcard profile information in
- Chatrooms (MUCs) can now be bookmarked by right-clicking on their entry in the Recent Chats list, or from the ‘cog’ menu for any room you’re currently in.
- The tooltips for MUC occupants now show their affiliation and role in the room (e.g. Administrator)
- Support for ‘right-to-left’ languages has been improved.
- A small number of emoticons will be automatically rendered when receiving text, e.g. :), this can be disabled from the View menu.
- Swift now distinguishes between contacts who have chosen to show as ‘away’ and those whose clients have noticed they’re idle (requires support in contacts' clients).
- Typing search terms with the roster selected will start to search for contacts matching the terms (useful for users with large rosters).
- An experimental ‘Trellis’ view for viewing several chats and rooms at once has been added. This is currently hidden; contact us if you’re interested in testing this feature.
- And assorted smaller features and usability enhancements.


3.0-beta1 ( 2015-02-23 )
------------------------
- Allow toggling of a more compact roster display.
- Remember status settings and provide quick access to them with searching of recent selections in the status setter.

2.1
---
- Fixed potential crash when using proxies on Mac OS X.

2.0-beta2 ( 2012-10-07 )
------------------------
- Enable auto-completion of nicknames that don't start with a letter.
- Generate crash dumps on Windows.
- Connection timeouts are now on each connection separately, instead of on the complete connection process.
- Don't allow pressing `<Enter>` in the roster to trigger logins.
- Don't lose security labels when correcting a message.
- Don't crash when completing user search without selection.
- Always auto join MUC with a consistent nickname.
- Renamed `swift` binary to `swift-im` on Linux.
- Avoid woosh down and woosh up on Mac when opening chat windows.
- Improved MUC invitation dialog.
- Fixed problem with displaying group names containing '&' in the 'Edit' dialog.
- Show stream encryption status in header.
- Show extended certificate dialog when encryption errors occur.
- Don't allow server command results to get interpreted as HTML.
- Additional connection settings (such as manual host setting, proxy configuration and BOSH) can now be specified through a new dialog on the login window.

Thanks to Tobias Markmann.


2.0-beta1 ( 2012-04-29 )
------------------------
- Windows packages are now built in Microsoft Installer's ".msi" format. Please 
  uninstall any older NSIS-based Swift packages before upgrading to this release.
- Suitable names will now be suggested from the contact's vcard when adding/editing their roster entry.
- It is now possible for sysadmins to deploy files with policies for configuration options, such as 
  making it impossible for users to save passwords or to force sound notifications off, or to set defaults.
- Swift will now use appropriate algorithms when run on a Windows platform locked down in FIPS-140-2 mode.
- Our TLS provider has been changed to the platform-provided one (Schannel) on Windows, 
  allowing us to use certificates (both file and card-based) from the system store (CAPI).
- Added support for message receipts in one-to-one conversations (XEP-0184).
- Added support for several MUC operations (kicking, banning, invites, topic changes, room destruction, 
  changing participant affiliations, ...).
- It is now possible to resize the font in the chat window conversations.
- Added support for message correction. Use 'up' to edit the previous message.
- A list of recent chats is now kept in the 'Chats' tab of the main window.
- Added support for server commands.
- Chat tabs with unread messages from several chats will now be a little more descriptive.
- Use a bar for showing where the last read messages in a chat are.
- Added support for SOCKS5 and HTTPConnect proxies. These settings are taken from the platform preferences.
- Retrieve incremental contact list on login on servers that support it (XEP-0237: Roster Versioning).
- Lots of bugfixes, performance fixes, and other changes.

Thanks to Tobias Markmann, Jan Kaluza, Thilo Cestonaro, Arnt Gulbrandsen, Vlad Voicu, Vitaly Takmazov,
Yoann Blein, Catalin Badea, Pavol Babincak, Mateusz Piekos, Alexey Melnikov and Soren Dreijer.


1.0 ( 2011-04-18 )
------------------
- Initial release.
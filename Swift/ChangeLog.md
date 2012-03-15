2.0-beta2
---------
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


2.0-beta1
---------
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


1.0
---
- Initial release.

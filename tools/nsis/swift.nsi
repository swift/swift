# define installer name
outFile "Swift-installer-win32.exe"
 
# set desktop as install directory
installDir "$PROGRAMFILES\Swift"

SetCompressor lzma

# default section start
section "Main install"
 
# define output path
setOutPath $INSTDIR
 
# specify files to go in output path
file ..\..\src\UI\Qt\release\*

# create start menu item
createShortCut "$SMPROGRAMS\Swift\Swift.lnk" "$INSTDIR\Swift.exe"
createShortCut "$SMPROGRAMS\Swift\Unistall Swift.lnk" "$INSTDIR\unistall.exe"

# We /could/ start on login:
# WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Run" "Swift" "$INSTDIR\Swift.exe"

# Add the information to Add/Remove
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Swift" "DisplayName" "Swift"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Swift" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Swift" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\""


# define uninstaller name
writeUninstaller $INSTDIR\uninstaller.exe
 
# default section end
sectionEnd

Section -Prerequisites
# http://nsis.sourceforge.net/Embedding_other_installers
  SetOutPath $INSTDIR\Prerequisites
  MessageBox MB_YESNO "Install C++ Runtime?" /SD IDYES IDNO endRuntime
    File ..\..\vcredist_x86.exe
    ExecWait "$INSTDIR\Prerequisites\vcredist_x86.exe"
    delete $INSTDIR\Prerequisites\vcredist_x86.exe
    delete $INSTDIR\Prerequisites
    Goto endRuntime
  endRuntime:
SectionEnd

section "autostart"
  MessageBox MB_YESNO "Run at startup?" /SD IDYES IDNO endAutostart
    WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Run" "Swift" "$INSTDIR\Swift.exe"
    Goto endAutostart
  endAutostart:
sectionEnd

# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"
section "Uninstall"
  MessageBox MB_YESNO "The uninstaller will delete the entire Swift folder, including any user-created files. Are you sure?" /SD IDYES IDNO endUninstall
    # Always delete uninstaller first
    delete $INSTDIR\uninstaller.exe
 
    # now delete installed files
    delete $INSTDIR\*
    Goto endUninstall
  endUninstall: 
sectionEnd


# TODO http://nsis.sourceforge.net/Check_whether_your_application_is_running_during_uninstallation
# http://nsis.sourceforge.net/Date_and_time_in_installer_or_application_name
# http://nsis.sourceforge.net/Removing_'Nullsoft_Install_System_vX.XX'_String_from_installer

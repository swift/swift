# set dependencies
!define buildDate "20090811"
!define swiftBuildDir "..\..\QtUI\Swift"
!define msvccRedistributableExe "vcredist_x86.exe"


# define installer name
outFile "Swift-installer-win32-${buildDate}.exe"
 
# set desktop as install directory
installDir "$PROGRAMFILES\Swift"

SetCompressor lzma

# default section start
section "Main install"
 
# define output path
setOutPath $INSTDIR
 
# Specify files to go in output path.
# If you update this list, update the uninstall list too.
file ${swiftBuildDir}\Swift.exe
file ${swiftBuildDir}\ssleay32.dll
file ${swiftBuildDir}\libeay32.dll
file ${swiftBuildDir}\phonon4.dll
file ${swiftBuildDir}\QtCore4.dll
file ${swiftBuildDir}\QtGui4.dll
file ${swiftBuildDir}\QtWebKit4.dll
file ${swiftBuildDir}\QtNetwork4.dll

# create start menu item
createShortCut "$SMPROGRAMS\Swift\Swift.lnk" "$INSTDIR\Swift.exe"
createShortCut "$SMPROGRAMS\Swift\Unistall Swift.lnk" "$INSTDIR\unistall.exe"

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
    File ${msvccRedistributableDir}\${msvccRedistributableExe}
    ExecWait "$INSTDIR\Prerequisites\${msvccRedistributableExe}"
    delete $INSTDIR\Prerequisites\${msvccRedistributableExe}
    delete $INSTDIR\Prerequisites
    Goto endRuntime
  endRuntime:
SectionEnd

section "autostart"
  MessageBox MB_YESNO "Would you like Swift to run at startup?" /SD IDYES IDNO endAutostart
    WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Run" "Swift" "$INSTDIR\Swift.exe"
    Goto endAutostart
  endAutostart:
sectionEnd

# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"
section "Uninstall"
    MessageBox MB_YESNO "The uninstaller will remove Swift. Are you sure?" /SD IDYES IDNO endUninstall
    # Always delete uninstaller first
    delete $INSTDIR\uninstaller.exe
 
    # now delete installed files
    delete $INSTDIR\Swift.exe
    delete $INSTDIR\ssleay32.dll
    delete $INSTDIR\libeay32.dll
    delete $INSTDIR\phonon4.dll
    delete $INSTDIR\QtCore4.dll
    delete $INSTDIR\QtGui4.dll
    delete $INSTDIR\QtWebKit4.dll
    delete $INSTDIR\QtNetwork4.dll
    Goto endUninstall
  endUninstall: 
sectionEnd


# TODO http://nsis.sourceforge.net/Check_whether_your_application_is_running_during_uninstallation
# http://nsis.sourceforge.net/Date_and_time_in_installer_or_application_name
# http://nsis.sourceforge.net/Removing_'Nullsoft_Install_System_vX.XX'_String_from_installer

# fsseudonymizer.nsi
# ------------------
# Begin: 2019/11/05

#Name of the program being installed
Name "Fsseudonymizer 1.0.0.20191105 (FSS edition)"

# NOTE: Can use the 'Icon' command to specify an icon file(.ico) for the .exe file.
Icon installerIcon.ico

# Installer name
outFile "FsseudonymizerFSS-1_0_0-x64.exe"

# Default installation directory
installDir "$PROGRAMFILES\Fsseudonymizer"

# Set the text to be shown in the license page
#LicenseData ../license.txt

# Create a custom page to ask install confirmation from the user.
page custom startDialog "" ": Start Dialog"
Function startDialog
  MessageBox MB_OKCANCEL "This application will install \
  Fsseudonymizer version 1.0.0 (FSS edition) \ 
  on your computer. Click OK to continue." \
  IDCANCEL NoCancelAbort 
  Abort
  NoCancelAbort: Quit  
FunctionEnd 

# Bring up the license page(pre-made in nSIS)
#page license

# Ask the user to select an installation Directory(pre-made in nSIS)
page directory

# Bring up the components page(pre-made in nSIS)
#page components

# Bring up the installation dialog(pre-made in NSIS)
page instfiles


page custom endDialog "" ": End Dialog"
Function endDialog
  MessageBox MB_OK "Installation is complete."
FunctionEnd

# The "-" before MainSection text indicates that this section
#	cannot be selectively installed by the user, i.e., it does not
#	show up in the components page.
section "-MainSection"
  # Set the installation directory
  setOutPath $INSTDIR

  # Delete some extraneous junk that might exist from previous installs
  ifFileExists "$INSTDIR\msvcrt.dll" DeleteMSVCRT DoNothingMSVCRT
  DeleteMSVCRT:
  Delete "$INSTDIR\msvcrt.dll"
  
  DoNothingMSVCRT:

  ifFileExists "$INSTDIR\msvcr110.dll" DeleteMSVCR110 DoNothingMSVCR110
  DeleteMSVCR110:
  Delete "$INSTDIR\msvcr110.dll"
  
  DoNothingMSVCR110:
  
  ifFileExists "$INSTDIR\libgcc_s_dw2-1.dll" DeleteDW2 DoNothingDW2
  DeleteDW2:
  Delete "$INSTDIR\libgcc_s_dw2.dll"
  
  DoNothingDW2:  
  

  # Copy each file in the installation directory
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\fsseudonymizer.exe"
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\fsseudonymizerc.exe"

  file "..\submodules\libmagic\bin64\magic1.dll"
  file "..\submodules\libmagic\bin64\libgnurx-0.dll"
  file "..\submodules\libmagic\bin64\magic"
  file "..\submodules\libmagic\bin64\magic.mgc"

  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\Qt5Core.dll"
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\Qt5Gui.dll"
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\Qt5Svg.dll"
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\Qt5Widgets.dll"
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\Qt5Xlsx.dll"
  
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\libEGL.dll"
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\libgcc_s_seh-1.dll"
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\libGLESV2.dll"
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\libstdc++-6.dll"
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\libwinpthread-1.dll"

  setOutPath "$INSTDIR\platforms"
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\platforms\qwindows.dll"

  setOutPath "$INSTDIR\styles"
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_64bit\release\styles\qwindowsvistastyle.dll"

  # Create an uninstaller
  writeUninstaller $INSTDIR\pseudoFishUninstall.exe

  # The following 2 commands add uninstall functionality in Add/Remove Programs
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fsseudonymizer" \
                   "DisplayName" "Fsseudonymizer"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fsseudonymizer" \
                   "UninstallString" "$INSTDIR\pseudoFishUninstall.exe"
sectionEnd

# This section can also be selectively installed by the user
#	and it is checked by default
section "Install Start menu shortcut"
  createShortCut "$SMPROGRAMS\Fsseudonymizer.lnk" "$INSTDIR\fsseudonymizer.exe"
sectionEnd

# This section can be selectively installed by the user
#	and the /o means that it is unchecked by default(optional)
section /o "Install desktop shortcut"
  setOutPath $DESKTOP
  # Create a shortcut to the executable file
  createShortCut "$DESKTOP\Fsseudonymizer.lnk" "$INSTDIR\fsseudonymizer.exe"
sectionEnd

# Bring up the uninstall confirmation page before uninstalling
uninstPage uninstConfirm "" "" ""

uninstPage instfiles

section "Uninstall"
  Delete "$INSTDIR\FsseudonymizerUninstall.exe"
  
  Delete "$INSTDIR\fsseudonymizer.exe"
  Delete "$INSTDIR\fsseudonymizerc.exe"

  Delete "$INSTDIR\magic1.dll"
  Delete "$INSTDIR\libgnurx-0.dll"
  Delete "$INSTDIR\magic"
  Delete "$INSTDIR\magic.mgc"
  
  Delete "$INSTDIR\Qt5Core.dll"
  Delete "$INSTDIR\Qt5Gui.dll"
  Delete "$INSTDIR\Qt5Svg.dll"
  Delete "$INSTDIR\Qt5Widgets.dll"
  Delete "$INSTDIR\Qt5Xlsx.dll"
  
  Delete "$INSTDIR\libEGL.dll"
  Delete "$INSTDIR\libgcc_s_seh-1.dll"
  Delete "$INSTDIR\libGLESV2.dll"
  Delete "$INSTDIR\libstdc++-6.dll"
  Delete "$INSTDIR\libwinpthread-1.dll"

  Delete "$INSTDIR\platforms\qminimal.dll"
  RMDir "$INSTDIR\platforms"
  
  Delete "$INSTDIR\styles\qwindowsvistastyle.dll"
  RMDir "$INSTDIR\styles"
  
  ifFileExists "$SMPROGRAMS\Fsseudonymizer.lnk" DeleteSMlink DoNothingSM
  DeleteSMlink:
  Delete "$SMPROGRAMS\Fsseudonymizer.lnk"
  
  DoNothingSM:

  ifFileExists "$DESKTOP\Fsseudonymizer.lnk" DeleteDesktopLink DoNothingDESK
  DeleteDesktopLink:
  Delete "$DESKTOP\Fsseudonymizer.lnk"

  DoNothingDESK:
  
  RMDir $INSTDIR

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fsseudonymizer"
sectionEnd

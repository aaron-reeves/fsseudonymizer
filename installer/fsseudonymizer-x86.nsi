# fsseudonymizer.nsi
# ------------------
# Begin: 2019/11/05

#Name of the program being installed
Name "Fsseudonymizer 1.1.1.20200213 (FSS edition)"

# NOTE: Can use the 'Icon' command to specify an icon file(.ico) for the .exe file.
Icon installerIcon.ico

# Installer name
outFile "FsseudonymizerFSS-1_1_1-x86.exe"

# Default installation directory
installDir "$PROGRAMFILES\Fsseudonymizer"

# Set the text to be shown in the license page
#LicenseData ../license.txt

# Create a custom page to ask install confirmation from the user.
page custom startDialog "" ": Start Dialog"
Function startDialog
  MessageBox MB_OKCANCEL "This application will install \
  Fsseudonymizer version 1.1.1 (FSS edition) \ 
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
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_32bit\release\fsseudonymizer.exe"
  file "..\Fsseudonymizer-build-Release-Desktop_Qt_5_12_4_MinGW_32bit\release\fsseudonymizerc.exe"

  file "..\submodules\libmagic\bin32\magic1.dll"
  file "..\submodules\libmagic\bin32\regex2.dll"
  file "..\submodules\libmagic\bin32\zlib1.dll"
  file "..\submodules\libmagic\bin32\magic"

  file "C:\Qt\5.12.4\mingw73_32\bin\Qt5Core.dll"
  file "C:\Qt\5.12.4\mingw73_32\bin\Qt5Gui.dll"
  file "C:\Qt\5.12.4\mingw73_32\bin\Qt5Svg.dll"
  file "C:\Qt\5.12.4\mingw73_32\bin\Qt5Widgets.dll"
  file "C:\Qt\5.12.4\mingw73_32\bin\Qt5Xlsx.dll"
  
  file "C:\Qt\5.12.4\mingw73_32\bin\libEGL.dll"
  file "C:\Qt\5.12.4\mingw73_32\bin\libgcc_s_dw2-1.dll"
  file "C:\Qt\5.12.4\mingw73_32\bin\libGLESV2.dll"
  file "C:\Qt\5.12.4\mingw73_32\bin\libstdc++-6.dll"
  file "C:\Qt\5.12.4\mingw73_32\bin\libwinpthread-1.dll"

  setOutPath "$INSTDIR\platforms"
  file "C:\Qt\5.12.4\mingw73_32\plugins\platforms\qwindows.dll"

  setOutPath "$INSTDIR\styles"
  file "C:\Qt\5.12.4\mingw73_32\plugins\styles\qwindowsvistastyle.dll"

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
  Delete "$INSTDIR\regex2.dll"
  Delete "$INSTDIR\zlib1.dll"
  Delete "$INSTDIR\magic"
  
  Delete "$INSTDIR\Qt5Core.dll"
  Delete "$INSTDIR\Qt5Gui.dll"
  Delete "$INSTDIR\Qt5Svg.dll"
  Delete "$INSTDIR\Qt5Widgets.dll"
  Delete "$INSTDIR\Qt5Xlsx.dll"
  
  Delete "$INSTDIR\libEGL.dll"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\libGLESV2.dll"
  Delete "$INSTDIR\libstdc++-6.dll"
  Delete "$INSTDIR\libwinpthread-1.dll"
  Delete "$INSTDIR\D3Dcompiler_47.dll"
  Delete "$INSTDIR\opengl32sw.dll"

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

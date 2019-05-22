Name "Themis"
OutFile "build\InstallThemis.exe"
LicenseData "LICENSE"
InstallDir "$PROGRAMFILES\Themis"

VIAddVersionKey "ProductName"     "Themis"
VIAddVersionKey "CompanyName"     "Cossack Labs Limited"
VIAddVersionKey "LegalCopyright"  "(c) Cossack Labs Limited"
VIAddVersionKey "FileDescription" "Themis library installer"
VIAddVersionKey "FileVersion"     "0.11.2"
VIAddVersionKey "ProductVersion"  "0.11.2"
VIFileVersion    0.11.2.0
VIProductVersion 0.11.2.0

Page license
Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

Section "Install"
	SetOutPath $INSTDIR
	File LICENSE
	File /r /x pkgconfig build\install\*
	WriteUninstaller $INSTDIR\Uninstall.exe
SectionEnd

Section "Uninstall"
	Delete $INSTDIR\Uninstall.exe
	Delete $INSTDIR\LICENSE
	RmDir /r /REBOOTOK $INSTDIR\bin
	RmDir /r           $INSTDIR\include
	RmDir /r           $INSTDIR\lib
	RmDir              $INSTDIR
SectionEnd

issue:
This application failed to start because it could not find or load the Qt platform plugin "windows"

solve:
copying below files to the ciwa.exe folder,
Qt5Core.dll, Qt5Gui.dll, Qt5Widgets.dll and a "platforms" directory with qminimal.dll, qoffscreen.dll, qwindows.dll.


database£º
copy¡°user.accdb¡±file to the ciwa.exe folde¡£

excel:
Qt Vs Tools->Qt Project Settings->Qt Modules
check "ActiveQt container"

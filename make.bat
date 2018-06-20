@ECHO *************************************
@ECHO *                                   *
@ECHO *         Start Compile	            *
@ECHO *                                   *
@ECHO *************************************

@ECHO OFF

set fts=%cd%\gzfts\gzfts.sln
set MT=%cd%\robot\motion_template\vcbuild\motion_template.sln
set agvsh=%cd%\robot\agv_shell\agv_shell.sln
set MN=%cd%\motion_net\motion_net.vcxproj
set PS=%cd%\path_search\PathSearch.vcxproj
set AI=%cd%\manage\agv_interface\agv_interface.vcxproj
set MI=%cd%\robot\motion_interface\motion_interface.vcxproj
set agvinfo=%cd%\agvinfo\libagvinfo\prj_win32\agvinfo.vcxproj

c:
cd \Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\

:debug
@echo start compile gzfts project...
devenv.com %fts% /Rebuild "Debug|Win32"

@echo start compile motion_net project...
devenv.com %MN% /Rebuild "Debug|Win32"

@echo start compile PathSearch project...
devenv.com %PS% /Rebuild "Debug|Win32"

@echo start compile agv_interface project...
devenv.com %AI% /Rebuild "Debug|Win32"

@echo start compile motion_interface project...
devenv.com %MI% /Rebuild "Debug|Win32"

@echo start compile motion_template project...
devenv.com %MT% /Rebuild "Debug|Win32"

@echo start compile agv_shell project...
devenv.com %agvsh% /Rebuild "Debug|Win32"

@echo start compile agvinfo project...
devenv.com %agvinfo% /Rebuild "Debug|Win32"

:release
@echo start compile gzfts project...
devenv.com %fts% /Rebuild "Release|Win32"

@echo start compile motion_net project...
devenv.com %MN% /Rebuild "Release|Win32"

@echo start compile PathSearch project...
devenv.com %PS% /Rebuild "Release|Win32"

@echo start compile agv_interface project...
devenv.com %AI% /Rebuild "Release|Win32"

@echo start compile motion_interface project...
devenv.com %MI% /Rebuild "Release|Win32"

@echo start compile agv_shell project...
devenv.com %agvsh% /Rebuild "Release|Win32"

@echo start compile motion_template project...
devenv.com %MT% /Rebuild "Release|Win32"

@echo start compile agvinfo project...
devenv.com %agvinfo% /Rebuild "Release|Win32"
:end
pause
echo good byed
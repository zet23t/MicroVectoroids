@echo off
echo Copying header files from srclib to arduino project directories ...

cd %~dp0

copy td2play\srclib\lib_*.* MicroVektoRoids >NUL

echo Done.
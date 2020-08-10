
mkdir EffekseerRuntimeDoc

echo doxygen(exe or bat)
call doxygen.bat Release/doxygen.template.ja
call doxygen.exe Release/doxygen.template.ja

call doxygen.bat Release/doxygen.template.en
call doxygen.exe Release/doxygen.template.en

rmdir /s /q EffekseerRuntimeDoc\ja\latex
rmdir /s /q EffekseerRuntimeDoc\en\latex

pause
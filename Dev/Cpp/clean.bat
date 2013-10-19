del *.sdf
del *.suo /A
del *.ncb /A

del release\*.pdb
del release\*.ilk
del release\*.lib
del release\*.exp


rmdir ipch /S /Q
rmdir Viewer\Debug /S /Q
rmdir Viewer\Release /S /Q
rmdir Viewer\Debug_DLL /S /Q
rmdir Viewer\Release_DLL /S /Q

pause
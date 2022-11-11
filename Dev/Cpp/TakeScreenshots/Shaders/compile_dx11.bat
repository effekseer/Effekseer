cd /d %~dp0

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tvs_4_0 /Emain /Fh HLSL_DX11_Header/mesh_vs.h HLSL_DX11/mesh_vs.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/mesh_ps.h HLSL_DX11/mesh_ps.fx

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/textures_ps.h HLSL_DX11/textures_ps.fx

pause

cd /d %~dp0

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tvs_4_0 /Emain /Fh HLSL_DX11_Header/static_mesh_vs.h HLSL_DX11/static_mesh_vs.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/static_mesh_ps.h HLSL_DX11/static_mesh_ps.fx

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tvs_4_0 /Emain /Fh HLSL_DX11_Header/line_vs.h HLSL_DX11/line_vs.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/line_ps.h HLSL_DX11/line_ps.fx

pause

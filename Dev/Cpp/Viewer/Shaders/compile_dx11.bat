cd /d %~dp0

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tvs_4_0 /Emain /Fh HLSL_DX11_Header/static_mesh_vs.h HLSL_DX11/static_mesh_vs.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/static_mesh_ps.h HLSL_DX11/static_mesh_ps.fx

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tvs_4_0 /Emain /Fh HLSL_DX11_Header/line_vs.h HLSL_DX11/line_vs.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/line_ps.h HLSL_DX11/line_ps.fx

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tvs_4_0 /Emain /Fh HLSL_DX11_Header/image_vs.h HLSL_DX11/image_vs.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/image_ps.h HLSL_DX11/image_ps.fx

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/white_particle_ps.h HLSL_DX11/white_particle_ps.fx

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tvs_4_0 /Emain /Fh HLSL_DX11_Header/postfx_basic_vs.h HLSL_DX11/postfx_basic_vs.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/postfx_overdraw_ps.h HLSL_DX11/postfx_overdraw_ps.fx

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/postfx_blend_ps.h HLSL_DX11/postfx_blend_ps.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/postfx_blur_h_ps.h HLSL_DX11/postfx_blur_h_ps.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/postfx_blur_v_ps.h HLSL_DX11/postfx_blur_v_ps.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/postfx_copy_ps.h HLSL_DX11/postfx_copy_ps.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/postfx_downsample_ps.h HLSL_DX11/postfx_downsample_ps.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/postfx_extract_ps.h HLSL_DX11/postfx_extract_ps.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/postfx_linear_to_srgb_ps.h HLSL_DX11/postfx_linear_to_srgb_ps.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0 /Emain /Fh HLSL_DX11_Header/postfx_tonemap_ps.h HLSL_DX11/postfx_tonemap_ps.fx

pause

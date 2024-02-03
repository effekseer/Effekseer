@echo off

cd /d %~dp0

set COMPILER="%DXSDK_DIR%Utilities\\bin\x86\fxc" /I..\..\EffekseerRendererCommon\EffekseerRendererCommon\Shader

%COMPILER% /Zpc /Tcs_5_0 /Emain /Fh ShaderHeader/gpu_particles_clear_cs.h Shader/gpu_particles_clear_cs.fx
%COMPILER% /Zpc /Tcs_5_0 /Emain /Fh ShaderHeader/gpu_particles_spawn_cs.h Shader/gpu_particles_spawn_cs.fx
%COMPILER% /Zpc /Tcs_5_0 /Emain /Fh ShaderHeader/gpu_particles_update_cs.h Shader/gpu_particles_update_cs.fx
%COMPILER% /Zpc /Tvs_5_0 /Emain /Fh ShaderHeader/gpu_particles_render_vs.h Shader/gpu_particles_render_vs.fx
%COMPILER% /Zpc /Tps_5_0 /Emain /Fh ShaderHeader/gpu_particles_render_ps.h Shader/gpu_particles_render_ps.fx

pause

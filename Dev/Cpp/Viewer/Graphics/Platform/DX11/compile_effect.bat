set FXC=%DXSDK_DIR%Utilities\\bin\x86\fxc

"%FXC%" /Zpc /Tvs_4_0 /EVS /Fh Shader/efk.GraphicsDX11.PostFX_Basic_VS.h Shader/postfx_basic_VS.fx
"%FXC%" /Zpc /Tps_4_0 /EPS /Fh Shader/efk.GraphicsDX11.PostFX_Copy_PS.h Shader/postfx_copy_PS.fx
"%FXC%" /Zpc /Tps_4_0 /EPS /Fh Shader/efk.GraphicsDX11.PostFX_Extract_PS.h Shader/postfx_extract_PS.fx
"%FXC%" /Zpc /Tps_4_0 /EPS /Fh Shader/efk.GraphicsDX11.PostFX_Downsample_PS.h Shader/postfx_downsample_PS.fx
"%FXC%" /Zpc /Tps_4_0 /EPS /Fh Shader/efk.GraphicsDX11.PostFX_Blend_PS.h Shader/postfx_blend_PS.fx
"%FXC%" /Zpc /Tps_4_0 /EPS /Fh Shader/efk.GraphicsDX11.PostFX_BlurH_PS.h Shader/postfx_blur_h_PS.fx
"%FXC%" /Zpc /Tps_4_0 /EPS /Fh Shader/efk.GraphicsDX11.PostFX_BlurV_PS.h Shader/postfx_blur_v_PS.fx
"%FXC%" /Zpc /Tps_4_0 /EPS /Fh Shader/efk.GraphicsDX11.PostFX_Tonemap_PS.h Shader/postfx_tonemap_PS.fx
"%FXC%" /Zpc /Tps_4_0 /EPS /Fh Shader/efk.GraphicsDX11.PostFX_LinearToSRGB_PS.h Shader/postfx_linear_to_srgb.fx

"%FXC%" /Zpc /Tvs_4_0_level_9_3 /EVS /Fh Shader/EffekseerRenderer.Tool_VS.h Shader/tool_renderer_VS.fx
"%FXC%" /Zpc /Tps_4_0_level_9_3 /EPS /Fh Shader/EffekseerRenderer.Tool_PS.h Shader/tool_renderer_PS.fx
"%FXC%" /Zpc /Tps_4_0_level_9_3 /EPS /Fh Shader/EffekseerRenderer.ToolNoTexture_PS.h Shader/tool_renderer_no_texture_PS.fx

pause
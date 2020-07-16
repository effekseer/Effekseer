"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tvs_4_0_level_9_3 /Emain /Fh Shader_15/EffekseerRenderer.Standard_VS.h Shader_15/standard_renderer_VS.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0_level_9_3 /Emain /Fh Shader_15/EffekseerRenderer.Standard_PS.h Shader_15/standard_renderer_PS.fx

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tvs_4_0_level_9_3 /Emain /Fh Shader_15/EffekseerRenderer.Standard_Distortion_VS.h Shader_15/standard_renderer_distortion_VS.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0_level_9_3 /Emain /Fh Shader_15/EffekseerRenderer.Standard_Distortion_PS.h Shader_15/standard_renderer_distortion_PS.fx

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tvs_4_0_level_9_3 /Emain /Fh Shader_15/EffekseerRenderer.Standard_Lighting_VS.h Shader_15/standard_renderer_lighting_VS.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0_level_9_3 /Emain /Fh Shader_15/EffekseerRenderer.Standard_Lighting_PS.h Shader_15/standard_renderer_lighting_PS.fx

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0_level_9_3 /Emain /D __INST__=40 /Fh Shader_15/EffekseerRenderer.ModelRenderer.ShaderTexture_PS.h Shader_15/model_renderer_texture_PS.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tvs_4_0_level_9_3 /Emain /D __INST__=40 /Fh Shader_15/EffekseerRenderer.ModelRenderer.ShaderTexture_VS.h Shader_15/model_renderer_texture_VS.fx

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tvs_4_0_level_9_3 /Emain /D __INST__=40 /Fh Shader_15/EffekseerRenderer.ModelRenderer.ShaderLightingTextureNormal_VS.h Shader_15/model_renderer_lighting_texture_normal_VS.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0_level_9_3 /Emain /D __INST__=40 /Fh Shader_15/EffekseerRenderer.ModelRenderer.ShaderLightingTextureNormal_PS.h Shader_15/model_renderer_lighting_texture_normal_PS.fx

"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tvs_4_0_level_9_3 /Emain /D __INST__=40 /Fh Shader_15/EffekseerRenderer.ModelRenderer.ShaderDistortion_VS.h Shader_15/model_renderer_distortion_VS.fx
"%DXSDK_DIR%Utilities\\bin\x86\fxc" /Zpc /Tps_4_0_level_9_3 /Emain /D __INST__=40 /Fh Shader_15/EffekseerRenderer.ModelRenderer.ShaderDistortion_PS.h Shader_15/model_renderer_distortion_PS.fx
pause
import os
import subprocess
import sys

transpiler_path = 'ShaderTranspiler'
root_path = 'EffekseerRendererDX11/EffekseerRenderer/Shader/'

dx9_root_path = 'EffekseerRendererDX9/EffekseerRenderer/Shader/'
dx12_root_path = 'EffekseerRendererDX12/EffekseerRenderer/Shader/'
metal_root_path = 'EffekseerRendererMetal/EffekseerRenderer/Shader/'
vulkan_root_path = 'EffekseerRendererVulkan/EffekseerRenderer/Shader/'

gl_2_root_path = 'EffekseerRendererGL/EffekseerRenderer/Shader_2/'
gl_3_root_path = 'EffekseerRendererGL/EffekseerRenderer/Shader_3/'
gl_es2_root_path = 'EffekseerRendererGL/EffekseerRenderer/Shader_ES2/'
gl_es3_root_path = 'EffekseerRendererGL/EffekseerRenderer/Shader_ES3/'

verts = [root_path + 'standard_renderer_VS.fx', root_path + 'standard_renderer_lighting_VS.fx',
         root_path + 'standard_renderer_distortion_VS.fx', root_path + 'model_renderer_texture_VS.fx', root_path + 'model_renderer_lighting_texture_normal_VS.fx', root_path + 'model_renderer_distortion_VS.fx']

frags = [root_path + 'standard_renderer_PS.fx', root_path + 'standard_renderer_lighting_PS.fx',
         root_path + 'standard_renderer_distortion_PS.fx', root_path + 'model_renderer_texture_PS.fx', root_path + 'model_renderer_lighting_texture_normal_PS.fx', root_path + 'model_renderer_distortion_PS.fx']

def fix_dx9_vs(path):
    f = open(path, 'r')
    lines = f.readlines()
    lines = [l.replace('uniform float4 gl_HalfPixel;', 'static const float4 gl_HalfPixel = 0.0f.xxxx;') for l in lines]
    f.close()

    f = open(path, 'w')
    f.writelines(lines)

try:
    subprocess.call(['ShaderTranspiler'])
except:
    print('Please put ShaderTranspiler from https://github.com/altseed/LLGI/tree/master/tools')
    sys.exit(1)    

dx9_common_flags = ['-D', '__INST__', '10']
llgi_common_flags = ['-D', '__INST__', '1']
gl2_common_flags = ['-D', 'DISABLE_INSTANCE', '1', '-D', '__OPENGL__', '1', '-D', '__OPENGL2__', '1', '--plain']
gl_common_flags = ['-D', 'DISABLE_INSTANCE', '1', '-D', '__OPENGL__', '1', '--plain']

# DX9
for f in (verts):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--vert', '-H', '--sm', '30', '--input', f, '--output', dx9_root_path + os.path.basename(f)] + dx9_common_flags)
    fix_dx9_vs(dx9_root_path + os.path.basename(f))

for f in (frags):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--frag', '-H', '--sm', '30', '--input', f, '--output', dx9_root_path + os.path.basename(f)] + dx9_common_flags)

# DX12
for f in (verts):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--vert', '-H', '--sm', '40', '--dx12', '--input', f, '--output', dx12_root_path + os.path.basename(f)] + llgi_common_flags)

for f in (frags):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--frag', '-H', '--sm', '40', '--dx12', '--input', f, '--output', dx12_root_path + os.path.basename(f)] + llgi_common_flags)

# Metal
for f in (verts):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--vert', '-M', '--input', f, '--output', metal_root_path + os.path.basename(f)] + llgi_common_flags)

for f in (frags):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--frag', '-M', '--input', f, '--output', metal_root_path + os.path.basename(f)] + llgi_common_flags)

# Vulkan
for f in (verts):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--vert', '-V', '--input', f, '--output', vulkan_root_path + os.path.basename(f) + '.vert'] + llgi_common_flags)

for f in (frags):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--frag', '-V', '--input', f, '--output', vulkan_root_path + os.path.basename(f) + '.frag'] + llgi_common_flags)

# OpenGL
for f in (verts):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--vert', '-G', '--sm', '120', '--input', f, '--output', gl_2_root_path + os.path.basename(f)] + gl2_common_flags)

for f in (frags):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--frag', '-G', '--sm', '120', '--input', f, '--output', gl_2_root_path + os.path.basename(f)] + gl2_common_flags)

for f in (verts):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--vert', '-G', '--sm', '330', '--input', f, '--output', gl_3_root_path + os.path.basename(f)] + gl_common_flags)

for f in (frags):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--frag', '-G', '--sm', '330', '--input', f, '--output', gl_3_root_path + os.path.basename(f)] + gl_common_flags)

for f in (verts):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--vert', '-G', '--es', '--sm', '200', '--input', f, '--output', gl_es2_root_path + os.path.basename(f)] + gl2_common_flags)

for f in (frags):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--frag', '-G', '--es', '--sm', '200', '--input', f, '--output', gl_es2_root_path + os.path.basename(f)] + gl2_common_flags)

for f in (verts):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--vert', '-G', '--es', '--sm', '300', '--input', f, '--output', gl_es3_root_path + os.path.basename(f)] + gl_common_flags)

for f in (frags):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--frag', '-G', '--es', '--sm', '300', '--input', f, '--output', gl_es3_root_path + os.path.basename(f)] + gl_common_flags)


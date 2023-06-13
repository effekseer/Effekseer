import os
import subprocess
import sys

transpiler_path = 'ShaderTranspiler'
root_path = 'EffekseerRendererDX11/EffekseerRendererDX11/Shader/'
include_path = 'EffekseerRendererCommon/EffekseerRendererCommon/Shader/'

dx9_root_path = 'EffekseerRendererDX9/EffekseerRendererDX9/Shader/'
dx12_root_path = 'EffekseerRendererDX12/EffekseerRendererDX12/Shader/'
metal_root_path = 'EffekseerRendererMetal/EffekseerRendererMetal/Shader/'
vulkan_root_path = 'EffekseerRendererVulkan/EffekseerRendererVulkan/Shader/'

gl_2_root_path = 'EffekseerRendererGL/EffekseerRendererGL/Shader_2/'
gl_3_root_path = 'EffekseerRendererGL/EffekseerRendererGL/Shader_3/'
gl_es2_root_path = 'EffekseerRendererGL/EffekseerRendererGL/Shader_ES2/'
gl_es3_root_path = 'EffekseerRendererGL/EffekseerRendererGL/Shader_ES3/'

verts = [
        root_path + 'ad_sprite_unlit_vs.fx',
        root_path + 'ad_sprite_lit_vs.fx',
        root_path + 'ad_sprite_distortion_vs.fx',
        root_path + 'ad_model_unlit_vs.fx',
        root_path + 'ad_model_lit_vs.fx',
        root_path + 'ad_model_distortion_vs.fx',
        root_path + 'sprite_unlit_vs.fx',
        root_path + 'sprite_lit_vs.fx',
        root_path + 'sprite_distortion_vs.fx',
        root_path + 'model_unlit_vs.fx',
        root_path + 'model_lit_vs.fx',
        root_path + 'model_distortion_vs.fx',
]

frags = [
        root_path + 'ad_model_unlit_ps.fx',
        root_path + 'ad_model_lit_ps.fx',
        root_path + 'ad_model_distortion_ps.fx',
        root_path + 'model_unlit_ps.fx',
        root_path + 'model_lit_ps.fx',
        root_path + 'model_distortion_ps.fx',
]

gpu_particles_verts = [
        root_path + 'gpu_particles_render_vs.fx',
]

gpu_particles_frags = [
        root_path + 'gpu_particles_render_ps.fx',
]

gpu_particles_comps = [
        root_path + 'gpu_particles_clear_cs.fx',
        root_path + 'gpu_particles_spawn_cs.fx',
        root_path + 'gpu_particles_update_cs.fx',
]

def fix_dx9_vs(path):
    f = open(path, 'r')
    lines = f.readlines()
    lines = [l.replace('uniform float4 gl_HalfPixel;',
                       'static const float4 gl_HalfPixel = 0.0f.xxxx;') for l in lines]
    f.close()

    f = open(path, 'w')
    f.writelines(lines)


try:
    subprocess.call([transpiler_path])
except:
    print('Please put ShaderTranspiler from https://github.com/altseed/LLGI/tree/master/tools')
    sys.exit(1)

dx9_common_flags = ['-I', include_path, '-D', '__INST__', '10', '-D', 'ENABLE_DIVISOR', '1', '-D', 'DISABLED_SOFT_PARTICLE', '1']
llgi_common_flags = ['-I', include_path, '-D', '__INST__', '40']

gl_common_flags = ['-I', include_path, '-D', '__INST__', '10', '-D', '__OPENGL__', '1', '--plain']
gl2_common_flags = ['-I', include_path, '-D', 'DISABLE_INSTANCE', '1', '-D', '__OPENGL__', '1', '-D', '__OPENGL2__', '1', '--plain', '-D', 'DISABLED_SOFT_PARTICLE', '1']


def transpile_dx9(shaderType, shaderModel, sources):
    for f in sources:
        print('Converting {}'.format(f))
        subprocess.call([transpiler_path, '--' + shaderType, '-H', '--sm', shaderModel, '--input',
                        f, '--output', dx9_root_path + os.path.basename(f)] + dx9_common_flags)
        if shaderType == "vert":
            fix_dx9_vs(dx9_root_path + os.path.basename(f))


def transpile_dx12(shaderType, shaderModel, sources):
    for f in (sources):
        print('Converting {}'.format(f))
        subprocess.call([transpiler_path, '--' + shaderType, '-H', '--sm', shaderModel, '--dx12', '--input',
                        f, '--output', dx12_root_path + os.path.basename(f)] + llgi_common_flags)


def transpile_metal(shaderType, sources):
    for f in (sources):
        print('Converting {}'.format(f))
        subprocess.call([transpiler_path, '--' + shaderType, '-M', '--input', f,
                        '--output', metal_root_path + os.path.basename(f)] + llgi_common_flags)


def transpile_vulkan(shaderType, sources):
    for f in (sources):
        print('Converting {}'.format(f))
        subprocess.call([transpiler_path, '--' + shaderType, '-V', '--input', f, '--output',
                        vulkan_root_path + os.path.basename(f) + '.' + shaderType] + llgi_common_flags)


def transpile_gl(shaderType, glver, sources):
    for f in (sources):
        print('Converting {}'.format(f))
        if glver == "gl2":
            subprocess.call([transpiler_path, '--' + shaderType, '-G', '--sm', '120', '--input',
                            f, '--output', gl_2_root_path + os.path.basename(f)] + gl2_common_flags)
        elif glver == "gl3":
            subprocess.call([transpiler_path, '--' + shaderType, '-G', '--sm', '330', '--input',
                            f, '--output', gl_3_root_path + os.path.basename(f)] + gl_common_flags)
        elif glver == "gles2":
            subprocess.call([transpiler_path, '--' + shaderType, '-G', '--es', '--sm', '200', '--input',
                            f, '--output', gl_es2_root_path + os.path.basename(f)] + gl2_common_flags)
        elif glver == "gles3":
            subprocess.call([transpiler_path, '--' + shaderType, '-G', '--es', '--sm', '300', '--input',
                            f, '--output', gl_es3_root_path + os.path.basename(f)] + gl_common_flags)


# Standard Shaders
transpile_dx9("vert", "30", verts)
transpile_dx9("frag", "30", frags)

transpile_dx12("vert", "40", verts)
transpile_dx12("frag", "40", frags)

transpile_metal("vert", verts)
transpile_metal("frag", frags)

transpile_vulkan("vert", verts)
transpile_vulkan("frag", frags)

transpile_gl("vert", "gl2", verts)
transpile_gl("frag", "gl2", frags)

transpile_gl("vert", "gl3", verts)
transpile_gl("frag", "gl3", frags)

transpile_gl("vert", "gles2", verts)
transpile_gl("frag", "gles2", frags)

transpile_gl("vert", "gles3", verts)
transpile_gl("frag", "gles3", frags)


# Gpu Particles Shaders
transpile_dx12("comp", "50", gpu_particles_comps)
transpile_dx12("vert", "50", gpu_particles_verts)
transpile_dx12("frag", "50", gpu_particles_frags)

transpile_metal("comp", gpu_particles_comps)
transpile_metal("vert", gpu_particles_verts)
transpile_metal("frag", gpu_particles_frags)

transpile_vulkan("comp", gpu_particles_comps)
transpile_vulkan("vert", gpu_particles_verts)
transpile_vulkan("frag", gpu_particles_frags)

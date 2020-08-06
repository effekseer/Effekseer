import os
import sys

verts = ['standard_renderer_VS', 'standard_renderer_lighting_VS',
         'standard_renderer_distortion_VS', 'model_renderer_texture_VS', 'model_renderer_lighting_texture_normal_VS', 'model_renderer_distortion_VS']

frags = ['standard_renderer_PS', 'standard_renderer_lighting_PS',
         'standard_renderer_distortion_PS', 'model_renderer_texture_PS', 'model_renderer_lighting_texture_normal_PS',  'model_renderer_distortion_PS']


gl_2_root_path = 'Shader_2/'
gl_3_root_path = 'Shader_3/'
gl_es2_root_path = 'Shader_ES2/'
gl_es3_root_path = 'Shader_ES3/'

gl_dst_path = 'ShaderHeader/'

def replace_es2(code):
    code = code.replace('#version 200 es','')
    code = code.replace('uniform highp', 'uniform ')
    code = code.replace('varying highp', 'varying ')
    return code

for fx in verts + frags:
    f_gl_2 = open(gl_2_root_path + fx + '.fx', 'r')
    gl_2 = f_gl_2.read()

    f_gl_3 = open(gl_3_root_path + fx + '.fx', 'r')
    gl_3 = f_gl_3.read()

    f_gl_es2 = open(gl_es2_root_path + fx + '.fx', 'r')
    gl_es2 = replace_es2(f_gl_es2.read())

    f_gl_es3 = open(gl_es3_root_path + fx + '.fx', 'r')
    gl_es3 = f_gl_es3.read()

    code = ''
    code += 'static const char {}_{}[] = R"(\n'.format(fx, 'gl2')
    code += gl_2
    code += ')";\n\n'

    code += 'static const char {}_{}[] = R"(\n'.format(fx, 'gl3')
    code += gl_3
    code += ')";\n\n'

    code += 'static const char {}_{}[] = R"(\n'.format(fx, 'gles2')
    code += gl_es2
    code += ')";\n\n'

    code += 'static const char {}_{}[] = R"(\n'.format(fx, 'gles3')
    code += gl_es3
    code += ')";\n\n'

    code += r'''
    static const char* get_{} (EffekseerRendererGL::OpenGLDeviceType deviceType)
    {{
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL3)
            return {}_{};
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL2)
            return {}_{};
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES3)
            return {}_{};
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES2 || deviceType == EffekseerRendererGL::OpenGLDeviceType::Emscripten)
            return {}_{};
        return nullptr;
    }}
    '''.format(fx, fx, 'gl3', fx, 'gl2', fx, 'gles3', fx, 'gles2')

    f = open(gl_dst_path + fx + '.h', 'w')
    f.write(code)

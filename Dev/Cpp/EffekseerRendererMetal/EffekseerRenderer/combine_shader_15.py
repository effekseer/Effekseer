import os
import sys

verts = ['standard_renderer_VS', 'standard_renderer_lighting_VS',
         'standard_renderer_distortion_VS', 'model_renderer_texture_VS', 'model_renderer_lighting_texture_normal_VS', 'model_renderer_distortion_VS']

frags = ['standard_renderer_PS', 'standard_renderer_lighting_PS',
         'standard_renderer_distortion_PS', 'model_renderer_texture_PS', 'model_renderer_lighting_texture_normal_PS',  'model_renderer_distortion_PS']


root_path = 'Shader_15/'
dst_path = 'ShaderHeader_15/'

for fx in verts + frags:
    f_gl_2 = open(root_path + fx + '.fx', 'r')
    gl_2 = f_gl_2.read()

    code = ''
    code += 'static const char metal_{}[] = R"(mtlcode\n'.format(fx)
    code += gl_2
    code += ')";\n\n'

    f = open(dst_path + fx + '.h', 'w')
    f.write(code)

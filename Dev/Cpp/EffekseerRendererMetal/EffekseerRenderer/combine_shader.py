import os
import sys

verts = ['standard_renderer_VS', 'standard_renderer_lighting_VS',
         'standard_renderer_distortion_VS', 'model_renderer_texture_VS', 'model_renderer_lighting_texture_normal_VS', 'model_renderer_distortion_VS',
         'sprite_unlit_vs', 'sprite_lit_vs', 'sprite_distortion_vs', 'model_unlit_vs',  'model_lit_vs', 'model_distortion_vs']

frags = ['standard_renderer_PS', 'standard_renderer_lighting_PS',
         'standard_renderer_distortion_PS', 'model_renderer_texture_PS', 'model_renderer_lighting_texture_normal_PS',  'model_renderer_distortion_PS',
         'sprite_unlit_ps', 'sprite_lit_ps', 'sprite_distortion_ps', 'model_unlit_ps',  'model_lit_ps', 'model_distortion_ps']


root_path = 'Shader/'
dst_path = 'ShaderHeader/'

for fx in verts + frags:
    f_gl_2 = open(root_path + fx + '.fx', 'r')
    gl_2 = f_gl_2.read()

    code = ''
    code += 'static const char metal_{}[] = R"(mtlcode\n'.format(fx)
    code += gl_2
    code += ')";\n\n'

    f = open(dst_path + fx + '.h', 'w')
    f.write(code)

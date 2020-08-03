import os
import sys
import subprocess

verts = ['standard_renderer_VS', 'standard_renderer_lighting_VS',
         'standard_renderer_distortion_VS', 'model_renderer_texture_VS', 'model_renderer_lighting_texture_normal_VS', 'model_renderer_distortion_VS']

frags = ['standard_renderer_PS', 'standard_renderer_lighting_PS',
         'standard_renderer_distortion_PS', 'model_renderer_texture_PS', 'model_renderer_lighting_texture_normal_PS',  'model_renderer_distortion_PS']


root_path = 'Shader/'
dst_path = 'ShaderHeader/'

for fx in verts:
    subprocess.call(["glslangValidator", "-V", "--vn", fx, "-o", dst_path + fx + '.h', root_path + fx + '.fx.vert'])

for fx in frags:
    subprocess.call(["glslangValidator", "-V", "--vn", fx, "-o", dst_path + fx + '.h', root_path + fx + '.fx.frag'])


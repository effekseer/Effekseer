import os
import sys
import subprocess

verts = ['ad_sprite_unlit_vs', 'ad_sprite_lit_vs', 'ad_sprite_distortion_vs', 'ad_model_unlit_vs',  'ad_model_lit_vs', 'ad_model_distortion_vs',
         'sprite_unlit_vs', 'sprite_lit_vs', 'sprite_distortion_vs', 'model_unlit_vs',  'model_lit_vs', 'model_distortion_vs']

frags = ['ad_model_unlit_ps',  'ad_model_lit_ps', 'ad_model_distortion_ps', 'model_unlit_ps',  'model_lit_ps', 'model_distortion_ps']


root_path = 'Shader/'
dst_path = 'ShaderHeader/'

for fx in verts:
    subprocess.call(["glslangValidator", "-V", "--vn", fx, "-o", dst_path + fx + '.h', root_path + fx + '.fx.vert'])

for fx in frags:
    subprocess.call(["glslangValidator", "-V", "--vn", fx, "-o", dst_path + fx + '.h', root_path + fx + '.fx.frag'])


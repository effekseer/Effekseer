import os
import sys
from pathlib import Path

root_path = Path('GLSL_GL/')
gl_dst_path = Path('GLSL_GL_Header/')

files = root_path.glob("*")


def replace_3(code):
    code = code.replace('layout(binding = 0) ', '')
    code = code.replace('layout(binding = 1) ', '')
    code = code.replace('layout(binding = 2) ', '')
    code = code.replace('layout(binding = 3) ', '')
    code = code.replace('layout(binding = 4) ', '')
    code = code.replace('layout(binding = 5) ', '')
    code = code.replace('layout(binding = 6) ', '')
    return code


for fx in files:

    key = os.path.splitext(fx.name)[0]

    f_gl = open(fx, 'r')
    gl_code = replace_3(f_gl.read())

    code = ''
    code += 'static const char gl_{}[] = R"(\n'.format(key)
    code += gl_code
    code += ')";\n\n'

    f = open(str(gl_dst_path) + '/' + key + '.h', 'w')
    f.write(code)

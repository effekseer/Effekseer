import os
import subprocess
import sys
from pathlib import Path

transpiler_path = 'ShaderTranspiler'
root_path = Path('Viewer/Shaders/HLSL_DX11/')
gl_root_path = Path('Viewer/Shaders/GLSL_GL/')

verts = root_path.glob("*_vs.fx")
frags = root_path.glob("*_ps.fx")

try:
    subprocess.call(['ShaderTranspiler'])
except:
    print('Please put ShaderTranspiler from https://github.com/altseed/LLGI/tree/master/tools')
    sys.exit(1)

gl_common_flags = ['-D', '__OPENGL__', '1', '--plain']

# OpenGL
for f in (verts):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--vert', '-G', '--sm', '330', '--input',
                     f, '--output', str(gl_root_path) + '/'  + os.path.basename(f)] + gl_common_flags)

for f in (frags):
    print('Converting {}'.format(f))
    subprocess.call(['ShaderTranspiler', '--frag', '-G', '--sm', '330', '--input',
                     f, '--output', str(gl_root_path) + '/' + os.path.basename(f)] + gl_common_flags)

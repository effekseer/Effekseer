import os
import shutil
import subprocess
import sys
from pathlib import Path


def find_shader_transpiler(script_dir):
    transpiler_path = os.environ.get('SHADER_TRANSPILER')
    if transpiler_path:
        return transpiler_path

    candidates = [
        script_dir / '3rdParty/LLGI/build-tool/tools/ShaderTranspiler/Debug/ShaderTranspiler.exe',
        script_dir / '3rdParty/LLGI/build-tool/tools/ShaderTranspiler/Release/ShaderTranspiler.exe',
        script_dir / '3rdParty/LLGI/build/tools/ShaderTranspiler/Debug/ShaderTranspiler.exe',
        script_dir / '3rdParty/LLGI/build/tools/ShaderTranspiler/Release/ShaderTranspiler.exe',
    ]
    return next((str(p) for p in candidates if p.exists()), 'ShaderTranspiler')


def ensure_shader_transpiler(transpiler_path):
    if not Path(transpiler_path).exists() and shutil.which(transpiler_path) is None:
        print('Please put ShaderTranspiler from https://github.com/altseed/LLGI/tree/master/tools')
        sys.exit(1)


def replace_glsl_330(code):
    for binding in range(7):
        code = code.replace('layout(binding = {}) '.format(binding), '')
    return code


def generate_gl_headers(gl_root_path, gl_header_root_path):
    gl_header_root_path.mkdir(parents=True, exist_ok=True)

    for fx in sorted(gl_root_path.glob("*")):
        if not fx.is_file():
            continue

        key = fx.stem
        with open(fx, 'r', encoding='utf-8') as f_gl:
            gl_code = replace_glsl_330(f_gl.read())

        code = ''
        code += 'static const char gl_{}[] = R"(\n'.format(key)
        code += gl_code
        code += ')";\n\n'

        with open(gl_header_root_path / (key + '.h'), 'w', encoding='utf-8', newline='\n') as f:
            f.write(code)


def transpile_gl_330(root_path, gl_root_path, transpiler_path):
    gl_root_path.mkdir(parents=True, exist_ok=True)
    ensure_shader_transpiler(transpiler_path)

    gl_common_flags = ['-D', '__OPENGL__', '1', '--plain']

    for f in sorted(root_path.glob("*_vs.fx")):
        print('Converting {}'.format(f), flush=True)
        subprocess.check_call([transpiler_path, '--vert', '-G', '--sm', '330', '--input',
                               str(f), '--output', str(gl_root_path / f.name)] + gl_common_flags)

    for f in sorted(root_path.glob("*_ps.fx")):
        print('Converting {}'.format(f), flush=True)
        subprocess.check_call([transpiler_path, '--frag', '-G', '--sm', '330', '--input',
                               str(f), '--output', str(gl_root_path / f.name)] + gl_common_flags)

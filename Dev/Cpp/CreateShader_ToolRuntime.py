import sys
from pathlib import Path

sys.dont_write_bytecode = True

from CreateShader_Common import find_shader_transpiler, generate_gl_headers, transpile_gl_330


def main():
    script_dir = Path(__file__).resolve().parent
    shader_root_path = script_dir / 'EffekseerToolRuntime/Shaders/'

    transpiler_path = find_shader_transpiler(script_dir)
    gl_root_path = shader_root_path / 'GLSL_GL/'
    gl_header_root_path = shader_root_path / 'GLSL_GL_Header/'

    transpile_gl_330(shader_root_path / 'HLSL_DX11/', gl_root_path, transpiler_path)
    generate_gl_headers(gl_root_path, gl_header_root_path)


if __name__ == '__main__':
    main()

import os
import re
import subprocess
import tempfile

verts = [
    "ad_sprite_unlit_vs",
    "ad_sprite_lit_vs",
    "ad_sprite_distortion_vs",
    "ad_model_unlit_vs",
    "ad_model_lit_vs",
    "ad_model_distortion_vs",
    "sprite_unlit_vs",
    "sprite_lit_vs",
    "sprite_distortion_vs",
    "model_unlit_vs",
    "model_lit_vs",
    "model_distortion_vs",
    "gpu_particles_render_vs",
]

frags = [
    "ad_model_unlit_ps",
    "ad_model_lit_ps",
    "ad_model_distortion_ps",
    "model_unlit_ps",
    "model_lit_ps",
    "model_distortion_ps",
    "gpu_particles_render_ps",
]

comps = [
    "gpu_particles_clear_cs",
    "gpu_particles_spawn_cs",
    "gpu_particles_update_cs",
]

script_dir = os.path.dirname(os.path.abspath(__file__))
cpp_dir = os.path.abspath(os.path.join(script_dir, "..", ".."))
transpiler = os.environ.get(
    "SHADER_TRANSPILER",
    os.path.join(cpp_dir, "3rdParty", "LLGI", "build-webgpu-tool", "tools", "ShaderTranspiler", "Debug", "ShaderTranspiler.exe"),
)
source_dir = os.path.join(cpp_dir, "EffekseerRendererDX12", "EffekseerRendererDX12", "Shader")
shader_dir = os.path.join(script_dir, "Shader")
header_dir = os.path.join(script_dir, "ShaderHeader")

os.makedirs(shader_dir, exist_ok=True)
os.makedirs(header_dir, exist_ok=True)


def zero_value(type_name):
    if type_name == "float":
        return "0.0f"

    match = re.match(r"float([234])$", type_name)
    if match:
        return "{}({})".format(type_name, ", ".join(["0.0f"] * int(match.group(1))))

    return "({})0".format(type_name)


def preprocess_input(name):
    input_path = os.path.join(source_dir, name + ".fx")
    with open(input_path, "r") as f:
        source = f.read()

    source = rewrite_uint4x3_asfloat(source)
    source = rewrite_texture_parameter_helpers(source)

    struct_match = re.search(r"struct\s+VS_Output\s*\{(?P<body>.*?)\};", source, re.S)
    output_match = re.search(r"VS_Output\s+Output\s*=\s*_(?P<id>\d+)\s*;", source)
    if struct_match and output_match:
        fields = []
        for field_match in re.finditer(r"\s*(?P<type>\w+)\s+(?P<name>\w+)\s*;", struct_match.group("body")):
            fields.append((field_match.group("type"), field_match.group("name")))

        if fields:
            const_name = "_" + output_match.group("id")
            source = re.sub(
                r"static\s+const\s+VS_Output\s+{}\s*=\s*\{{.*?\}}\s*;\s*".format(const_name),
                "",
                source,
                count=1,
                flags=re.S,
            )

            lines = ["VS_Output Output;"]
            for type_name, field_name in fields:
                lines.append("    Output.{} = {};".format(field_name, zero_value(type_name)))

            source = re.sub(r"VS_Output\s+Output\s*=\s*{}\s*;".format(const_name), "\n".join(lines), source, count=1)

    output_path = os.path.join(input_dir, name + ".fx")
    with open(output_path, "w", newline="\n") as f:
        f.write(source)

    return output_path


def split_top_level_arguments(text):
    args = []
    start = 0
    depth = 0
    for i, c in enumerate(text):
        if c == "(":
            depth += 1
        elif c == ")":
            depth -= 1
        elif c == "," and depth == 0:
            args.append(text[start:i].strip())
            start = i + 1

    args.append(text[start:].strip())
    return args


def find_function(source, signature):
    start = source.find(signature)
    if start < 0:
        return None

    brace = source.find("{", start + len(signature))
    if brace < 0:
        return None

    depth = 1
    end = brace + 1
    while end < len(source) and depth > 0:
        c = source[end]
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
        end += 1

    if depth != 0:
        return None

    return (start, end, source[start:end])


def rewrite_texture_parameter_helpers(source):
    uv_signature = "float2 UVDistortionOffset(Texture2D<float4> t, SamplerState s, float2 uv, float2 uvInversed, bool convertFromSRGB)"
    uv_function = find_function(source, uv_signature)
    if uv_function:
        _, _, text = uv_function
        uv_text = text.replace(uv_signature, "float2 UVDistortionOffset(float2 uv, float2 uvInversed, bool convertFromSRGB)")
        uv_text = uv_text.replace("t.Sample(s, uv)", "_uvDistortionTex.Sample(sampler_uvDistortionTex, uv)")

        blend_text = text.replace(uv_signature, "float2 BlendUVDistortionOffset(float2 uv, float2 uvInversed, bool convertFromSRGB)")
        blend_text = blend_text.replace("t.Sample(s, uv)", "_blendUVDistortionTex.Sample(sampler_blendUVDistortionTex, uv)")

        source = source[: uv_function[0]] + uv_text + "\n\n" + blend_text + source[uv_function[1] :]
        source = source.replace("UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, ", "UVDistortionOffset(")
        source = source.replace(
            "UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, ",
            "BlendUVDistortionOffset(",
        )

    flip_signature = "void ApplyFlipbook(inout float4 dst, Texture2D<float4> t, SamplerState s, float4 flipbookParameter, float4 vcolor, float2 nextUV, float flipbookRate, bool convertFromSRGB)"
    flip_function = find_function(source, flip_signature)
    if flip_function:
        _, _, text = flip_function
        flip_text = text.replace(
            flip_signature,
            "void ApplyFlipbook(inout float4 dst, float4 flipbookParameter, float4 vcolor, float2 nextUV, float flipbookRate, bool convertFromSRGB)",
        )
        flip_text = flip_text.replace("t.Sample(s, nextUV)", "_colorTex.Sample(sampler_colorTex, nextUV)")

        source = source[: flip_function[0]] + flip_text + source[flip_function[1] :]
        source = source.replace("ApplyFlipbook(param_6, _colorTex, sampler_colorTex, ", "ApplyFlipbook(param_6, ")
        source = source.replace("ApplyFlipbook(param_4, _colorTex, sampler_colorTex, ", "ApplyFlipbook(param_4, ")

    return source


def rewrite_uint4x3_asfloat(source):
    pattern = "asfloat(uint4x3("
    cursor = 0
    output = []

    while True:
        start = source.find(pattern, cursor)
        if start < 0:
            output.append(source[cursor:])
            break

        output.append(source[cursor:start])
        args_start = start + len(pattern)
        depth = 1
        end = args_start
        while end < len(source) and depth > 0:
            c = source[end]
            if c == "(":
                depth += 1
            elif c == ")":
                depth -= 1
            end += 1

        if depth != 0 or end >= len(source) or source[end] != ")":
            output.append(source[start:end])
            cursor = end
            continue

        args = split_top_level_arguments(source[args_start : end - 1])
        if len(args) == 12:
            rows = []
            for row in range(4):
                row_args = args[row * 3 : row * 3 + 3]
                rows.append("asfloat(uint3({}))".format(", ".join(row_args)))
            output.append("float4x3({})".format(", ".join(rows)))
            cursor = end + 1
        else:
            output.append(source[start : end + 1])
            cursor = end + 1

    return "".join(output)


def write_header(name, data):
    path = os.path.join(header_dir, name + ".h")
    with open(path, "w", newline="\n") as f:
        f.write("#pragma once\n\n")
        f.write("static const unsigned char webgpu_" + name + "[] = {\n")
        for i in range(0, len(data), 16):
            chunk = data[i : i + 16]
            f.write("\t")
            f.write(", ".join("0x{:02x}".format(b) for b in chunk))
            if i + 16 < len(data):
                f.write(",")
            f.write("\n")
        f.write("};\n")


def transpile(name, stage, ext):
    if not os.path.isfile(transpiler):
        raise FileNotFoundError(
            "ShaderTranspiler was not found: {}\n"
            "Build LLGI with -DBUILD_WEBGPU=ON -DBUILD_TOOL=ON, or set SHADER_TRANSPILER to ShaderTranspiler.exe.".format(
                transpiler
            )
        )

    input_path = preprocess_input(name)
    output_path = os.path.join(shader_dir, name + ".wgsl")
    compiled_path = os.path.join(shader_dir, name + ".wgslc")

    subprocess.check_call(
        [
            transpiler,
            stage,
            "-W",
            "--fix-wgsl-matrix-direction",
            "--input",
            input_path,
            "--output",
            output_path,
            "--compiled-output",
            compiled_path,
        ]
    )

    with open(output_path, "r", newline="\n") as f:
        wgsl = f.read()
    with open(output_path, "w", newline="\n") as f:
        f.write(wgsl)
    with open(compiled_path, "wb") as f:
        f.write(b"wgslcode")
        f.write(wgsl.encode("utf-8"))

    with open(compiled_path, "rb") as f:
        write_header(name, f.read())


with tempfile.TemporaryDirectory(prefix="effekseer_webgpu_shader_") as input_dir:
    for shader in verts:
        transpile(shader, "--vert", "vert")

    for shader in frags:
        transpile(shader, "--frag", "frag")

    for shader in comps:
        transpile(shader, "--comp", "comp")

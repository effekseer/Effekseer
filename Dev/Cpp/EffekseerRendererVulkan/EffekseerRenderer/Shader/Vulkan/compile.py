import sys
import subprocess

files = [
    "model.frag",
    "model.vert",
    "model_distortion_no_texture.frag",
    "model_distortion.frag",
    "model_distortion.vert",
    "model_l.frag",
    "model_l.vert",
    "model_ln.frag",
    "model_ln.vert",
    "model_lt.frag",
    "model_lt.vert",
    "model_ltn.frag",
    "model_ltn.vert",
    "model_t.frag",
    "model_t.vert",
    "standard_distortion_no_texture.frag",
    "standard_distortion.frag",
    "standard_distortion.vert",
    "standard_no_texture.frag",
    "standard.frag",
    "standard.vert",
]

for file in files:
    spv_file = file + ".spv"
    subprocess.call(["glslangValidator", "-V", "-o", spv_file, file])

    output = ""
    i = 0
    with open(spv_file, "rb") as f:
        while True:
            d = f.read(1)
            if len(d) == 0:
                break
            output += '0x{:02x}, '.format(d[0])
            if i % 16 == 15:
                output += '\n'
            i += 1

    wf = open(spv_file + '.inl', 'w')
    wf.write(output)
    wf.close()

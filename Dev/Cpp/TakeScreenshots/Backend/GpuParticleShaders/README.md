
$Env:PATH+=";D:\Proj\Effekseer\build\Dev\Cpp\3rdParty\LLGI\tools\ShaderTranspiler\Debug"

ShaderTranspiler --vert -G --input perticle-emit.vert.hlsl --output ../GLSL/perticle-emit.vert.glsl
ShaderTranspiler --vert -G --input perticle-emit.frag.hlsl --output ../GLSL/perticle-emit.frag.glsl

ShaderTranspiler --vert -G --input perticle-render.vert.hlsl --output ../GLSL/perticle-render.vert.glsl
ShaderTranspiler --vert -G --input perticle-render.frag.hlsl --output ../GLSL/perticle-render.frag.glsl

ShaderTranspiler --vert -G --input perticle-update.vert.hlsl --output ../GLSL/perticle-update.vert.glsl
ShaderTranspiler --vert -G --input perticle-update.frag.hlsl --output ../GLSL/perticle-update.frag.glsl

ShaderTranspiler --vert -G --input perticle-update.vert.hlsl --output ../GLSL/perticle-update.vert.glsl
ShaderTranspiler --vert -G --input perticle-update.frag.hlsl --output ../GLSL/perticle-update.frag.glsl






D:\Proj\Effekseer\build\Dev\Cpp\3rdParty\LLGI\tools\ShaderTranspiler\Debug\ShaderTranspiler --vert -H --input perticle-emit.vert.hlsl --output ../HLSL2/perticle-emit.vert.hlsl
D:\Proj\Effekseer\build\Dev\Cpp\3rdParty\LLGI\tools\ShaderTranspiler\Debug\ShaderTranspiler --vert -H --input perticle-emit.frag.hlsl --output ../HLSL2/perticle-emit.frag.hlsl

D:\Proj\Effekseer\build\Dev\Cpp\3rdParty\LLGI\tools\ShaderTranspiler\Debug\ShaderTranspiler --vert -H --input perticle-render.frag.hlsl --output ../HLSL2/perticle-render.frag.hlsl
D:\Proj\Effekseer\build\Dev\Cpp\3rdParty\LLGI\tools\ShaderTranspiler\Debug\ShaderTranspiler --vert -H --input perticle-render.vert.hlsl --output ../HLSL2/perticle-render.vert.hlsl

D:\Proj\Effekseer\build\Dev\Cpp\3rdParty\LLGI\tools\ShaderTranspiler\Debug\ShaderTranspiler --vert -H --input perticle-update.vert.hlsl --output ../HLSL2/perticle-update.vert.hlsl
D:\Proj\Effekseer\build\Dev\Cpp\3rdParty\LLGI\tools\ShaderTranspiler\Debug\ShaderTranspiler --vert -H --input perticle-update.frag.hlsl --output ../HLSL2/perticle-update.frag.hlsl

D:\Proj\Effekseer\build\Dev\Cpp\3rdParty\LLGI\tools\ShaderTranspiler\Debug\ShaderTranspiler --vert -H --input trail-render.frag.hlsl --output ../HLSL2/trail-render.frag.hlsl
D:\Proj\Effekseer\build\Dev\Cpp\3rdParty\LLGI\tools\ShaderTranspiler\Debug\ShaderTranspiler --vert -H --input trail-render.vert.hlsl --output ../HLSL2/trail-render.vert.hlsl

D:\Proj\Effekseer\build\Dev\Cpp\3rdParty\LLGI\tools\ShaderTranspiler\Debug\ShaderTranspiler --vert -H --input trail-update.frag.hlsl --output ../HLSL2/trail-update.frag.hlsl
D:\Proj\Effekseer\build\Dev\Cpp\3rdParty\LLGI\tools\ShaderTranspiler\Debug\ShaderTranspiler --vert -H --input trail-update.vert.hlsl --output ../HLSL2/trail-update.vert.hlsl




dxc -T ps_4_0 -P ../HLSL/perticle-emit.frag.hlsl perticle-emit.frag.hlsl
dxc -T ps_4_0 -P ../HLSL/perticle-emit.vert.hlsl perticle-emit.vert.hlsl

dxc -T ps_4_0 -P ../HLSL/perticle-render.frag.hlsl perticle-render.frag.hlsl
dxc -T ps_4_0 -P ../HLSL/perticle-render.vert.hlsl perticle-render.vert.hlsl

dxc -T ps_4_0 -P ../HLSL/perticle-update.frag.hlsl perticle-update.frag.hlsl
dxc -T ps_4_0 -P ../HLSL/perticle-update.vert.hlsl perticle-update.vert.hlsl

dxc -T ps_4_0 -P ../HLSL/trail-render.frag.hlsl trail-render.frag.hlsl
dxc -T ps_4_0 -P ../HLSL/trail-render.vert.hlsl trail-render.vert.hlsl

dxc -T ps_4_0 -P ../HLSL/trail-update.frag.hlsl trail-update.frag.hlsl
dxc -T ps_4_0 -P ../HLSL/trail-update.vert.hlsl trail-update.vert.hlsl


dxc -T ps_4_0 -E -Fo ../HLSL/perticle-emit.vert.hlsl -P ../HLSL/perticle-emit.vert.hlsl

dxc -T ps_4_0 -E -Fo ../HLSL/perticle-render.frag.hlsl -P ../HLSL/perticle-render.frag.hlsl
dxc -T ps_4_0 -E -Fo ../HLSL/perticle-render.vert.hlsl -P ../HLSL/perticle-render.vert.hlsl

dxc -T ps_4_0 -E -Fo ../HLSL/perticle-update.frag.hlsl -P ../HLSL/perticle-update.frag.hlsl
dxc -T ps_4_0 -E -Fo ../HLSL/perticle-update.vert.hlsl -P ../HLSL/perticle-update.vert.hlsl

dxc -T ps_4_0 -E -Fo ../HLSL/trail-update.frag.hlsl -P ../HLSL/trail-render.frag.hlsl
dxc -T ps_4_0 -E -Fo ../HLSL/trail-update.vert.hlsl -P ../HLSL/trail-render.vert.hlsl

dxc -T ps_4_0 -E -Fo ../HLSL/trail-update.frag.hlsl -P ../HLSL/trail-update.frag.hlsl
dxc -T ps_4_0 -E -Fo ../HLSL/trail-update.vert.hlsl -P ../HLSL/trail-update.vert.hlsl

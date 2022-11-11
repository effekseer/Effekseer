
$Env:PATH+=";D:\Proj\Effekseer\build\Dev\Cpp\3rdParty\LLGI\tools\ShaderTranspiler\Debug"

ShaderTranspiler --vert -G --input perticle-emit.vert.hlsl --output ../GLSL/perticle-emit.vert.glsl
ShaderTranspiler --vert -G --input perticle-emit.frag.hlsl --output ../GLSL/perticle-emit.frag.glsl

ShaderTranspiler --vert -G --input perticle-render.vert.hlsl --output ../GLSL/perticle-render.vert.glsl
ShaderTranspiler --vert -G --input perticle-render.frag.hlsl --output ../GLSL/perticle-render.frag.glsl

ShaderTranspiler --vert -G --input perticle-update.vert.hlsl --output ../GLSL/perticle-update.vert.glsl
ShaderTranspiler --vert -G --input perticle-update.frag.hlsl --output ../GLSL/perticle-update.frag.glsl




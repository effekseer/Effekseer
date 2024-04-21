#version 430

struct ParticleData
{
    uint FlagBits;
    uint Seed;
    float LifeAge;
    uint InheritColor;
    uint Color;
    uint Direction;
    uvec2 Velocity;
    mat4x3 Transform;
};

struct VS_Input
{
    vec3 Pos;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec2 UV;
    vec4 Color;
    uint InstanceID;
    uint VertexID;
};

struct VS_Output
{
    vec4 Pos;
    vec2 UV;
    vec4 Color;
    vec3 WorldN;
    vec3 WorldB;
    vec3 WorldT;
};

struct ParameterData
{
    int EmitCount;
    int EmitPerFrame;
    float EmitOffset;
    uint Padding0;
    vec2 LifeTime;
    uint EmitShapeType;
    uint EmitRotationApplied;
    vec4 EmitShapeData[2];
    vec3 Direction;
    float Spread;
    vec2 InitialSpeed;
    vec2 Damping;
    vec4 AngularOffset[2];
    vec4 AngularVelocity[2];
    vec4 ScaleData1[2];
    vec4 ScaleData2[2];
    vec3 ScaleEasing;
    uint ScaleFlags;
    vec3 Gravity;
    uint Padding2;
    vec3 VortexCenter;
    float VortexRotation;
    vec3 VortexAxis;
    float VortexAttraction;
    float TurbulencePower;
    uint TurbulenceSeed;
    float TurbulenceScale;
    float TurbulenceOctave;
    uint RenderState;
    uint ShapeType;
    uint ShapeData;
    float ShapeSize;
    float Emissive;
    float FadeIn;
    float FadeOut;
    uint MaterialType;
    uvec4 ColorData;
    vec3 ColorEasing;
    uint ColorFlags;
};

struct RenderConstants
{
    vec3 CameraPos;
    uint CoordinateReversed;
    vec3 CameraFront;
    float Reserved1;
    vec3 LightDir;
    float Reserved2;
    vec4 LightColor;
    vec4 LightAmbient;
    mat4 ProjMat;
    mat4 CameraMat;
    mat4x3 BillboardMat;
    mat4x3 YAxisFixedMat;
};

struct EmitterData
{
    uint FlagBits;
    uint Seed;
    uint ParticleHead;
    uint ParticleSize;
    uint TrailHead;
    uint TrailSize;
    uint TrailPhase;
    uint NextEmitCount;
    uint TotalEmitCount;
    uint EmitPointCount;
    float TimeCount;
    float TimeStopped;
    uint Reserved0;
    uint Reserved1;
    float DeltaTime;
    uint Color;
    mat4x3 Transform;
};

struct TrailData
{
    vec3 Position;
    uint Direction;
};

layout(set = 0, binding = 1, std140) uniform cb1
{
    ParameterData paramData;
} _121;

layout(set = 0, binding = 0, std140) uniform cb0
{
    layout(row_major) RenderConstants constants;
} _136;

layout(set = 0, binding = 2, std140) uniform cb2
{
    layout(row_major) EmitterData emitter;
} _265;

layout(set = 2, binding = 1, std430) readonly buffer Trails
{
    TrailData _data[];
} Trails_1;

layout(set = 2, binding = 0, std430) readonly buffer Particles
{
    layout(row_major) ParticleData _data[];
} Particles_1;

layout(location = 0) in vec3 input_Pos;
layout(location = 1) in vec3 input_Normal;
layout(location = 2) in vec3 input_Binormal;
layout(location = 3) in vec3 input_Tangent;
layout(location = 4) in vec2 input_UV;
layout(location = 5) in vec4 input_Color;
layout(location = 0) out vec2 _entryPointOutput_UV;
layout(location = 1) out vec4 _entryPointOutput_Color;
layout(location = 2) out vec3 _entryPointOutput_WorldN;
layout(location = 3) out vec3 _entryPointOutput_WorldB;
layout(location = 4) out vec3 _entryPointOutput_WorldT;

vec3 UnpackNormalizedFloat3(uint bits)
{
    vec3 v = vec3(uvec3(bits, bits >> uint(10), bits >> uint(20)) & uvec3(1023u));
    return ((v / vec3(1023.0)) * 2.0) - vec3(1.0);
}

void transformSprite(ParticleData particle, inout vec3 position)
{
    position = particle.Transform * vec4(position, 0.0);
    if (_121.paramData.ShapeData == 0u)
    {
        position = _136.constants.BillboardMat * vec4(position, 0.0);
    }
    else
    {
        if (_121.paramData.ShapeData == 1u)
        {
            uint param = particle.Direction;
            vec3 U = normalize(UnpackNormalizedFloat3(param));
            vec3 F = _136.constants.CameraFront;
            vec3 R = normalize(cross(U, F));
            F = normalize(cross(R, U));
            position = mat3(vec3(R), vec3(U), vec3(F)) * position;
        }
        else
        {
            if (_121.paramData.ShapeData == 2u)
            {
                position = _136.constants.YAxisFixedMat * vec4(position, 0.0);
            }
        }
    }
    position += particle.Transform[3];
}

void transformModel(ParticleData particle, inout vec3 position)
{
    if (_136.constants.CoordinateReversed != 0u)
    {
        position.z = -position.z;
    }
    position = particle.Transform * vec4(position, 1.0);
}

void transformTrail(ParticleData particle, inout vec3 position, inout vec2 uv, uint instanceID, uint vertexID)
{
    uint updateCount = (particle.FlagBits >> uint(1)) & 255u;
    uint trailLength = min(_121.paramData.ShapeData, updateCount);
    uint segmentID = min((vertexID / 2u), trailLength);
    vec3 trailPosition;
    vec3 trailDirection;
    if (segmentID == 0u)
    {
        trailPosition = particle.Transform[3];
        uint param = particle.Direction;
        trailDirection = normalize(UnpackNormalizedFloat3(param));
    }
    else
    {
        uint trailID = _265.emitter.TrailHead + (instanceID * _121.paramData.ShapeData);
        trailID += ((((_121.paramData.ShapeData + _265.emitter.TrailPhase) - segmentID) + 1u) % _121.paramData.ShapeData);
        TrailData trail;
        trail.Position = Trails_1._data[trailID].Position;
        trail.Direction = Trails_1._data[trailID].Direction;
        trailPosition = trail.Position;
        uint param_1 = trail.Direction;
        trailDirection = normalize(UnpackNormalizedFloat3(param_1));
        uv.y = float(segmentID) / float(trailLength);
    }
    vec3 trailTangent = normalize(cross(_136.constants.CameraFront, trailDirection));
    position = (trailTangent * position.x) * _121.paramData.ShapeSize;
    position += trailPosition;
}

vec4 UnpackColor(uint color32)
{
    return vec4(float(color32 & 255u), float((color32 >> uint(8)) & 255u), float((color32 >> uint(16)) & 255u), float((color32 >> uint(24)) & 255u)) / vec4(255.0);
}

VS_Output _main(VS_Input _input)
{
    uint index = _265.emitter.ParticleHead + _input.InstanceID;
    ParticleData particle;
    particle.FlagBits = Particles_1._data[index].FlagBits;
    particle.Seed = Particles_1._data[index].Seed;
    particle.LifeAge = Particles_1._data[index].LifeAge;
    particle.InheritColor = Particles_1._data[index].InheritColor;
    particle.Color = Particles_1._data[index].Color;
    particle.Direction = Particles_1._data[index].Direction;
    particle.Velocity = Particles_1._data[index].Velocity;
    particle.Transform = Particles_1._data[index].Transform;
    VS_Output _output;
    if ((particle.FlagBits & 1u) != 0u)
    {
        vec3 position = _input.Pos;
        vec2 uv = _input.UV;
        vec4 color = _input.Color;
        if (_121.paramData.ShapeType == 0u)
        {
            ParticleData param = particle;
            vec3 param_1 = position;
            transformSprite(param, param_1);
            position = param_1;
        }
        else
        {
            if (_121.paramData.ShapeType == 1u)
            {
                ParticleData param_2 = particle;
                vec3 param_3 = position;
                transformModel(param_2, param_3);
                position = param_3;
            }
            else
            {
                if (_121.paramData.ShapeType == 2u)
                {
                    ParticleData param_4 = particle;
                    vec3 param_5 = position;
                    vec2 param_6 = uv;
                    uint param_7 = _input.InstanceID;
                    uint param_8 = _input.VertexID;
                    transformTrail(param_4, param_5, param_6, param_7, param_8);
                    position = param_5;
                    uv = param_6;
                }
            }
        }
        uint param_9 = particle.Color;
        color *= UnpackColor(param_9);
        vec4 _432 = color;
        vec3 _434 = _432.xyz * _121.paramData.Emissive;
        color.x = _434.x;
        color.y = _434.y;
        color.z = _434.z;
        _output.Pos = (vec4(position, 1.0) * _136.constants.CameraMat) * _136.constants.ProjMat;
        _output.UV = uv;
        _output.Color = color;
        if (_121.paramData.MaterialType == 1u)
        {
            _output.WorldN = particle.Transform * vec4(_input.Normal, 0.0);
            _output.WorldB = particle.Transform * vec4(_input.Binormal, 0.0);
            _output.WorldT = particle.Transform * vec4(_input.Tangent, 0.0);
        }
    }
    else
    {
        _output.Pos = vec4(0.0);
        _output.UV = vec2(0.0);
        _output.Color = vec4(0.0);
        if (_121.paramData.MaterialType == 1u)
        {
            _output.WorldN = vec3(0.0);
            _output.WorldB = vec3(0.0);
            _output.WorldT = vec3(0.0);
        }
    }
    return _output;
}

void main()
{
    VS_Input _input;
    _input.Pos = input_Pos;
    _input.Normal = input_Normal;
    _input.Binormal = input_Binormal;
    _input.Tangent = input_Tangent;
    _input.UV = input_UV;
    _input.Color = input_Color;
    _input.InstanceID = uint(gl_InstanceIndex);
    _input.VertexID = uint(gl_VertexIndex);
    VS_Output flattenTemp = _main(_input);
    vec4 _position = flattenTemp.Pos;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_WorldN = flattenTemp.WorldN;
    _entryPointOutput_WorldB = flattenTemp.WorldB;
    _entryPointOutput_WorldT = flattenTemp.WorldT;
}


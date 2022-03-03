float2 UVDistortionOffset(const Texture2D t, const SamplerState s, float2 uv, float2 uvInversed)
{
    float2 UVOffset = t.Sample(s, uv).rg * 2.0 - 1.0;
    UVOffset.y *= -1.0;
    UVOffset.y = uvInversed.x + uvInversed.y * UVOffset.y;
    return UVOffset;
}
void ApplyTextureBlending(inout float4 dstColor, const float4 blendColor, const float blendType)
{
    // alpha blend
    if(blendType.x == 0)
    {
        dstColor.rgb = blendColor.a * blendColor.rgb + (1.0 - blendColor.a) * dstColor.rgb;
    }
    // add
    else if(blendType.x == 1)
    {
        dstColor.rgb += blendColor.rgb * blendColor.a;
    }
    // sub
    else if(blendType.x == 2)
    {
       dstColor.rgb -= blendColor.rgb * blendColor.a;
    }
    // mul
    else if(blendType.x == 3)
    {
       dstColor.rgb *= blendColor.rgb * blendColor.a;
    }
}

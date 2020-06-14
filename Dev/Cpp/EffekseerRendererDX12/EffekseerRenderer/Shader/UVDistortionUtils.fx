float2 GetPixelSize(Texture2D Texture)
{
    float2 size;
    float level;
    Texture.GetDimensions(0, size.x, size.y, level);

    return 1.0 / size;
}

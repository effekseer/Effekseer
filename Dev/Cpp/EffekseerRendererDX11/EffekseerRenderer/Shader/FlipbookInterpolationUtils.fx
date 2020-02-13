
float2 GetFlipbookOneSizeUV(float DivideX, float DivideY)
{
    return (float2(1.0, 1.0) / float2(DivideX, DivideY));
}

float2 GetFlipbookOriginUV(float2 FlipbookUV, float FlipbookIndex, float DivideX, float DivideY)
{
    float2 DivideIndex;
    DivideIndex.x = int(FlipbookIndex) % int(DivideX);
    DivideIndex.y = int(FlipbookIndex) / int(DivideY);

    float2 FlipbookOneSize = GetFlipbookOneSizeUV(DivideX, DivideY);
    float2 UVOffset = DivideIndex * FlipbookOneSize;
    
    float2 OriginUV = FlipbookUV - UVOffset;
    OriginUV *= float2(DivideX, DivideY);
    
    return OriginUV;
}

float2 GetFlipbookUVForIndex(float2 OriginUV, float Index, float DivideX, float DivideY)
{
    float2 DivideIndex;
    DivideIndex.x = int(Index) % int(DivideX);
    DivideIndex.y = int(Index) / int(DivideY);

    float2 FlipbookOneSize = GetFlipbookOneSizeUV(DivideX, DivideY);
    
    return (OriginUV * FlipbookOneSize) + (DivideIndex * FlipbookOneSize);
}

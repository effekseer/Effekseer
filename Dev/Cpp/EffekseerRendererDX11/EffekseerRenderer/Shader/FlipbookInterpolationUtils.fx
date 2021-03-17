
#ifdef __OPENGL2__
float IntMod(float x, float y)
{
	return floor(fmod(x, y));
}
#endif

float2 GetFlipbookOneSizeUV(float DivideX, float DivideY)
{
    return (float2(1.0, 1.0) / float2(DivideX, DivideY));
}

float2 GetFlipbookOriginUV(float2 FlipbookUV, float FlipbookIndex, float DivideX, float DivideY)
{
    float2 DivideIndex;

#ifdef __OPENGL2__
	DivideIndex.x = IntMod(FlipbookIndex, DivideX);
#else
	DivideIndex.x = int(FlipbookIndex) % int(DivideX);
#endif
	DivideIndex.y = int(FlipbookIndex) / int(DivideX);

    float2 FlipbookOneSize = GetFlipbookOneSizeUV(DivideX, DivideY);
    float2 UVOffset = DivideIndex * FlipbookOneSize;
    
    float2 OriginUV = FlipbookUV - UVOffset;
    OriginUV *= float2(DivideX, DivideY);
    
    return OriginUV;
}

float2 GetFlipbookUVForIndex(float2 OriginUV, float Index, float DivideX, float DivideY)
{
    float2 DivideIndex;
#ifdef __OPENGL2__
	DivideIndex.x = IntMod(Index, DivideX);
#else
	DivideIndex.x = int(Index) % int(DivideX);
#endif
    DivideIndex.y = int(Index) / int(DivideX);

    float2 FlipbookOneSize = GetFlipbookOneSizeUV(DivideX, DivideY);
    
    return (OriginUV * FlipbookOneSize) + (DivideIndex * FlipbookOneSize);
}

void ApplyFlipbookVS(inout float flipbookRate, inout float2 flipbookUV, float4 flipbookParameter, float flipbookIndex, float2 uv)
{
	if (flipbookParameter.x > 0)
	{
		flipbookRate = frac(flipbookIndex);

		float Index = floor(flipbookIndex);
		float IndexOffset = 1.0;

		float NextIndex = Index + IndexOffset;
        
        float FlipbookMaxCount = (flipbookParameter.z * flipbookParameter.w);

		// loop none
		if (flipbookParameter.y == 0)
		{
			if (NextIndex >= FlipbookMaxCount)
			{
				NextIndex = FlipbookMaxCount - 1;
				Index = FlipbookMaxCount - 1;
			}
		}
		// loop
		else if (flipbookParameter.y == 1)
		{
            Index %= FlipbookMaxCount;
			NextIndex %= FlipbookMaxCount;
		}
		// loop reverse
		else if (flipbookParameter.y == 2)
		{
            bool Reverse = floor(Index / FlipbookMaxCount) % 2 == 1;
			Index %= FlipbookMaxCount;
            if (Reverse)
			{
				Index = FlipbookMaxCount - 1 - floor(Index);
			}
            
            Reverse = floor(NextIndex / FlipbookMaxCount) % 2 == 1;
			NextIndex %= FlipbookMaxCount;
            if (Reverse)
			{
				NextIndex = FlipbookMaxCount - 1 - floor(NextIndex);
			}
		}

		float2 OriginUV = GetFlipbookOriginUV(uv, Index, flipbookParameter.z, flipbookParameter.w);
		flipbookUV = GetFlipbookUVForIndex(OriginUV, NextIndex, flipbookParameter.z, flipbookParameter.w);
	}
}

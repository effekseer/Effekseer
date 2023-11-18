
#ifdef __OPENGL2__
float IntMod(float x, float y)
{
	return floor(fmod(x, y));
}
#endif

float2 GetFlipbookOriginUV(float2 FlipbookUV, float FlipbookIndex, float DivideX, float2 flipbookOneSize, float2 flipbookOffset)
{
    float2 DivideIndex;

#ifdef __OPENGL2__
	DivideIndex.x = IntMod(FlipbookIndex, DivideX);
#else
	DivideIndex.x = int(FlipbookIndex) % int(DivideX);
#endif
	DivideIndex.y = int(FlipbookIndex) / int(DivideX);

	float2 UVOffset = DivideIndex * flipbookOneSize + flipbookOffset;
    return FlipbookUV - UVOffset;
}

float2 GetFlipbookUVForIndex(float2 OriginUV, float Index, float DivideX, float2 flipbookOneSize, float2 flipbookOffset)
{
    float2 DivideIndex;
#ifdef __OPENGL2__
	DivideIndex.x = IntMod(Index, DivideX);
#else
	DivideIndex.x = int(Index) % int(DivideX);
#endif
    DivideIndex.y = int(Index) / int(DivideX);

    return OriginUV + DivideIndex * flipbookOneSize + flipbookOffset;
}

void ApplyFlipbookVS(inout float flipbookRate, inout float2 flipbookUV, float4 flipbookParameter1, float4 flipbookParameter2, float flipbookIndex, float2 uv, float2 uvInversed)
{
	const float flipbookEnabled = flipbookParameter1.x;
	const float flipbookLoopType = flipbookParameter1.y;
	const float divideX = flipbookParameter1.z;
	const float divideY = flipbookParameter1.w;

	const float2 flipbookOneSize = flipbookParameter2.xy;
	const float2 flipbookOffset = flipbookParameter2.zw;

	if (flipbookEnabled > 0)
	{
		flipbookRate = frac(flipbookIndex);

		float Index = floor(flipbookIndex);
		float IndexOffset = 1.0;

		float NextIndex = Index + IndexOffset;
        
        float FlipbookMaxCount = (divideX * divideY);

		// loop none
		if (flipbookLoopType == 0)
		{
			if (NextIndex >= FlipbookMaxCount)
			{
				NextIndex = FlipbookMaxCount - 1;
				Index = FlipbookMaxCount - 1;
			}
		}
		// loop
		else if (flipbookLoopType == 1)
		{
            Index %= FlipbookMaxCount;
			NextIndex %= FlipbookMaxCount;
		}
		// loop reverse
		else if (flipbookLoopType == 2)
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

		float2 notInversedUV = uv;
		notInversedUV.y = uvInversed.x + uvInversed.y * notInversedUV.y;
		float2 OriginUV = GetFlipbookOriginUV(notInversedUV, Index, divideX, flipbookOneSize, flipbookOffset);
		flipbookUV = GetFlipbookUVForIndex(OriginUV, NextIndex, divideX, flipbookOneSize, flipbookOffset);
		flipbookUV.y = uvInversed.x + uvInversed.y * flipbookUV.y;
	}
}


#ifndef __EFFEKSEER_PARAMETERS_H__
#define __EFFEKSEER_PARAMETERS_H__

#include "../Effekseer.Base.Pre.h"
#include "../Effekseer.InternalStruct.h"

namespace Effekseer
{

/**!
	@brief indexes of dynamic parameter
*/
struct RefMinMax
{
	int32_t Max = -1;
	int32_t Min = -1;
};

/**
	@brief
	\~English	How to treat an uv of texture type for ribbon and track
	\~Japanese	リボンと軌跡向けのテクスチャのUVの扱い方
*/
enum class TextureUVType : int32_t
{
	Strech,
	Tile,
};

/**
	@brief	\~english	uv texture parameters which is passed into a renderer
			\~japanese	レンダラーに渡されるUVTextureに関するパラメーター
*/
struct NodeRendererTextureUVTypeParameter
{
	TextureUVType Type = TextureUVType::Strech;
	float TileLength = 0.0f;
	int32_t TileEdgeHead = 0;
	int32_t TileEdgeTail = 0;
	float TileLoopAreaBegin = 0.0f;
	float TileLoopAreaEnd = 0.0f;

	void Load(uint8_t*& pos, int32_t version);
};

//! calculate dynamic equation and assign a result
void ApplyEq(float& dstParam, Effect* e, InstanceGlobal* instg, Instance* parrentInstance, IRandObject* rand, int dpInd, const float& originalParam);

//! calculate dynamic equation and return a result
Vec3f ApplyEq(Effect* e,
			  InstanceGlobal* instg,
			  Instance* parrentInstance,
			  IRandObject* rand,
			  const int& dpInd,
			  const Vec3f& originalParam,
			  const std::array<float, 3>& scale,
			  const std::array<float, 3>& scaleInv);

//! calculate dynamic equation and return a result
random_float ApplyEq(Effect* e, InstanceGlobal* instg, Instance* parrentInstance, IRandObject* rand, const RefMinMax& dpInd, random_float originalParam);

//! calculate dynamic equation and return a result
random_vector3d ApplyEq(Effect* e,
						InstanceGlobal* instg,
						Instance* parrentInstance,
						IRandObject* rand,
						const RefMinMax& dpInd,
						random_vector3d originalParam,
						const std::array<float, 3>& scale,
						const std::array<float, 3>& scaleInv);

//! calculate dynamic equation and return a result
random_int ApplyEq(Effect* e, InstanceGlobal* instg, Instance* parrentInstance, IRandObject* rand, const RefMinMax& dpInd, random_int originalParam);

} // namespace Effekseer

#endif // __EFFEKSEER_PARAMETERS_H__
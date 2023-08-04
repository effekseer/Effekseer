#ifndef __EFFEKSEERRENDERER_FLAGS_H__
#define __EFFEKSEERRENDERER_FLAGS_H__

namespace EffekseerRenderer
{
enum class RendererShaderType
{
	Unlit,
	Lit,
	BackDistortion,
	AdvancedUnlit,
	AdvancedLit,
	AdvancedBackDistortion,
	Material,
};
}

#endif
#ifndef __EFFEKSEER_EXTERNALMODEL_H__
#define __EFFEKSEER_EXTERNALMODEL_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Matrix43.h"
#include "Model/Effekseer.Model.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

struct ExternalModel
{
	ModelRef Model;
	Matrix43 Transform;

	ExternalModel()
	{
		Transform.Indentity();
	}

	ExternalModel(const ModelRef& model, const Matrix43& transform)
		: Model(model)
		, Transform(transform)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_EXTERNALMODEL_H__

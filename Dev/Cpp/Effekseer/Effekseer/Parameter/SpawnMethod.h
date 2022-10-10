#pragma once

#include "../Effekseer.Base.h"
#include "../Effekseer.InternalStruct.h"
#include "../Model/Model.h"
#include "../Utils/BinaryVersion.h"

#include <stdint.h>

namespace Effekseer
{
struct ParameterGenerationLocation
{
	int EffectsRotation;

	enum class AxisType : int32_t
	{
		X,
		Y,
		Z,
	};

	enum
	{
		TYPE_POINT = 0,
		TYPE_SPHERE = 1,
		TYPE_MODEL = 2,
		TYPE_CIRCLE = 3,
		TYPE_LINE = 4,

		TYPE_DWORD = 0x7fffffff,
	} type;

	enum eModelType
	{
		MODELTYPE_RANDOM = 0,
		MODELTYPE_VERTEX = 1,
		MODELTYPE_VERTEX_RANDOM = 2,
		MODELTYPE_FACE = 3,
		MODELTYPE_FACE_RANDOM = 4,

		MODELTYPE_DWORD = 0x7fffffff,
	};

	enum eCircleType
	{
		CIRCLE_TYPE_RANDOM = 0,
		CIRCLE_TYPE_ORDER = 1,
		CIRCLE_TYPE_REVERSE_ORDER = 2,
	};

	enum class LineType : int32_t
	{
		Random = 0,
		Order = 1,
	};

	union
	{
		struct
		{
			random_vector3d location;
		} point;

		struct
		{
			random_float radius;
			random_float rotation_x;
			random_float rotation_y;
		} sphere;

		struct
		{
			ModelReferenceType Reference;
			int32_t index;
			eModelType type;
		} model;

		struct
		{
			int32_t division;
			random_float radius;
			random_float angle_start;
			random_float angle_end;
			eCircleType type;
			AxisType axisDirection;
			random_float angle_noize;
		} circle;

		struct
		{
			int32_t division;
			random_vector3d position_start;
			random_vector3d position_end;
			random_float position_noize;
			LineType type;
		} line;
	};

	void MakeCoordinateSystemLH()
	{
		auto& GenerationLocation = *this;

		if (GenerationLocation.type == ParameterGenerationLocation::TYPE_POINT)
		{
			GenerationLocation.point.location.min.z *= -1.0f;
			GenerationLocation.point.location.max.z *= -1.0f;
		}
		else if (GenerationLocation.type == ParameterGenerationLocation::TYPE_LINE)
		{
			GenerationLocation.line.position_start.min.z *= -1.0f;
			GenerationLocation.line.position_start.max.z *= -1.0f;
			GenerationLocation.line.position_end.min.z *= -1.0f;
			GenerationLocation.line.position_end.max.z *= -1.0f;
		}
		else if (GenerationLocation.type == ParameterGenerationLocation::TYPE_CIRCLE)
		{
			if (GenerationLocation.circle.axisDirection == ParameterGenerationLocation::AxisType::X)
			{
				GenerationLocation.circle.angle_start.min *= -1.0f;
				GenerationLocation.circle.angle_start.max *= -1.0f;
				GenerationLocation.circle.angle_end.min *= -1.0f;
				GenerationLocation.circle.angle_end.max *= -1.0f;

				GenerationLocation.circle.angle_noize.min *= -1.0f;
				GenerationLocation.circle.angle_noize.max *= -1.0f;

				GenerationLocation.circle.radius.min *= -1.0f;
				GenerationLocation.circle.radius.max *= -1.0f;
			}
			else if (GenerationLocation.circle.axisDirection == ParameterGenerationLocation::AxisType::Y)
			{
				GenerationLocation.circle.angle_start.min *= -1.0f;
				GenerationLocation.circle.angle_start.max *= -1.0f;
				GenerationLocation.circle.angle_end.min *= -1.0f;
				GenerationLocation.circle.angle_end.max *= -1.0f;

				GenerationLocation.circle.angle_noize.min *= -1.0f;
				GenerationLocation.circle.angle_noize.max *= -1.0f;
			}
		}
		else if (GenerationLocation.type == ParameterGenerationLocation::TYPE_SPHERE)
		{
			GenerationLocation.sphere.rotation_x.max *= -1.0f;
			GenerationLocation.sphere.rotation_x.min *= -1.0f;
			GenerationLocation.sphere.rotation_y.max *= -1.0f;
			GenerationLocation.sphere.rotation_y.min *= -1.0f;
		}
	}

	void load(uint8_t*& pos, int32_t version)
	{
		memcpy(&EffectsRotation, pos, sizeof(int));
		pos += sizeof(int);

		memcpy(&type, pos, sizeof(int));
		pos += sizeof(int);

		if (type == TYPE_POINT)
		{
			memcpy(&point, pos, sizeof(point));
			pos += sizeof(point);
		}
		else if (type == TYPE_SPHERE)
		{
			memcpy(&sphere, pos, sizeof(sphere));
			pos += sizeof(sphere);
		}
		else if (type == TYPE_MODEL)
		{
			model.Reference = ModelReferenceType::File;

			if (version >= Version16Alpha3)
			{
				memcpy(&model.Reference, pos, sizeof(int32_t));
				pos += sizeof(int32_t);
			}

			memcpy(&model.index, pos, sizeof(int32_t));
			pos += sizeof(int32_t);

			memcpy(&model.type, pos, sizeof(int32_t));
			pos += sizeof(int32_t);
		}
		else if (type == TYPE_CIRCLE)
		{
			if (version < 10)
			{
				memcpy(&circle, pos, sizeof(circle) - sizeof(circle.axisDirection) - sizeof(circle.angle_noize));
				pos += sizeof(circle) - sizeof(circle.axisDirection) - sizeof(circle.angle_noize);
				circle.axisDirection = AxisType::Z;
				circle.angle_noize.max = 0;
				circle.angle_noize.min = 0;
			}
			else
			{
				memcpy(&circle, pos, sizeof(circle));
				pos += sizeof(circle);
			}
		}
		else if (type == TYPE_LINE)
		{
			memcpy(&line, pos, sizeof(line));
			pos += sizeof(line);
		}
	}

	SIMD::Mat43f GenerateGenerationPosition(const Effect& effect, int instanceNumber, int parentTime, float magnification, CoordinateSystem coordinateSystem, RandObject& rand)
	{
		const auto& param = *this;

		SIMD::Mat43f ret;

		if (param.type == ParameterGenerationLocation::TYPE_POINT)
		{
			SIMD::Vec3f p = param.point.location.getValue(rand);
			ret = SIMD::Mat43f::Translation(p.GetX(), p.GetY(), p.GetZ());
		}
		else if (param.type == ParameterGenerationLocation::TYPE_LINE)
		{
			SIMD::Vec3f s = param.line.position_start.getValue(rand);
			SIMD::Vec3f e = param.line.position_end.getValue(rand);
			auto noize = param.line.position_noize.getValue(rand);
			auto division = Max(1, param.line.division);

			SIMD::Vec3f dir = e - s;

			if (dir.IsZero())
			{
				ret = SIMD::Mat43f::Translation(0, 0, 0);
			}
			else
			{
				auto len = dir.GetLength();
				dir /= len;

				int32_t target = 0;
				if (param.line.type == ParameterGenerationLocation::LineType::Order)
				{
					target = instanceNumber % division;
				}
				else if (param.line.type == ParameterGenerationLocation::LineType::Random)
				{
					target = (int32_t)((division)*rand.GetRand());
					if (target == division)
						target -= 1;
				}

				auto d = 0.0f;
				if (division > 1)
				{
					d = (len / (float)(division - 1)) * target;
				}

				d += noize;

				s += dir * d;

				SIMD::Vec3f xdir;
				SIMD::Vec3f ydir;
				SIMD::Vec3f zdir;

				if (fabs(dir.GetY()) > 0.999f)
				{
					xdir = dir;
					zdir = SIMD::Vec3f::Cross(xdir, SIMD::Vec3f(-1, 0, 0)).Normalize();
					ydir = SIMD::Vec3f::Cross(zdir, xdir).Normalize();
				}
				else
				{
					xdir = dir;
					ydir = SIMD::Vec3f::Cross(SIMD::Vec3f(0, 0, 1), xdir).Normalize();
					zdir = SIMD::Vec3f::Cross(xdir, ydir).Normalize();
				}

				if (param.EffectsRotation)
				{
					ret.X.SetX(xdir.GetX());
					ret.Y.SetX(xdir.GetY());
					ret.Z.SetX(xdir.GetZ());

					ret.X.SetY(ydir.GetX());
					ret.Y.SetY(ydir.GetY());
					ret.Z.SetY(ydir.GetZ());

					ret.X.SetZ(zdir.GetX());
					ret.Y.SetZ(zdir.GetY());
					ret.Z.SetZ(zdir.GetZ());
				}
				else
				{
					ret = SIMD::Mat43f::Identity;
				}

				ret.X.SetW(s.GetX());
				ret.Y.SetW(s.GetY());
				ret.Z.SetW(s.GetZ());
			}
		}
		else if (param.type == ParameterGenerationLocation::TYPE_SPHERE)
		{
			SIMD::Mat43f mat_x = SIMD::Mat43f::RotationX(param.sphere.rotation_x.getValue(rand));
			SIMD::Mat43f mat_y = SIMD::Mat43f::RotationY(param.sphere.rotation_y.getValue(rand));
			float r = param.sphere.radius.getValue(rand);
			ret = SIMD::Mat43f::Translation(0, r, 0) * mat_x * mat_y;
		}
		else if (param.type == ParameterGenerationLocation::TYPE_MODEL)
		{
			ret = SIMD::Mat43f::Identity;
			ModelRef modelRef = nullptr;
			const ParameterGenerationLocation::eModelType modelType = param.model.type;

			if (param.model.Reference == ModelReferenceType::File)
			{
				modelRef = effect.GetModel(param.model.index);
			}
			else if (param.model.Reference == ModelReferenceType::Procedural)
			{
				modelRef = effect.GetProceduralModel(param.model.index);
			}

			{
				if (modelRef != nullptr)
				{
					Model::Emitter emitter;

					if (modelType == ParameterGenerationLocation::MODELTYPE_RANDOM)
					{
						emitter = modelRef->GetEmitter(&rand,
													   parentTime,
													   coordinateSystem,
													   magnification);
					}
					else if (modelType == ParameterGenerationLocation::MODELTYPE_VERTEX)
					{
						emitter = modelRef->GetEmitterFromVertex(instanceNumber,
																 parentTime,
																 coordinateSystem,
																 magnification);
					}
					else if (modelType == ParameterGenerationLocation::MODELTYPE_VERTEX_RANDOM)
					{
						emitter = modelRef->GetEmitterFromVertex(&rand,
																 parentTime,
																 coordinateSystem,
																 magnification);
					}
					else if (modelType == ParameterGenerationLocation::MODELTYPE_FACE)
					{
						emitter = modelRef->GetEmitterFromFace(instanceNumber,
															   parentTime,
															   coordinateSystem,
															   magnification);
					}
					else if (modelType == ParameterGenerationLocation::MODELTYPE_FACE_RANDOM)
					{
						emitter = modelRef->GetEmitterFromFace(&rand,
															   parentTime,
															   coordinateSystem,
															   magnification);
					}

					ret = SIMD::Mat43f::Translation(emitter.Position);

					if (param.EffectsRotation)
					{
						ret.X.SetX(emitter.Binormal.X);
						ret.Y.SetX(emitter.Binormal.Y);
						ret.Z.SetX(emitter.Binormal.Z);

						ret.X.SetY(emitter.Tangent.X);
						ret.Y.SetY(emitter.Tangent.Y);
						ret.Z.SetY(emitter.Tangent.Z);

						ret.X.SetZ(emitter.Normal.X);
						ret.Y.SetZ(emitter.Normal.Y);
						ret.Z.SetZ(emitter.Normal.Z);
					}
				}
			}
		}
		else if (param.type == ParameterGenerationLocation::TYPE_CIRCLE)
		{
			ret = SIMD::Mat43f::Identity;
			float radius = param.circle.radius.getValue(rand);
			float start = param.circle.angle_start.getValue(rand);
			float end = param.circle.angle_end.getValue(rand);
			int32_t div = Max(param.circle.division, 1);

			int32_t target = 0;
			if (param.circle.type == ParameterGenerationLocation::CIRCLE_TYPE_ORDER)
			{
				target = instanceNumber % div;
			}
			else if (param.circle.type == ParameterGenerationLocation::CIRCLE_TYPE_REVERSE_ORDER)
			{
				target = div - 1 - (instanceNumber % div);
			}
			else if (param.circle.type == ParameterGenerationLocation::CIRCLE_TYPE_RANDOM)
			{
				target = (int32_t)((div)*rand.GetRand());
				if (target == div)
					target -= 1;
			}

			float angle = (end - start) * ((float)target / (float)div) + start;

			angle += param.circle.angle_noize.getValue(rand);

			switch (param.circle.axisDirection)
			{
			case ParameterGenerationLocation::AxisType::X:
				ret = SIMD::Mat43f::Translation(0, 0, radius) * SIMD::Mat43f::RotationX(angle);
				break;
			case ParameterGenerationLocation::AxisType::Y:
				ret = SIMD::Mat43f::Translation(radius, 0, 0) * SIMD::Mat43f::RotationY(angle);
				break;
			case ParameterGenerationLocation::AxisType::Z:
				ret = SIMD::Mat43f::Translation(0, radius, 0) * SIMD::Mat43f::RotationZ(angle);
				break;
			}
		}
		else
		{
			ret = SIMD::Mat43f::Identity;
		}

		return ret;
	}
};
} // namespace Effekseer
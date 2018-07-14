
#ifndef	__EFFEKSEERRENDERER_RIBBON_RENDERER_BASE_H__
#define	__EFFEKSEERRENDERER_RIBBON_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <string.h>

#include "EffekseerRenderer.CommonUtils.h"
#include "EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRenderer.VertexBufferBase.h"
#include "EffekseerRenderer.IndexBufferBase.h"
#include "EffekseerRenderer.StandardRenderer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	typedef ::Effekseer::RibbonRenderer::NodeParameter efkRibbonNodeParam;
	typedef ::Effekseer::RibbonRenderer::InstanceParameter efkRibbonInstanceParam;
	typedef ::Effekseer::Vector3D efkVector3D;

	template<typename RENDERER, typename VERTEX_NORMAL, typename VERTEX_DISTORTION>
	class RibbonRendererBase
		: public ::Effekseer::RibbonRenderer
	{
	private:

		/**
		@brief Spline generator
		@note
		Reference https://qiita.com/edo_m18/items/f2f0c6bf9032b0ec12d4
		*/
		class Spline
		{
			std::vector<efkVector3D>	a;
			std::vector<efkVector3D>	b;
			std::vector<efkVector3D>	c;
			std::vector<efkVector3D>	d;
			std::vector<efkVector3D>	w;
			std::vector<bool>			isSame;

		public:

			void AddVertex(const efkVector3D& v)
			{
				a.push_back(v);
				if (a.size() >= 2)
				{
					isSame.push_back(a[a.size() - 1] == a[a.size() - 2]);
				}
			}

			void Calculate()
			{
				b.resize(a.size());
				c.resize(a.size());
				d.resize(a.size());
				w.resize(a.size());

				for (auto i = 1; i < a.size() - 1; i++)
				{
					c[i] = (a[i - 1] + a[i] * (-2.0) + a[i + 1]) * 3.0;
				}

				for (auto i = 1; i < a.size() - 1; i++)
				{
					auto tmp = efkVector3D(4.0, 4.0, 4.0) - w[i - 1];
					c[i] = (c[i] - c[i - 1]) / tmp;
					w[i] = efkVector3D(1.0, 1.0, 1.0) / tmp;
				}

				for (auto i = (a.size() - 1) - 1; i > 0; i--)
				{
					c[i] = c[i] - c[i + 1] * w[i];
				}

				for (auto i = 0; i < a.size() - 1; i++)
				{
					d[i] = (c[i + 1] - c[i]) / 3.0;
					b[i] = a[i + 1] - a[i] - c[i] - d[i];
				}
			}

			void Reset()
			{
				a.clear();
				b.clear();
				c.clear();
				d.clear();
				w.clear();
				isSame.clear();
			}

			efkVector3D GetValue(float t)
			{
				auto j = floorf(t);

				if (j < 0)
				{
					j = 0;
				}

				if (j > a.size())
				{
					j = a.size() - 1;
				}

				auto dt = t - j;

				if (j < isSame.size() && isSame[j]) return a[j];

				return a[j] + (b[j] + (c[j] + d[j] * dt) * dt) * dt;
			}
		};

	protected:
		RENDERER*						m_renderer;
		int32_t							m_ribbonCount;

		int32_t							m_ringBufferOffset;
		uint8_t*						m_ringBufferData;

		efkRibbonNodeParam					innstancesNodeParam;
		std::vector<efkRibbonInstanceParam>	instances;
		Spline								spline_left;
		Spline								spline_right;

		template<typename VERTEX>
		void RenderSplines(const ::Effekseer::Matrix44& camera)
		{
			auto& parameter = innstancesNodeParam;

			VERTEX* verteies0 = (VERTEX*)m_ringBufferData;

			// Calculate spline
			if (parameter.SplineDivision > 1)
			{
				spline_left.Reset();
				spline_right.Reset();

				for (auto loop = 0; loop < instances.size(); loop++)
				{
					auto pl = efkVector3D();
					auto pr = efkVector3D();

					auto& param = instances[loop];

					pl.X = param.Positions[0];
					pl.Y = 0.0f;
					pl.Z = 0.0f;

					pr.X = param.Positions[1];
					pr.Y = 0.0f;
					pr.Z = 0.0f;

					if (parameter.ViewpointDependent)
					{
						const ::Effekseer::Matrix43& mat = param.SRTMatrix43;
						::Effekseer::Vector3D s;
						::Effekseer::Matrix43 r;
						::Effekseer::Vector3D t;
						mat.GetSRT(s, r, t);

						// extend
						pl.X = pl.X * s.X;
						pr.X = pr.X * s.X;

						::Effekseer::Vector3D F;
						::Effekseer::Vector3D R;
						::Effekseer::Vector3D U;

						U = ::Effekseer::Vector3D(r.Value[1][0], r.Value[1][1], r.Value[1][2]);

						::Effekseer::Vector3D::Normal(F, -m_renderer->GetCameraFrontDirection());

						::Effekseer::Vector3D::Normal(R, ::Effekseer::Vector3D::Cross(R, U, F));
						::Effekseer::Vector3D::Normal(F, ::Effekseer::Vector3D::Cross(F, R, U));

						::Effekseer::Matrix43 mat_rot;

						mat_rot.Value[0][0] = -R.X;
						mat_rot.Value[0][1] = -R.Y;
						mat_rot.Value[0][2] = -R.Z;
						mat_rot.Value[1][0] = U.X;
						mat_rot.Value[1][1] = U.Y;
						mat_rot.Value[1][2] = U.Z;
						mat_rot.Value[2][0] = F.X;
						mat_rot.Value[2][1] = F.Y;
						mat_rot.Value[2][2] = F.Z;
						mat_rot.Value[3][0] = t.X;
						mat_rot.Value[3][1] = t.Y;
						mat_rot.Value[3][2] = t.Z;


						::Effekseer::Vector3D::Transform(
							pl,
							pl,
							mat_rot);

						::Effekseer::Vector3D::Transform(
							pr,
							pr,
							mat_rot);

						spline_left.AddVertex(pl);
						spline_right.AddVertex(pr);
					}
					else
					{
						::Effekseer::Vector3D::Transform(
							pl,
							pl,
							param.SRTMatrix43);

						::Effekseer::Vector3D::Transform(
							pr,
							pr,
							param.SRTMatrix43);

						spline_left.AddVertex(pl);
						spline_right.AddVertex(pr);
					}
				}

				spline_left.Calculate();
				spline_right.Calculate();
			}


			for (auto loop = 0; loop < instances.size(); loop++)
			{
				auto& param = instances[loop];

				for (auto sploop = 0; sploop < parameter.SplineDivision; sploop++)
				{
					bool isFirst = param.InstanceIndex == 0 && sploop == 0;
					bool isLast = param.InstanceIndex == (param.InstanceCount - 1);

					VERTEX* verteies = (VERTEX*)m_ringBufferData;

					float percent_instance = sploop / (float)parameter.SplineDivision;

					if (parameter.SplineDivision > 1)
					{
						verteies[0].Pos = spline_left.GetValue(param.InstanceIndex + sploop / (float)parameter.SplineDivision);
						verteies[1].Pos = spline_right.GetValue(param.InstanceIndex + sploop / (float)parameter.SplineDivision);

						verteies[0].SetColor(Effekseer::Color::Lerp(param.Colors[0], param.Colors[2], percent_instance));
						verteies[1].SetColor(Effekseer::Color::Lerp(param.Colors[1], param.Colors[3], percent_instance));
					}
					else
					{
						for (int i = 0; i < 2; i++)
						{
							verteies[i].Pos.X = param.Positions[i];
							verteies[i].Pos.Y = 0.0f;
							verteies[i].Pos.Z = 0.0f;
							verteies[i].SetColor(param.Colors[i]);
						}
					}


					float percent = (float)(param.InstanceIndex  * parameter.SplineDivision + sploop) / (float)((param.InstanceCount - 1) * parameter.SplineDivision);

					verteies[0].UV[0] = param.UV.X;
					verteies[0].UV[1] = param.UV.Y + percent * param.UV.Height;

					verteies[1].UV[0] = param.UV.X + param.UV.Width;
					verteies[1].UV[1] = param.UV.Y + percent * param.UV.Height;

					if (parameter.ViewpointDependent)
					{
						const ::Effekseer::Matrix43& mat = param.SRTMatrix43;
						::Effekseer::Vector3D s;
						::Effekseer::Matrix43 r;
						::Effekseer::Vector3D t;
						mat.GetSRT(s, r, t);

						if (parameter.SplineDivision > 1)
						{
						}
						else
						{
							for (int i = 0; i < 2; i++)
							{
								verteies[i].Pos.X = verteies[i].Pos.X * s.X;
							}

							::Effekseer::Vector3D F;
							::Effekseer::Vector3D R;
							::Effekseer::Vector3D U;

							U = ::Effekseer::Vector3D(r.Value[1][0], r.Value[1][1], r.Value[1][2]);

							::Effekseer::Vector3D::Normal(F, -m_renderer->GetCameraFrontDirection());

							::Effekseer::Vector3D::Normal(R, ::Effekseer::Vector3D::Cross(R, U, F));
							::Effekseer::Vector3D::Normal(F, ::Effekseer::Vector3D::Cross(F, R, U));

							::Effekseer::Matrix43 mat_rot;

							mat_rot.Value[0][0] = -R.X;
							mat_rot.Value[0][1] = -R.Y;
							mat_rot.Value[0][2] = -R.Z;
							mat_rot.Value[1][0] = U.X;
							mat_rot.Value[1][1] = U.Y;
							mat_rot.Value[1][2] = U.Z;
							mat_rot.Value[2][0] = F.X;
							mat_rot.Value[2][1] = F.Y;
							mat_rot.Value[2][2] = F.Z;
							mat_rot.Value[3][0] = t.X;
							mat_rot.Value[3][1] = t.Y;
							mat_rot.Value[3][2] = t.Z;

							for (int i = 0; i < 2; i++)
							{
								::Effekseer::Vector3D::Transform(
									verteies[i].Pos,
									verteies[i].Pos,
									mat_rot);
							}
						}
					}
					else
					{
						if (parameter.SplineDivision > 1)
						{
						}
						else
						{
							for (int i = 0; i < 2; i++)
							{
								::Effekseer::Vector3D::Transform(
									verteies[i].Pos,
									verteies[i].Pos,
									param.SRTMatrix43);
							}
						}
					}

					if (isFirst || isLast)
					{
						m_ringBufferData += sizeof(VERTEX) * 2;
					}
					else
					{
						verteies[2] = verteies[0];
						verteies[3] = verteies[1];
						m_ringBufferData += sizeof(VERTEX) * 4;
					}

					if (!isFirst)
					{
						m_ribbonCount++;
					}

					if (isLast)
					{
						break;
					}
				}
			}

			// Apply distortion
			if (sizeof(VERTEX) == sizeof(VERTEX_DISTORTION))
			{
				VERTEX_DISTORTION* vs_ = (VERTEX_DISTORTION*)verteies0;

				Effekseer::Vector3D axisBefore;

				for (int32_t i = 0; i < (instances.size() - 1) * parameter.SplineDivision + 1; i++)
				{
					bool isFirst_ = (i == 0);
					bool isLast_ = (i == ((instances.size() - 1) * parameter.SplineDivision));

					Effekseer::Vector3D axis;
					Effekseer::Vector3D pos;

					if (isFirst_)
					{
						axis = (vs_[3].Pos - vs_[1].Pos);
						Effekseer::Vector3D::Normal(axis, axis);
						axisBefore = axis;
					}
					else if (isLast_)
					{
						axis = axisBefore;
					}
					else
					{
						Effekseer::Vector3D axisOld = axisBefore;
						axis = (vs_[5].Pos - vs_[3].Pos);
						Effekseer::Vector3D::Normal(axis, axis);
						axisBefore = axis;

						axis = (axisBefore + axisOld) / 2.0f;
						Effekseer::Vector3D::Normal(axis, axis);
					}

					auto tangent = vs_[1].Pos - vs_[0].Pos;
					Effekseer::Vector3D::Normal(tangent, tangent);

					if (isFirst_)
					{
						vs_[0].Binormal = axis;
						vs_[1].Binormal = axis;

						vs_[0].Tangent = tangent;
						vs_[1].Tangent = tangent;

						vs_ += 2;

					}
					else if (isLast_)
					{
						vs_[0].Binormal = axis;
						vs_[1].Binormal = axis;

						vs_[0].Tangent = tangent;
						vs_[1].Tangent = tangent;

						vs_ += 2;
					}
					else
					{
						vs_[0].Binormal = axis;
						vs_[1].Binormal = axis;
						vs_[2].Binormal = axis;
						vs_[3].Binormal = axis;

						vs_[0].Tangent = tangent;
						vs_[1].Tangent = tangent;
						vs_[2].Tangent = tangent;
						vs_[3].Tangent = tangent;

						vs_ += 4;
					}
				}
			}
		}

	public:

		RibbonRendererBase(RENDERER* renderer)
			: m_renderer(renderer)
			, m_ribbonCount(0)
			, m_ringBufferOffset(0)
			, m_ringBufferData(NULL)
		{
		}

		virtual ~RibbonRendererBase()
		{
		}


	protected:

		void Rendering_(const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera)
		{
			if (parameter.Distortion)
			{
				Rendering_Internal<VERTEX_DISTORTION>(parameter, instanceParameter, userData, camera);
			}
			else
			{
				Rendering_Internal<VERTEX_NORMAL>(parameter, instanceParameter, userData, camera);
			}
		}

		template<typename VERTEX>
		void Rendering_Internal(const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera)
		{
			if (m_ringBufferData == NULL) return;
			if (instanceParameter.InstanceCount < 2) return;

			bool isFirst = instanceParameter.InstanceIndex == 0;
			bool isLast = instanceParameter.InstanceIndex == (instanceParameter.InstanceCount - 1);

			auto& param = instanceParameter;

			if (isFirst)
			{
				instances.reserve(param.InstanceCount);
				instances.resize(0);
				innstancesNodeParam = parameter;
			}

			instances.push_back(param);

			if (isLast)
			{
				RenderSplines<VERTEX>(camera);
			}

			/*
			VERTEX* verteies = (VERTEX*)m_ringBufferData;

			for( int i = 0; i < 2; i++ )
			{
			verteies[i].Pos.X = instanceParameter.Positions[i];
			verteies[i].Pos.Y = 0.0f;
			verteies[i].Pos.Z = 0.0f;
			verteies[i].SetColor( instanceParameter.Colors[i] );
			}

			float percent = (float) instanceParameter.InstanceIndex / (float) (instanceParameter.InstanceCount - 1);

			verteies[0].UV[0] = instanceParameter.UV.X;
			verteies[0].UV[1] = instanceParameter.UV.Y + percent * instanceParameter.UV.Height;

			verteies[1].UV[0] = instanceParameter.UV.X + instanceParameter.UV.Width;
			verteies[1].UV[1] = instanceParameter.UV.Y + percent * instanceParameter.UV.Height;

			if( parameter.ViewpointDependent)
			{
			const ::Effekseer::Matrix43& mat = instanceParameter.SRTMatrix43;
			::Effekseer::Vector3D s;
			::Effekseer::Matrix43 r;
			::Effekseer::Vector3D t;
			mat.GetSRT( s, r, t );

			// 拡大
			for( int i = 0; i < 2; i++ )
			{
			verteies[i].Pos.X = verteies[i].Pos.X * s.X;
			}

			::Effekseer::Vector3D F;
			::Effekseer::Vector3D R;
			::Effekseer::Vector3D U;

			U = ::Effekseer::Vector3D( r.Value[1][0], r.Value[1][1], r.Value[1][2] );

			::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D( - camera.Values[0][2], - camera.Values[1][2], - camera.Values[2][2] ) );

			::Effekseer::Vector3D::Normal( R, ::Effekseer::Vector3D::Cross( R, U, F ) );
			::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D::Cross( F, R, U ) );

			::Effekseer::Matrix43 mat_rot;

			mat_rot.Value[0][0] = - R.X;
			mat_rot.Value[0][1] = - R.Y;
			mat_rot.Value[0][2] = - R.Z;
			mat_rot.Value[1][0] = U.X;
			mat_rot.Value[1][1] = U.Y;
			mat_rot.Value[1][2] = U.Z;
			mat_rot.Value[2][0] = F.X;
			mat_rot.Value[2][1] = F.Y;
			mat_rot.Value[2][2] = F.Z;
			mat_rot.Value[3][0] = t.X;
			mat_rot.Value[3][1] = t.Y;
			mat_rot.Value[3][2] = t.Z;

			for( int i = 0; i < 2; i++ )
			{
			::Effekseer::Vector3D::Transform(
			verteies[i].Pos,
			verteies[i].Pos,
			mat_rot );
			}
			}
			else
			{
			for( int i = 0; i < 2; i++ )
			{
			::Effekseer::Vector3D::Transform(
			verteies[i].Pos,
			verteies[i].Pos,
			instanceParameter.SRTMatrix43 );
			}
			}

			if( isFirst || isLast )
			{
			m_ringBufferData += sizeof(VERTEX) * 2;
			}
			else
			{
			verteies[2] = verteies[0];
			verteies[3] = verteies[1];
			m_ringBufferData += sizeof(VERTEX) * 4;
			}

			if( !isFirst )
			{
			m_ribbonCount++;
			}

			// Apply distortion
			if (isLast && sizeof(VERTEX) == sizeof(VERTEX_DISTORTION))
			{
			VERTEX_DISTORTION* vs_ = (VERTEX_DISTORTION*) (m_ringBufferData - sizeof(VERTEX_DISTORTION) * (instanceParameter.InstanceCount - 1) * 4);

			Effekseer::Vector3D axisBefore;

			for (int32_t i = 0; i < instanceParameter.InstanceCount; i++)
			{
			bool isFirst_ = (i == 0);
			bool isLast_ = (i == (instanceParameter.InstanceCount - 1));

			Effekseer::Vector3D axis;
			Effekseer::Vector3D pos;

			if (isFirst_)
			{
			axis = (vs_[3].Pos - vs_[1].Pos);
			Effekseer::Vector3D::Normal(axis, axis);
			axisBefore = axis;
			}
			else if (isLast_)
			{
			axis = axisBefore;
			}
			else
			{
			Effekseer::Vector3D axisOld = axisBefore;
			axis = (vs_[5].Pos - vs_[3].Pos);
			Effekseer::Vector3D::Normal(axis, axis);
			axisBefore = axis;

			axis = (axisBefore + axisOld) / 2.0f;
			Effekseer::Vector3D::Normal(axis, axis);
			}

			auto tangent = vs_[1].Pos - vs_[0].Pos;
			Effekseer::Vector3D::Normal(tangent, tangent);

			if (isFirst_)
			{
			vs_[0].Binormal = axis;
			vs_[1].Binormal = axis;

			vs_[0].Tangent = tangent;
			vs_[1].Tangent = tangent;

			vs_ += 2;

			}
			else if (isLast_)
			{
			vs_[0].Binormal = axis;
			vs_[1].Binormal = axis;

			vs_[0].Tangent = tangent;
			vs_[1].Tangent = tangent;

			vs_ += 2;
			}
			else
			{
			vs_[0].Binormal = axis;
			vs_[1].Binormal = axis;
			vs_[2].Binormal = axis;
			vs_[3].Binormal = axis;

			vs_[0].Tangent = tangent;
			vs_[1].Tangent = tangent;
			vs_[2].Tangent = tangent;
			vs_[3].Tangent = tangent;

			vs_ += 4;
			}
			}
			}
			*/
		}

	public:

		void BeginRenderingGroup(const efkRibbonNodeParam& param, int32_t count, void* userData) override
		{
			m_ribbonCount = 0;
			int32_t vertexCount = ((count - 1) * param.SplineDivision) * 4;
			if (vertexCount <= 0) return;

			EffekseerRenderer::StandardRendererState state;
			state.AlphaBlend = param.AlphaBlend;
			state.CullingType = ::Effekseer::CullingType::Double;
			state.DepthTest = param.ZTest;
			state.DepthWrite = param.ZWrite;
			state.TextureFilterType = param.TextureFilter;
			state.TextureWrapType = param.TextureWrap;

			state.Distortion = param.Distortion;
			state.DistortionIntensity = param.DistortionIntensity;

			if (param.ColorTextureIndex >= 0)
			{
				if (state.Distortion)
				{
					state.TexturePtr = param.EffectPointer->GetDistortionImage(param.ColorTextureIndex);
				}
				else
				{
					state.TexturePtr = param.EffectPointer->GetColorImage(param.ColorTextureIndex);
				}
			}
			else
			{
				state.TexturePtr = nullptr;
			}

			m_renderer->GetStandardRenderer()->UpdateStateAndRenderingIfRequired(state);

			m_renderer->GetStandardRenderer()->BeginRenderingAndRenderingIfRequired(vertexCount, m_ringBufferOffset, (void*&)m_ringBufferData);
		}

		void Rendering(const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData) override
		{
			Rendering_(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
		}
	};
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_RIBBON_RENDERER_H__
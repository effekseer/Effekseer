using System;
using System.Collections.Generic;
using System.Linq;
using Effekseer.Data;
using Effekseer.Data.Value;
using Effekseer.Utl;

namespace Effekseer.Binary
{
	class RingShapeParameter
	{
		public static byte[] GetBytes(Data.RingShapeParameter value, ExporterVersion version)
		{
			List<byte[]> data = new List<byte[]>();
			data.Add(BitConverter.GetBytes((int)value.Type.Value));

			if(value.Type.Value == Data.RingShapeType.Donut)
			{
			}
			else if (value.Type.Value == Data.RingShapeType.Crescent)
			{
				data.Add(BitConverter.GetBytes(value.Crescent.StartingFade.Value));
				data.Add(BitConverter.GetBytes(value.Crescent.EndingFade.Value));

				data.Add(((int)value.Crescent.StartingAngle.Value).GetBytes());

				if (value.Crescent.StartingAngle.Value == Data.FixedRandomEasingType.Fixed)
				{
					data.Add(value.Crescent.StartingAngle_Fixed.Value.GetBytes());
				}
				else if (value.Crescent.StartingAngle.Value == Data.FixedRandomEasingType.Random)
				{
					data.Add(value.Crescent.StartingAngle_Random.Max.GetBytes());
					data.Add(value.Crescent.StartingAngle_Random.Min.GetBytes());
				}
				else if (value.Crescent.StartingAngle.Value == Data.FixedRandomEasingType.Easing)
				{
					Utils.ExportEasing(value.Crescent.StartingAngle_Easing, 1.0f, data, version, version >= ExporterVersion.Ver16Alpha9);
				}

				data.Add(((int)value.Crescent.EndingAngle.Value).GetBytes());

				if (value.Crescent.EndingAngle.Value == Data.FixedRandomEasingType.Fixed)
				{
					data.Add(value.Crescent.EndingAngle_Fixed.Value.GetBytes());
				}
				else if (value.Crescent.EndingAngle.Value == Data.FixedRandomEasingType.Random)
				{
					data.Add(value.Crescent.EndingAngle_Random.Max.GetBytes());
					data.Add(value.Crescent.EndingAngle_Random.Min.GetBytes());
				}
				else if (value.Crescent.EndingAngle.Value == Data.FixedRandomEasingType.Easing)
				{
					Utils.ExportEasing(value.Crescent.EndingAngle_Easing, 1.0f, data, version, version >= ExporterVersion.Ver16Alpha9);
				}
			}

			return data.SelectMany(_ => _).ToArray();
		}
	}

	class TextureUVTypeParameter
	{
		public static byte[] GetBytes(Data.TextureUVTypeParameter value)
		{
			List<byte[]> data = new List<byte[]>();
			data.Add(BitConverter.GetBytes((int)value.Type.Value));

			if(value.Type.Value == Data.TextureUVType.Tile)
			{
				data.Add(value.TileEdgeHead.Value.GetBytes());
				data.Add(value.TileEdgeTail.Value.GetBytes());
				data.Add(value.TileLoopingArea.X.Value.GetBytes());
				data.Add(value.TileLoopingArea.Y.GetBytes());
			}

			return data.SelectMany(_ => _).ToArray();
		}
	}

	class RendererValues
	{
		public static byte[] GetBytes(Data.RendererValues value, SortedDictionary<string, int> texture_and_index, SortedDictionary<string, int> normalTexture_and_index, SortedDictionary<string, int> model_and_index, Dictionary<Data.ProceduralModelParameter, int> pmodel_and_index, ExporterVersion version)
		{
			List<byte[]> data = new List<byte[]>();

			// Fallback
			if (version < ExporterVersion.Ver16Alpha2)
			{
				if (value != null && value.Type.Value == Data.RendererValues.ParamaterType.Model && value.Model.ModelReference.Value == Data.ModelReferenceType.ProceduralModel)
				{
					value = null;
				}
			}

			if (value != null)
			{
				data.Add(value.Type.GetValueAsInt().GetBytes());
				switch (value.Type.Value)
				{
					case Data.RendererValues.ParamaterType.None:
						break;
					case Data.RendererValues.ParamaterType.Sprite:
						AddSpriteData(value, data);
						break;
					case Data.RendererValues.ParamaterType.Ribbon:
						AddRibbonData(value, data);
						break;
					case Data.RendererValues.ParamaterType.Ring:
						AddRingData(value, data, version);
						break;
					case Data.RendererValues.ParamaterType.Model:
						AddModelData(value, model_and_index, pmodel_and_index, version, data);
						break;
					case Data.RendererValues.ParamaterType.Track:
						AddTrackData(value, data);
						break;
				}
			}
			else
			{
				data.Add(((int)Data.RendererValues.ParamaterType.None).GetBytes());
			}

			return data.ToArray().ToArray();
		}

		private static void AddSpriteData(Data.RendererValues value, List<byte[]> data)
		{
			var param = value.Sprite;

			data.Add(param.RenderingOrder);
			data.Add(param.Billboard);

			AddColorAll();
			AddPartialColor();
			AddPosition();

			void AddColorAll()
			{
				data.Add(param.ColorAll);	// 全体色
				if (param.ColorAll.Value == Data.StandardColorType.Fixed)
				{
					var color_all = (byte[]) param.ColorAll_Fixed;
					data.Add(color_all);
				}
				else if (param.ColorAll.Value == Data.StandardColorType.Random)
				{
					var color_random = (byte[]) param.ColorAll_Random;
					data.Add(color_random);
				}
				else if (param.ColorAll.Value == Data.StandardColorType.Easing)
				{
					AddColorEasing(data, value.Sprite.ColorAll_Easing);
				}
				else if (param.ColorAll.Value == Data.StandardColorType.FCurve)
				{
					var bytes = param.ColorAll_FCurve.FCurve.GetBytes();
					data.Add(bytes);
				}
			}

			void AddPartialColor()
			{
				data.Add(param.Color);

				if (param.Color.Value == Data.RendererValues.SpriteParamater.ColorType.Default)
				{
				}
				else if (param.Color.Value == Data.RendererValues.SpriteParamater.ColorType.Fixed)
				{
					var color_ll = (byte[]) param.Color_Fixed_LL;
					var color_lr = (byte[]) param.Color_Fixed_LR;
					var color_ul = (byte[]) param.Color_Fixed_UL;
					var color_ur = (byte[]) param.Color_Fixed_UR;

					data.Add(color_ll);
					data.Add(color_lr);
					data.Add(color_ul);
					data.Add(color_ur);
				}
			}

			void AddPosition()
			{
				data.Add(BitConverter.GetBytes((int) Data.RendererValues.SpriteParamater.PositionType.Fixed));

				if (param.Position.Value == Data.RendererValues.SpriteParamater.PositionType.Default)
				{
					data.Add(BitConverter.GetBytes(-0.5f));
					data.Add(BitConverter.GetBytes(-0.5f));
					data.Add(BitConverter.GetBytes(+0.5f));
					data.Add(BitConverter.GetBytes(-0.5f));
					data.Add(BitConverter.GetBytes(-0.5f));
					data.Add(BitConverter.GetBytes(+0.5f));
					data.Add(BitConverter.GetBytes(+0.5f));
					data.Add(BitConverter.GetBytes(+0.5f));
				}
				else if (param.Position.Value == Data.RendererValues.SpriteParamater.PositionType.Fixed)
				{
					var pos_ll = (byte[]) param.Position_Fixed_LL.GetBytes();
					var pos_lr = (byte[]) param.Position_Fixed_LR.GetBytes();
					var pos_ul = (byte[]) param.Position_Fixed_UL.GetBytes();
					var pos_ur = (byte[]) param.Position_Fixed_UR.GetBytes();

					data.Add(pos_ll);
					data.Add(pos_lr);
					data.Add(pos_ul);
					data.Add(pos_ur);
				}
			}
		}

		private static void AddRibbonData(Data.RendererValues value, List<byte[]> data)
		{
			var ribbonParameter = value.Ribbon;

			// texture uv mode from 1.5
			data.Add(TextureUVTypeParameter.GetBytes(value.TextureUVType));

			data.Add((ribbonParameter.ViewpointDependent ? 1 : 0).GetBytes());

			AddColorAll();
			AddPartialColor();
			AddPosition();

			data.Add(BitConverter.GetBytes(ribbonParameter.SplineDivision.Value));

			void AddColorAll()
			{
				data.Add(ribbonParameter.ColorAll);

				if (ribbonParameter.ColorAll.Value == Data.RendererValues.RibbonParamater.ColorAllType.Fixed)
				{
					var color_all = (byte[]) ribbonParameter.ColorAll_Fixed;
					data.Add(color_all);
				}
				else if (ribbonParameter.ColorAll.Value == Data.RendererValues.RibbonParamater.ColorAllType.Random)
				{
					var color_random = (byte[]) ribbonParameter.ColorAll_Random;
					data.Add(color_random);
				}
				else if (ribbonParameter.ColorAll.Value == Data.RendererValues.RibbonParamater.ColorAllType.Easing)
				{
					AddColorEasing(data, ribbonParameter.ColorAll_Easing);
				}
			}

			void AddPartialColor()
			{
				data.Add(ribbonParameter.Color);

				if (ribbonParameter.Color.Value == Data.RendererValues.RibbonParamater.ColorType.Default)
				{
				}
				else if (ribbonParameter.Color.Value == Data.RendererValues.RibbonParamater.ColorType.Fixed)
				{
					var color_l = (byte[]) ribbonParameter.Color_Fixed_L;
					var color_r = (byte[]) ribbonParameter.Color_Fixed_R;

					data.Add(color_l);
					data.Add(color_r);
				}
			}

			void AddPosition()
			{
				data.Add(BitConverter.GetBytes((int) Data.RendererValues.RibbonParamater.PositionType.Fixed));

				if (ribbonParameter.Position.Value == Data.RendererValues.RibbonParamater.PositionType.Default)
				{
					data.Add(BitConverter.GetBytes(-0.5f));
					data.Add(BitConverter.GetBytes(+0.5f));
				}
				else if (ribbonParameter.Position.Value == Data.RendererValues.RibbonParamater.PositionType.Fixed)
				{
					var pos_l = (byte[]) ribbonParameter.Position_Fixed_L.GetBytes();
					var pos_r = (byte[]) ribbonParameter.Position_Fixed_R.GetBytes();
					data.Add(pos_l);
					data.Add(pos_r);
				}
			}
		}

		private static void AddRingData(Data.RendererValues value, List<byte[]> data, ExporterVersion version)
		{
			var ringParameter = value.Ring;

			data.Add(ringParameter.RenderingOrder);
			data.Add(ringParameter.Billboard);

			// from 1.5
			data.Add(RingShapeParameter.GetBytes(ringParameter.RingShape, version));

			data.Add(ringParameter.VertexCount.Value.GetBytes());

			AddViewingAngle();

			AddLocation(ringParameter.Outer,
				ringParameter.Outer_Fixed,
				ringParameter.Outer_PVA,
				ringParameter.Outer_Easing);
			AddLocation(ringParameter.Inner,
				ringParameter.Inner_Fixed,
				ringParameter.Inner_PVA,
				ringParameter.Inner_Easing);

			AddCenterRatio();

			AddColor(ringParameter.OuterColor,
				ringParameter.OuterColor_Fixed,
				ringParameter.OuterColor_Random,
				ringParameter.OuterColor_Easing);
			AddColor(ringParameter.CenterColor,
				ringParameter.CenterColor_Fixed,
				ringParameter.CenterColor_Random,
				ringParameter.CenterColor_Easing);
			AddColor(ringParameter.InnerColor,
				ringParameter.InnerColor_Fixed,
				ringParameter.InnerColor_Random,
				ringParameter.InnerColor_Easing);

			void AddViewingAngle()
			{
				// it will removed

				data.Add(ringParameter.ViewingAngle);
				if (ringParameter.ViewingAngle.GetValue() == Data.RendererValues.RingParamater.ViewingAngleType.Fixed)
				{
					data.Add(ringParameter.ViewingAngle_Fixed.Value.GetBytes());
				}
				else if (ringParameter.ViewingAngle.GetValue() == Data.RendererValues.RingParamater.ViewingAngleType.Random)
				{
					data.Add(ringParameter.ViewingAngle_Random.Max.GetBytes());
					data.Add(ringParameter.ViewingAngle_Random.Min.GetBytes());
				}
				else if (ringParameter.ViewingAngle.GetValue() == Data.RendererValues.RingParamater.ViewingAngleType.Easing)
				{
					AddFloatEasing(data, ringParameter.ViewingAngle_Easing);
				}
			}

			void AddLocation(Enum<Data.RendererValues.RingParamater.LocationType> locationType,
				Data.RendererValues.RingParamater.FixedLocation @fixed,
				Data.RendererValues.RingParamater.PVALocation pva,
				Vector2DEasingParamater easingParam)
			{
				data.Add(locationType);
				if (locationType.GetValue() == Data.RendererValues.RingParamater.LocationType.Fixed)
				{
					data.Add((@fixed.Location.X.Value).GetBytes());
					data.Add((@fixed.Location.Y.Value).GetBytes());
				}
				else if (locationType.GetValue() == Data.RendererValues.RingParamater.LocationType.PVA)
				{
					data.Add(pva.Location.GetBytes());
					data.Add(pva.Velocity.GetBytes());
					data.Add(pva.Acceleration.GetBytes());
				}
				else if (locationType.GetValue() == Data.RendererValues.RingParamater.LocationType.Easing)
				{
					data.Add((byte[]) easingParam.Start.GetBytes());
					data.Add((byte[]) easingParam.End.GetBytes());

					AddEasing(data, easingParam.StartSpeed, easingParam.EndSpeed);
				}
			}

			void AddCenterRatio()
			{
				data.Add(ringParameter.CenterRatio);
				if (ringParameter.CenterRatio.GetValue() == Data.RendererValues.RingParamater.CenterRatioType.Fixed)
				{
					data.Add(ringParameter.CenterRatio_Fixed.Value.GetBytes());
				}
				else if (ringParameter.CenterRatio.GetValue() == Data.RendererValues.RingParamater.CenterRatioType.Random)
				{
					data.Add(ringParameter.CenterRatio_Random.Max.GetBytes());
					data.Add(ringParameter.CenterRatio_Random.Min.GetBytes());
				}
				else if (ringParameter.CenterRatio.GetValue() == Data.RendererValues.RingParamater.CenterRatioType.Easing)
				{
					Utils.ExportEasing(ringParameter.CenterRatio_Easing, 1.0f, data, version, version >= ExporterVersion.Ver16Alpha9);
				}
			}

			void AddColor(Enum<Data.RendererValues.RingParamater.ColorType> parameter,
				Color @fixed,
				ColorWithRandom random,
				ColorEasingParamater easingParam)
			{
				data.Add(parameter);
				if (parameter.Value == Data.RendererValues.RingParamater.ColorType.Fixed)
				{
					data.Add((byte[]) @fixed);
				}
				else if (parameter.Value == Data.RendererValues.RingParamater.ColorType.Random)
				{
					data.Add((byte[]) random);
				}
				else if (parameter.Value == Data.RendererValues.RingParamater.ColorType.Easing)
				{
					AddColorEasing(data, easingParam);
				}
			}
		}

		private static void AddModelData(
			Data.RendererValues value,
			SortedDictionary<string, int> model_and_index,
			Dictionary<ProceduralModelParameter, int> pmodel_and_index,
			ExporterVersion version,
			List<byte[]> data)
		{
			var param = value.Model;

			if (version >= ExporterVersion.Ver16Alpha3)
			{
				var refType = (int) value.Model.ModelReference.Value;
				data.Add((refType).GetBytes());
			}

			AddModelReference();

			data.Add(param.Billboard);

			data.Add(((int) param.Culling.Value).GetBytes());

			// 全体色
			OutputStandardColor(data, param.Color, param.Color_Fixed, param.Color_Random, param.Color_Easing,
				param.Color_FCurve);

			void AddModelReference()
			{
				if (value.Model.ModelReference.Value == Data.ModelReferenceType.File)
				{
					data.Add((1.0f).GetBytes());

					if (param.Model.RelativePath != string.Empty)
					{
						var relativePath = Utils.GetModelPath(param.Model);
						data.Add(model_and_index[relativePath].GetBytes());
					}
					else
					{
						data.Add((-1).GetBytes());
					}
				}
				else if (value.Model.ModelReference.Value == Data.ModelReferenceType.ProceduralModel)
				{
					if (value.Model.Reference.Value != null)
					{
						var ind = pmodel_and_index[value.Model.Reference.Value];
						data.Add(ind.GetBytes());
					}
					else
					{
						data.Add((-1).GetBytes());
					}
				}
			}
		}

		private static void AddTrackData(Data.RendererValues value, List<byte[]> data)
		{
			// texture uv mode from 1.5
			data.Add(TextureUVTypeParameter.GetBytes(value.TextureUVType));

			var param = value.Track;
			data.Add(param.TrackSizeFor);
			data.Add(BitConverter.GetBytes(param.TrackSizeFor_Fixed.Value));

			data.Add(param.TrackSizeMiddle);
			data.Add(BitConverter.GetBytes(param.TrackSizeMiddle_Fixed.Value));

			data.Add(param.TrackSizeBack);
			data.Add(BitConverter.GetBytes(param.TrackSizeBack_Fixed.Value));

			data.Add(BitConverter.GetBytes(param.SplineDivision.Value));

			OutputStandardColor(data, param.ColorLeft, param.ColorLeft_Fixed, param.ColorLeft_Random, param.ColorLeft_Easing,
				param.ColorLeft_FCurve);
			OutputStandardColor(data, param.ColorLeftMiddle, param.ColorLeftMiddle_Fixed, param.ColorLeftMiddle_Random,
				param.ColorLeftMiddle_Easing, param.ColorLeftMiddle_FCurve);

			OutputStandardColor(data, param.ColorCenter, param.ColorCenter_Fixed, param.ColorCenter_Random,
				param.ColorCenter_Easing, param.ColorCenter_FCurve);
			OutputStandardColor(data, param.ColorCenterMiddle, param.ColorCenterMiddle_Fixed, param.ColorCenterMiddle_Random,
				param.ColorCenterMiddle_Easing, param.ColorCenterMiddle_FCurve);

			OutputStandardColor(data, param.ColorRight, param.ColorRight_Fixed, param.ColorRight_Random,
				param.ColorRight_Easing, param.ColorRight_FCurve);
			OutputStandardColor(data, param.ColorRightMiddle, param.ColorRightMiddle_Fixed, param.ColorRightMiddle_Random,
				param.ColorRightMiddle_Easing, param.ColorRightMiddle_FCurve);
		}

		private static void OutputStandardColor(
			List<byte[]> data, 
			Data.Value.Enum<Data.StandardColorType> color,
			Data.Value.Color color_fixed,
			Data.Value.ColorWithRandom color_Random,
			Data.ColorEasingParamater color_Easing,
			Data.ColorFCurveParameter color_FCurve)
		{
			data.Add(color);

			if (color.Value == Data.StandardColorType.Fixed)
			{
				var color_all = (byte[])color_fixed;
				data.Add(color_all);
			}
			else if (color.Value == Data.StandardColorType.Random)
			{
				var color_random = (byte[])color_Random;
				data.Add(color_random);
			}
			else if (color.Value == Data.StandardColorType.Easing)
			{
				AddColorEasing(data, color_Easing);
			}
			else if (color.Value == Data.StandardColorType.FCurve)
			{
				var bytes = color_FCurve.FCurve.GetBytes();
				data.Add(bytes);
			}
		}

		private static void AddEasing(List<byte[]> data, Enum<EasingStart> start, Enum<EasingEnd> end)
		{
			var easing = Utl.MathUtl.Easing((float) start.Value, (float) end.Value);
			data.Add(BitConverter.GetBytes(easing[0]));
			data.Add(BitConverter.GetBytes(easing[1]));
			data.Add(BitConverter.GetBytes(easing[2]));
		}
		
		private static void AddColorEasing(List<byte[]> data, ColorEasingParamater easingParam)
		{
			data.Add((byte[]) easingParam.Start);
			data.Add((byte[]) easingParam.End);
			AddEasing(data, easingParam.StartSpeed, easingParam.EndSpeed);
		}
		
		private static void AddFloatEasing(List<byte[]> data, FloatEasingParamater easingParam)
		{
			data.Add(easingParam.Start.Max.GetBytes());
			data.Add(easingParam.Start.Min.GetBytes());
			data.Add(easingParam.End.Max.GetBytes());
			data.Add(easingParam.End.Min.GetBytes());
			AddEasing(data, easingParam.StartSpeed, easingParam.EndSpeed);
		}
	}
}

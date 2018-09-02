using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Utl;

namespace Effekseer.Binary
{
	class RendererValues
	{
		public static byte[] GetBytes(Data.RendererValues value, Dictionary<string, int> texture_and_index, Dictionary<string, int> normalTexture_and_index, Dictionary<string, int> model_and_index)
		{
			List<byte[]> data = new List<byte[]>();

			if (value == null)
			{
				data.Add(((int)(Data.RendererValues.ParamaterType.None)).GetBytes());
			}
			else
			{
				data.Add(value.Type.GetValueAsInt().GetBytes());
			}
			

			if (value == null)
			{ 
			
			}
			else if (value.Type.Value == Data.RendererValues.ParamaterType.None)
			{
				
			}
			else if (value.Type.Value == Data.RendererValues.ParamaterType.Sprite)
			{
				var param = value.Sprite;

				data.Add(param.RenderingOrder);
				//data.Add(sprite_paramater.AlphaBlend);
				data.Add(param.Billboard);

				// 全体色
				data.Add(param.ColorAll);

				if (param.ColorAll.Value == Data.StandardColorType.Fixed)
				{
					var color_all = (byte[])param.ColorAll_Fixed;
					data.Add(color_all);
				}
				else if (param.ColorAll.Value == Data.StandardColorType.Random)
				{
					var color_random = (byte[])param.ColorAll_Random;
					data.Add(color_random);
				}
				else if (param.ColorAll.Value == Data.StandardColorType.Easing)
				{
					var easing = Utl.MathUtl.Easing((float)value.Sprite.ColorAll_Easing.StartSpeed.Value, (float)value.Sprite.ColorAll_Easing.EndSpeed.Value);
					data.Add((byte[])value.Sprite.ColorAll_Easing.Start);
					data.Add((byte[])value.Sprite.ColorAll_Easing.End);
					data.Add(BitConverter.GetBytes(easing[0]));
					data.Add(BitConverter.GetBytes(easing[1]));
					data.Add(BitConverter.GetBytes(easing[2]));
				}
				else if (param.ColorAll.Value == Data.StandardColorType.FCurve)
				{
					var bytes = param.ColorAll_FCurve.FCurve.GetBytes();
					data.Add(bytes);
				}
				
				// 部分色
				data.Add(param.Color);

				if (param.Color.Value == Data.RendererValues.SpriteParamater.ColorType.Default)
				{
					
				}
				else if (param.Color.Value == Data.RendererValues.SpriteParamater.ColorType.Fixed)
				{
					var color_ll = (byte[])param.Color_Fixed_LL;
					var color_lr = (byte[])param.Color_Fixed_LR;
					var color_ul = (byte[])param.Color_Fixed_UL;
					var color_ur = (byte[])param.Color_Fixed_UR;

					data.Add(color_ll);
					data.Add(color_lr);
					data.Add(color_ul);
					data.Add(color_ur);
					
				}

				// 座標
				//data.Add(sprite_paramater.Position);
				data.Add(BitConverter.GetBytes((int)Data.RendererValues.SpriteParamater.PositionType.Fixed));

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
					var pos_ll = (byte[])param.Position_Fixed_LL.GetBytes();
					var pos_lr = (byte[])param.Position_Fixed_LR.GetBytes();
					var pos_ul = (byte[])param.Position_Fixed_UL.GetBytes();
					var pos_ur = (byte[])param.Position_Fixed_UR.GetBytes();

					data.Add(pos_ll);
					data.Add(pos_lr);
					data.Add(pos_ul);
					data.Add(pos_ur);
				}

				// テクスチャ番号
				/*
				if (sprite_paramater.ColorTexture.RelativePath != string.Empty)
				{
					data.Add(texture_and_index[sprite_paramater.ColorTexture.RelativePath].GetBytes());
				}
				else
				{
					data.Add((-1).GetBytes());
				}
				*/
			}
			else if (value.Type.Value == Data.RendererValues.ParamaterType.Ribbon)
			{
				var ribbonParamater = value.Ribbon;

				//data.Add(ribbonParamater.AlphaBlend);

				if (ribbonParamater.ViewpointDependent)
				{
					data.Add((1).GetBytes());
				}
				else
				{
					data.Add((0).GetBytes());
				}

				// 全体色
				data.Add(ribbonParamater.ColorAll);

				if (ribbonParamater.ColorAll.Value == Data.RendererValues.RibbonParamater.ColorAllType.Fixed)
				{
					var color_all = (byte[])ribbonParamater.ColorAll_Fixed;
					data.Add(color_all);
				}
				else if (ribbonParamater.ColorAll.Value == Data.RendererValues.RibbonParamater.ColorAllType.Random)
				{
					var color_random = (byte[])ribbonParamater.ColorAll_Random;
					data.Add(color_random);
				}
				else if (ribbonParamater.ColorAll.Value == Data.RendererValues.RibbonParamater.ColorAllType.Easing)
				{
					var easing = Utl.MathUtl.Easing((float)ribbonParamater.ColorAll_Easing.StartSpeed.Value, (float)ribbonParamater.ColorAll_Easing.EndSpeed.Value);
					data.Add((byte[])ribbonParamater.ColorAll_Easing.Start);
					data.Add((byte[])ribbonParamater.ColorAll_Easing.End);
					data.Add(BitConverter.GetBytes(easing[0]));
					data.Add(BitConverter.GetBytes(easing[1]));
					data.Add(BitConverter.GetBytes(easing[2]));
				}

				// 部分色
				data.Add(ribbonParamater.Color);

				if (ribbonParamater.Color.Value == Data.RendererValues.RibbonParamater.ColorType.Default)
				{

				}
				else if (ribbonParamater.Color.Value == Data.RendererValues.RibbonParamater.ColorType.Fixed)
				{
					var color_l = (byte[])ribbonParamater.Color_Fixed_L;
					var color_r = (byte[])ribbonParamater.Color_Fixed_R;

					data.Add(color_l);
					data.Add(color_r);
				}

				// 座標
				//data.Add(ribbonParamater.Position);
				data.Add(BitConverter.GetBytes((int)Data.RendererValues.RibbonParamater.PositionType.Fixed));

				if (ribbonParamater.Position.Value == Data.RendererValues.RibbonParamater.PositionType.Default)
				{
					data.Add(BitConverter.GetBytes(-0.5f));
					data.Add(BitConverter.GetBytes(+0.5f));
				}
				else if (ribbonParamater.Position.Value == Data.RendererValues.RibbonParamater.PositionType.Fixed)
				{
					var pos_l = (byte[])ribbonParamater.Position_Fixed_L.GetBytes();
					var pos_r = (byte[])ribbonParamater.Position_Fixed_R.GetBytes();
					data.Add(pos_l);
					data.Add(pos_r);
				}

				data.Add(BitConverter.GetBytes(ribbonParamater.SplineDivision.Value));

				// テクスチャ番号
				/*
				if (ribbonParamater.ColorTexture.RelativePath != string.Empty)
				{
					data.Add(texture_and_index[ribbonParamater.ColorTexture.RelativePath].GetBytes());
				}
				else
				{
					data.Add((-1).GetBytes());
				}
				*/
			}
            else if (value.Type.Value == Data.RendererValues.ParamaterType.Ring)
            {
                var ringParamater = value.Ring;

                data.Add(ringParamater.RenderingOrder);
                //data.Add(ringParamater.AlphaBlend);
                data.Add(ringParamater.Billboard);

                data.Add(ringParamater.VertexCount.Value.GetBytes());

                data.Add(ringParamater.ViewingAngle);
                if (ringParamater.ViewingAngle.GetValue() == Data.RendererValues.RingParamater.ViewingAngleType.Fixed)
                {
                    data.Add(ringParamater.ViewingAngle_Fixed.Value.GetBytes());
                }
                else if (ringParamater.ViewingAngle.GetValue() == Data.RendererValues.RingParamater.ViewingAngleType.Random)
                {
                    data.Add(ringParamater.ViewingAngle_Random.Max.GetBytes());
                    data.Add(ringParamater.ViewingAngle_Random.Min.GetBytes());
                }
                else if (ringParamater.ViewingAngle.GetValue() == Data.RendererValues.RingParamater.ViewingAngleType.Easing)
                {
                    var easing = Utl.MathUtl.Easing(
                        (float)ringParamater.ViewingAngle_Easing.StartSpeed.Value,
                        (float)ringParamater.ViewingAngle_Easing.EndSpeed.Value);

                    data.Add(ringParamater.ViewingAngle_Easing.Start.Max.GetBytes());
                    data.Add(ringParamater.ViewingAngle_Easing.Start.Min.GetBytes());
                    data.Add(ringParamater.ViewingAngle_Easing.End.Max.GetBytes());
                    data.Add(ringParamater.ViewingAngle_Easing.End.Min.GetBytes());
                    data.Add(BitConverter.GetBytes(easing[0]));
                    data.Add(BitConverter.GetBytes(easing[1]));
                    data.Add(BitConverter.GetBytes(easing[2]));
                }

                data.Add(ringParamater.Outer);
                if (ringParamater.Outer.GetValue() == Data.RendererValues.RingParamater.LocationType.Fixed)
                {
                    data.Add((ringParamater.Outer_Fixed.Location.X.Value).GetBytes());
					data.Add((ringParamater.Outer_Fixed.Location.Y.Value).GetBytes());
                }
                else if (ringParamater.Outer.GetValue() == Data.RendererValues.RingParamater.LocationType.PVA)
                {
					data.Add(ringParamater.Outer_PVA.Location.GetBytes());
					data.Add(ringParamater.Outer_PVA.Velocity.GetBytes());
					data.Add(ringParamater.Outer_PVA.Acceleration.GetBytes());
                }
                else if (ringParamater.Outer.GetValue() == Data.RendererValues.RingParamater.LocationType.Easing)
                {
                    var easing = Utl.MathUtl.Easing(
                        (float)ringParamater.Outer_Easing.StartSpeed.Value, 
                        (float)ringParamater.Outer_Easing.EndSpeed.Value);

                    data.Add((byte[])ringParamater.Outer_Easing.Start.GetBytes());
                    data.Add((byte[])ringParamater.Outer_Easing.End.GetBytes());
                    data.Add(BitConverter.GetBytes(easing[0]));
                    data.Add(BitConverter.GetBytes(easing[1]));
                    data.Add(BitConverter.GetBytes(easing[2]));
                }

                data.Add(ringParamater.Inner);
                if (ringParamater.Inner.GetValue() == Data.RendererValues.RingParamater.LocationType.Fixed)
                {
					data.Add((ringParamater.Inner_Fixed.Location.X.Value).GetBytes());
					data.Add((ringParamater.Inner_Fixed.Location.Y.Value).GetBytes());
                }
                else if (ringParamater.Inner.GetValue() == Data.RendererValues.RingParamater.LocationType.PVA)
                {
					data.Add(ringParamater.Inner_PVA.Location.GetBytes());
					data.Add(ringParamater.Inner_PVA.Velocity.GetBytes());
					data.Add(ringParamater.Inner_PVA.Acceleration.GetBytes());
                }
                else if (ringParamater.Inner.GetValue() == Data.RendererValues.RingParamater.LocationType.Easing)
                {
                    var easing = Utl.MathUtl.Easing(
                        (float)ringParamater.Inner_Easing.StartSpeed.Value,
                        (float)ringParamater.Inner_Easing.EndSpeed.Value);

                    data.Add((byte[])ringParamater.Inner_Easing.Start.GetBytes());
                    data.Add((byte[])ringParamater.Inner_Easing.End.GetBytes());
                    data.Add(BitConverter.GetBytes(easing[0]));
                    data.Add(BitConverter.GetBytes(easing[1]));
                    data.Add(BitConverter.GetBytes(easing[2]));
                }

                data.Add(ringParamater.CenterRatio);
                if (ringParamater.CenterRatio.GetValue() == Data.RendererValues.RingParamater.CenterRatioType.Fixed)
                {
                    data.Add(ringParamater.CenterRatio_Fixed.Value.GetBytes());
                }
                else if (ringParamater.CenterRatio.GetValue() == Data.RendererValues.RingParamater.CenterRatioType.Random)
                {
                    data.Add(ringParamater.CenterRatio_Random.Max.GetBytes());
                    data.Add(ringParamater.CenterRatio_Random.Min.GetBytes());
                }
                else if (ringParamater.CenterRatio.GetValue() == Data.RendererValues.RingParamater.CenterRatioType.Easing)
                {
                    var easing = Utl.MathUtl.Easing(
                        (float)ringParamater.CenterRatio_Easing.StartSpeed.Value,
                        (float)ringParamater.CenterRatio_Easing.EndSpeed.Value);

                    data.Add(ringParamater.CenterRatio_Easing.Start.Max.GetBytes());
                    data.Add(ringParamater.CenterRatio_Easing.Start.Min.GetBytes());
                    data.Add(ringParamater.CenterRatio_Easing.End.Max.GetBytes());
                    data.Add(ringParamater.CenterRatio_Easing.End.Min.GetBytes());
                    data.Add(BitConverter.GetBytes(easing[0]));
                    data.Add(BitConverter.GetBytes(easing[1]));
                    data.Add(BitConverter.GetBytes(easing[2]));
                }

                data.Add(ringParamater.OuterColor);
                if (ringParamater.OuterColor.Value == Data.RendererValues.RingParamater.ColorType.Fixed)
                {
                    data.Add((byte[])ringParamater.OuterColor_Fixed);
                }
                else if (ringParamater.OuterColor.Value == Data.RendererValues.RingParamater.ColorType.Random)
                {
                    data.Add((byte[])ringParamater.OuterColor_Random);
                }
                else if (ringParamater.OuterColor.Value == Data.RendererValues.RingParamater.ColorType.Easing)
                {
                    var easing = Utl.MathUtl.Easing(
                        (float)ringParamater.OuterColor_Easing.StartSpeed.Value, 
                        (float)ringParamater.OuterColor_Easing.EndSpeed.Value);
                    data.Add((byte[])ringParamater.OuterColor_Easing.Start);
                    data.Add((byte[])ringParamater.OuterColor_Easing.End);
                    data.Add(BitConverter.GetBytes(easing[0]));
                    data.Add(BitConverter.GetBytes(easing[1]));
                    data.Add(BitConverter.GetBytes(easing[2]));
                }

                data.Add(ringParamater.CenterColor);
                if (ringParamater.CenterColor.Value == Data.RendererValues.RingParamater.ColorType.Fixed)
                {
                    data.Add((byte[])ringParamater.CenterColor_Fixed);
                }
                else if (ringParamater.CenterColor.Value == Data.RendererValues.RingParamater.ColorType.Random)
                {
                    data.Add((byte[])ringParamater.CenterColor_Random);
                }
                else if (ringParamater.CenterColor.Value == Data.RendererValues.RingParamater.ColorType.Easing)
                {
                    var easing = Utl.MathUtl.Easing(
                        (float)ringParamater.CenterColor_Easing.StartSpeed.Value,
                        (float)ringParamater.CenterColor_Easing.EndSpeed.Value);
                    data.Add((byte[])ringParamater.CenterColor_Easing.Start);
                    data.Add((byte[])ringParamater.CenterColor_Easing.End);
                    data.Add(BitConverter.GetBytes(easing[0]));
                    data.Add(BitConverter.GetBytes(easing[1]));
                    data.Add(BitConverter.GetBytes(easing[2]));
                }

                data.Add(ringParamater.InnerColor);
                if (ringParamater.InnerColor.Value == Data.RendererValues.RingParamater.ColorType.Fixed)
                {
                    data.Add((byte[])ringParamater.InnerColor_Fixed);
                }
                else if (ringParamater.InnerColor.Value == Data.RendererValues.RingParamater.ColorType.Random)
                {
                    data.Add((byte[])ringParamater.InnerColor_Random);
                }
                else if (ringParamater.InnerColor.Value == Data.RendererValues.RingParamater.ColorType.Easing)
                {
                    var easing = Utl.MathUtl.Easing(
                        (float)ringParamater.InnerColor_Easing.StartSpeed.Value,
                        (float)ringParamater.InnerColor_Easing.EndSpeed.Value);
                    data.Add((byte[])ringParamater.InnerColor_Easing.Start);
                    data.Add((byte[])ringParamater.InnerColor_Easing.End);
                    data.Add(BitConverter.GetBytes(easing[0]));
                    data.Add(BitConverter.GetBytes(easing[1]));
                    data.Add(BitConverter.GetBytes(easing[2]));
                }

                // テクスチャ番号
                /*
				if (ringParamater.ColorTexture.RelativePath != string.Empty)
                {
                    data.Add(texture_and_index[ringParamater.ColorTexture.RelativePath].GetBytes());
                }
                else
                {
                    data.Add((-1).GetBytes());
                }
				*/
            }
			else if (value.Type.Value == Data.RendererValues.ParamaterType.Model)
			{
				var param = value.Model;

				data.Add((1.0f).GetBytes());

				if (param.Model.RelativePath != string.Empty)
				{
					var relative_path = param.Model.RelativePath;

					if (string.IsNullOrEmpty(System.IO.Path.GetDirectoryName(relative_path)))
					{
						relative_path = System.IO.Path.GetFileNameWithoutExtension(relative_path) + ".efkmodel";
					}
					else
					{
						relative_path = System.IO.Path.GetDirectoryName(relative_path) + "/" + System.IO.Path.GetFileNameWithoutExtension(relative_path) + ".efkmodel";
					}

					data.Add(model_and_index[relative_path].GetBytes());
				}
				else
				{
					data.Add((-1).GetBytes());
				}

				if (param.NormalTexture.RelativePath != string.Empty)
				{
					data.Add(normalTexture_and_index[param.NormalTexture.RelativePath].GetBytes());
				}
				else
				{
					data.Add((-1).GetBytes());
				}

				data.Add(param.Billboard);

				if (param.Lighting.Value)
				{
					data.Add((1).GetBytes());
				}
				else
				{
					data.Add((0).GetBytes());
				}

				data.Add(((int)param.Culling.Value).GetBytes());

				// 全体色
				OutputStandardColor(data, param.Color, param.Color_Fixed, param.Color_Random, param.Color_Easing, param.Color_FCurve);
			}
			else if (value.Type.Value == Data.RendererValues.ParamaterType.Track)
			{
				var param = value.Track;
				data.Add(param.TrackSizeFor);
				data.Add(BitConverter.GetBytes(param.TrackSizeFor_Fixed.Value));

				data.Add(param.TrackSizeMiddle);
				data.Add(BitConverter.GetBytes(param.TrackSizeMiddle_Fixed.Value));

				data.Add(param.TrackSizeBack);
				data.Add(BitConverter.GetBytes(param.TrackSizeBack_Fixed.Value));
				
				data.Add(BitConverter.GetBytes(param.SplineDivision.Value));

				OutputStandardColor(data, param.ColorLeft, param.ColorLeft_Fixed, param.ColorLeft_Random, param.ColorLeft_Easing, param.ColorLeft_FCurve);
				OutputStandardColor(data, param.ColorLeftMiddle, param.ColorLeftMiddle_Fixed, param.ColorLeftMiddle_Random, param.ColorLeftMiddle_Easing, param.ColorLeftMiddle_FCurve);

				OutputStandardColor(data, param.ColorCenter, param.ColorCenter_Fixed, param.ColorCenter_Random, param.ColorCenter_Easing, param.ColorCenter_FCurve);
				OutputStandardColor(data, param.ColorCenterMiddle, param.ColorCenterMiddle_Fixed, param.ColorCenterMiddle_Random, param.ColorCenterMiddle_Easing, param.ColorCenterMiddle_FCurve);

				OutputStandardColor(data, param.ColorRight, param.ColorRight_Fixed, param.ColorRight_Random, param.ColorRight_Easing, param.ColorRight_FCurve);
				OutputStandardColor(data, param.ColorRightMiddle, param.ColorRightMiddle_Fixed, param.ColorRightMiddle_Random, param.ColorRightMiddle_Easing, param.ColorRightMiddle_FCurve);
			}

			return data.ToArray().ToArray();
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
				var easing = Utl.MathUtl.Easing((float)color_Easing.StartSpeed.Value, (float)color_Easing.EndSpeed.Value);
				data.Add((byte[])color_Easing.Start);
				data.Add((byte[])color_Easing.End);
				data.Add(BitConverter.GetBytes(easing[0]));
				data.Add(BitConverter.GetBytes(easing[1]));
				data.Add(BitConverter.GetBytes(easing[2]));
			}
			else if (color.Value == Data.StandardColorType.FCurve)
			{
				var bytes = color_FCurve.FCurve.GetBytes();
				data.Add(bytes);
			}
		}
	}
}

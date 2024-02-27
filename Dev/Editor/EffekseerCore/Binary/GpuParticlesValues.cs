using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Data.Value;
using Effekseer.Data;
using Effekseer.Utils;
using Microsoft.Scripting.Utils;
using static IronPython.Modules._ast;
using static Effekseer.InternalScript.SSAGenerator;
using static Effekseer.Data.GpuParticlesValues.ScaleParams;

namespace Effekseer.Binary
{
	class GpuParticlesValues
	{
		public static byte[] GetBytes(Data.GpuParticlesValues value, 
			SortedDictionary<string, int> colorTexAndIndex, 
			SortedDictionary<string, int> normalTexAndIndex,
			SortedDictionary<string, int> modelAndIndex)
		{
			List<byte[]> data = new List<byte[]>();

			data.Add(value.Enabled.Value.GetBytes());
			if (!value.Enabled.Value)
			{
				return value.Enabled.Value.GetBytes();
			}

			data.Add(value.Basic.EmitCount.GetBytes(-1));
			data.Add(value.Basic.EmitPerFrame.GetBytes());
			data.Add(value.Basic.EmitOffset.GetBytes());
			data.Add(value.Basic.LifeTime.GetBytes());

			data.Add(((byte)value.EmitShape.Shape.Value).GetBytes());
			data.Add((value.EmitShape.RotationApplied.Value ? (byte)1 : (byte)0).GetBytes());
			switch (value.EmitShape.Shape.Value)
			{
				case Data.GpuParticlesValues.EmitShapeParams.ShapeType.Point:
					break;
				case Data.GpuParticlesValues.EmitShapeParams.ShapeType.Line:
					data.Add(value.EmitShape.LineStart.GetBytes());
					data.Add(value.EmitShape.LineEnd.GetBytes());
					data.Add(value.EmitShape.LineWidth.GetBytes());
					break;
				case Data.GpuParticlesValues.EmitShapeParams.ShapeType.Circle:
					data.Add(value.EmitShape.CircleAxis.GetBytes());
					data.Add(value.EmitShape.CircleInner.GetBytes());
					data.Add(value.EmitShape.CircleOuter.GetBytes());
					break;
				case Data.GpuParticlesValues.EmitShapeParams.ShapeType.Sphere:
					data.Add(value.EmitShape.SphereRadius.GetBytes());
					break;
				case Data.GpuParticlesValues.EmitShapeParams.ShapeType.Model:
					AddResource(data, modelAndIndex, Utils.GetModelPath(value.EmitShape.ModelPath));
					data.Add(value.EmitShape.ModelSize.GetBytes());
					break;
			}

			data.Add(value.Velocity.Direction.GetBytes());
			data.Add(value.Velocity.Spread.GetBytes());
			data.Add(value.Velocity.InitialSpeed.GetBytes());
			data.Add(value.Velocity.Damping.GetBytes());

			data.Add(value.Rotation.InitialAngle.GetBytes());
			data.Add(value.Rotation.AngularVelocity.GetBytes());

			// ScaleParams
			data.Add(((byte)value.Scale.Type.Value).GetBytes());
			switch (value.Scale.Type.Value)
			{
				case ParamaterType.Fixed:
					data.Add(value.Scale.Fixed.SingleScale.GetBytes());
					data.Add(value.Scale.Fixed.XYZScale.GetBytes());
					break;
				case ParamaterType.Easing:
					data.Add(value.Scale.Easing.InitialSingleScale.GetBytes());
					data.Add(value.Scale.Easing.InitialXYZScale.GetBytes());
					data.Add(value.Scale.Easing.TerminalSingleScale.GetBytes());
					data.Add(value.Scale.Easing.TerminalXYZScale.GetBytes());
					AddEasing(data, value.Scale.Easing.StartSpeed, value.Scale.Easing.EndSpeed);
					break;
			}

			data.Add(value.Force.Gravity.GetBytes(1.0f / 60.0f));
			data.Add(value.Force.Vortex.Rotation.GetBytes(1.0f / 60.0f));
			data.Add(value.Force.Vortex.Attraction.GetBytes(1.0f / 60.0f));
			data.Add(value.Force.Vortex.Center.GetBytes());
			data.Add(value.Force.Vortex.Axis.GetBytes());
			data.Add(value.Force.Turbulence.Power.GetBytes(1.0f / 60.0f));
			data.Add(value.Force.Turbulence.Seed.GetBytes());
			data.Add(value.Force.Turbulence.Scale.GetBytes());
			data.Add(value.Force.Turbulence.Octave.GetBytes());

			data.Add(((byte)value.RenderBasic.BlendType.Value).GetBytes());
			data.Add(((byte)(value.RenderBasic.ZWrite.Value ? 1 : 0)).GetBytes());
			data.Add(((byte)(value.RenderBasic.ZTest.Value ? 1 : 0)).GetBytes());

			// RenderShapeParams
			data.Add(((byte)value.RenderShape.Shape.Value).GetBytes());
			switch (value.RenderShape.Shape.Value)
			{
				case Data.GpuParticlesValues.RenderShapeParams.ShapeType.Sprite:
					data.Add(value.RenderShape.SpriteBillboard.GetBytes());
					break;
				case Data.GpuParticlesValues.RenderShapeParams.ShapeType.Model:
					AddResource(data, modelAndIndex, Utils.GetModelPath(value.RenderShape.ModelPath));
					break;
				case Data.GpuParticlesValues.RenderShapeParams.ShapeType.Trail:
					data.Add(value.RenderShape.TrailLength.GetBytes());
					break;
			}
			data.Add(value.RenderShape.ShapeSize.GetBytes());

			// RenderColorParams
			AddColorAll(data, value);
			data.Add(value.RenderColor.Emissive.GetBytes());
			data.Add(value.RenderColor.FadeIn.GetBytes());
			data.Add(value.RenderColor.FadeOut.GetBytes());

			// RenderMaterialParams
			data.Add(((byte)value.RenderMaterial.Material.Value).GetBytes());
			AddResource(data, colorTexAndIndex, value.RenderMaterial.ColorTexture.Path.RelativePath);
			AddResource(data, normalTexAndIndex, value.RenderMaterial.NormalTexture.Path.RelativePath);
			data.Add((-1).GetBytes());
			data.Add((-1).GetBytes());
			data.Add(((byte)value.RenderMaterial.ColorTexture.Filter.Value).GetBytes());
			data.Add(((byte)value.RenderMaterial.NormalTexture.Filter.Value).GetBytes());
			data.Add(((byte)0).GetBytes());
			data.Add(((byte)0).GetBytes());
			data.Add(((byte)value.RenderMaterial.ColorTexture.Wrap.Value).GetBytes());
			data.Add(((byte)value.RenderMaterial.NormalTexture.Wrap.Value).GetBytes());
			data.Add(((byte)0).GetBytes());
			data.Add(((byte)0).GetBytes());

			return data.ToArray().ToArray();
		}

		private static void AddResource(List<byte[]> data, SortedDictionary<string, int> pathToIdx, string path)
		{
			if (pathToIdx.ContainsKey(path))
			{
				data.Add(pathToIdx[path].GetBytes());
			}
			else
			{
				data.Add((-1).GetBytes());
			}
		}

		private static void AddColorAll(List<byte[]> data, Data.GpuParticlesValues value)
		{
			var colorSpace = ColorSpace.RGBA;
			switch (value.RenderColor.ColorAll.Type.Value)
			{
				case Data.StandardColorType.Random:
					colorSpace = value.RenderColor.ColorAll.Random.ColorSpace;
					break;
				case Data.StandardColorType.Easing:
					colorSpace = value.RenderColor.ColorAll.Easing.Start.ColorSpace;
					break;
			}
			data.Add(((byte)value.RenderColor.ColorInheritType.Value).GetBytes());
			data.Add(((byte)value.RenderColor.ColorAll.Type.Value).GetBytes());
			data.Add(((byte)colorSpace).GetBytes());

			switch (value.RenderColor.ColorAll.Type.Value)
			{
				case Data.StandardColorType.Fixed:
					data.Add(value.RenderColor.ColorAll.Fixed.GetBytes());
					break;
				case Data.StandardColorType.Random:
					AddColor(data, value.RenderColor.ColorAll.Random);
					break;
				case Data.StandardColorType.Easing:
					AddColorEasing(data, value.RenderColor.ColorAll.Easing);
					break;
				case Data.StandardColorType.FCurve:
					AddColorFCurve(data, value.RenderColor.ColorAll.FCurve, (int)value.Basic.LifeTime.ValueMax);
					break;
				case Data.StandardColorType.Gradient:
					AddColorGradient(data, value.RenderColor.ColorAll.Gradient);
					break;
			}
		}

		private static void AddColor(List<byte[]> data, ColorWithRandom color)
		{
			data.Add(new byte[] {
				(byte)color.R.Max, (byte)color.G.Max, (byte)color.B.Max, (byte)color.A.Max,
				(byte)color.R.Min, (byte)color.G.Min, (byte)color.B.Min, (byte)color.A.Min
			});
		}

		private static void AddEasing(List<byte[]> data, Enum<EasingStart> start, Enum<EasingEnd> end)
		{
			var easing = MathUtl.Easing((float)start.Value, (float)end.Value);
			data.Add(BitConverter.GetBytes(easing[0]));
			data.Add(BitConverter.GetBytes(easing[1]));
			data.Add(BitConverter.GetBytes(easing[2]));
		}

		private static void AddColorEasing(List<byte[]> data, ColorEasingParamater easingParam)
		{
			AddColor(data, easingParam.Start);
			AddColor(data, easingParam.End);
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

		const int GradientResolution = 32;

		private static void AddColorFCurve(List<byte[]> data, ColorFCurveParameter fcurveParam, int lifetime)
		{
			if (fcurveParam.FCurve.Timeline.Value == FCurveTimelineMode.Percent)
			{
				lifetime = 100;
			}

			var buf = new byte[32 * 4];
			for (int i = 0; i < 32; i++)
			{
				int position = (int)((float)i * (float)lifetime / (float)(GradientResolution - 1));
				buf[i * 4 + 0] = (byte)Math.Clamp(fcurveParam.FCurve.R.GetValue(position), 0.0f, 255.0f);
				buf[i * 4 + 1] = (byte)Math.Clamp(fcurveParam.FCurve.G.GetValue(position), 0.0f, 255.0f);
				buf[i * 4 + 2] = (byte)Math.Clamp(fcurveParam.FCurve.B.GetValue(position), 0.0f, 255.0f);
				buf[i * 4 + 3] = (byte)Math.Clamp(fcurveParam.FCurve.A.GetValue(position), 0.0f, 255.0f);
			}
			data.Add(buf);
		}

		private static void AddColorGradient(List<byte[]> data, Gradient gradientParam)
		{
			var buf = new byte[32 * 4];
			for (int i = 0; i < 32; i++)
			{
				float[] color = gradientParam.Sample(i / (float)(GradientResolution - 1));

				buf[i * 4 + 0] = (byte)Math.Clamp(color[0] * 255.0f, 0.0f, 255.0f);
				buf[i * 4 + 1] = (byte)Math.Clamp(color[1] * 255.0f, 0.0f, 255.0f);
				buf[i * 4 + 2] = (byte)Math.Clamp(color[2] * 255.0f, 0.0f, 255.0f);
				buf[i * 4 + 3] = (byte)Math.Clamp(color[3] * 255.0f, 0.0f, 255.0f);
			}
			data.Add(buf);
		}
	}
}
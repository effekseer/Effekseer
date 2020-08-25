using Effekseer.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Effekseer.Utl;

namespace Effekseer.Utils
{

	class ProjectVersionUpdator
	{
		/// <summary>
		/// Update data
		/// </summary>
		/// <param name="node">Root node</param>
		/// <returns></returns>
		public virtual bool Update(Data.NodeRoot rootNode)
		{
			return false;
		}

		/// <summary>
		/// Update data
		/// </summary>
		/// <param name="document"></param>
		/// <returns></returns>
		public virtual bool Update(System.Xml.XmlDocument document)
		{
			return true;
		}
	}

	class ProjectVersionUpdator14xTo15x : ProjectVersionUpdator
	{
		public override bool Update(NodeRoot rootNode)
		{
			{
				Action<Data.NodeBase> convert = null;
				convert = (n) =>
				{
					var n_ = n as Data.Node;

					if (n_ != null && n_.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Ring)
					{
						var rp = n_.DrawingValues.Ring;
						if (rp.ViewingAngle.Value != Data.RendererValues.RingParamater.ViewingAngleType.Fixed ||
							rp.ViewingAngle_Fixed.Value != 360)
						{
							var rc = rp.RingShape.Crescent;
							rp.RingShape.Type.SetValue(Data.RingShapeType.Crescent);
							rc.StartingAngle.SetValue((Data.FixedRandomEasingType)(int)rp.ViewingAngle.Value);
							rc.EndingAngle.SetValue((Data.FixedRandomEasingType)(int)rp.ViewingAngle.Value);

							rc.StartingAngle_Fixed.SetValue((360 - rp.ViewingAngle_Fixed.Value) / 2 + 90);
							rc.EndingAngle_Fixed.SetValue(360 - (360 - rp.ViewingAngle_Fixed.Value) / 2 + 90);

							rc.StartingAngle_Random.SetMax((360 - rp.ViewingAngle_Random.Min) / 2 + 90);
							rc.StartingAngle_Random.SetMin((360 - rp.ViewingAngle_Random.Max) / 2 + 90);

							rc.EndingAngle_Random.SetMax(360 - (360 - rp.ViewingAngle_Random.Max) / 2 + 90);
							rc.EndingAngle_Random.SetMin(360 - (360 - rp.ViewingAngle_Random.Min) / 2 + 90);

							rc.StartingAngle_Easing.Start.SetMax((360 - rp.ViewingAngle_Easing.Start.Min) / 2 + 90);
							rc.StartingAngle_Easing.Start.SetMin((360 - rp.ViewingAngle_Easing.Start.Max) / 2 + 90);
							rc.StartingAngle_Easing.End.SetMax((360 - rp.ViewingAngle_Easing.End.Min) / 2 + 90);
							rc.StartingAngle_Easing.End.SetMin((360 - rp.ViewingAngle_Easing.End.Max) / 2 + 90);

							rc.EndingAngle_Easing.Start.SetMax(360 - (360 - rp.ViewingAngle_Easing.Start.Max) / 2 + 90);
							rc.EndingAngle_Easing.Start.SetMin(360 - (360 - rp.ViewingAngle_Easing.Start.Min) / 2 + 90);
							rc.EndingAngle_Easing.End.SetMax(360 - (360 - rp.ViewingAngle_Easing.End.Max) / 2 + 90);
							rc.EndingAngle_Easing.End.SetMin(360 - (360 - rp.ViewingAngle_Easing.End.Min) / 2 + 90);
						}
					}

					for (int i = 0; i < n.Children.Count; i++)
					{
						convert(n.Children[i]);
					}
				};

				convert(rootNode);
			}

			{
				// Fcurve
				var fcurves = Core.GetFCurveParameterNode(rootNode);

				Action<Utl.ParameterTreeNode> convert = null;
				convert = (n) =>
				{
					foreach (var fcurve in n.Parameters)
					{
						var fcurve2 = fcurve.Item2 as Data.Value.FCurveVector2D;
						var fcurve3 = fcurve.Item2 as Data.Value.FCurveVector3D;
						var fcurvecolor = fcurve.Item2 as Data.Value.FCurveColorRGBA;

						if (fcurve2 != null)
						{
							fcurve2.Timeline.SetValue(Data.Value.FCurveTimelineMode.Time);
							if (!fcurve2.X.Sampling.IsValueAssigned) fcurve2.X.Sampling.SetValue(5);
							if (!fcurve2.Y.Sampling.IsValueAssigned) fcurve2.Y.Sampling.SetValue(5);
						}

						if (fcurve3 != null)
						{
							fcurve3.Timeline.SetValue(Data.Value.FCurveTimelineMode.Time);
							if (!fcurve3.X.Sampling.IsValueAssigned) fcurve3.X.Sampling.SetValue(5);
							if (!fcurve3.Y.Sampling.IsValueAssigned) fcurve3.Y.Sampling.SetValue(5);
							if (!fcurve3.Z.Sampling.IsValueAssigned) fcurve3.Z.Sampling.SetValue(5);
						}

						if (fcurvecolor != null)
						{
							fcurvecolor.Timeline.SetValue(Data.Value.FCurveTimelineMode.Time);
							if (!fcurvecolor.R.Sampling.IsValueAssigned) fcurvecolor.R.Sampling.SetValue(5);
							if (!fcurvecolor.G.Sampling.IsValueAssigned) fcurvecolor.G.Sampling.SetValue(5);
							if (!fcurvecolor.B.Sampling.IsValueAssigned) fcurvecolor.B.Sampling.SetValue(5);
							if (!fcurvecolor.A.Sampling.IsValueAssigned) fcurvecolor.A.Sampling.SetValue(5);
						}
					}

					foreach (var c in n.Children)
					{
						convert(c);
					}
				};

				convert(fcurves);
			}

			return true;
		}

		public override bool Update(XmlDocument document)
		{
			{
				Action<System.Xml.XmlNode> replace = null;
				replace = (node) =>
				{
					if ((node.Name == "RenderCommon") && node.ChildNodes.Count > 0)
					{
						if (node["Distortion"] != null && node["Distortion"].GetText() == "True")
						{
							node.RemoveChild(node["Material"]);
							node.AppendChild(document.CreateTextElement("Material", (int)Data.RendererCommonValues.MaterialType.BackDistortion));
						}
					}
					else
					{
						for (int i = 0; i < node.ChildNodes.Count; i++)
						{
							replace(node.ChildNodes[i]);
						}
					}
				};

				replace(document);
			}

			List<System.Xml.XmlNode> nodes = new List<System.Xml.XmlNode>();

			Action<System.Xml.XmlNode> collectNodes = null;
			collectNodes = (node) =>
			{
				if (node.Name == "Node")
				{
					nodes.Add(node);
				}

				for (int i = 0; i < node.ChildNodes.Count; i++)
				{
					collectNodes(node.ChildNodes[i]);
				}
			};

			collectNodes((XmlNode)document);

			foreach (var node in nodes)
			{
				var rendererCommon = node["RendererCommonValues"];
				var renderer = node["DrawingValues"];

				if (renderer != null && rendererCommon != null)
				{
					if (renderer["Type"] != null && renderer["Type"].GetTextAsInt() == (int)Data.RendererValues.ParamaterType.Model)
					{
						if (renderer["Model"]["Lighting"] == null || renderer["Model"]["Lighting"].GetText() == "True")
						{
							if (node["Material"] != null)
							{
								rendererCommon.RemoveChild(node["Material"]);
							}

							rendererCommon.AppendChild(document.CreateTextElement("Material", (int)Data.RendererCommonValues.MaterialType.Lighting));
						}
					}
				}

				if (rendererCommon != null)
				{
					if (rendererCommon["Distortion"] != null && rendererCommon["Distortion"].GetText() == "True")
					{
						if (node["Material"] != null)
						{
							rendererCommon.RemoveChild(node["Material"]);
						}

						rendererCommon.AppendChild(document.CreateTextElement("Material", (int)Data.RendererCommonValues.MaterialType.BackDistortion));
					}
				}

				if (renderer != null && rendererCommon != null)
				{
					if (renderer["Type"] != null && renderer["Type"].GetTextAsInt() == (int)Data.RendererValues.ParamaterType.Model)
					{
						if (renderer["Model"]["NormalTexture"] != null)
						{
							if (rendererCommon["Filter"] != null)
							{
								rendererCommon.AppendChild(document.CreateTextElement("Filter2", rendererCommon["Filter"].GetText()));
							}

							if (rendererCommon["Wrap"] != null)
							{
								rendererCommon.AppendChild(document.CreateTextElement("Wrap2", rendererCommon["Wrap"].GetText()));
							}
						}
					}
				}

				if (renderer != null && rendererCommon != null)
				{
					if (renderer["Type"] != null && renderer["Type"].GetTextAsInt() == (int)Data.RendererValues.ParamaterType.Model)
					{
						if (renderer["Model"]["NormalTexture"] != null)
						{
							rendererCommon.AppendChild(document.CreateTextElement("NormalTexture", renderer["Model"]["NormalTexture"].GetText()));
						}
					}
				}
			}

			return true;
		}
	}

	class ProjectVersionUpdator15xTo16x : ProjectVersionUpdator
	{
		public override bool Update(System.Xml.XmlDocument document)
		{
			List<System.Xml.XmlNode> nodes = new List<System.Xml.XmlNode>();

			Action<System.Xml.XmlNode> collectNodes = null;
			collectNodes = (node) =>
			{
				if (node.Name == "Node")
				{
					nodes.Add(node);
				}

				for (int i = 0; i < node.ChildNodes.Count; i++)
				{
					collectNodes(node.ChildNodes[i]);
				}
			};

			collectNodes((XmlNode)document);

			foreach (var node in nodes)
			{
				var rendererCommon = node["RendererCommonValues"];

				if (rendererCommon != null)
				{
					if (rendererCommon["UVAnimation"] != null)
					{
						System.Xml.XmlNode uvAnimationParamNode = document.CreateElement("AnimationParams");

						int count = rendererCommon["UVAnimation"].ChildNodes.Count;
						for (int i = 0; i < count; i++)
						{
							uvAnimationParamNode.AppendChild(rendererCommon["UVAnimation"].ChildNodes[0]);
						}

						rendererCommon["UVAnimation"].AppendChild(uvAnimationParamNode);
					}
				}
			}

			return true;
		}
	}
}

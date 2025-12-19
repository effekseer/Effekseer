using Effekseer.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Effekseer.Utils;

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

				Action<Utils.ParameterTreeNode> convert = null;
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

	class ProjectVersionUpdator15xTo16Alpha1 : ProjectVersionUpdator
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
					if (rendererCommon["UVAnimation"] != null && rendererCommon["UVAnimation"]["AnimationParams"] == null)
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

	class ProjectVersionUpdator16Alpha1To16Alpha2 : ProjectVersionUpdator
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
				var labs = node["LocationAbsValues"];

				if (labs != null)
				{
					// old gravity and attractive force to new
					if (labs["Type"] != null && labs["Type"].GetTextAsInt() != 0)
					{
						var lff = document.CreateElement("LocalForceField4");

						var type = labs["Type"].GetTextAsInt();
						if (type == 1)
						{
							lff.AppendChild(document.CreateTextElement("Type", (int)LocalForceFieldType.Gravity));
						}
						else if (type == 2)
						{
							lff.AppendChild(document.CreateTextElement("Type", (int)LocalForceFieldType.AttractiveForce));
						}

						if (labs["Gravity"] != null)
						{
							lff.AppendChild(labs["Gravity"]);
						}

						if (labs["AttractiveForce"] != null)
						{
							lff.AppendChild(labs["AttractiveForce"]);

							if (lff["AttractiveForce"]["Force"] != null)
							{
								var force = lff["AttractiveForce"]["Force"].GetTextAsFloat();
								lff.AppendChild(document.CreateTextElement("Power", force.ToString()));
							}
						}

						labs.AppendChild(lff);
					}

					Action<XmlElement> convert = (elm) =>
					{
						if (elm == null)
							return;

						var typeInt = elm["Type"]?.GetTextAsInt();
						var type = typeInt.HasValue ? (Data.LocalForceFieldType)(typeInt.Value) : Data.LocalForceFieldType.None;

						if (type == LocalForceFieldType.Turbulence)
						{
							if (elm["Turbulence"] != null && elm["Turbulence"]["Strength"] != null)
							{
								var strength = elm["Turbulence"]["Strength"].GetTextAsFloat();
								elm.AppendChild(document.CreateTextElement("Power", strength.ToString()));
							}
						}

						float defaultPower = 1.0f;

						if (type == LocalForceFieldType.Turbulence)
						{
							defaultPower = 0.1f;
						}

						if (elm["Power"] != null)
						{
							defaultPower = elm["Power"].GetTextAsFloat();
						}

						if (type == LocalForceFieldType.Turbulence)
						{
							defaultPower *= 10.0f;
						}

						if (type == LocalForceFieldType.Vortex)
						{
							defaultPower /= 5.0f;
						}

						if (elm["Power"] != null)
						{
							elm.RemoveChild(elm["Power"]);
						}

						elm.AppendChild(document.CreateTextElement("Power", defaultPower.ToString()));

						if (type == LocalForceFieldType.Vortex)
						{
							if (elm["Vortex"] == null)
							{
								elm.AppendChild(document.CreateElement("Vortex"));
							}

							elm["Vortex"].AppendChild(document.CreateTextElement("VortexType", ((int)ForceFieldVortexType.ConstantSpeed).ToString()));
						}

						if (type == LocalForceFieldType.Turbulence)
						{
							if (elm["Turbulence"] == null)
							{
								elm.AppendChild(document.CreateElement("Turbulence"));
							}

							elm["Turbulence"].AppendChild(document.CreateTextElement("TurbulenceType", ((int)ForceFieldTurbulenceType.Complicated).ToString()));
						}
					};

					convert(labs["LocalForceField1"]);
					convert(labs["LocalForceField2"]);
					convert(labs["LocalForceField3"]);
				}
			}

			return true;
		}
	}

	class ProjectVersionUpdator16Alpha2To16x : ProjectVersionUpdator
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
				var rv = node["DrawingValues"];
				var rcv = node["AdvancedRendererCommonValuesValues"];

				if (rv != null)
				{
					if ((rv["EnableFalloff"] != null || rv["FalloffParam"] != null) && rcv == null)
					{
						node.AppendChild(document.CreateElement("AdvancedRendererCommonValuesValues"));
						rcv = node["AdvancedRendererCommonValuesValues"];
					}

					if (rv["EnableFalloff"] != null)
					{
						rcv.AppendChild(rv["EnableFalloff"]);
					}

					if (rv["FalloffParam"] != null)
					{
						rcv.AppendChild(rv["FalloffParam"]);
					}
				}
			}

			return true;
		}
	}

	class ProjectVersionUpdator16Alpha5To16x : ProjectVersionUpdator
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
				var rcv1 = node["AdvancedRendererCommonValuesValues"];
				if (rcv1 != null)
				{
					var enableAlphaTexture = rcv1["EnableAlphaTexture"];
					if (enableAlphaTexture != null)
					{
						var alphaTextureParam = rcv1["AlphaTextureParam"] as XmlNode;
						alphaTextureParam?.PrependChild(document.CreateTextElement("Enabled", enableAlphaTexture.InnerText));
					}

					var enableUVDistortion = rcv1["EnableUVDistortion"];
					if (enableUVDistortion != null)
					{
						var uvDistortionParam = rcv1["UVDistortionParam"] as XmlNode;
						uvDistortionParam?.PrependChild(document.CreateTextElement("Enabled", enableUVDistortion.InnerText));
					}

					var alphaCutoffParam = rcv1["AlphaCutoffParam"] as XmlNode;
					if (alphaCutoffParam != null)
					{
						var typeNode = alphaCutoffParam["Type"];
						var fixedNode = alphaCutoffParam["Fixed"];
						bool enableAlphaCutoff =
							(typeNode != null && int.Parse(typeNode.InnerText) != 0) ||
							(fixedNode != null && float.Parse(fixedNode["Threshold"].InnerText) != 0.0f);
						alphaCutoffParam.PrependChild(document.CreateTextElement("Enabled", enableAlphaCutoff.ToString()));
					}

					var enableFalloff = rcv1["EnableFalloff"];
					if (enableFalloff != null)
					{
						var falloffParam = rcv1["FalloffParam"] as XmlNode;
						falloffParam.PrependChild(document.CreateTextElement("Enabled", enableFalloff.InnerText));
					}

					var softParticleDistance = rcv1["SoftParticleDistance"];
					var softParticleDistanceNear = rcv1["SoftParticleDistanceNear"];
					var softParticleDistanceNearOffset = rcv1["SoftParticleDistanceNearOffset"];

					if (softParticleDistance != null || softParticleDistanceNear != null || softParticleDistanceNearOffset != null)
					{
						var softParticleParams = document.CreateElement("SoftParticleParams");

						if (softParticleDistance != null)
						{
							softParticleParams.AppendChild(document.CreateTextElement("Enabled", (softParticleDistance.GetTextAsFloat() != 0.0f).ToString()));
							softParticleParams.AppendChild(document.CreateTextElement("Distance", softParticleDistance.InnerText));
						}
						if (softParticleDistanceNear != null)
						{
							softParticleParams.AppendChild(document.CreateTextElement("DistanceNear", softParticleDistanceNear.InnerText));
						}
						if (softParticleDistanceNearOffset != null)
						{
							softParticleParams.AppendChild(document.CreateTextElement("DistanceNearOffset", softParticleDistanceNearOffset.InnerText));
						}

						rcv1.AppendChild(softParticleParams);
					}
				}

				var rcv2 = node["AdvancedRendererCommonValues2Values"];
				if (rcv2 != null)
				{
					node.RemoveChild(rcv2);

					if (rcv1 == null)
					{
						rcv1 = document.CreateElement("AdvancedRendererCommonValuesValues");
						node.AppendChild(rcv1);
					}

					var blendTextureParams = rcv2["BlendTextureParams"];
					var enableBlendTexture = rcv2["EnableBlendTexture"];

					if (enableBlendTexture != null && blendTextureParams != null)
					{
						rcv2.RemoveChild(blendTextureParams);
						rcv2.RemoveChild(enableBlendTexture);
						blendTextureParams.AppendChild(document.CreateTextElement("Enabled", enableBlendTexture.InnerText));
						rcv1.AppendChild(blendTextureParams);
					}
				}
			}

			return true;
		}
	}

	class ProjectVersionUpdator16Alpha8To16x : ProjectVersionUpdator
	{
		public override bool Update(System.Xml.XmlDocument document)
		{
			var proceduralElement = document["EffekseerProject"]["ProcedualModel"];
			if (proceduralElement != null)
			{
				document["EffekseerProject"].RemoveChild(proceduralElement);
				var newNode = document.CreateElement("ProceduralModel");
				var newNode2 = document.CreateElement("ProceduralModels");
				newNode.AppendChild(newNode2);

				List<XmlNode> children = new List<XmlNode>();

				for (int i = 0; i < proceduralElement.FirstChild.ChildNodes.Count; i++)
				{
					children.Add(proceduralElement.FirstChild.ChildNodes[i]);
				}

				foreach (var child in children)
				{
					Action<XmlNode, XmlNode, string> moveElement = (XmlNode dst, XmlNode src, string name) =>
					 {
						 var node = src[name];
						 if (node != null)
						 {
							 dst.AppendChild(node);
						 }
					 };

					Action<XmlNode, XmlNode, string, string> moveAndRenameElement = (XmlNode dst, XmlNode src, string newName, string name) =>
					{
						var node = src[name];
						if (node != null)
						{
							src.RemoveChild(node);

							var nn = document.CreateElement(newName);
							while (node.ChildNodes.Count > 0)
							{
								nn.AppendChild(node.FirstChild);
							}
							dst.AppendChild(nn);
						}
					};

					var mesh = document.CreateElement("Mesh");
					var ribbon = document.CreateElement("Ribbon");
					var shape = document.CreateElement("Shape");
					var shapeNoise = document.CreateElement("ShapeNoise");
					var vertexColor = document.CreateElement("VertexColor");

					moveElement(mesh, child, "AngleBeginEnd");
					moveElement(mesh, child, "Divisions");

					moveElement(ribbon, child, "CrossSection");
					moveElement(ribbon, child, "Rotate");
					moveElement(ribbon, child, "Vertices");
					moveElement(ribbon, child, "RibbonScales");
					moveElement(ribbon, child, "RibbonAngles");
					moveElement(ribbon, child, "RibbonNoises");
					moveElement(ribbon, child, "Count");

					moveElement(shape, child, "PrimitiveType");
					moveElement(shape, child, "Radius");
					moveElement(shape, child, "Radius2");
					moveElement(shape, child, "Depth");
					moveElement(shape, child, "DepthMin");
					moveElement(shape, child, "DepthMax");
					moveElement(shape, child, "Point1");
					moveElement(shape, child, "Point2");
					moveElement(shape, child, "Point3");
					moveElement(shape, child, "Point4");
					moveElement(shape, child, "AxisType");

					moveElement(shapeNoise, child, "TiltNoiseFrequency");
					moveElement(shapeNoise, child, "TiltNoiseOffset");
					moveElement(shapeNoise, child, "TiltNoisePower");
					moveElement(shapeNoise, child, "WaveNoiseFrequency");
					moveElement(shapeNoise, child, "WaveNoiseOffset");
					moveElement(shapeNoise, child, "WaveNoisePower");
					moveElement(shapeNoise, child, "CurlNoiseFrequency");
					moveElement(shapeNoise, child, "CurlNoiseOffset");
					moveElement(shapeNoise, child, "CurlNoisePower");

					moveAndRenameElement(vertexColor, child, "ColorUpperLeft", "ColorLeft");
					moveAndRenameElement(vertexColor, child, "ColorUpperCenter", "ColorCenter");
					moveAndRenameElement(vertexColor, child, "ColorUpperRight", "ColorRight");
					moveAndRenameElement(vertexColor, child, "ColorMiddleLeft", "ColorLeftMiddle");
					moveAndRenameElement(vertexColor, child, "ColorMiddleCenter", "ColorCenterMiddle");
					moveAndRenameElement(vertexColor, child, "ColorMiddleRight", "ColorRightMiddle");
					moveElement(vertexColor, child, "ColorCenterArea");

					child.AppendChild(mesh);
					child.AppendChild(ribbon);
					child.AppendChild(shape);
					child.AppendChild(shapeNoise);
					child.AppendChild(vertexColor);

					newNode2.AppendChild(child);
				}

				document["EffekseerProject"].AppendChild(newNode);
			}
			return true;
		}
	}

	class ProjectVersionUpdator16xTo17Alpha1 : ProjectVersionUpdator
	{
		public override bool Update(NodeRoot rootNode)
		{
			Action<Data.NodeBase> convert = null;
			convert = (n) =>
			{
				var n_ = n as Data.Node;

				if (n_ != null && (n_.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Ribbon || n_.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Track))
				{
					var rp = n_.DrawingValues;
					rp.TrailSmoothing.SetValueDirectly(TrailSmoothingType.Disabled);
					rp.TrailTimeSource.SetValueDirectly(TrailTimeSourceType.FirstParticle);
				}

				if (n_ != null && n_.DrawingValues.Type.Value == RendererValues.ParamaterType.Sprite)
				{
					StandardColor sc = new StandardColor();
					sc.Type = n_.DrawingValues.Sprite.ColorAll;
					sc.Fixed = n_.DrawingValues.Sprite.ColorAll_Fixed;
					sc.Random = n_.DrawingValues.Sprite.ColorAll_Random;
					sc.Easing = n_.DrawingValues.Sprite.ColorAll_Easing;
					sc.FCurve = n_.DrawingValues.Sprite.ColorAll_FCurve;
					var text = Core.Copy("Temp", sc);
					Core.Paste("Temp", n_.DrawingValues.ColorAll, text);
				}

				if (n_ != null && n_.DrawingValues.Type.Value == RendererValues.ParamaterType.Model)
				{
					StandardColor sc = new StandardColor();
					sc.Type = n_.DrawingValues.Model.Color;
					sc.Fixed = n_.DrawingValues.Model.Color_Fixed;
					sc.Random = n_.DrawingValues.Model.Color_Random;
					sc.Easing = n_.DrawingValues.Model.Color_Easing;
					sc.FCurve = n_.DrawingValues.Model.Color_FCurve;

					var text = Core.Copy("Temp", sc);
					Core.Paste("Temp", n_.DrawingValues.ColorAll, text);
				}

				for (int i = 0; i < n.Children.Count; i++)
				{
					convert(n.Children[i]);
				}
			};

			convert(rootNode);

			return true;
		}
	}


	class ProjectVersionUpdator17Alpha1To17Alpha2 : ProjectVersionUpdator
	{
		public override bool Update(NodeRoot rootNode)
		{
			Action<Data.NodeBase> convert = null;
			convert = (n) =>
			{
				var n_ = n as Data.Node;

				if (n_ != null && n_.DrawingValues.Type.Value == RendererValues.ParamaterType.Track)
				{
					{
						StandardColor sc = new StandardColor();
						sc.Type = n_.DrawingValues.Track.ColorLeft;
						sc.Fixed = n_.DrawingValues.Track.ColorLeft_Fixed;
						sc.Random = n_.DrawingValues.Track.ColorLeft_Random;
						sc.Easing = n_.DrawingValues.Track.ColorLeft_Easing;
						sc.FCurve = n_.DrawingValues.Track.ColorLeft_FCurve;
						var text = Core.Copy("Temp", sc);
						Core.Paste("Temp", n_.DrawingValues.TrailColorLeft, text);
					}

					{
						StandardColor sc = new StandardColor();
						sc.Type = n_.DrawingValues.Track.ColorLeftMiddle;
						sc.Fixed = n_.DrawingValues.Track.ColorLeftMiddle_Fixed;
						sc.Random = n_.DrawingValues.Track.ColorLeftMiddle_Random;
						sc.Easing = n_.DrawingValues.Track.ColorLeftMiddle_Easing;
						sc.FCurve = n_.DrawingValues.Track.ColorLeftMiddle_FCurve;
						var text = Core.Copy("Temp", sc);
						Core.Paste("Temp", n_.DrawingValues.TrailColorLeftMiddle, text);
					}

					{
						StandardColor sc = new StandardColor();
						sc.Type = n_.DrawingValues.Track.ColorCenter;
						sc.Fixed = n_.DrawingValues.Track.ColorCenter_Fixed;
						sc.Random = n_.DrawingValues.Track.ColorCenter_Random;
						sc.Easing = n_.DrawingValues.Track.ColorCenter_Easing;
						sc.FCurve = n_.DrawingValues.Track.ColorCenter_FCurve;
						var text = Core.Copy("Temp", sc);
						Core.Paste("Temp", n_.DrawingValues.TrailColorCenter, text);
					}

					{
						StandardColor sc = new StandardColor();
						sc.Type = n_.DrawingValues.Track.ColorCenterMiddle;
						sc.Fixed = n_.DrawingValues.Track.ColorCenterMiddle_Fixed;
						sc.Random = n_.DrawingValues.Track.ColorCenterMiddle_Random;
						sc.Easing = n_.DrawingValues.Track.ColorCenterMiddle_Easing;
						sc.FCurve = n_.DrawingValues.Track.ColorCenterMiddle_FCurve;
						var text = Core.Copy("Temp", sc);
						Core.Paste("Temp", n_.DrawingValues.TrailColorCenterMiddle, text);
					}

					{
						StandardColor sc = new StandardColor();
						sc.Type = n_.DrawingValues.Track.ColorRight;
						sc.Fixed = n_.DrawingValues.Track.ColorRight_Fixed;
						sc.Random = n_.DrawingValues.Track.ColorRight_Random;
						sc.Easing = n_.DrawingValues.Track.ColorRight_Easing;
						sc.FCurve = n_.DrawingValues.Track.ColorRight_FCurve;
						var text = Core.Copy("Temp", sc);
						Core.Paste("Temp", n_.DrawingValues.TrailColorRight, text);
					}

					{
						StandardColor sc = new StandardColor();
						sc.Type = n_.DrawingValues.Track.ColorRightMiddle;
						sc.Fixed = n_.DrawingValues.Track.ColorRightMiddle_Fixed;
						sc.Random = n_.DrawingValues.Track.ColorRightMiddle_Random;
						sc.Easing = n_.DrawingValues.Track.ColorRightMiddle_Easing;
						sc.FCurve = n_.DrawingValues.Track.ColorRightMiddle_FCurve;
						var text = Core.Copy("Temp", sc);
						Core.Paste("Temp", n_.DrawingValues.TrailColorRightMiddle, text);
					}
				}

				for (int i = 0; i < n.Children.Count; i++)
				{
					convert(n.Children[i]);
				}
			};

			convert(rootNode);

			return true;
		}
	}

	class ProjectVersionUpdator17To18Beta2 : ProjectVersionUpdator
	{
		public override bool Update(NodeRoot rootNode)
		{
			Action<Data.NodeBase> convert = null;
			convert = (n) =>
			{
				var node = n as Data.Node;

				if (node != null)
				{
					var common = node.CommonValues;

					common.Generation.Timing.SetValueDirectly(Data.CommonValues.GenerationTimingType.Continuous);
					common.Removal.WhenLifeIsExtinct.SetValueDirectly(common.RemoveWhenLifeIsExtinct.GetValue());
					common.Removal.WhenParentIsRemoved.SetValueDirectly(common.RemoveWhenParentIsRemoved.GetValue());
					common.Removal.WhenAllChildrenAreRemoved.SetValueDirectly(common.RemoveWhenAllChildrenAreRemoved.GetValue());

					if (common.TriggerParam != null)
					{
						common.Removal.TriggerToRemove.SetValueDirectly(common.TriggerParam.ToRemove.GetValue());
						common.Generation.ToStartGeneration.SetValueDirectly(common.TriggerParam.ToStartGeneration.GetValue());
						common.Generation.ToStopGeneration.SetValueDirectly(common.TriggerParam.ToStopGeneration.GetValue());
					}

					common.Generation.GenerationTime.SetCenterDirectly(common.GenerationTime.Center);
					common.Generation.GenerationTime.SetMaxDirectly(common.GenerationTime.Max);
					common.Generation.GenerationTime.SetMinDirectly(common.GenerationTime.Min);
					common.Generation.GenerationTime.DrawnAs = common.GenerationTime.DrawnAs;
					common.Generation.GenerationTime.Step = common.GenerationTime.Step;
					common.Generation.GenerationTime.DynamicEquationMax.SetValueWithIndex(common.GenerationTime.DynamicEquationMax.Index);
					common.Generation.GenerationTime.DynamicEquationMin.SetValueWithIndex(common.GenerationTime.DynamicEquationMin.Index);

					common.Generation.GenerationTimeOffset.SetCenterDirectly(common.GenerationTimeOffset.Center);
					common.Generation.GenerationTimeOffset.SetMaxDirectly(common.GenerationTimeOffset.Max);
					common.Generation.GenerationTimeOffset.SetMinDirectly(common.GenerationTimeOffset.Min);
					common.Generation.GenerationTimeOffset.DrawnAs = common.GenerationTimeOffset.DrawnAs;
					common.Generation.GenerationTimeOffset.Step = common.GenerationTimeOffset.Step;
					common.Generation.GenerationTimeOffset.DynamicEquationMax.SetValueWithIndex(common.GenerationTimeOffset.DynamicEquationMax.Index);
					common.Generation.GenerationTimeOffset.DynamicEquationMin.SetValueWithIndex(common.GenerationTimeOffset.DynamicEquationMin.Index);
				}

				for (int i = 0; i < n.Children.Count; i++)
				{
					convert(n.Children[i]);
				}
			};

			convert(rootNode);
			return true;
		}
	}
}

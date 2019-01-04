using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Reflection;
using Effekseer.Utl;
using System.Threading;
using System.Threading.Tasks;

namespace Effekseer.Data
{
	public class IO
	{
		public static XmlElement SaveObjectToElement(XmlDocument doc, string element_name, object o, bool isClip)
		{
			XmlElement e_o = doc.CreateElement(element_name);

			var properties = o.GetType().GetProperties(BindingFlags.Public | BindingFlags.Instance);

			foreach (var property in properties)
			{
				var io_attribute = property.GetCustomAttributes(typeof(IOAttribute), false).FirstOrDefault() as IOAttribute;
				if (io_attribute != null && !io_attribute.Export) continue;

				var method = typeof(IO).GetMethod("SaveToElement", new Type[] { typeof(XmlDocument), typeof(string), property.PropertyType, typeof(bool) });
				if (method != null)
				{
					var property_value = property.GetValue(o, null);
					var element = method.Invoke(null, new object[] { doc, property.Name, property_value, isClip });

					if (element != null)
					{
						e_o.AppendChild(element as XmlNode);
					}
				}
				else
				{
					if (io_attribute != null && io_attribute.Export)
					{
						var property_value = property.GetValue(o, null);
						var element = SaveObjectToElement(doc, property.Name, property_value, isClip);

						if (element != null && element.ChildNodes.Count > 0)
						{
							e_o.AppendChild(element as XmlNode);
						}
					}
				}
			}

			if(e_o.ChildNodes.Count > 0) return e_o;
			return null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, NodeBase node, bool isClip)
		{
			return SaveObjectToElement(doc, element_name, node, isClip);
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, NodeBase.ChildrenCollection children, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			for (int i = 0; i < children.Count; i++)
			{
				var e_node = SaveToElement(doc, children[i].GetType().Name, children[i], isClip);
				e.AppendChild(e_node);
			}

			return e;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.String value, bool isClip)
		{
			if (value.DefaultValue == value.Value && !isClip) return null;
			var text = value.GetValue().ToString();
			return doc.CreateTextElement(element_name, text);
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.Boolean value, bool isClip)
		{
			if (value.DefaultValue == value.Value && !isClip) return null;
			var text = value.GetValue().ToString();
			return doc.CreateTextElement(element_name, text);
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.Int value, bool isClip)
		{
			if (value.Value == value.DefaultValue && !isClip) return null;
			var text = value.GetValue().ToString();
			return doc.CreateTextElement(element_name, text);
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.Float value, bool isClip)
		{
			if (value.Value == value.DefaultValue && !isClip) return null;
			var text = value.GetValue().ToString();
			return doc.CreateTextElement(element_name, text);
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.IntWithInifinite value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var v = SaveToElement(doc, "Value", value.Value, isClip);
			var i = SaveToElement(doc, "Infinite", value.Infinite, isClip);
			
			if (v != null) e.AppendChild(v);
			if (i != null) e.AppendChild(i);

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.Vector2D value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var x = SaveToElement(doc, "X", value.X, isClip);
			var y = SaveToElement(doc, "Y", value.Y, isClip);

			if (x != null) e.AppendChild(x);
			if (y != null) e.AppendChild(y);

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.Vector3D value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var x = SaveToElement(doc, "X", value.X, isClip);
			var y = SaveToElement(doc, "Y", value.Y, isClip);
			var z = SaveToElement(doc, "Z", value.Z, isClip);

			if (x != null) e.AppendChild(x);
			if (y != null) e.AppendChild(y);
			if (z != null) e.AppendChild(z);

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.Color value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var r = SaveToElement(doc, "R", value.R, isClip);
			var g = SaveToElement(doc, "G", value.G, isClip);
			var b = SaveToElement(doc, "B", value.B, isClip);
			var a = SaveToElement(doc, "A", value.A, isClip);

			if (r != null) e.AppendChild(r);
			if (g != null) e.AppendChild(g);
			if (b != null) e.AppendChild(b);
			if (a != null) e.AppendChild(a);

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.IntWithRandom value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			if (value.DefaultValueCenter != value.Center || isClip) e.AppendChild(doc.CreateTextElement("Center", value.Center.ToString()));
			if (value.DefaultValueMax != value.Max || isClip) e.AppendChild(doc.CreateTextElement("Max", value.Max.ToString()));
			if (value.DefaultValueMin != value.Min || isClip) e.AppendChild(doc.CreateTextElement("Min", value.Min.ToString()));
			if (value.DefaultDrawnAs != value.DrawnAs || isClip) e.AppendChild(doc.CreateTextElement("DrawnAs", (int)value.DrawnAs));

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.FloatWithRandom value, bool isClip)
		{
			var e = doc.CreateElement(element_name);

			if (value.DefaultValueCenter != value.Center || isClip) e.AppendChild(doc.CreateTextElement("Center", value.Center.ToString()));
			if (value.DefaultValueMax != value.Max || isClip) e.AppendChild(doc.CreateTextElement("Max", value.Max.ToString()));
			if (value.DefaultValueMin != value.Min || isClip) e.AppendChild(doc.CreateTextElement("Min", value.Min.ToString()));
			if (value.DefaultDrawnAs != value.DrawnAs || isClip) e.AppendChild(doc.CreateTextElement("DrawnAs", (int)value.DrawnAs));
			
			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.Vector2DWithRandom value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var x = SaveToElement(doc, "X", value.X, isClip);
			var y = SaveToElement(doc, "Y", value.Y, isClip);
			var da = (value.DefaultDrawnAs != value.DrawnAs || isClip) ? doc.CreateTextElement("DrawnAs", (int)value.DrawnAs) : null;

			if (x != null) e.AppendChild(x);
			if (y != null) e.AppendChild(y);
			if (da != null) e.AppendChild(da);

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.Vector3DWithRandom value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var x = SaveToElement(doc, "X", value.X, isClip);
			var y = SaveToElement(doc, "Y", value.Y, isClip);
			var z = SaveToElement(doc, "Z", value.Z, isClip);
			var da = (value.DefaultDrawnAs != value.DrawnAs || isClip) ? doc.CreateTextElement("DrawnAs", (int)value.DrawnAs) : null;

			if (x != null) e.AppendChild(x);
			if (y != null) e.AppendChild(y);
			if (z != null) e.AppendChild(z);
			if (da != null) e.AppendChild(da);

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.ColorWithRandom value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var r = SaveToElement(doc, "R", value.R, isClip);
			var g = SaveToElement(doc, "G", value.G, isClip);
			var b = SaveToElement(doc, "B", value.B, isClip);
			var a = SaveToElement(doc, "A", value.A, isClip);
			var da = (value.DefaultDrawnAs != value.DrawnAs || isClip) ? doc.CreateTextElement("DrawnAs", (int)value.DrawnAs) : null;
			var cs = (value.DefaultColorSpace != value.ColorSpace || isClip) ? doc.CreateTextElement("ColorSpace", (int)value.ColorSpace) : null;

			if (r != null) e.AppendChild(r);
			if (g != null) e.AppendChild(g);
			if (b != null) e.AppendChild(b);
			if (a != null) e.AppendChild(a);
			if (da != null) e.AppendChild(da);
			if (cs != null) e.AppendChild(cs);
			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.EnumBase value, bool isClip)
		{
			if (value.GetValueAsInt() == value.GetDefaultValueAsInt() && !isClip) return null;

			var text = value.GetValueAsInt().ToString();
			return doc.CreateTextElement(element_name, text);
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.Path value, bool isClip)
		{
			if (value.DefaultValue == value.GetAbsolutePath() && !isClip) return null;

			var text = "";
			if(!isClip && value.IsRelativeSaved)
				text = value.GetRelativePath();
			else
				text = value.GetAbsolutePath();

			return doc.CreateTextElement(element_name, text);
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.FCurveVector2D value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var keys = doc.CreateElement("Keys");
			var x = doc.CreateElement("X");
			var y = doc.CreateElement("Y");

			int index = 0;

			Action<Value.FCurve<float>, XmlElement> setValues = (v, xml) =>
				{
					index = 0;

					var st = SaveToElement(doc, "StartType", v.StartType, isClip);
					var et = SaveToElement(doc, "EndType", v.EndType, isClip);
					var omax = SaveToElement(doc, "OffsetMax", v.OffsetMax, isClip);
					var omin = SaveToElement(doc, "OffsetMin", v.OffsetMin, isClip);
					var s = SaveToElement(doc, "Sampling", v.Sampling, isClip);

					if (st != null) xml.AppendChild(st);
					if (et != null) xml.AppendChild(et);
					if (omax != null) xml.AppendChild(omax);
					if (omin != null) xml.AppendChild(omin);
					if (s != null) xml.AppendChild(s);

					foreach (var k_ in v.Keys)
					{
						var k = doc.CreateElement("Key" + index.ToString());
						k.AppendChild(doc.CreateTextElement("Frame", k_.Frame.ToString()));
						k.AppendChild(doc.CreateTextElement("Value", k_.ValueAsFloat.ToString()));
						k.AppendChild(doc.CreateTextElement("LeftX", k_.LeftX.ToString()));
						k.AppendChild(doc.CreateTextElement("LeftY", k_.LeftY.ToString()));
						k.AppendChild(doc.CreateTextElement("RightX", k_.RightX.ToString()));
						k.AppendChild(doc.CreateTextElement("RightY", k_.RightY.ToString()));

						k.AppendChild(doc.CreateTextElement("InterpolationType", k_.InterpolationType.GetValueAsInt()));

						xml.AppendChild(k);
						index++;
					}
				};

			setValues(value.X, x);
			setValues(value.Y, y);

			if (x.ChildNodes.Count > 0) keys.AppendChild(x);
			if (y.ChildNodes.Count > 0) keys.AppendChild(y);
			if (keys.ChildNodes.Count > 0) e.AppendChild(keys);

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.FCurveVector3D value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var keys = doc.CreateElement("Keys");
			var x = doc.CreateElement("X");
			var y = doc.CreateElement("Y");
			var z = doc.CreateElement("Z");

			int index = 0;

			Action<Value.FCurve<float>, XmlElement> setValues = (v, xml) =>
			{
				index = 0;

				var st = SaveToElement(doc, "StartType", v.StartType, isClip);
				var et = SaveToElement(doc, "EndType", v.EndType, isClip);
				var omax = SaveToElement(doc, "OffsetMax", v.OffsetMax, isClip);
				var omin = SaveToElement(doc, "OffsetMin", v.OffsetMin, isClip);
				var s = SaveToElement(doc, "Sampling", v.Sampling, isClip);

				if (st != null) xml.AppendChild(st);
				if (et != null) xml.AppendChild(et);
				if (omax != null) xml.AppendChild(omax);
				if (omin != null) xml.AppendChild(omin);
				if (s != null) xml.AppendChild(s);

				foreach (var k_ in v.Keys)
				{
					var k = doc.CreateElement("Key" + index.ToString());
					k.AppendChild(doc.CreateTextElement("Frame", k_.Frame.ToString()));
					k.AppendChild(doc.CreateTextElement("Value", k_.ValueAsFloat.ToString()));
					k.AppendChild(doc.CreateTextElement("LeftX", k_.LeftX.ToString()));
					k.AppendChild(doc.CreateTextElement("LeftY", k_.LeftY.ToString()));
					k.AppendChild(doc.CreateTextElement("RightX", k_.RightX.ToString()));
					k.AppendChild(doc.CreateTextElement("RightY", k_.RightY.ToString()));

					k.AppendChild(doc.CreateTextElement("InterpolationType", k_.InterpolationType.GetValueAsInt()));

					xml.AppendChild(k);
					index++;
				}
			};

			setValues(value.X, x);
			setValues(value.Y, y);
			setValues(value.Z, z);

			if (x.ChildNodes.Count > 0) keys.AppendChild(x);
			if (y.ChildNodes.Count > 0) keys.AppendChild(y);
			if (z.ChildNodes.Count > 0) keys.AppendChild(z);
			if (keys.ChildNodes.Count > 0) e.AppendChild(keys);

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.FCurveColorRGBA value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var keys = doc.CreateElement("Keys");
			var r = doc.CreateElement("R");
			var g = doc.CreateElement("G");
			var b = doc.CreateElement("B");
			var a = doc.CreateElement("A");

			int index = 0;

			Action<Value.FCurve<byte>, XmlElement> setValues = (v, xml) =>
			{
				index = 0;

				var st = SaveToElement(doc, "StartType", v.StartType, isClip);
				var et = SaveToElement(doc, "EndType", v.EndType, isClip);
				var omax = SaveToElement(doc, "OffsetMax", v.OffsetMax, isClip);
				var omin = SaveToElement(doc, "OffsetMin", v.OffsetMin, isClip);
				var s = SaveToElement(doc, "Sampling", v.Sampling, isClip);

				if (st != null) xml.AppendChild(st);
				if (et != null) xml.AppendChild(et);
				if (omax != null) xml.AppendChild(omax);
				if (omin != null) xml.AppendChild(omin);
				if (s != null) xml.AppendChild(s);

				foreach (var k_ in v.Keys)
				{
					var k = doc.CreateElement("Key" + index.ToString());
					k.AppendChild(doc.CreateTextElement("Frame", k_.Frame.ToString()));
					k.AppendChild(doc.CreateTextElement("Value", k_.ValueAsFloat.ToString()));
					k.AppendChild(doc.CreateTextElement("LeftX", k_.LeftX.ToString()));
					k.AppendChild(doc.CreateTextElement("LeftY", k_.LeftY.ToString()));
					k.AppendChild(doc.CreateTextElement("RightX", k_.RightX.ToString()));
					k.AppendChild(doc.CreateTextElement("RightY", k_.RightY.ToString()));

					k.AppendChild(doc.CreateTextElement("InterpolationType", k_.InterpolationType.GetValueAsInt()));

					xml.AppendChild(k);
					index++;
				}
			};

			setValues(value.R, r);
			setValues(value.G, g);
			setValues(value.B, b);
			setValues(value.A, a);

			if (r.ChildNodes.Count > 0) keys.AppendChild(r);
			if (g.ChildNodes.Count > 0) keys.AppendChild(g);
			if (b.ChildNodes.Count > 0) keys.AppendChild(b);
			if (a.ChildNodes.Count > 0) keys.AppendChild(a);

			if (keys.ChildNodes.Count > 0) e.AppendChild(keys);

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static void LoadObjectFromElement(XmlElement e, ref object o, bool isClip)
		{
			var o_type = o.GetType();

			foreach (var _ch_node in e.ChildNodes)
			{
				var ch_node = _ch_node as XmlElement;
				var local_name = ch_node.LocalName;

				var property = o_type.GetProperty(local_name);
				if (property == null) continue;

				var io_attribute = property.GetCustomAttributes(typeof(IOAttribute), false).FirstOrDefault() as IOAttribute;
				if (io_attribute != null && !io_attribute.Import) continue;

				var method = typeof(IO).GetMethod("LoadFromElement", new Type[] { typeof(XmlElement), property.PropertyType, typeof(bool) });
				if (method != null)
				{
					var property_value = property.GetValue(o, null);
					method.Invoke(null, new object[] { ch_node, property_value, isClip });
				}
				else
				{
					if (io_attribute != null && io_attribute.Import)
					{
						var property_value = property.GetValue(o, null);
						LoadObjectFromElement(ch_node, ref property_value, isClip);
					}
				}
			}
		}

		public static void LoadFromElement(XmlElement e, NodeBase node, bool isClip)
		{
			var o = node as object;
			LoadObjectFromElement(e, ref o, isClip);
		}

		public static void LoadFromElement(XmlElement e, NodeBase.ChildrenCollection children, bool isClip)
		{
			children.Node.ClearChildren();

			for (var i = 0; i < e.ChildNodes.Count; i++)
			{
				var e_child = e.ChildNodes[i] as XmlElement;
				if (e_child.LocalName != "Node") continue;

				var node = children.Node.AddChild();
				LoadFromElement(e_child, node, isClip);
			}
		}

		public static void LoadFromElement(XmlElement e, Value.String value, bool isClip)
		{
			var text = e.GetText();
			value.SetValue(text);
		}

		public static void LoadFromElement(XmlElement e, Value.Boolean value, bool isClip)
		{
			var text = e.GetText();
			var parsed = false;
			if (bool.TryParse(text, out parsed))
			{
				value.SetValue(parsed);
			}
		}

		public static void LoadFromElement(XmlElement e, Value.Int value, bool isClip)
		{
			var text = e.GetText();
			var parsed = 0;
			if (int.TryParse(text, System.Globalization.NumberStyles.Integer, Setting.NFI, out parsed))
			{
				value.SetValue(parsed);
			}
		}

		public static void LoadFromElement(XmlElement e, Value.Float value, bool isClip)
		{
			var text = e.GetText();
			var parsed = 0.0f;
			if (float.TryParse(text, System.Globalization.NumberStyles.Float, Setting.NFI, out parsed))
			{
				value.SetValue(parsed);
			}
		}

		public static void LoadFromElement(XmlElement e, Value.IntWithInifinite value, bool isClip)
		{
			var e_value = e["Value"] as XmlElement;
			var e_infinite = e["Infinite"] as XmlElement;

			// Convert int into intWithInfinit
			if(e_value == null && e_infinite == null)
			{
				var i = e.GetTextAsInt();
				value.Value.SetValue(i);
			}
			else
			{
				if (e_value != null) LoadFromElement(e_value, value.Value, isClip);
				if (e_infinite != null) LoadFromElement(e_infinite, value.Infinite, isClip);
			}
		}

		public static void LoadFromElement(XmlElement e, Value.Vector2D value, bool isClip)
		{
			var e_x = e["X"] as XmlElement;
			var e_y = e["Y"] as XmlElement;

			if (e_x != null) LoadFromElement(e_x, value.X, isClip);
			if (e_y != null) LoadFromElement(e_y, value.Y, isClip);
		}

		public static void LoadFromElement(XmlElement e, Value.Vector3D value, bool isClip)
		{
			var e_x = e["X"] as XmlElement;
			var e_y = e["Y"] as XmlElement;
			var e_z = e["Z"] as XmlElement;

			if (e_x != null) LoadFromElement(e_x, value.X, isClip);
			if (e_y != null) LoadFromElement(e_y, value.Y, isClip);
			if (e_z != null) LoadFromElement(e_z, value.Z, isClip);
		}

		public static void LoadFromElement(XmlElement e, Value.Color value, bool isClip)
		{
			var e_r = e["R"] as XmlElement;
			var e_g = e["G"] as XmlElement;
			var e_b = e["B"] as XmlElement;
			var e_a = e["A"] as XmlElement;

			if (e_r != null) LoadFromElement(e_r, value.R, isClip);
			if (e_g != null) LoadFromElement(e_g, value.G, isClip);
			if (e_b != null) LoadFromElement(e_b, value.B, isClip);
			if (e_a != null) LoadFromElement(e_a, value.A, isClip);
		}

		public static void LoadFromElement(XmlElement e, Value.IntWithRandom value, bool isClip)
		{
			var e_c = e["Center"];
			var e_max = e["Max"];
			var e_min = e["Min"];
			var e_da = e["DrawnAs"];

			if (e_c != null)
			{
				var center = e_c.GetTextAsInt();
				value.SetCenter(center);
			}

			if (e_max != null)
			{
				var max = e_max.GetTextAsInt();
				value.SetMax(max);
			}
			else
			{
				value.SetMax(value.DefaultValueMax);
			}

			if (e_min != null)
			{
				var min = e_min.GetTextAsInt();
				value.SetMin(min);
			}
			else
			{
				value.SetMin(value.DefaultValueMin);
			}

			if (e_da != null)
			{
				value.DrawnAs = (DrawnAs)e_da.GetTextAsInt();
			}
		}

		public static void LoadFromElement(XmlElement e, Value.FloatWithRandom value, bool isClip)
		{
			var e_c = e["Center"];
			var e_max = e["Max"];
			var e_min = e["Min"];
			var e_da = e["DrawnAs"];

			if (e_c != null)
			{
				var center = e_c.GetTextAsFloat();
				value.SetCenter(center);
			}

			if (e_max != null)
			{
				var max = e_max.GetTextAsFloat();
				value.SetMax(max);
			}
			else
			{
				value.SetMax(value.DefaultValueMax);
			}

			if (e_min != null)
			{
				var min = e_min.GetTextAsFloat();
				value.SetMin(min);
			}
			else
			{
				value.SetMin(value.DefaultValueMin);
			}

			if (e_da != null)
			{
				value.DrawnAs = (DrawnAs)e_da.GetTextAsInt();
			}
		}

		public static void LoadFromElement(XmlElement e, Value.Vector2DWithRandom value, bool isClip)
		{
			var e_x = e["X"] as XmlElement;
			var e_y = e["Y"] as XmlElement;
			var e_da = e["DrawnAs"];

			// Convert Vector2D into Vector2DWithRandom
			if(e_x != null)
			{
				if(e_da == null && e_x["Max"] == null && e_x["Min"] == null && e_x["Center"] == null)
				{
					var x = e_x.GetTextAsFloat();
					value.X.SetCenter(x);
				}
				else
				{
					LoadFromElement(e_x, value.X, isClip);
				}
			}

			if (e_y != null)
			{
				if (e_da == null && e_y["Max"] == null && e_y["Min"] == null && e_y["Center"] == null)
				{
					var y = e_y.GetTextAsFloat();
					value.Y.SetCenter(y);
				}
				else
				{
					LoadFromElement(e_y, value.Y, isClip);
				}
			}

			if (e_da != null)
			{
				value.DrawnAs = (DrawnAs)e_da.GetTextAsInt();
			}
		}

		public static void LoadFromElement(XmlElement e, Value.Vector3DWithRandom value, bool isClip)
		{
			var e_x = e["X"] as XmlElement;
			var e_y = e["Y"] as XmlElement;
			var e_z = e["Z"] as XmlElement;
			var e_da = e["DrawnAs"];

			if (e_x != null) LoadFromElement(e_x, value.X, isClip);
			if (e_y != null) LoadFromElement(e_y, value.Y, isClip);
			if (e_z != null) LoadFromElement(e_z, value.Z, isClip);

			if (e_da != null)
			{
				value.DrawnAs = (DrawnAs)e_da.GetTextAsInt();
			}
		}

		public static void LoadFromElement(XmlElement e, Value.ColorWithRandom value, bool isClip)
		{
			var e_r = e["R"] as XmlElement;
			var e_g = e["G"] as XmlElement;
			var e_b = e["B"] as XmlElement;
			var e_a = e["A"] as XmlElement;
			var e_da = e["DrawnAs"];
			var e_cs = e["ColorSpace"];

			if (e_r != null) LoadFromElement(e_r, value.R, isClip);
			if (e_g != null) LoadFromElement(e_g, value.G, isClip);
			if (e_b != null) LoadFromElement(e_b, value.B, isClip);
			if (e_a != null) LoadFromElement(e_a, value.A, isClip);

			if (e_da != null)
			{
				value.DrawnAs = (DrawnAs)e_da.GetTextAsInt();
			}

			if (e_cs != null)
			{
				value.SetColorSpace((ColorSpace)e_cs.GetTextAsInt(), false, false);
			}
		}

		public static void LoadFromElement(XmlElement e, Value.Path value, bool isClip)
		{
			var text = e.GetText();

			if (!isClip)
				value.SetRelativePath(text);
			else
				value.SetAbsolutePath(text);
		}

		public static void LoadFromElement(XmlElement e, Value.EnumBase value, bool isClip)
		{
			var text = e.GetText();
			var parsed = 0;
			if (int.TryParse(text, out parsed))
			{
				value.SetValue(parsed);
			}
		}

		public static void LoadFromElement(XmlElement e, Value.FCurveVector2D value, bool isClip)
		{
			var e_keys = e["Keys"] as XmlElement;
			if (e_keys == null) return;

			var e_x = e_keys["X"] as XmlElement;
			var e_y = e_keys["Y"] as XmlElement;

			Action<Data.Value.FCurve<float>, XmlElement> import = (v_, e_) =>
			{
				foreach (XmlElement r in e_.ChildNodes)
				{
					if (r.Name.StartsWith("Key"))
					{
						var f = r.GetTextAsInt("Frame");
						var v = r.GetTextAsFloat("Value");
						var lx = r.GetTextAsFloat("LeftX");
						var ly = r.GetTextAsFloat("LeftY");
						var rx = r.GetTextAsFloat("RightX");
						var ry = r.GetTextAsFloat("RightY");
						var i = r.GetTextAsInt("InterpolationType");
						var s = r.GetTextAsInt("Sampling");

						var t = new Value.FCurveKey<float>(f, v);
						t.SetLeftDirectly(lx, ly);
						t.SetRightDirectly(rx, ry);
						t.InterpolationType.SetValue(i);

						v_.AddKeyDirectly(t);
					}
					else if (r.Name.StartsWith("StartType"))
					{
						var v = r.GetTextAsInt();
						v_.StartType.SetValue(v);
					}
					else if (r.Name.StartsWith("EndType"))
					{
						var v = r.GetTextAsInt();
						v_.EndType.SetValue(v);
					}
					else if (r.Name.StartsWith("OffsetMax"))
					{
						var v = r.GetTextAsFloat();
						v_.OffsetMax.SetValueDirectly(v);
					}
					else if (r.Name.StartsWith("OffsetMin"))
					{
						var v = r.GetTextAsFloat();
						v_.OffsetMin.SetValueDirectly(v);
					}
					else if (r.Name.StartsWith("Sampling"))
					{
						var v = r.GetTextAsInt();
						v_.Sampling.SetValueDirectly(v);
					}
				}
			};

			if (e_x != null) import(value.X, e_x);
			if (e_y != null) import(value.Y, e_y);
		}

		public static void LoadFromElement(XmlElement e, Value.FCurveVector3D value, bool isClip)
		{
			var e_keys = e["Keys"] as XmlElement;
			if (e_keys == null) return;

			var e_x = e_keys["X"] as XmlElement;
			var e_y = e_keys["Y"] as XmlElement;
			var e_z = e_keys["Z"] as XmlElement;

			Action<Data.Value.FCurve<float>, XmlElement> import = (v_, e_) =>
			{
				foreach (XmlElement r in e_.ChildNodes)
				{
					if (r.Name.StartsWith("Key"))
					{
						var f = r.GetTextAsInt("Frame");
						var v = r.GetTextAsFloat("Value");
						var lx = r.GetTextAsFloat("LeftX");
						var ly = r.GetTextAsFloat("LeftY");
						var rx = r.GetTextAsFloat("RightX");
						var ry = r.GetTextAsFloat("RightY");
						var i = r.GetTextAsInt("InterpolationType");

						var t = new Value.FCurveKey<float>(f, v);
						t.SetLeftDirectly(lx, ly);
						t.SetRightDirectly(rx, ry);
						t.InterpolationType.SetValue(i);

						v_.AddKeyDirectly(t);
					}
					else if (r.Name.StartsWith("StartType"))
					{
						var v = r.GetTextAsInt();
						v_.StartType.SetValue(v);
					}
					else if (r.Name.StartsWith("EndType"))
					{
						var v = r.GetTextAsInt();
						v_.EndType.SetValue(v);
					}
					else if (r.Name.StartsWith("OffsetMax"))
					{
						var v = r.GetTextAsFloat();
						v_.OffsetMax.SetValueDirectly(v);
					}
					else if (r.Name.StartsWith("OffsetMin"))
					{
						var v = r.GetTextAsFloat();
						v_.OffsetMin.SetValueDirectly(v);
					}
					else if (r.Name.StartsWith("Sampling"))
					{
						var v = r.GetTextAsInt();
						v_.Sampling.SetValueDirectly(v);
					}
				}
			};

			if (e_x != null) import(value.X, e_x);
			if (e_y != null) import(value.Y, e_y);
			if (e_z != null) import(value.Z, e_z);
		}

		public static void LoadFromElement(XmlElement e, Value.FCurveColorRGBA value, bool isClip)
		{
			Action<Data.Value.FCurve<byte>, XmlElement> import = (v_, e_) =>
				{
					foreach (XmlElement r in e_.ChildNodes)
					{
						if (r.Name.StartsWith("Key"))
						{
							var f = r.GetTextAsInt("Frame");
							var v = r.GetTextAsFloat("Value");
							var lx = r.GetTextAsFloat("LeftX");
							var ly = r.GetTextAsFloat("LeftY");
							var rx = r.GetTextAsFloat("RightX");
							var ry = r.GetTextAsFloat("RightY");
							var i = r.GetTextAsInt("InterpolationType");

							var t = new Value.FCurveKey<byte>(f, (byte)v);
							t.SetLeftDirectly(lx, ly);
							t.SetRightDirectly(rx, ry);
							t.InterpolationType.SetValue(i);

							v_.AddKeyDirectly(t);
						}
						else if (r.Name.StartsWith("StartType"))
						{
							var v = r.GetTextAsInt();
							v_.StartType.SetValue(v);
						}
						else if (r.Name.StartsWith("EndType"))
						{
							var v = r.GetTextAsInt();
							v_.EndType.SetValue(v);
						}
						else if (r.Name.StartsWith("OffsetMax"))
						{
							var v = r.GetTextAsFloat();
							v_.OffsetMax.SetValueDirectly(v);
						}
						else if (r.Name.StartsWith("OffsetMin"))
						{
							var v = r.GetTextAsFloat();
							v_.OffsetMin.SetValueDirectly(v);
						}
						else if (r.Name.StartsWith("Sampling"))
						{
							var v = r.GetTextAsInt();
							v_.Sampling.SetValueDirectly(v);
						}
					}
				};


			var e_keys = e["Keys"] as XmlElement;
			if (e_keys == null) return;

			var e_r = e_keys["R"] as XmlElement;
			var e_g = e_keys["G"] as XmlElement;
			var e_b = e_keys["B"] as XmlElement;
			var e_a = e_keys["A"] as XmlElement;

			if (e_r != null) import(value.R, e_r);
			if (e_g != null) import(value.G, e_g);
			if (e_b != null) import(value.B, e_b);
			if (e_a != null) import(value.A, e_a);
		}
	}
}

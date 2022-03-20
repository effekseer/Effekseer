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
		static Dictionary<Type, Func<XmlDocument, string, object, bool, XmlElement>> saveEvents = new Dictionary<Type, Func<XmlDocument, string, object, bool, XmlElement>>();

		static Dictionary<Type, Action<XmlElement, object, bool>> loadEvents = new Dictionary<Type, Action<XmlElement, object, bool>>();

		static List<MethodInfo> loadingMethodInfos = new List<MethodInfo>();

		static List<MethodInfo> savingMethodInfos = new List<MethodInfo>();

		static IO()
		{
			var methods = typeof(IO).GetMethods();
			loadingMethodInfos = methods.Where(_ => _.IsStatic && _.Name == "LoadFromElement" && _.GetParameters().Length == 3).ToList();
			savingMethodInfos = methods.Where(_ => _.IsStatic && _.Name == "SaveToElement" && _.GetParameters().Length == 4).ToList();
		}

		static bool CreateLoadingMethod(Type target, Type t = null)
		{
			if (t == null)
				t = target;

			if (loadEvents.ContainsKey(target))
				return true;

			if (t.IsGenericType)
			{
				var def = t.GetGenericTypeDefinition();

				var found = loadingMethodInfos.FirstOrDefault(_ =>
				{
					var param = _.GetParameters();
					return param.Length == 3 && param[1].ParameterType.IsGenericType && param[1].ParameterType.GetGenericTypeDefinition() == def;
				});

				if (found != null)
				{
					var m = found.MakeGenericMethod(new[] { t.GenericTypeArguments[0] });
					loadEvents.Add(target, (e, o, b) =>
					{
						m.Invoke(null, new[] { e, o, b });
					});
					return true;
				}
			}

			if (t.BaseType != typeof(Object))
			{
				return CreateLoadingMethod(target, t.BaseType);
			}

			return false;
		}

		static bool CreateSavingMethod(Type target, Type t = null)
		{
			if (t == null)
				t = target;

			if (saveEvents.ContainsKey(target))
				return true;

			if (t.IsGenericType)
			{
				var def = t.GetGenericTypeDefinition();

				var found = savingMethodInfos.FirstOrDefault(_ =>
				{
					var param = _.GetParameters();
					return param[2].ParameterType.IsGenericType && param[2].ParameterType.GetGenericTypeDefinition() == def;
				});

				if (found != null)
				{
					var m = found.MakeGenericMethod(new[] { t.GenericTypeArguments[0] });
					saveEvents.Add(target, (e, s, o, b) =>
					{
						return (XmlElement)m.Invoke(null, new[] { e, s, o, b });
					});
					return true;
				}
			}

			if (t.BaseType != typeof(Object))
			{
				return CreateSavingMethod(target, t.BaseType);
			}

			return false;
		}

		public static void ExtendSupportedType(Type type, Func<XmlDocument, string, object, bool, XmlElement> save, Action<XmlElement, object, bool> load)
		{
			saveEvents.Add(type, save);
			loadEvents.Add(type, load);
		}

		public static XmlElement SaveObjectToElement(XmlDocument doc, string element_name, object o, bool isClip)
		{
			XmlElement e_o = doc.CreateElement(element_name);

			var properties = o.GetType().GetProperties(BindingFlags.Public | BindingFlags.Instance);

			foreach (var property in properties)
			{
				var io_attribute = property.GetCustomAttributes(typeof(IOAttribute), false).FirstOrDefault() as IOAttribute;
				if (io_attribute != null && !io_attribute.Export) continue;

				var propertyName = property.Name;
				var propertyType = property.PropertyType;
				var property_value = property.GetValue(o, null);
				var requiredToExport = io_attribute != null && io_attribute.Export;

				SaveObjectToElement(doc, isClip, e_o, requiredToExport, propertyName, propertyType, property_value);
			}

			if (e_o.ChildNodes.Count > 0) return e_o;
			return null;
		}

		private static void SaveObjectToElement(XmlDocument doc, bool isClip, XmlElement e_o, bool requiredToExport, string propertyName, Type propertyType, object property_value)
		{
			var method = typeof(IO).GetMethod("SaveToElement", new Type[] { typeof(XmlDocument), typeof(string), propertyType, typeof(bool) });
			if (method != null)
			{
				var element = method.Invoke(null, new object[] { doc, propertyName, property_value, isClip });

				if (element != null)
				{
					e_o.AppendChild(element as XmlNode);
				}
			}
			else if (CreateSavingMethod(propertyType) && saveEvents.ContainsKey(propertyType))
			{
				var element = saveEvents[propertyType](doc, propertyName, property_value, isClip);

				if (element != null)
				{
					e_o.AppendChild(element as XmlNode);
				}
			}
			else
			{
				if (requiredToExport)
				{
					var element = SaveObjectToElement(doc, propertyName, property_value, isClip);

					if (element != null && element.ChildNodes.Count > 0)
					{
						e_o.AppendChild(element as XmlNode);
					}
				}
			}
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

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, MaterialFileParameter mfp, bool isClip)
		{
			var e = doc.CreateElement(element_name);

			var e_path = SaveToElement(doc, "Path", mfp.Path, isClip);
			if (e_path != null)
			{
				e.AppendChild(e_path);
			}

			var e_float1 = doc.CreateElement("Float1");
			var e_float2 = doc.CreateElement("Float2");
			var e_float3 = doc.CreateElement("Float3");
			var e_float4 = doc.CreateElement("Float4");
			var e_texture = doc.CreateElement("Texture");
			var e_gradient = doc.CreateElement("Gradient");


			// check file info
			var info = new Utl.MaterialInformation();
			if (info.Load(mfp.Path.AbsolutePath))
			{
				var uniforms = mfp.GetUniforms(info);

				foreach (var uniform in uniforms.Where(_ => _.Item1 != null))
				{
					var status = uniform.Item1;

					if (status.Value is Data.Value.Vector4D)
					{
						var v = status.Value as Data.Value.Vector4D;
						var v_k = doc.CreateTextElement("Key", status.Key.ToString());
						var v_e = SaveToElement(doc, "Value", v, isClip);
						if (v_e != null)
						{
							var e_root = doc.CreateElement("KeyValue");
							e_root.AppendChild(v_k);
							e_root.AppendChild(v_e);
							e_float4.AppendChild(e_root);
						}
					}
					else if (status.Value is Data.Value.Vector3D)
					{
						var v = status.Value as Data.Value.Vector3D;
						var v_k = doc.CreateTextElement("Key", status.Key.ToString());
						var v_e = SaveToElement(doc, "Value", v, isClip);
						if (v_e != null)
						{
							var e_root = doc.CreateElement("KeyValue");
							e_root.AppendChild(v_k);
							e_root.AppendChild(v_e);
							e_float3.AppendChild(e_root);
						}
					}
					if (status.Value is Data.Value.Vector2D)
					{
						var v = status.Value as Data.Value.Vector2D;
						var v_k = doc.CreateTextElement("Key", status.Key.ToString());
						var v_e = SaveToElement(doc, "Value", v, isClip);
						if (v_e != null)
						{
							var e_root = doc.CreateElement("KeyValue");
							e_root.AppendChild(v_k);
							e_root.AppendChild(v_e);
							e_float2.AppendChild(e_root);
						}
					}
					else if (status.Value is Data.Value.Float)
					{
						var v = status.Value as Data.Value.Float;
						var v_k = doc.CreateTextElement("Key", status.Key.ToString());
						var v_e = SaveToElement(doc, "Value", v, isClip);
						if (v_e != null)
						{
							var e_root = doc.CreateElement("KeyValue");
							e_root.AppendChild(v_k);
							e_root.AppendChild(v_e);
							e_float1.AppendChild(e_root);
						}
					}
				}

				var textures = mfp.GetTextures(info);

				foreach (var kv in textures)
				{
					var status = kv.Item1;

					if (status == null)
						continue;

					// regard as defalt texture
					if (!status.IsShown)
						continue;

					if (status.Value is Data.Value.PathForImage)
					{
						var v = status.Value as Data.Value.PathForImage;
						var v_k = doc.CreateTextElement("Key", status.Key.ToString());
						var v_e = SaveToElement(doc, "Value", v, isClip);
						if (v_e != null)
						{
							var e_root = doc.CreateElement("KeyValue");
							e_root.AppendChild(v_k);
							e_root.AppendChild(v_e);
							e_texture.AppendChild(e_root);
						}
					}
				}

				var gradients = mfp.GetGradients(info);
				foreach (var kv in gradients)
				{
					var status = kv.Item1;

					if (status == null)
						continue;

					if (!status.IsShown)
						continue;

					if (status.Value is Data.Value.Gradient)
					{
						var v = status.Value as Data.Value.Gradient;
						var e_root = doc.CreateElement("KeyValue");
						var v_k = doc.CreateTextElement("Key", status.Key.ToString());
						SaveObjectToElement(doc, true, e_root, true, "Value", typeof(Data.Value.Gradient), v);

						if (e_root.ChildNodes.Count > 0)
						{
							e_root.AppendChild(v_k);
							e_gradient.AppendChild(e_root);
						}
					}
				}
			}
			else
			{
				foreach (var status in mfp.GetValueStatus())
				{
					if (status.Value is Data.Value.Vector4D)
					{
						var v = status.Value as Data.Value.Vector4D;
						var v_k = doc.CreateTextElement("Key", status.Key.ToString());
						var v_e = SaveToElement(doc, "Value", v, isClip);
						if (v_e != null)
						{
							var e_root = doc.CreateElement("KeyValue");
							e_root.AppendChild(v_k);
							e_root.AppendChild(v_e);
							e_float4.AppendChild(e_root);
						}
					}
					if (status.Value is Data.Value.Vector3D)
					{
						var v = status.Value as Data.Value.Vector3D;
						var v_k = doc.CreateTextElement("Key", status.Key.ToString());
						var v_e = SaveToElement(doc, "Value", v, isClip);
						if (v_e != null)
						{
							var e_root = doc.CreateElement("KeyValue");
							e_root.AppendChild(v_k);
							e_root.AppendChild(v_e);
							e_float3.AppendChild(e_root);
						}
					}
					if (status.Value is Data.Value.Vector2D)
					{
						var v = status.Value as Data.Value.Vector2D;
						var v_k = doc.CreateTextElement("Key", status.Key.ToString());
						var v_e = SaveToElement(doc, "Value", v, isClip);
						if (v_e != null)
						{
							var e_root = doc.CreateElement("KeyValue");
							e_root.AppendChild(v_k);
							e_root.AppendChild(v_e);
							e_float2.AppendChild(e_root);
						}
					}
					else if (status.Value is Data.Value.Float)
					{
						var v = status.Value as Data.Value.Float;
						var v_k = doc.CreateTextElement("Key", status.Key.ToString());
						var v_e = SaveToElement(doc, "Value", v, isClip);
						if (v_e != null)
						{
							var e_root = doc.CreateElement("KeyValue");
							e_root.AppendChild(v_k);
							e_root.AppendChild(v_e);
							e_float1.AppendChild(e_root);
						}
					}
					else if (status.Value is Data.Value.PathForImage)
					{
						// regard as defalt texture
						if (!status.IsShown)
							continue;

						var v = status.Value as Data.Value.PathForImage;
						var v_k = doc.CreateTextElement("Key", status.Key.ToString());
						var v_e = SaveToElement(doc, "Value", v, isClip);

						if (v_e != null)
						{
							var e_root = doc.CreateElement("KeyValue");
							e_root.AppendChild(v_k);
							e_root.AppendChild(v_e);
							e_texture.AppendChild(e_root);
						}
					}
					else if(status.Value is Data.Value.Gradient)
					{
						var v = status.Value as Data.Value.Gradient;
						var e_root = doc.CreateElement("KeyValue");
						var v_k = doc.CreateTextElement("Key", status.Key.ToString());
						SaveObjectToElement(doc, true, e_root, true, "Value", typeof(Data.Value.Gradient), v);

						if (e_root.ChildNodes.Count > 0)
						{
							e_root.AppendChild(v_k);
							e_gradient.AppendChild(e_root);
						}
					}
				}
			}

			if (e_float1.ChildNodes.Count > 0)
			{
				e.AppendChild(e_float1);
			}

			if (e_float2.ChildNodes.Count > 0)
			{
				e.AppendChild(e_float2);
			}

			if (e_float3.ChildNodes.Count > 0)
			{
				e.AppendChild(e_float3);
			}

			if (e_float4.ChildNodes.Count > 0)
			{
				e.AppendChild(e_float4);
			}

			if (e_texture.ChildNodes.Count > 0)
			{
				e.AppendChild(e_texture);
			}

			if (e_gradient.ChildNodes.Count > 0)
			{
				e.AppendChild(e_gradient);
			}


			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Data.DynamicInputCollection collection, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			for (int i = 0; i < collection.Values.Count; i++)
			{
				var e_node = SaveToElement(doc, collection.Values[i].GetType().Name, collection.Values[i], true);
				if (e_node != null)
				{
					e.AppendChild(e_node);
				}
			}

			return e;
		}

		public static XmlElement SaveToElement<T>(XmlDocument doc, string element_name, Data.Value.ObjectCollection<T> collection, bool isClip) where T : class, new()
		{
			var e = doc.CreateElement(element_name);
			for (int i = 0; i < collection.Values.Count; i++)
			{
				var name = collection.Values[i].GetType().Name;
				// a node must be generated

				var e_node = (XmlNode)SaveObjectToElement(doc, name, collection.Values[i], true);
				if (e_node != null)
				{
					e.AppendChild(e_node);
				}
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

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.Int2 value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var x = SaveToElement(doc, "X", value.X, isClip);
			var y = SaveToElement(doc, "Y", value.Y, isClip);

			if (x != null) e.AppendChild(x);
			if (y != null) e.AppendChild(y);

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.Float value, bool isClip)
		{
			if (value.DynamicEquation.Index >= 0)
			{
				var e = doc.CreateElement(element_name);
				var text = value.GetValue().ToString();
				var value_ = doc.CreateTextElement("Value", text);
				e.AppendChild(value_);

				var d = SaveToElement(doc, "DynamicEquation", value.DynamicEquation, isClip);
				if (d != null)
				{
					e.AppendChild(d);
				}

				return e.ChildNodes.Count > 0 ? e : null;
			}
			else
			{
				if (value.Value == value.DefaultValue && !isClip) return null;
				var text = value.GetValue().ToString();
				return doc.CreateTextElement(element_name, text);
			}
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.IntWithInifinite value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var v = SaveToElement(doc, "Value", value.Value, isClip);
			var i = SaveToElement(doc, "Infinite", value.Infinite, isClip);

			if (v != null) e.AppendChild(v);
			if (i != null) e.AppendChild(i);

			var d = SaveToElement(doc, "DynamicEquation", value.DynamicEquation, isClip);
			if (d != null)
			{
				e.AppendChild(d);
			}

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

			var d = SaveToElement(doc, "DynamicEquation", value.DynamicEquation, isClip);
			if (d != null)
			{
				e.AppendChild(d);
			}

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.Vector4D value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var x = SaveToElement(doc, "X", value.X, isClip);
			var y = SaveToElement(doc, "Y", value.Y, isClip);
			var z = SaveToElement(doc, "Z", value.Z, isClip);
			var w = SaveToElement(doc, "W", value.W, isClip);

			if (x != null) e.AppendChild(x);
			if (y != null) e.AppendChild(y);
			if (z != null) e.AppendChild(z);
			if (w != null) e.AppendChild(w);

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

			var d_min = SaveToElement(doc, "DynamicEquationMin", value.DynamicEquationMin, isClip);

			if (d_min != null)
			{
				e.AppendChild(d_min);
			}

			var d_max = SaveToElement(doc, "DynamicEquationMax", value.DynamicEquationMax, isClip);

			if (d_max != null)
			{
				e.AppendChild(d_max);
			}

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.FloatWithRandom value, bool isClip)
		{
			var e = doc.CreateElement(element_name);

			if (value.DefaultValueCenter != value.Center || isClip) e.AppendChild(doc.CreateTextElement("Center", value.Center.ToString()));
			if (value.DefaultValueMax != value.Max || isClip) e.AppendChild(doc.CreateTextElement("Max", value.Max.ToString()));
			if (value.DefaultValueMin != value.Min || isClip) e.AppendChild(doc.CreateTextElement("Min", value.Min.ToString()));
			if (value.DefaultDrawnAs != value.DrawnAs || isClip) e.AppendChild(doc.CreateTextElement("DrawnAs", (int)value.DrawnAs));

			var d_min = SaveToElement(doc, "DynamicEquationMin", value.DynamicEquationMin, isClip);

			if (d_min != null)
			{
				e.AppendChild(d_min);
			}

			var d_max = SaveToElement(doc, "DynamicEquationMax", value.DynamicEquationMax, isClip);

			if (d_max != null)
			{
				e.AppendChild(d_max);
			}

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

			var d_min = SaveToElement(doc, "DynamicEquationMin", value.DynamicEquationMin, isClip);

			if (d_min != null)
			{
				e.AppendChild(d_min);
			}

			var d_max = SaveToElement(doc, "DynamicEquationMax", value.DynamicEquationMax, isClip);

			if (d_max != null)
			{
				e.AppendChild(d_max);
			}

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
			if (!isClip && value.IsRelativeSaved)
				text = value.GetRelativePath();
			else
				text = value.GetAbsolutePath();

			return doc.CreateTextElement(element_name, text);
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.FCurveVector2D value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var timeline = SaveToElement(doc, "Timeline", value.Timeline, isClip);
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
						k.AppendChild(doc.CreateTextElement("InterpolationType", ((int)k_.InterpolationType).ToString()));

						xml.AppendChild(k);
						index++;
					}
				};

			setValues(value.X, x);
			setValues(value.Y, y);

			if (timeline != null) keys.AppendChild(timeline);
			if (x.ChildNodes.Count > 0) keys.AppendChild(x);
			if (y.ChildNodes.Count > 0) keys.AppendChild(y);
			if (keys.ChildNodes.Count > 0) e.AppendChild(keys);

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.FCurveVector3D value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var timeline = SaveToElement(doc, "Timeline", value.Timeline, isClip);
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
					k.AppendChild(doc.CreateTextElement("InterpolationType", ((int)k_.InterpolationType).ToString()));

					xml.AppendChild(k);
					index++;
				}
			};

			setValues(value.X, x);
			setValues(value.Y, y);
			setValues(value.Z, z);

			if (timeline != null) keys.AppendChild(timeline);
			if (x.ChildNodes.Count > 0) keys.AppendChild(x);
			if (y.ChildNodes.Count > 0) keys.AppendChild(y);
			if (z.ChildNodes.Count > 0) keys.AppendChild(z);
			if (keys.ChildNodes.Count > 0) e.AppendChild(keys);

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.FCurveColorRGBA value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var timeline = SaveToElement(doc, "Timeline", value.Timeline, isClip);
			var keys = doc.CreateElement("Keys");
			var r = doc.CreateElement("R");
			var g = doc.CreateElement("G");
			var b = doc.CreateElement("B");
			var a = doc.CreateElement("A");

			int index = 0;

			Action<Value.FCurve<int>, XmlElement> setValues = (v, xml) =>
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
					k.AppendChild(doc.CreateTextElement("InterpolationType", ((int)k_.InterpolationType).ToString()));

					xml.AppendChild(k);
					index++;
				}
			};

			setValues(value.R, r);
			setValues(value.G, g);
			setValues(value.B, b);
			setValues(value.A, a);

			if (timeline != null) keys.AppendChild(timeline);
			if (r.ChildNodes.Count > 0) keys.AppendChild(r);
			if (g.ChildNodes.Count > 0) keys.AppendChild(g);
			if (b.ChildNodes.Count > 0) keys.AppendChild(b);
			if (a.ChildNodes.Count > 0) keys.AppendChild(a);

			if (keys.ChildNodes.Count > 0) e.AppendChild(keys);

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Value.FCurveScalar value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var timeline = SaveToElement(doc, "Timeline", value.Timeline, isClip);
			var keys = doc.CreateElement("Keys");
			var s_value = doc.CreateElement("S");

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
					k.AppendChild(doc.CreateTextElement("InterpolationType", ((int)k_.InterpolationType).ToString()));

					xml.AppendChild(k);
					index++;
				}
			};

			setValues(value.S, s_value);

			if (timeline != null) keys.AppendChild(timeline);
			if (s_value.ChildNodes.Count > 0) keys.AppendChild(s_value);
			if (keys.ChildNodes.Count > 0) e.AppendChild(keys);

			return e.ChildNodes.Count > 0 ? e : null;
		}


		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Data.DynamicInput value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var input = SaveToElement(doc, "Input", value.Input, isClip);
			if (input != null)
			{
				e.AppendChild(input);
			}

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, Data.DynamicEquation value, bool isClip)
		{
			var e = doc.CreateElement(element_name);
			var name = SaveToElement(doc, "Name", value.Name, isClip);
			if (name != null)
			{
				e.AppendChild(name);
			}

			var code = SaveToElement(doc, "Code", value.Code, isClip);
			if (code != null)
			{
				e.AppendChild(code);
			}

			return e.ChildNodes.Count > 0 ? e : null;
		}

		public static XmlElement SaveToElement<T>(XmlDocument doc, string element_name, Data.Value.ObjectReference<T> de, bool isClip) where T : class
		{
			var d_ind = de.Index;
			if (d_ind >= 0)
			{
				var d = doc.CreateTextElement(element_name, d_ind.ToString());
				return d;
			}

			return null;
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

				bool isImportRequired = io_attribute != null && io_attribute.Import;
				var property_value = property.GetValue(o, null);
				var propertyType = property.PropertyType;

				LoadObjectFromElement(isClip, ch_node, isImportRequired, ref property_value, propertyType);
			}
		}

		private static void LoadObjectFromElement(bool isClip, XmlElement ch_node, bool isImportRequired, ref object property_value, Type propertyType)
		{
			var method = typeof(IO).GetMethod("LoadFromElement", new Type[] { typeof(XmlElement), propertyType, typeof(bool) });
			if (method != null)
			{
				method.Invoke(null, new object[] { ch_node, property_value, isClip });
			}
			else if (CreateLoadingMethod(propertyType) && loadEvents.ContainsKey(propertyType))
			{
				loadEvents[propertyType](ch_node, property_value, isClip);
			}
			else
			{
				if (isImportRequired)
				{
					LoadObjectFromElement(ch_node, ref property_value, isClip);
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

		public static void LoadFromElement(XmlElement e, MaterialFileParameter mfp, bool isClip)
		{
			var e_path = e["Path"] as XmlElement;
			if (e_path != null)
			{
				LoadFromElement(e_path, mfp.Path, isClip);
			}

			var e_float1 = e["Float1"] as XmlElement;
			var e_float2 = e["Float2"] as XmlElement;
			var e_float3 = e["Float3"] as XmlElement;
			var e_float4 = e["Float4"] as XmlElement;
			var e_texture = e["Texture"] as XmlElement;
			var e_gradient = e["Gradient"] as XmlElement;

			if (e_float1 != null)
			{
				for (var i = 0; i < e_float1.ChildNodes.Count; i++)
				{
					var e_child = e_float1.ChildNodes[i] as XmlElement;

					// compatibility
					string key = string.Empty;
					XmlElement valueElement = null;
					if (MaterialFileParameter.GetVersionOfKey(e_child.Name) == 0)
					{
						key = e_child.Name;
						valueElement = e_child;
					}
					else
					{
						var e_k = e_child["Key"] as XmlElement;
						valueElement = e_child["Value"] as XmlElement;

						if (e_k != null)
						{
							key = e_k.GetText();
						}
					}

					var vs = mfp.FindValue(key, null, false);
					if (vs != null)
					{
						var v = vs.Value as Value.Float;
						LoadFromElement(valueElement, v, isClip);
					}
				}
			}

			if (e_float2 != null)
			{
				for (var i = 0; i < e_float2.ChildNodes.Count; i++)
				{
					var e_child = e_float2.ChildNodes[i] as XmlElement;

					// compatibility
					string key = string.Empty;
					XmlElement valueElement = null;
					if (MaterialFileParameter.GetVersionOfKey(e_child.Name) == 0)
					{
						key = e_child.Name;
						valueElement = e_child;
					}
					else
					{
						var e_k = e_child["Key"] as XmlElement;
						valueElement = e_child["Value"] as XmlElement;

						if (e_k != null)
						{
							key = e_k.GetText();
						}
					}

					var vs = mfp.FindValue(key, null, false);
					if (vs != null)
					{
						var v = vs.Value as Value.Vector2D;
						LoadFromElement(valueElement, v, isClip);
					}
				}
			}

			if (e_float3 != null)
			{
				for (var i = 0; i < e_float3.ChildNodes.Count; i++)
				{
					var e_child = e_float3.ChildNodes[i] as XmlElement;

					// compatibility
					string key = string.Empty;
					XmlElement valueElement = null;
					if (MaterialFileParameter.GetVersionOfKey(e_child.Name) == 0)
					{
						key = e_child.Name;
						valueElement = e_child;
					}
					else
					{
						var e_k = e_child["Key"] as XmlElement;
						valueElement = e_child["Value"] as XmlElement;

						if (e_k != null)
						{
							key = e_k.GetText();
						}
					}

					var vs = mfp.FindValue(key, null, false);
					if (vs != null)
					{
						var v = vs.Value as Value.Vector3D;
						LoadFromElement(valueElement, v, isClip);
					}
				}
			}

			if (e_float4 != null)
			{
				for (var i = 0; i < e_float4.ChildNodes.Count; i++)
				{
					var e_child = e_float4.ChildNodes[i] as XmlElement;

					// compatibility
					string key = string.Empty;
					XmlElement valueElement = null;
					if (MaterialFileParameter.GetVersionOfKey(e_child.Name) == 0)
					{
						key = e_child.Name;
						valueElement = e_child;
					}
					else
					{
						var e_k = e_child["Key"] as XmlElement;
						valueElement = e_child["Value"] as XmlElement;

						if (e_k != null)
						{
							key = e_k.GetText();
						}
					}

					var vs = mfp.FindValue(key, null, false);
					if (vs != null)
					{
						var v = vs.Value as Value.Vector4D;
						LoadFromElement(valueElement, v, isClip);
					}
				}
			}

			if (e_texture != null)
			{
				for (var i = 0; i < e_texture.ChildNodes.Count; i++)
				{
					var e_child = e_texture.ChildNodes[i] as XmlElement;

					// compatibility
					string key = string.Empty;
					XmlElement valueElement = null;
					if (MaterialFileParameter.GetVersionOfKey(e_child.Name) == 0)
					{
						key = e_child.Name;
						valueElement = e_child;
					}
					else
					{
						var e_k = e_child["Key"] as XmlElement;
						valueElement = e_child["Value"] as XmlElement;

						if (e_k != null)
						{
							key = e_k.GetText();
						}
					}

					var vs = mfp.FindValue(key, null, false);
					if (vs != null)
					{
						var v = vs.Value as Value.PathForImage;
						LoadFromElement(valueElement, v, isClip);
					}
				}
			}

			if (e_gradient != null)
			{
				for (var i = 0; i < e_gradient.ChildNodes.Count; i++)
				{
					var e_child = e_gradient.ChildNodes[i] as XmlElement;

					string key = string.Empty;

					var e_k = e_child["Key"] as XmlElement;
					var valueElement = e_child["Value"] as XmlElement;

					if (e_k != null)
					{
						key = e_k.GetText();
					}


					var vs = mfp.FindValue(key, null, false);
					if (vs != null)
					{
						var v = vs.Value;
						LoadObjectFromElement(isClip, valueElement, true, ref v, typeof(Data.Value.Gradient));
					}
				}
			}
		}

		public static void LoadFromElement(XmlElement e, Data.DynamicInputCollection collection, bool isClip)
		{
			collection.Values.Clear();

			for (var i = 0; i < e.ChildNodes.Count; i++)
			{
				var e_child = e.ChildNodes[i] as XmlElement;
				var element = new DynamicInput();
				LoadFromElement(e_child, element, isClip);
				collection.Values.Add(element);
			}
		}

		public static void LoadFromElement<T>(XmlElement e, Data.Value.ObjectCollection<T> collection, bool isClip) where T : class, new()
		{
			collection.Clear();

			for (var i = 0; i < e.ChildNodes.Count; i++)
			{
				var e_child = e.ChildNodes[i] as XmlElement;
				var element = new T();

				var obj = (Object)element;

				LoadObjectFromElement(e_child, ref obj, isClip);

				collection.Add(element);
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

		public static void LoadFromElement(XmlElement e, Value.Int2 value, bool isClip)
		{
			var e_x = e["X"] as XmlElement;
			var e_y = e["Y"] as XmlElement;

			if (e_x != null) LoadFromElement(e_x, value.X, isClip);
			if (e_y != null) LoadFromElement(e_y, value.Y, isClip);
		}

		public static void LoadFromElement(XmlElement e, Value.Float value, bool isClip)
		{
			if (e.HasChildNodes && e.ChildNodes[0].HasChildNodes)
			{
				// with dynamic equation
				var e_value = e["Value"] as XmlElement;

				if (e_value != null)
				{
					LoadFromElement(e_value, value, isClip);
				}

				var e_d = e["DynamicEquation"] as XmlElement;

				if (e_d != null)
				{
					LoadFromElement(e_d, value.DynamicEquation, isClip);
					value.IsDynamicEquationEnabled.SetValue(true);
				}
			}
			else
			{
				// without dynamic equation
				var text = e.GetText();
				var parsed = 0.0f;
				if (float.TryParse(text, System.Globalization.NumberStyles.Float, Setting.NFI, out parsed))
				{
					value.SetValue(parsed);
				}
			}
		}

		public static void LoadFromElement(XmlElement e, Value.IntWithInifinite value, bool isClip)
		{
			var e_value = e["Value"] as XmlElement;
			var e_infinite = e["Infinite"] as XmlElement;

			// Convert int into intWithInfinit
			if (e_value == null && e_infinite == null)
			{
				var i = e.GetTextAsInt();
				value.Value.SetValue(i);
			}
			else
			{
				if (e_value != null) LoadFromElement(e_value, value.Value, isClip);
				if (e_infinite != null) LoadFromElement(e_infinite, value.Infinite, isClip);
			}

			var e_d = e["DynamicEquation"] as XmlElement;

			if (e_d != null)
			{
				LoadFromElement(e_d, value.DynamicEquation, isClip);
				value.IsDynamicEquationEnabled.SetValue(true);
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
			var e_d = e["DynamicEquation"] as XmlElement;

			if (e_x != null) LoadFromElement(e_x, value.X, isClip);
			if (e_y != null) LoadFromElement(e_y, value.Y, isClip);
			if (e_z != null) LoadFromElement(e_z, value.Z, isClip);

			if (e_d != null)
			{
				LoadFromElement(e_d, value.DynamicEquation, isClip);
				value.IsDynamicEquationEnabled.SetValue(true);
			}
		}

		public static void LoadFromElement(XmlElement e, Value.Vector4D value, bool isClip)
		{
			var e_x = e["X"] as XmlElement;
			var e_y = e["Y"] as XmlElement;
			var e_z = e["Z"] as XmlElement;
			var e_w = e["W"] as XmlElement;

			if (e_x != null) LoadFromElement(e_x, value.X, isClip);
			if (e_y != null) LoadFromElement(e_y, value.Y, isClip);
			if (e_z != null) LoadFromElement(e_z, value.Z, isClip);
			if (e_w != null) LoadFromElement(e_w, value.W, isClip);
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

			int valueMax = value.DefaultValueMax;
			int valueMin = value.DefaultValueMin;

			if (e_max != null)
			{
				valueMax = e_max.GetTextAsInt();
			}

			if (e_min != null)
			{
				valueMin = e_min.GetTextAsInt();
			}

			var correctMin = Math.Min(valueMax, valueMin);
			var correctMax = Math.Max(valueMax, valueMin);

			value.SetMax(correctMax);
			value.SetMin(correctMin);

			if (e_da != null)
			{
				value.DrawnAs = (DrawnAs)e_da.GetTextAsInt();
			}

			var e_d_min = e["DynamicEquationMin"] as XmlElement;
			var e_d_max = e["DynamicEquationMax"] as XmlElement;

			if (e_d_min != null)
			{
				LoadFromElement(e_d_min, value.DynamicEquationMin, isClip);
				value.IsDynamicEquationEnabled.SetValue(true);
			}

			if (e_d_max != null)
			{
				LoadFromElement(e_d_max, value.DynamicEquationMax, isClip);
				value.IsDynamicEquationEnabled.SetValue(true);
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

			var valueMax = value.DefaultValueMax;
			var valueMin = value.DefaultValueMin;

			if (e_max != null)
			{
				valueMax = e_max.GetTextAsFloat();
			}

			if (e_min != null)
			{
				valueMin = e_min.GetTextAsFloat();
			}

			var correctMin = Math.Min(valueMax, valueMin);
			var correctMax = Math.Max(valueMax, valueMin);

			value.SetMax(correctMax);
			value.SetMin(correctMin);

			if (e_da != null)
			{
				value.DrawnAs = (DrawnAs)e_da.GetTextAsInt();
			}

			var e_d_min = e["DynamicEquationMin"] as XmlElement;
			var e_d_max = e["DynamicEquationMax"] as XmlElement;

			if (e_d_min != null)
			{
				LoadFromElement(e_d_min, value.DynamicEquationMin, isClip);
				value.IsDynamicEquationEnabled.SetValue(true);
			}

			if (e_d_max != null)
			{
				LoadFromElement(e_d_max, value.DynamicEquationMax, isClip);
				value.IsDynamicEquationEnabled.SetValue(true);
			}
		}

		public static void LoadFromElement(XmlElement e, Value.Vector2DWithRandom value, bool isClip)
		{
			var e_x = e["X"] as XmlElement;
			var e_y = e["Y"] as XmlElement;
			var e_da = e["DrawnAs"];

			// Convert Vector2D into Vector2DWithRandom
			if (e_x != null)
			{
				if (e_da == null && e_x["Max"] == null && e_x["Min"] == null && e_x["Center"] == null)
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
			var e_d_min = e["DynamicEquationMin"] as XmlElement;
			var e_d_max = e["DynamicEquationMax"] as XmlElement;

			if (e_x != null) LoadFromElement(e_x, value.X, isClip);
			if (e_y != null) LoadFromElement(e_y, value.Y, isClip);
			if (e_z != null) LoadFromElement(e_z, value.Z, isClip);

			if (e_da != null)
			{
				value.DrawnAs = (DrawnAs)e_da.GetTextAsInt();
			}

			if (e_d_min != null)
			{
				LoadFromElement(e_d_min, value.DynamicEquationMin, isClip);
				value.IsDynamicEquationEnabled.SetValue(true);
			}

			if (e_d_max != null)
			{
				LoadFromElement(e_d_max, value.DynamicEquationMax, isClip);
				value.IsDynamicEquationEnabled.SetValue(true);
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

			var e_timeline = e_keys["Timeline"] as XmlElement;
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
						t.SetInterpolationType((Value.FCurveInterpolation)i);

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

			if (e_timeline != null) LoadFromElement(e_timeline, value.Timeline, isClip);
			if (e_x != null) import(value.X, e_x);
			if (e_y != null) import(value.Y, e_y);
		}

		public static void LoadFromElement(XmlElement e, Value.FCurveVector3D value, bool isClip)
		{
			var e_keys = e["Keys"] as XmlElement;
			if (e_keys == null) return;

			var e_timeline = e_keys["Timeline"] as XmlElement;
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
						t.SetInterpolationType((Value.FCurveInterpolation)i);

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

			if (e_timeline != null) LoadFromElement(e_timeline, value.Timeline, isClip);
			if (e_x != null) import(value.X, e_x);
			if (e_y != null) import(value.Y, e_y);
			if (e_z != null) import(value.Z, e_z);
		}

		public static void LoadFromElement(XmlElement e, Value.FCurveColorRGBA value, bool isClip)
		{
			Action<Data.Value.FCurve<int>, XmlElement> import = (v_, e_) =>
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

							var t = new Value.FCurveKey<int>(f, (int)v);
							t.SetLeftDirectly(lx, ly);
							t.SetRightDirectly(rx, ry);
							t.SetInterpolationType((Value.FCurveInterpolation)i);

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

			var e_timeline = e_keys["Timeline"] as XmlElement;
			var e_r = e_keys["R"] as XmlElement;
			var e_g = e_keys["G"] as XmlElement;
			var e_b = e_keys["B"] as XmlElement;
			var e_a = e_keys["A"] as XmlElement;

			if (e_timeline != null) LoadFromElement(e_timeline, value.Timeline, isClip);
			if (e_r != null) import(value.R, e_r);
			if (e_g != null) import(value.G, e_g);
			if (e_b != null) import(value.B, e_b);
			if (e_a != null) import(value.A, e_a);
		}

		public static void LoadFromElement(XmlElement e, Value.FCurveScalar value, bool isClip)
		{
			var e_keys = e["Keys"] as XmlElement;
			if (e_keys == null) return;

			var e_timeline = e_keys["Timeline"] as XmlElement;
			var e_s = e_keys["S"] as XmlElement;

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
						t.SetInterpolationType((Value.FCurveInterpolation)i);

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

			if (e_timeline != null) LoadFromElement(e_timeline, value.Timeline, isClip);
			if (e_s != null) import(value.S, e_s);
		}

		public static void LoadFromElement(XmlElement e, Data.DynamicInput value, bool isClip)
		{
			var e_input = e["Input"] as XmlElement;
			if (e_input != null) LoadFromElement(e_input, value.Input, isClip);
		}

		public static void LoadFromElement(XmlElement e, Data.DynamicEquation value, bool isClip)
		{
			var e_name = e["Name"] as XmlElement;
			var e_x = e["Code"] as XmlElement;

			if (e_name != null) LoadFromElement(e_name, value.Name, isClip);
			if (e_x != null) LoadFromElement(e_x, value.Code, isClip);
		}

		public static void LoadFromElement<T>(XmlElement e, Data.Value.ObjectReference<T> de, bool isClip) where T : class
		{
			var ind = e.GetTextAsInt();
			de.SetValueWithIndex(ind);
		}
	}
}
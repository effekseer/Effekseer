using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum UVTextureReferenceTargetType
	{
		[Key(key= "UVTextureReferenceTargetType_None")]
		None = 0,
		[Key(key = "UVTextureReferenceTargetType_Texture1")]
		Texture1 = 1,
		[Key(key = "UVTextureReferenceTargetType_Texture2")]
		Texture2 = 2,
		[Key(key = "UVTextureReferenceTargetType_Texture3")]
		Texture3 = 3,
		[Key(key = "UVTextureReferenceTargetType_Texture4")]
		Texture4 = 4,
	}

	public enum CustomDataType
	{
		[Key(key = "CustomDataType_None")]
		None = 0,

		[Key(key = "CustomDataType_Fixed2D")]
		Fixed2D = 20,

		[Key(key = "CustomDataType_Random2D")]
		Random2D = 21,

		[Key(key = "CustomDataType_Easing2D")]
		Easing2D = 22,

		[Key(key = "CustomDataType_FCurve2D")]
		FCurve2D = 23,

		[Key(key = "CustomDataType_Fixed4D")]
		Fixed4D = 40,

		[Key(key = "CustomDataType_FCurveColor")]
		FCurveColor = 53,
	}

	public class CustomDataParameter : IEditableValueCollection
	{
		int customDataNum = 0;
		internal string Name = string.Empty;
		internal string Desc = string.Empty;

		[Name(language = Language.Japanese, value = "カスタムデータ")]
		[Name(language = Language.English, value = "Custom Data")]
		[Selector(ID = 10)]
		public Value.Enum<CustomDataType> CustomData
		{
			get;
			private set;
		}

		[Selected(ID = 10, Value = (int)CustomDataType.Fixed2D)]
		public Value.Vector2D Fixed { get; private set; }

		[Selected(ID = 10, Value = (int)CustomDataType.Random2D)]
		public Value.Vector2DWithRandom Random { get; private set; }

		[Selected(ID = 10, Value = (int)CustomDataType.Easing2D)]
		[IO(Export = true)]
		public Vector2DEasingParamater Easing { get; private set; }

		[Selected(ID = 10, Value = (int)CustomDataType.FCurve2D)]
		public Value.FCurveVector2D FCurve { get; private set; }

		[Selected(ID = 10, Value = (int)CustomDataType.Fixed4D)]
		public Value.Vector4D Fixed4 { get; private set; }

		[Selected(ID = 10, Value = (int)CustomDataType.FCurveColor)]
		public Value.FCurveColorRGBA FCurveColor { get; private set; }

		public CustomDataParameter(int customDataNum)
		{
			this.customDataNum = customDataNum;
			CustomData = new Value.Enum<CustomDataType>();
			Fixed = new Value.Vector2D();
			Random = new Value.Vector2DWithRandom();
			Easing = new Vector2DEasingParamater();
			FCurve = new Value.FCurveVector2D();
			Fixed4 = new Value.Vector4D();
			FCurveColor = new Value.FCurveColorRGBA();
		}

		public EditableValue[] GetValues()
		{
			var ret = new List<EditableValue>();

			EditableValue ev = new EditableValue();
			ev.Value = CustomData;

			if(Name != string.Empty)
			{
				ev.Title = Name;
				ev.Description = Desc;
			}
			else
			{
				if (Core.Language == Language.English)
				{
					ev.Title = "CustomData" + customDataNum.ToString();
				}

				if (Core.Language == Language.Japanese)
				{
					ev.Title = "カスタムデータ" + customDataNum.ToString();
				}

				ev.Description = "";
			}

			ev.IsShown = true;
			ev.IsUndoEnabled = true;
			ev.SelfSelectorID = 10;
			ret.Add(ev);

			ret.Add(EditableValue.Create(Fixed, this.GetType().GetProperty("Fixed")));
			ret.Add(EditableValue.Create(Random, this.GetType().GetProperty("Random")));
			ret.Add(EditableValue.Create(Easing, this.GetType().GetProperty("Easing")));
			ret.Add(EditableValue.Create(FCurve, this.GetType().GetProperty("FCurve")));
			ret.Add(EditableValue.Create(Fixed4, this.GetType().GetProperty("Fixed4")));
			ret.Add(EditableValue.Create(FCurveColor, this.GetType().GetProperty("FCurveColor")));

			return ret.ToArray();
		}

		public event ChangedValueEventHandler OnChanged;

		public void Changed()
		{
			if(OnChanged != null)
			{
				OnChanged(this, null);
			}
		}
	}

	public class MaterialFileParameter : IEditableValueCollection
	{
		string selfSummary = string.Empty;
		string selfDetail = string.Empty;

		[Shown(Shown = true)]
		[Key(key = "MaterialFileParameter_Path")]
		public Value.PathForMaterial Path
		{
			get;
			private set;
		}

		List<ValueStatus> valueStatuses = new List<ValueStatus>();
		RendererCommonValues rcValues = null;

		public ValueStatus[] GetValueStatus()
		{
			return valueStatuses.ToArray();
		}

		public ValueStatus FindValue(string key, HashSet<ValueStatus> blacklist = null, bool withName = false)
		{
			var statusKey = StatusKey.From(key);

			foreach(var kv in valueStatuses)
			{
				if (blacklist != null && blacklist.Contains(kv)) continue;
				if (kv.Key.IsSame(statusKey, withName)) return kv;
			}

			return null;
		}

		public MaterialFileParameter(RendererCommonValues rcValues)
		{
			this.rcValues = rcValues;
			Path = new Value.PathForMaterial(Resources.GetString("MaterialFilter"), true);
			Path.OnChanged += Path_OnChanged;
		}

		private void Path_OnChanged(object sender, ChangedValueEventArgs e)
		{
			// Apply values
			Utl.MaterialInformation info = new Utl.MaterialInformation();
			info.Load(Path.GetAbsolutePath());

			ApplyMaterial(info);

			if(info.CustomData.Count() > 0)
			{
				rcValues.CustomData1.Name = info.CustomData[0].Summaries[Core.Language];
				rcValues.CustomData1.Desc = info.CustomData[0].Descriptions[Core.Language];
			}
			else
			{
				rcValues.CustomData1.Name = string.Empty;
				rcValues.CustomData1.Desc = string.Empty;
			}

			if (info.CustomData.Count() > 1)
			{
				rcValues.CustomData2.Name = info.CustomData[1].Summaries[Core.Language];
				rcValues.CustomData2.Desc = info.CustomData[1].Descriptions[Core.Language];
			}
			else
			{
				rcValues.CustomData2.Name = string.Empty;
				rcValues.CustomData2.Desc = string.Empty;
			}

			rcValues.CustomData1.Changed();
			rcValues.CustomData2.Changed();
		}

		public EditableValue[] GetValues()
		{
			var ret = new List<EditableValue>();

			// self
			{
				EditableValue ev = new EditableValue();
				ev.Value = this;
				ev.Title = "";
				ev.Description = "";
				ev.IsShown = true;
				ev.IsUndoEnabled = false;
				//ret.Add(ev);
			}

			var propPath = EditableValue.Create(Path, this.GetType().GetProperty("Path"));

			if(!string.IsNullOrEmpty(selfSummary))
			{
				propPath.Title = selfSummary;
			}

			if (!string.IsNullOrEmpty(selfDetail))
			{
				propPath.Description = selfDetail;
			}

			ret.Add(propPath);

			foreach (var v in valueStatuses.OrderBy(_ => (_ as ValueStatus).Priority))
			{
				EditableValue ev = new EditableValue();
				var status = v as ValueStatus;
				ev.Value = status.Value;
				ev.Title = status.Name;
				ev.Description = status.Description;
				ev.IsShown = status.IsShown;
				ev.IsUndoEnabled = true;
				ret.Add(ev);
			}

			return ret.ToArray();
		}

		public void ApplyMaterial(Utl.MaterialInformation info)
		{
			bool isChanged = false;

			var previous_selfSummary = selfSummary;
			var previous_selfDetail = selfDetail;

			if (info.Names.ContainsKey(Core.Language))
			{
				selfSummary = info.Names[Core.Language];
			}
			else
			{
				selfSummary = "";
			}


			if (info.Descriptions.ContainsKey(Core.Language))
			{
				selfDetail = info.Descriptions[Core.Language];
			}
			else
			{
				selfDetail = "";
			}

			if (previous_selfSummary != selfSummary) isChanged = true;
			if (previous_selfDetail != selfDetail) isChanged = true;

			HashSet<ValueStatus> usedValueStatuses = new HashSet<ValueStatus>();
			HashSet<object> finished = new HashSet<object>();
			
			foreach (var withNameFlag in new[] { false, true} )
			{
				foreach (var texture in info.Textures)
				{
					if (finished.Contains(texture)) continue;

					var key = StatusKey.From(texture);

					Func<string> getName = () =>
					{
						var ret = "";
						if (texture.Summaries.ContainsKey(Core.Language))
						{
							ret = texture.Summaries[Core.Language];
						}

						if (string.IsNullOrEmpty(ret))
						{
							ret = texture.Name;
						}

						if (string.IsNullOrEmpty(ret))
						{
							ret = texture.UniformName;
						}

						return ret;
					};

					Func<string> getDesc = () =>
					{
						var ret = "";
						if (texture.Descriptions.ContainsKey(Core.Language))
						{
							ret = texture.Descriptions[Core.Language];
						}

						return ret;
					};

					ValueStatus status = null;

					var foundValue = FindValue(key.ToString(), usedValueStatuses, withNameFlag);
					if (foundValue != null)
					{
						status = foundValue;
						if (status.IsShown != texture.IsParam)
						{
							status.IsShown = texture.IsParam;
							isChanged = true;
						}

						// update default path
						if(texture.IsParam)
						{
							if ((foundValue.Value as Value.PathForImage).AbsolutePath == string.Empty)
							{
								(foundValue.Value as Value.PathForImage).SetAbsolutePathDirectly(texture.DefaultPath);
								isChanged = true;
							}

							(foundValue.Value as Value.PathForImage).SetDefaultAbsolutePath(texture.DefaultPath);
						}
						else
						{
							if((foundValue.Value as Value.PathForImage).AbsolutePath != texture.DefaultPath)
							{
								(foundValue.Value as Value.PathForImage).SetAbsolutePathDirectly(texture.DefaultPath);
								isChanged = true;
							}

							(foundValue.Value as Value.PathForImage).SetDefaultAbsolutePath(texture.DefaultPath);
						}
					}
					else
					{
						// create only when value is not found even if withName flag is true
						if (!withNameFlag) continue;

						status = new ValueStatus();
						var value = new Value.PathForImage(Resources.GetString("ImageFilter"), true, texture.DefaultPath);
						status.Value = value;
						status.IsShown = texture.IsParam;
						status.Priority = texture.Priority;
						valueStatuses.Add(status);

						if(!string.IsNullOrEmpty(texture.DefaultPath))
						{
							value.SetAbsolutePathDirectly(texture.DefaultPath);
						}

						isChanged = true;
					}

					if (status.Name != getName()) isChanged = true;
					if (status.Description != getDesc()) isChanged = true;

					status.Key = key;
					status.Name = getName();
					status.Description = getDesc();
					usedValueStatuses.Add(status);
					finished.Add(texture);
				}

				foreach (var uniform in info.Uniforms)
				{
					if (finished.Contains(uniform)) continue;

					var key = StatusKey.From(uniform);

					Func<string> getName = () =>
					{
						var ret = "";
						if (uniform.Summaries.ContainsKey(Core.Language))
						{
							ret = uniform.Summaries[Core.Language];
						}

						if (string.IsNullOrEmpty(ret))
						{
							ret = uniform.Name;
						}

						if (string.IsNullOrEmpty(ret))
						{
							ret = uniform.UniformName;
						}

						return ret;
					};

					Func<string> getDesc = () =>
					{
						var ret = "";
						if (uniform.Descriptions.ContainsKey(Core.Language))
						{
							ret = uniform.Descriptions[Core.Language];
						}

						return ret;
					};

					ValueStatus status = null;

					var foundValue = FindValue(key.ToString(), usedValueStatuses, withNameFlag);
					if (foundValue != null)
					{
						status = foundValue;
						
						status.IsShown = true;
						isChanged = true;

						if (uniform.Type == 0)
						{
							(status.Value as Value.Float).ChangeDefaultValue(uniform.DefaultValues[0]);
						}
						else if (uniform.Type == 1)
						{
							(status.Value as Value.Vector2D).ChangeDefaultValue(uniform.DefaultValues[0], uniform.DefaultValues[1]);
						}
						else if (uniform.Type == 2)
						{
							(status.Value as Value.Vector3D).ChangeDefaultValue(uniform.DefaultValues[0], uniform.DefaultValues[1], uniform.DefaultValues[2]);
						}
						else
						{
							(status.Value as Value.Vector4D).ChangeDefaultValue(uniform.DefaultValues[0], uniform.DefaultValues[1], uniform.DefaultValues[2], uniform.DefaultValues[3]);
						}
						
					}
					else
					{
						// create only when value is not found even if withName flag is true
						if (!withNameFlag) continue;

						if (uniform.Type == 0)
						{
							status = new ValueStatus();
							var value = new Value.Float(uniform.DefaultValues[0]);
							value.SetValueDirectly(uniform.DefaultValues[0]);
							status.Value = value;
							status.IsShown = true;
							status.Priority = uniform.Priority;
							valueStatuses.Add(status);
							isChanged = true;
						}
						else if(uniform.Type == 1)
						{
							status = new ValueStatus();
							var value = new Value.Vector2D(uniform.DefaultValues[0], uniform.DefaultValues[1]);
							value.X.SetValueDirectly(uniform.DefaultValues[0]);
							value.Y.SetValueDirectly(uniform.DefaultValues[1]);
							status.Value = value;
							status.IsShown = true;
							status.Priority = uniform.Priority;
							valueStatuses.Add(status);
							isChanged = true;
						}
						else if (uniform.Type == 2)
						{
							status = new ValueStatus();
							var value = new Value.Vector3D(uniform.DefaultValues[0], uniform.DefaultValues[1], uniform.DefaultValues[2]);
							value.X.SetValueDirectly(uniform.DefaultValues[0]);
							value.Y.SetValueDirectly(uniform.DefaultValues[1]);
							value.Z.SetValueDirectly(uniform.DefaultValues[2]);
							status.Value = value;
							status.IsShown = true;
							status.Priority = uniform.Priority;
							valueStatuses.Add(status);
							isChanged = true;
						}
						else
						{
							status = new ValueStatus();
							var value = new Value.Vector4D(uniform.DefaultValues[0], uniform.DefaultValues[1], uniform.DefaultValues[2], uniform.DefaultValues[3]);
							value.X.SetValueDirectly(uniform.DefaultValues[0]);
							value.Y.SetValueDirectly(uniform.DefaultValues[1]);
							value.Z.SetValueDirectly(uniform.DefaultValues[2]);
							value.W.SetValueDirectly(uniform.DefaultValues[3]);
							status.Value = value;
							status.IsShown = true;
							status.Priority = uniform.Priority;
							valueStatuses.Add(status);
							isChanged = true;
						}
					}

					if (status.Name != getName()) isChanged = true;
					if (status.Description != getDesc()) isChanged = true;

					status.Key = key;
					status.Name = getName();
					status.Description = getDesc();
					usedValueStatuses.Add(status);
					finished.Add(uniform);
				}
			}

			foreach (var kts in valueStatuses)
			{
				if(!usedValueStatuses.Contains(kts))
				{
					var status = kts;
					if (status.IsShown)
					{
						status.IsShown = false;
						isChanged = true;
					}
				}
			}

			if(info.CustomData.Count() > 0)
			{
				rcValues.CustomData1.Fixed4.ChangeDefaultValue(
					info.CustomData[0].DefaultValues[0],
					info.CustomData[0].DefaultValues[1],
					info.CustomData[0].DefaultValues[2],
					info.CustomData[0].DefaultValues[3]);
			}

			if (info.CustomData.Count() > 1)
			{
				rcValues.CustomData2.Fixed4.ChangeDefaultValue(
					info.CustomData[1].DefaultValues[0],
					info.CustomData[1].DefaultValues[1],
					info.CustomData[1].DefaultValues[2],
					info.CustomData[1].DefaultValues[3]);
			}

			if (isChanged && OnChanged != null)
			{
				OnChanged(this, null);
			}
		}

		public List<Tuple35<ValueStatus, Utl.MaterialInformation.TextureInformation>> GetTextures(Utl.MaterialInformation info)
		{
			var ret = new List<Tuple35<ValueStatus, Utl.MaterialInformation.TextureInformation>>();

			HashSet<ValueStatus> usedValueStatuses = new HashSet<ValueStatus>();

			foreach (var texture in info.Textures)
			{
				var key = StatusKey.From(texture);
				var value = FindValue(key.ToString(), usedValueStatuses);

				if (value != null)
				{
					ret.Add(Tuple35.Create(value, texture));
					usedValueStatuses.Add(value);
				}
				else
				{
					ret.Add(Tuple35.Create((ValueStatus)(null), texture));
				}
			}

			return ret;
		}

		public List<Tuple35<ValueStatus, Utl.MaterialInformation.UniformInformation>> GetUniforms(Utl.MaterialInformation info)
		{
			var ret = new List<Tuple35<ValueStatus, Utl.MaterialInformation.UniformInformation>>();

			foreach (var uniform in info.Uniforms)
			{
				var key = StatusKey.From(uniform);
				var value = FindValue(key.ToString());

				if (value != null)
				{
					ret.Add(Tuple35.Create(value, uniform));
				}
				else
				{
					ret.Add(Tuple35.Create((ValueStatus)(null), uniform));
				}
			}

			return ret;
		}

		/// <summary>
		/// Get key version. For compatiblity
		/// </summary>
		/// <param name="key"></param>
		/// <returns></returns>
		public static int GetVersionOfKey(string key)
		{
			if (key.EndsWith("__TYPE__U0")) return 0;
			if (key.EndsWith("__TYPE__U1")) return 0;
			if (key.EndsWith("__TYPE__U2")) return 0;
			if (key.EndsWith("__TYPE__U3")) return 0;
			if (key.EndsWith("__TYPE__U4")) return 0;
			if (key.EndsWith("__TYPE__T")) return 0;
			return 1;
		}

		public class ValueStatus
		{
			public StatusKey Key = new StatusKey();
			public object Value = null;
			public string Name = string.Empty;
			public string Description = string.Empty;
			public bool IsShown = false;
			public int Priority = 1;
		}

		public class StatusKey
		{
			public string Name = string.Empty;
			public string UniformName = string.Empty;
			public string Footer = string.Empty;

			public static StatusKey From(string key)
			{
				var version = GetVersionOfKey(key);

				StatusKey status = new StatusKey();

				if(version > 0)
				{
					// TODO : rename to unity flag
#if SCRIPT_ENABLED
					var labels = key.Split(' ');
					status.Name = System.Net.WebUtility.UrlDecode(labels[0]);
					status.UniformName = System.Net.WebUtility.UrlDecode(labels[1]);
					status.Footer = labels[2];
#endif
					return status;
				}
				else
				{
					if (key.EndsWith("__TYPE__U0")) status.Footer = "TYPE_U0";
					if (key.EndsWith("__TYPE__U1")) status.Footer = "TYPE_U1";
					if (key.EndsWith("__TYPE__U2")) status.Footer = "TYPE_U2";
					if (key.EndsWith("__TYPE__U3")) status.Footer = "TYPE_U3";
					if (key.EndsWith("__TYPE__U4")) status.Footer = "TYPE_U4";
					if (key.EndsWith("__TYPE__T")) status.Footer = "TYPE_T";

					key = key.Replace("__TYPE__U0", "");
					key = key.Replace("__TYPE__U1", "");
					key = key.Replace("__TYPE__U2", "");
					key = key.Replace("__TYPE__U3", "");
					key = key.Replace("__TYPE__U4", "");
					key = key.Replace("__TYPE__T", "");

					status.Name = key;
					status.UniformName = key;
					return status;
				}
			}

			public static StatusKey From(Utl.MaterialInformation.UniformInformation info)
			{
				StatusKey status = new StatusKey();
				status.Name = info.Name;
				status.UniformName = info.UniformName;
				status.Footer = "TYPE_U" + info.Type;
				return status;
			}

			public static StatusKey From(Utl.MaterialInformation.TextureInformation info)
			{
				StatusKey status = new StatusKey();
				status.Name = info.Name;
				status.UniformName = info.UniformName;
				status.Footer = "TYPE_T";
				return status;
			}

			public bool IsSame(StatusKey statusKey, bool withName)
			{
				if (Footer != statusKey.Footer) return false;
				if (UniformName == statusKey.UniformName) return true;
				if (withName && Name == statusKey.Name) return true;
				return false;
			}

			public override string ToString()
			{
				// TODO : rename to unity flag
#if SCRIPT_ENABLED
				return System.Net.WebUtility.UrlEncode(Name) + " " + System.Net.WebUtility.UrlEncode(UniformName) + " " + Footer;
#else
				return string.Empty;
#endif
			}
		}

		public event ChangedValueEventHandler OnChanged;
	}

	public class RendererCommonValues
	{
		[Selector(ID = 3)]
		[Key(key = "BRS_Material")]
		public Value.Enum<MaterialType> Material
		{
			get;
			private set;
		}

#if __EFFEKSEER_BUILD_VERSION16__
		[Selected(ID = 3, Value = (int)MaterialType.Default)]
		[Selected(ID = 3, Value = (int)MaterialType.Lighting)]
		[Name(language = Language.Japanese, value = "エミッシブ倍率")]
		[Name(language = Language.English, value = "Emissive Scaling")]
		public Value.Int EmissiveScaling { get; private set; }
#endif

		[Selected(ID = 3, Value = (int)MaterialType.Default)]
		[Selected(ID = 3, Value = (int)MaterialType.BackDistortion)]
		[Selected(ID = 3, Value = (int)MaterialType.Lighting)]
		[Key(key = "BRS_ColorTexture")]
		public Value.PathForImage ColorTexture
		{
			get;
			private set;
		}

		[Selected(ID = 3, Value = (int)MaterialType.Default)]
		[Selected(ID = 3, Value = (int)MaterialType.BackDistortion)]
		[Selected(ID = 3, Value = (int)MaterialType.Lighting)]
		[Key(key = "BRS_Filter")]
		public Value.Enum<FilterType> Filter { get; private set; }

		[Selected(ID = 3, Value = (int)MaterialType.Default)]
		[Selected(ID = 3, Value = (int)MaterialType.BackDistortion)]
		[Selected(ID = 3, Value = (int)MaterialType.Lighting)]
		[Key(key = "BRS_Wrap")]
		public Value.Enum<WrapType> Wrap { get; private set; }

		[Selected(ID = 3, Value = (int)MaterialType.Lighting)]
		[Key(key = "BRS_NormalTexture")]
		public Value.PathForImage NormalTexture
		{
			get;
			private set;
		}

		[Selected(ID = 3, Value = (int)MaterialType.Lighting)]
		[Key(key = "BRS_Filter2")]
		public Value.Enum<FilterType> Filter2 { get; private set; }

		[Selected(ID = 3, Value = (int)MaterialType.Lighting)]
		[Key(key = "BRS_Wrap2")]
		public Value.Enum<WrapType> Wrap2 { get; private set; }

		[Selected(ID = 3, Value = (int)MaterialType.BackDistortion)]
		[Key(key = "BRS_DistortionIntensity")]
		public Value.Float DistortionIntensity { get; private set; }

		[Selected(ID = 3, Value = (int)MaterialType.File)]
		[IO(Export = true)]
		public MaterialFileParameter MaterialFile
		{
			get;
			private set;
		}

		[Key(key = "BRS_AlphaBlend")]
		public Value.Enum<AlphaBlendType> AlphaBlend { get; private set; }

		[Key(key = "BRS_ZWrite")]
		public Value.Boolean ZWrite { get; private set; }

		[Key(key = "BRS_ZTest")]
		public Value.Boolean ZTest { get; private set; }

		[Selector(ID = 0)]
		[Key(key = "BRS_FadeInType")]
		public Value.Enum<FadeType> FadeInType
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 0)]
		[IO(Export = true)]
		public NoneParamater FadeInNone
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 1)]
		[IO(Export = true)]
		public FadeInParamater FadeIn
		{
			get;
			private set;
		}

		[Selector(ID = 1)]
		[Key(key = "BRS_FadeOutType")]
		public Value.Enum<FadeType> FadeOutType
		{
			get;
			private set;
		}

		[Selected(ID = 1, Value = 0)]
		[IO(Export = true)]
		public NoneParamater FadeOutNone
		{
			get;
			private set;
		}

		[Selected(ID = 1, Value = 1)]
		[IO(Export = true)]
		public FadeOutParamater FadeOut
		{
			get;
			private set;
		}

		[Selector(ID = 2)]
		[Key(key = "BRS_UV")]
		public Value.Enum<UVType> UV
		{
			get;
			private set;
		}

		[Selected(ID = 2, Value = 1)]
		[Selected(ID = 2, Value = 2)]
		[Selected(ID = 2, Value = 3)]
		[Selected(ID = 2, Value = 4)]
		[IO(Export = true)]
		[Key(key = "BRS_UVTextureReferenceTarget")]
		public Value.Enum<UVTextureReferenceTargetType> UVTextureReferenceTarget
		{
			get;
			private set;
		}

		[Selected(ID = 2, Value = 0)]
		[IO(Export = true)]
		public UVDefaultParamater UVDefault { get; private set; }

		[Selected(ID = 2, Value = 1)]
		[IO(Export = true)]
		public UVFixedParamater UVFixed { get; private set; }

		[Selected(ID = 2, Value = 2)]
		[IO(Export = true)]
#if __EFFEKSEER_BUILD_VERSION16__
		public UVAnimationSupportedFrameBlendParameter UVAnimation { get; private set; }
#else
		public UVAnimationParamater UVAnimation { get; private set; }
#endif

		[Selected(ID = 2, Value = 3)]
		[IO(Export = true)]
		public UVScrollParamater UVScroll { get; private set; }

		[Selected(ID = 2, Value = 4)]
		[IO(Export = true)]
		public UVFCurveParamater UVFCurve { get; private set; }

#if __EFFEKSEER_BUILD_VERSION16__
		// alpha texture uv
		[Selector(ID = 101)]
		[Name(language = Language.Japanese, value = "UV(アルファ画像)")]
		[Name(language = Language.English, value = "UV(α Texture)")]
		public Value.Enum<UVType> UV2
		{
			get;
			private set;
		}

		[Selected(ID = 101, Value = 0)]
		[IO(Export = true)]
		public UVDefaultParamater UV2Default { get; private set; }

		[Selected(ID = 101, Value = 1)]
		[IO(Export = true)]
		public UVFixedParamater UV2Fixed { get; private set; }

		[Selected(ID = 101, Value = 2)]
		[IO(Export = true)]
		public UVAnimationParamater UV2Animation { get; private set; }

		[Selected(ID = 101, Value = 3)]
		[IO(Export = true)]
		public UVScrollParamater UV2Scroll { get; private set; }

		[Selected(ID = 101, Value = 4)]
		[IO(Export = true)]
		public UVFCurveParamater UV2FCurve { get; private set; }

		// uv distortion texture uv
		[Selector(ID = 102)]
		[Name(language = Language.Japanese, value = "UV(UV歪み画像)")]
		[Name(language = Language.English, value = "UV(UV Distortion Texture)")]
		public Value.Enum<UVType> UV3 { get; private set; }

		[Selected(ID = 102, Value = 0)]
		[IO(Export = true)]
		public UVDefaultParamater UV3Default { get; private set; }

		[Selected(ID = 102, Value = 1)]
		[IO(Export = true)]
		public UVFixedParamater UV3Fixed { get; private set; }

		[Selected(ID = 102, Value = 2)]
		[IO(Export = true)]
		public UVAnimationParamater UV3Animation { get; private set; }

		[Selected(ID = 102, Value = 3)]
		[IO(Export = true)]
		public UVScrollParamater UV3Scroll { get; private set; }

		[Selected(ID = 102, Value = 4)]
		[IO(Export = true)]
		public UVFCurveParamater UV3FCurve { get; private set; }

		// blend texture uv
		[Selector(ID = 103)]
		[Name(language = Language.Japanese, value = "UV(ブレンド画像)")]
		[Name(language = Language.English, value = "UV(Blend Texture)")]
		public Value.Enum<UVType> UV4 { get; private set; }

		[Selected(ID = 103, Value = 0)]
		[IO(Export = true)]
		public UVDefaultParamater UV4Default { get; private set; }

		[Selected(ID = 103, Value = 1)]
		[IO(Export = true)]
		public UVFixedParamater UV4Fixed { get; private set; }

		[Selected(ID = 103, Value = 2)]
		[IO(Export = true)]
		public UVAnimationParamater UV4Animation { get; private set; }

		[Selected(ID = 103, Value = 3)]
		[IO(Export = true)]
		public UVScrollParamater UV4Scroll { get; private set; }

		[Selected(ID = 103, Value = 4)]
		[IO(Export = true)]
		public UVFCurveParamater UV4FCurve { get; private set; }

		// blend alpha Texture
		[Selector(ID = 104)]
		[Name(language = Language.Japanese, value = "UV(ブレンドアルファ画像)")]
		[Name(language = Language.English, value = "UV(Blend Alpha Texture)")]
		public Value.Enum<UVType> UV5 { get; private set; }

		[Selected(ID = 104, Value = 0)]
		[IO(Export = true)]
		public UVDefaultParamater UV5Default { get; private set; }

		[Selected(ID = 104, Value = 1)]
		[IO(Export = true)]
		public UVFixedParamater UV5Fixed { get; private set; }

		[Selected(ID = 104, Value = 2)]
		[IO(Export = true)]
		public UVAnimationParamater UV5Animation { get; private set; }

		[Selected(ID = 104, Value = 3)]
		[IO(Export = true)]
		public UVScrollParamater UV5Scroll { get; private set; }

		[Selected(ID = 104, Value = 4)]
		[IO(Export = true)]
		public UVFCurveParamater UV5FCurve { get; private set; }

		// blend uv distortion textute
		[Selector(ID = 105)]
		[Name(language = Language.Japanese, value = "UV(ブレンドUV歪み画像)")]
		[Name(language = Language.English, value = "UV(Blend UV Distortion Texture)")]
		public Value.Enum<UVType> UV6 { get; private set; }

		[Selected(ID = 105, Value = 0)]
		[IO(Export = true)]
		public UVDefaultParamater UV6Default { get; private set; }

		[Selected(ID = 105, Value = 1)]
		[IO(Export = true)]
		public UVFixedParamater UV6Fixed { get; private set; }

		[Selected(ID = 105, Value = 2)]
		[IO(Export = true)]
		public UVAnimationParamater UV6Animation { get; private set; }

		[Selected(ID = 105, Value = 3)]
		[IO(Export = true)]
		public UVScrollParamater UV6Scroll { get; private set; }

		[Selected(ID = 105, Value = 4)]
		[IO(Export = true)]
		public UVFCurveParamater UV6FCurve { get; private set; }
#endif

		[Key(key = "BRS_ColorInheritType")]
		public Value.Enum<ParentEffectType> ColorInheritType
		{
			get;
			private set;
		}

		[IO(Export = true)]
		[Key(key = "BRS_CustomData1")]
		public CustomDataParameter CustomData1 { get; private set; }

		[IO(Export = true)]
		[Key(key = "BRS_CustomData2")]
		public CustomDataParameter CustomData2 { get; private set; }
		internal RendererCommonValues()
		{
			Material = new Value.Enum<MaterialType>(MaterialType.Default);
			MaterialFile = new MaterialFileParameter(this);

#if __EFFEKSEER_BUILD_VERSION16__
			EmissiveScaling = new Value.Int(1, int.MaxValue, 1);
#endif

			ColorTexture = new Value.PathForImage(Resources.GetString("ImageFilter"), true, "");
			Filter = new Value.Enum<FilterType>(FilterType.Linear);
			Wrap = new Value.Enum<WrapType>(WrapType.Repeat);

			NormalTexture = new Value.PathForImage(Resources.GetString("ImageFilter"), true, "");
			Filter2 = new Value.Enum<FilterType>(FilterType.Linear);
			Wrap2 = new Value.Enum<WrapType>(WrapType.Repeat);

			AlphaBlend = new Value.Enum<AlphaBlendType>(AlphaBlendType.Blend);
			
			FadeInType = new Value.Enum<FadeType>(FadeType.None);
			FadeInNone = new NoneParamater();
			FadeIn = new FadeInParamater();

			FadeOutType = new Value.Enum<FadeType>();
			FadeOutNone = new NoneParamater();
			FadeOut = new FadeOutParamater();

			UV = new Value.Enum<UVType>();

			UVTextureReferenceTarget = new Value.Enum<UVTextureReferenceTargetType>(UVTextureReferenceTargetType.Texture1);

			UVDefault = new UVDefaultParamater();
			UVFixed = new UVFixedParamater();
#if __EFFEKSEER_BUILD_VERSION16__
			UVAnimation = new UVAnimationSupportedFrameBlendParameter();
#else
			UVAnimation = new UVAnimationParamater();
#endif
			UVScroll = new UVScrollParamater();
			UVFCurve = new UVFCurveParamater();

#if __EFFEKSEER_BUILD_VERSION16__
			UV2 = new Value.Enum<UVType>();
			UV2Default = new UVDefaultParamater();
			UV2Fixed = new UVFixedParamater();
			UV2Animation = new UVAnimationParamater();
			UV2Scroll = new UVScrollParamater();
			UV2FCurve = new UVFCurveParamater();

			UV3 = new Value.Enum<UVType>();
			UV3Default = new UVDefaultParamater();
			UV3Fixed = new UVFixedParamater();
			UV3Animation = new UVAnimationParamater();
			UV3Scroll = new UVScrollParamater();
			UV3FCurve = new UVFCurveParamater();

			UV4 = new Value.Enum<UVType>();
			UV4Default = new UVDefaultParamater();
			UV4Fixed = new UVFixedParamater();
			UV4Animation = new UVAnimationParamater();
			UV4Scroll = new UVScrollParamater();
			UV4FCurve = new UVFCurveParamater();

			UV5 = new Value.Enum<UVType>();
			UV5Default = new UVDefaultParamater();
			UV5Fixed = new UVFixedParamater();
			UV5Animation = new UVAnimationParamater();
			UV5Scroll = new UVScrollParamater();
			UV5FCurve = new UVFCurveParamater();

			UV6 = new Value.Enum<UVType>();
			UV6Default = new UVDefaultParamater();
			UV6Fixed = new UVFixedParamater();
			UV6Animation = new UVAnimationParamater();
			UV6Scroll = new UVScrollParamater();
			UV6FCurve = new UVFCurveParamater();
#endif

			ZWrite = new Value.Boolean(false);
			ZTest = new Value.Boolean(true);

			ColorInheritType = new Value.Enum<ParentEffectType>(ParentEffectType.NotBind);

			DistortionIntensity = new Value.Float(1.0f, float.MaxValue, float.MinValue, 0.1f);

			CustomData1 = new CustomDataParameter(1);
			CustomData2 = new CustomDataParameter(2);
		}

		public class NoneParamater
		{
			internal NoneParamater()
			{
			}
		}

		public class FadeInParamater
		{
			[Key(key = "BRS_FadeIn_Frame")]
			public Value.Float Frame { get; private set; }

			[Key(key = "BRS_FadeIn_StartSpeed")]
			public Value.Enum<EasingStart> StartSpeed
			{
				get;
				private set;
			}

			[Key(key = "BRS_FadeIn_EndSpeed")]
			public Value.Enum<EasingEnd> EndSpeed
			{
				get;
				private set;
			}

			public FadeInParamater()
			{
				Frame = new Value.Float(1, float.MaxValue, 0);
				StartSpeed = new Value.Enum<EasingStart>(EasingStart.Start);
				EndSpeed = new Value.Enum<EasingEnd>(EasingEnd.End);
			}
		}

		public class FadeOutParamater
		{
			[Key(key = "BRS_FadeOut_Frame")]
			public Value.Float Frame { get; private set; }

			[Key(key = "BRS_FadeOut_StartSpeed")]
			public Value.Enum<EasingStart> StartSpeed
			{
				get;
				private set;
			}

			[Key(key = "BRS_FadeOut_EndSpeed")]
			public Value.Enum<EasingEnd> EndSpeed
			{
				get;
				private set;
			}

			public FadeOutParamater()
			{
				Frame = new Value.Float(1, float.MaxValue, 0);
				StartSpeed = new Value.Enum<EasingStart>(EasingStart.Start);
				EndSpeed = new Value.Enum<EasingEnd>(EasingEnd.End);
			}
		}

		public class UVDefaultParamater
		{
		}

		public class UVFixedParamater
		{
			[Key(key = "UVFixedParamater_Start")]
			public Value.Vector2D Start { get; private set; }

			[Key(key = "UVFixedParamater_Size")]
			public Value.Vector2D Size { get; private set; }

			public UVFixedParamater()
			{
				Start = new Value.Vector2D();
				Size = new Value.Vector2D();
			}
		}

		public class UVAnimationParamater
		{
			[Key(key = "UVAnimationParamater_Start")]
			public Value.Vector2D Start { get; private set; }

			[Key(key = "UVAnimationParamater_Size")]
			public Value.Vector2D Size { get; private set; }

			[Key(key = "UVAnimationParamater_FrameLength")]
			public Value.IntWithInifinite FrameLength { get; private set; }

			[Key(key = "UVAnimationParamater_FrameCountX")]
			public Value.Int FrameCountX { get; private set; }

			[Key(key = "UVAnimationParamater_FrameCountY")]
			public Value.Int FrameCountY { get; private set; }

			[Key(key = "UVAnimationParamater_LoopType")]
			public Value.Enum<LoopType> LoopType { get; private set; }

			[Key(key = "UVAnimationParamater_StartSheet")]
			public Value.IntWithRandom StartSheet { get; private set; }

			public UVAnimationParamater()
			{
				Start = new Value.Vector2D();
				Size = new Value.Vector2D();
				FrameLength = new Value.IntWithInifinite(1, false, int.MaxValue, 1);
				FrameCountX = new Value.Int(1, int.MaxValue, 1);
				FrameCountY = new Value.Int(1, int.MaxValue, 1);
				LoopType = new Value.Enum<LoopType>(RendererCommonValues.LoopType.Once);
				StartSheet = new Value.IntWithRandom(0, int.MaxValue, 0);
			}
		}

#if __EFFEKSEER_BUILD_VERSION16__
		public class UVAnimationSupportedFrameBlendParameter
		{
			[IO(Export = true)]
			public UVAnimationParamater AnimationParams { get; private set; }

			[IO(Export = true)]
			[Name(language = Language.Japanese, value = "アニメーション補間")]
			[Name(language = Language.English, value = "Flipbook Frame Blend Type")]
			public Value.Enum<FlipbookInterpolationType> FlipbookInterpolationType { get; private set; }

			public UVAnimationSupportedFrameBlendParameter() : base()
			{
				AnimationParams = new UVAnimationParamater();
				FlipbookInterpolationType = new Value.Enum<FlipbookInterpolationType>(RendererCommonValues.FlipbookInterpolationType.None);
			}
		}
#endif

		public class UVScrollParamater
		{
			[Key(key = "UVScrollParamater_Start")]
			public Value.Vector2DWithRandom Start { get; private set; }

			[Key(key = "UVScrollParamater_Size")]
			public Value.Vector2DWithRandom Size { get; private set; }

			[Key(key = "UVScrollParamater_Speed")]
			public Value.Vector2DWithRandom Speed { get; private set; }

			public UVScrollParamater()
			{
				Start = new Value.Vector2DWithRandom();
				Size = new Value.Vector2DWithRandom();
				Speed = new Value.Vector2DWithRandom();
			}
		}

		public class UVFCurveParamater
		{
			[Key(key = "UVFCurveParamater_Start")]
			[IO(Export = true)]
			public Value.FCurveVector2D Start { get; private set; }

			[Key(key = "UVFCurveParamater_Size")]
			[IO(Export = true)]
			public Value.FCurveVector2D Size { get; private set; }

			public UVFCurveParamater()
			{
				Start = new Value.FCurveVector2D();
				Size = new Value.FCurveVector2D();
			}
		}

		public enum MaterialType : int
		{
			[Key(key = "MaterialType_Default")]
			Default = 0,

			[Key(key = "MaterialType_BackDistortion")]
			BackDistortion = 6,

			[Key(key = "MaterialType_Lighting")]
			Lighting = 7,

			[Key(key = "MaterialType_File")]
			File = 128,
		}
		public enum FadeType : int
		{
			[Key(key = "FadeType_Use")]
			Use = 1,
			[Key(key = "FadeType_None")]
			None = 0,
		}

		public enum FilterType : int
		{
			[Key(key = "FilterType_Nearest")]
			Nearest = 0,
			[Key(key = "FilterType_Linear")]
			Linear = 1,
		}

		public enum WrapType : int
		{
			[Key(key = "WrapType_Repeat")]
			Repeat = 0,
			[Key(key = "WrapType_Clamp")]
			Clamp = 1,
		}

		public enum UVType : int
		{
			[Key(key = "UVType_Default")]
			Default = 0,
			[Key(key = "UVType_Fixed")]
			Fixed = 1,
			[Key(key = "UVType_Animation")]
			Animation = 2,
			[Key(key = "UVType_Scroll")]
			Scroll = 3,
			[Key(key = "UVType_FCurve")]
			FCurve = 4,
		}

		public enum LoopType : int
		{
			[Key(key = "UVLoopType_Once")]
			Once = 0,
			[Key(key = "UVLoopType_Loop")]
			Loop = 1,
			[Key(key = "UVLoopType_ReverceLoop")]
			ReverceLoop = 2,
		}

#if __EFFEKSEER_BUILD_VERSION16__
		public enum FlipbookInterpolationType : int
		{
			[Name(value = "なし", language = Language.Japanese)]
			[Name(value = "None", language = Language.English)]
			None = 0,

			[Name(value = "線形補間", language = Language.Japanese)]
			[Name(value = "Lerp", language = Language.English)]
			Lerp = 1,
		}
#endif

	}
}

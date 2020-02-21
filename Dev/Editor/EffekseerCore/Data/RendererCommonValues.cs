using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum UVTextureReferenceTargetType
	{
		[Name(language = Language.Japanese, value = "なし(128x128)")]
		[Name(language = Language.English, value = "None(128x128)")]
		None = 0,
		[Name(language = Language.Japanese, value = "画像1")]
		[Name(language = Language.English, value = "Image1")]
		Texture1 = 1,
		[Name(language = Language.Japanese, value = "画像2")]
		[Name(language = Language.English, value = "Image2")]
		Texture2 = 2,
		[Name(language = Language.Japanese, value = "画像3")]
		[Name(language = Language.English, value = "Image3")]
		Texture3 = 3,
		[Name(language = Language.Japanese, value = "画像4")]
		[Name(language = Language.English, value = "Image4")]
		Texture4 = 4,
	}

	public enum CustomDataType
	{
		[Name(language = Language.Japanese, value = "なし")]
		[Name(language = Language.English, value = "None")]
		None = 0,

		[Name(language = Language.Japanese, value = "固定2")]
		[Name(language = Language.English, value = "Fixed2")]
		Fixed2D = 20,

		[Name(language = Language.Japanese, value = "ランダム2")]
		[Name(language = Language.English, value = "Random2")]
		Random2D = 21,

		[Name(language = Language.Japanese, value = "イージング2")]
		[Name(language = Language.English, value = "Easing2")]
		Easing2D = 22,

		[Name(language = Language.Japanese, value = "Fカーブ2")]
		[Name(language = Language.English, value = "FCurve2")]
		FCurve2D = 23,

		[Name(language = Language.Japanese, value = "固定4")]
		[Name(language = Language.English, value = "Fixed4")]
		Fixed4D = 40,

		[Name(language = Language.Japanese, value = "Fカーブ色")]
		[Name(language = Language.English, value = "FCurve-Color")]
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
		[Name(language = Language.Japanese, value = "パス")]
		[Name(language = Language.English, value = "Path")]
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
					var labels = key.Split(' ');
					status.Name = System.Net.WebUtility.UrlDecode(labels[0]);
					status.UniformName = System.Net.WebUtility.UrlDecode(labels[1]);
					status.Footer = labels[2];
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
				return System.Net.WebUtility.UrlEncode(Name) + " " + System.Net.WebUtility.UrlEncode(UniformName) + " " + Footer;
			}
		}

		public event ChangedValueEventHandler OnChanged;
	}

	public class RendererCommonValues
	{
		[Selector(ID = 3)]
		[Name(language = Language.Japanese, value = "マテリアル")]
		[Name(language = Language.English, value = "Material")]
		public Value.Enum<MaterialType> Material
		{
			get;
			private set;
		}

		[Selected(ID = 3, Value = (int)MaterialType.Default)]
		[Selected(ID = 3, Value = (int)MaterialType.BackDistortion)]
		[Selected(ID = 3, Value = (int)MaterialType.Lighting)]
		[Name(language = Language.Japanese, value = "色/歪み画像")]
		[Description(language = Language.Japanese, value = "色/歪みを表す画像")]
		[Name(language = Language.English, value = "Texture")]
		[Description(language = Language.English, value = "Image that represents color/distortion")]

		public Value.PathForImage ColorTexture
		{
			get;
			private set;
		}

		[Selected(ID = 3, Value = (int)MaterialType.Default)]
		[Selected(ID = 3, Value = (int)MaterialType.BackDistortion)]
		[Selected(ID = 3, Value = (int)MaterialType.Lighting)]
		[Name(language = Language.Japanese, value = "フィルタ")]
		[Name(language = Language.English, value = "Filter")]
		public Value.Enum<FilterType> Filter { get; private set; }

		[Selected(ID = 3, Value = (int)MaterialType.Default)]
		[Selected(ID = 3, Value = (int)MaterialType.BackDistortion)]
		[Selected(ID = 3, Value = (int)MaterialType.Lighting)]
		[Name(language = Language.Japanese, value = "外側")]
		[Name(language = Language.English, value = "Wrap")]
		public Value.Enum<WrapType> Wrap { get; private set; }

		[Selected(ID = 3, Value = (int)MaterialType.Lighting)]
		[Name(language = Language.Japanese, value = "法線画像")]
		[Description(language = Language.Japanese, value = "法線を表す画像")]
		[Name(language = Language.English, value = "Normal Map")]
		[Description(language = Language.English, value = "Image representing normal vectors")]
		public Value.PathForImage NormalTexture
		{
			get;
			private set;
		}

		[Selected(ID = 3, Value = (int)MaterialType.Lighting)]
		[Name(language = Language.Japanese, value = "フィルタ")]
		[Name(language = Language.English, value = "Filter")]
		public Value.Enum<FilterType> Filter2 { get; private set; }

		[Selected(ID = 3, Value = (int)MaterialType.Lighting)]
		[Name(language = Language.Japanese, value = "外側")]
		[Name(language = Language.English, value = "Wrap")]
		public Value.Enum<WrapType> Wrap2 { get; private set; }

		[Selected(ID = 3, Value = (int)MaterialType.BackDistortion)]
		[Name(language = Language.Japanese, value = "歪み強度")]
		[Name(language = Language.English, value = "Distortion\nIntensity")]
		public Value.Float DistortionIntensity { get; private set; }

		[Selected(ID = 3, Value = (int)MaterialType.File)]
		[IO(Export = true)]
		public MaterialFileParameter MaterialFile
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "ブレンド")]
		[Name(language = Language.English, value = "Blend")]
		public Value.Enum<AlphaBlendType> AlphaBlend { get; private set; }

		[Name(language = Language.Japanese, value = "深度書き込み")]
		[Name(language = Language.English, value = "Depth Set")]
		public Value.Boolean ZWrite { get; private set; }

		[Name(language = Language.Japanese, value = "深度テスト")]
		[Name(language = Language.English, value = "Depth Test")]
		public Value.Boolean ZTest { get; private set; }

		[Selector(ID = 0)]
		[Name(language = Language.Japanese, value = "フェードイン")]
		[Name(language = Language.English, value = "Fade-In")]
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
		[Name(language = Language.Japanese, value = "フェードアウト")]
		[Name(language = Language.English, value = "Fade-Out")]
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
		[Name(language = Language.Japanese, value = "UV")]
		[Name(language = Language.English, value = "UV")]
		public Value.Enum<UVType> UV
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "参照画像")]
		[Name(language = Language.English, value = "Referenced")]
		[Selected(ID = 2, Value = 1)]
		[Selected(ID = 2, Value = 2)]
		[Selected(ID = 2, Value = 3)]
		[IO(Export = true)]
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
		public UVAnimationParamater UVAnimation { get; private set; }

		[Selected(ID = 2, Value = 3)]
		[IO(Export = true)]
		public UVScrollParamater UVScroll { get; private set; }

		[Selected(ID = 2, Value = 4)]
		[IO(Export = true)]
		public UVFCurveParamater UVFCurve { get; private set; }

		[Name(language = Language.Japanese, value = "色への影響")]
		[Description(language = Language.Japanese, value = "親ノードからの色への影響")]
		[Name(language = Language.English, value = "Inherit Color")]
		[Description(language = Language.English, value = "When this instance should copy its parent node's color")]
		public Value.Enum<ParentEffectType> ColorInheritType
		{
			get;
			private set;
		}

#if __EFFEKSEER_BUILD_VERSION16__
		[Selector(ID = 100)]
		[IO(Export = true)]
		[Name(language = Language.Japanese, value = "アルファ画像を有効")]
		public Value.Boolean EnableAlphaTexture { get; private set; }

		[IO(Export = true)]
		[Selected(ID = 100, Value = 0)]
		public AlphaTextureParameter AlphaTextureParam { get; private set; }

		public class AlphaTextureParameter
		{
			[Name(language = Language.Japanese, value = "アルファ画像")]
			[Name(language = Language.English, value = "α Texture")]
			public Value.PathForImage Texture
			{
				get; private set;
			}

			[Name(language = Language.Japanese, value = "フィルタ(アルファ画像)")]
			[Name(language = Language.English, value = "Filter(α Texture)")]
			public Value.Enum<FilterType> Filter { get; private set; }

			[Name(language = Language.Japanese, value = "外側(アルファ画像)")]
			[Name(language = Language.English, value = "Wrap(α Texture)")]
			public Value.Enum<WrapType> Wrap { get; private set; }

			[Selector(ID = 101)]
			[Name(language = Language.Japanese, value = "UV(アルファ画像)")]
			[Name(language = Language.English, value = "UV(α Texture)")]
			public Value.Enum<UVType> UV
			{
				get;
				private set;
			}

			[Selected(ID = 101, Value = 0)]
			[IO(Export = true)]
			public UVDefaultParamater UVDefault { get; private set; }

			[Selected(ID = 101, Value = 1)]
			[IO(Export = true)]
			public UVFixedParamater UVFixed { get; private set; }

			[Selected(ID = 101, Value = 2)]
			[IO(Export = true)]
			public UVAnimationParamater UVAnimation { get; private set; }

			[Selected(ID = 101, Value = 3)]
			[IO(Export = true)]
			public UVScrollParamater UVScroll { get; private set; }

			[Selected(ID = 101, Value = 4)]
			[IO(Export = true)]
			public UVFCurveParamater UVFCurve { get; private set; }

			public AlphaTextureParameter()
			{
				Texture = new Value.PathForImage(Resources.GetString("ImageFilter"), true, "");
				Filter = new Value.Enum<FilterType>(FilterType.Linear);
				Wrap = new Value.Enum<WrapType>(WrapType.Repeat);
				UV = new Value.Enum<UVType>();
				UVDefault = new UVDefaultParamater();
				UVFixed = new UVFixedParamater();
				UVAnimation = new UVAnimationParamater();
				UVScroll = new UVScrollParamater();
				UVFCurve = new UVFCurveParamater();
			}
		}

		[Name(language = Language.Japanese, value = "アルファクランチ")]
		[Name(language = Language.English, value = "Alpha Crunch")]
		[IO(Export = true)]
		public Value.Enum<AlphaCrunchType> AlphaCrunchTypeValue { get; private set; }
#endif

		[Name(language = Language.Japanese, value = "カスタムデータ")]
		[Name(language = Language.English, value = "Custom data")]
		[IO(Export = true)]
		public CustomDataParameter CustomData1 { get; private set; }

		[Name(language = Language.Japanese, value = "カスタムデータ")]
		[Name(language = Language.English, value = "Custom data")]
		[IO(Export = true)]
		public CustomDataParameter CustomData2 { get; private set; }
		internal RendererCommonValues()
		{
			Material = new Value.Enum<MaterialType>(MaterialType.Default);
			MaterialFile = new MaterialFileParameter(this);

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
			UVAnimation = new UVAnimationParamater();
			UVScroll = new UVScrollParamater();
			UVFCurve = new UVFCurveParamater();

			ZWrite = new Value.Boolean(false);
			ZTest = new Value.Boolean(true);

			ColorInheritType = new Value.Enum<ParentEffectType>(ParentEffectType.NotBind);

			DistortionIntensity = new Value.Float(1.0f, float.MaxValue, float.MinValue, 0.1f);

#if __EFFEKSEER_BUILD_VERSION16__
			EnableAlphaTexture = new Value.Boolean(false);
			AlphaTextureParam = new AlphaTextureParameter();

			AlphaCrunchTypeValue = new Value.Enum<AlphaCrunchType>(RendererCommonValues.AlphaCrunchType.None);
#endif

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
			[Name(value = "フレーム数", language = Language.Japanese)]
			[Description(language = Language.Japanese, value = "生成からフェードインが終了するまでのフレーム数")]
			[Name(value = "Frame Count", language = Language.English)]
			[Description(language = Language.English, value = "Duration in frames of the fade-in transition")]
			public Value.Float Frame { get; private set; }

			[Name(language = Language.Japanese, value = "始点速度")]
			[Description(language = Language.Japanese, value = "始点速度")]
			[Name(language = Language.English, value = "Ease In")]
			[Description(language = Language.English, value = "Initial speed (of the tween)")]
			public Value.Enum<EasingStart> StartSpeed
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "終点速度")]
			[Description(language = Language.Japanese, value = "終点速度")]
			[Name(language = Language.English, value = "Ease Out")]
			[Description(language = Language.English, value = "Final speed (of the tween)")]
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
			[Name(value = "フレーム数", language = Language.Japanese)]
			[Description(language = Language.Japanese, value = "フェードアウトが開始してから終了するまでのフレーム数")]
			[Name(value = "Frame Count", language = Language.English)]
			[Description(language = Language.English, value = "Duration in frames of the fade-out transition")]
			public Value.Float Frame { get; private set; }

			[Name(language = Language.Japanese, value = "始点速度")]
			[Description(language = Language.Japanese, value = "始点速度")]
			[Name(language = Language.English, value = "Ease In")]
			[Description(language = Language.English, value = "Initial speed (of the tween)")]
			public Value.Enum<EasingStart> StartSpeed
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "終点速度")]
			[Description(language = Language.Japanese, value = "終点速度")]
			[Name(language = Language.English, value = "Ease Out")]
			[Description(language = Language.English, value = "Final speed (of the tween)")]
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
			[Name(value = "始点", language = Language.Japanese)]
			[Name(value = "Start", language = Language.English)]
			public Value.Vector2D Start { get; private set; }
			[Name(value = "大きさ", language = Language.Japanese)]
			[Name(value = "Size", language = Language.English)]
			public Value.Vector2D Size { get; private set; }

			public UVFixedParamater()
			{
				Start = new Value.Vector2D();
				Size = new Value.Vector2D();
			}
		}

		public class UVAnimationParamater
		{
			[Name(value = "始点", language = Language.Japanese)]
			[Name(value = "Start", language = Language.English)]
			public Value.Vector2D Start { get; private set; }
			[Name(value = "大きさ", language = Language.Japanese)]
			[Name(value = "Size", language = Language.English)]
			public Value.Vector2D Size { get; private set; }

			[Name(value = "1枚あたりの時間", language = Language.Japanese)]
			[Name(value = "Frame Length", language = Language.English)]
			public Value.IntWithInifinite FrameLength { get; private set; }

			[Name(value = "横方向枚数", language = Language.Japanese)]
			[Name(value = "X-Count", language = Language.English)]
			public Value.Int FrameCountX { get; private set; }

			[Name(value = "縦方向枚数", language = Language.Japanese)]
			[Name(value = "Y-Count", language = Language.English)]
			public Value.Int FrameCountY { get; private set; }

			[Name(value = "ループ", language = Language.Japanese)]
			[Name(value = "Loop", language = Language.English)]
			public Value.Enum<LoopType> LoopType { get; private set; }

			[Name(value = "開始枚数", language = Language.Japanese)]
			[Name(value = "Start Sheet", language = Language.English)]
			public Value.IntWithRandom StartSheet { get; private set; }

#if __EFFEKSEER_BUILD_VERSION16__
			[Name(value = "アニメーション補間", language = Language.Japanese)]
			public Value.Enum<FlipbookInterpolationType> FlipbookInterpolationType { get; private set; }
#endif

			public UVAnimationParamater()
			{
				Start = new Value.Vector2D();
				Size = new Value.Vector2D();
				FrameLength = new Value.IntWithInifinite(1, false, int.MaxValue, 1);
				FrameCountX = new Value.Int(1, int.MaxValue, 1);
				FrameCountY = new Value.Int(1, int.MaxValue, 1);
				LoopType = new Value.Enum<LoopType>(RendererCommonValues.LoopType.Once);
				StartSheet = new Value.IntWithRandom(0, int.MaxValue, 0);

#if __EFFEKSEER_BUILD_VERSION16__
				FlipbookInterpolationType = new Value.Enum<FlipbookInterpolationType>(RendererCommonValues.FlipbookInterpolationType.None);
#endif
			}
		}

		public class UVScrollParamater
		{
			[Name(value = "始点", language = Language.Japanese)]
			[Name(value = "Start", language = Language.English)]
			public Value.Vector2DWithRandom Start { get; private set; }

			[Name(value = "大きさ", language = Language.Japanese)]
			[Name(value = "Size", language = Language.English)]
			public Value.Vector2DWithRandom Size { get; private set; }

			[Name(value = "移動速度", language = Language.Japanese)]
			[Name(value = "Scroll Speed", language = Language.English)]
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
			[Name(value = "始点", language = Language.Japanese)]
			[Name(value = "Start", language = Language.English)]
			[IO(Export = true)]
			public Value.FCurveVector2D Start { get; private set; }

			[Name(value = "大きさ", language = Language.Japanese)]
			[Name(value = "Size", language = Language.English)]
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
			[Name(value = "標準", language = Language.Japanese)]
			[Name(value = "Default", language = Language.English)]
			Default = 0,

			[Name(value = "歪み(背景)", language = Language.Japanese)]
			[Name(value = "Distortion(Back)", language = Language.English)]
			BackDistortion = 6,

			[Name(value = "ライティング", language = Language.Japanese)]
			[Name(value = "Lighting", language = Language.English)]
			Lighting = 7,

			[Name(value = "ファイル", language = Language.Japanese)]
			[Name(value = "File", language = Language.English)]
			File = 128,
		}
		public enum FadeType : int
		{
			[Name(value = "有り", language = Language.Japanese)]
			[Name(value = "Enabled", language = Language.English)]
			Use = 1,
			[Name(value = "無し", language = Language.Japanese)]
			[Name(value = "Disabled", language = Language.English)]
			None = 0,
		}

		public enum FilterType : int
		{
			[Name(value = "最近傍", language = Language.Japanese)]
			[Name(value = "Nearest-Neighbor", language = Language.English)]
			Nearest = 0,
			[Name(value = "線形", language = Language.Japanese)]
			[Name(value = "Linear Interpolation", language = Language.English)]
			Linear = 1,
		}

		public enum WrapType : int
		{
			[Name(value = "繰り返し", language = Language.Japanese)]
			[Name(value = "Repeat", language = Language.English)]
			Repeat = 0,
			[Name(value = "クランプ", language = Language.Japanese)]
			[Name(value = "Clamp", language = Language.English)]
			Clamp = 1,
		}

		public enum UVType : int
		{
			[Name(value = "標準", language = Language.Japanese)]
			[Name(value = "Standard", language = Language.English)]
			Default = 0,
			[Name(value = "固定", language = Language.Japanese)]
			[Name(value = "Fixed", language = Language.English)]
			Fixed = 1,
			[Name(value = "アニメーション", language = Language.Japanese)]
			[Name(value = "Animation", language = Language.English)]
			Animation = 2,
			[Name(value = "スクロール", language = Language.Japanese)]
			[Name(value = "Scroll", language = Language.English)]
			Scroll = 3,
			[Name(value = "Fカーブ", language = Language.Japanese)]
			[Name(value = "F-Curve", language = Language.English)]
			FCurve = 4,
		}

		public enum LoopType : int
		{
			[Name(value = "なし", language = Language.Japanese)]
			[Name(value = "None", language = Language.English)]
			Once = 0,
			[Name(value = "ループ", language = Language.Japanese)]
			[Name(value = "Loop", language = Language.English)]
			Loop = 1,
			[Name(value = "逆ループ", language = Language.Japanese)]
			[Name(value = "Reverse Loop", language = Language.English)]
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

		public enum AlphaCrunchType : int
		{
			[Name(value = "なし", language = Language.Japanese)]
			[Name(value = "None", language = Language.English)]
			None = 0,

			[Name(value = "アルファ閾値", language = Language.Japanese)]
			[Name(value = "Alpha Threshold", language = Language.English)]
			AlphaThreashold = 1,

			[Name(value = "4点補間", language = Language.Japanese)]
			[Name(value = "Four Point Interpolation", language = Language.English)]
			FourPointInterpolation = 2,

			[Name(value = "イージング", language = Language.Japanese)]
			[Name(value = "Easing", language = Language.English)]
			Easing = 3,

			[Name(value = "Fカーブ", language = Language.Japanese)]
			[Name(value = "F Curve", language = Language.English)]
			FCurve = 4,
		}
#endif

	}
}

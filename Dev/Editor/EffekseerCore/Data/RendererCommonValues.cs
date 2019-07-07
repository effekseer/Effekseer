using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{

	public class MaterialFileParameter : IEditableValueCollection
	{
		[Shown(Shown = true)]
		[Name(language = Language.Japanese, value = "パス")]
		[Name(language = Language.English, value = "Path")]
		public Value.PathForMaterial Path
		{
			get;
			private set;
		}

		public Dictionary<string, object> KeyValues
		{
			get
			{
				return keyToValues;
			}
		}

		Dictionary<string, object> keyToValues = new Dictionary<string, object>();

		public MaterialFileParameter()
		{
			Path = new Value.PathForMaterial(Resources.GetString("MaterialFilter"), true);
			Path.OnChanged += Path_OnChanged;
		}

		private void Path_OnChanged(object sender, ChangedValueEventArgs e)
		{
			// Apply values
			Utl.MaterialInformation info = new Utl.MaterialInformation();
			info.Load(Path.GetAbsolutePath());

			ApplyMaterial(info);
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

			// need to filter
			var propPath = EditableValue.Create(Path, this.GetType().GetProperty("Path"));
			ret.Add(propPath);

			foreach(var kv in keyToValues)
			{
				EditableValue ev = new EditableValue();
				var status = kv.Value as ValueStatus;
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

			var textureKeys = info.Textures.Select(_ => CreateKey(_)).ToList();

			foreach (var kts in keyToValues)
			{
				if(!textureKeys.Contains(kts.Key))
				{
					var status = kts.Value as ValueStatus;
					if(status.IsShown)
					{
						status.IsShown = false;
						isChanged = true;
					}
				}
			}

			var uniformKeys = info.Uniforms.Select(_ => CreateKey(_)).ToList();

			foreach (var kts in keyToValues)
			{
				if (!uniformKeys.Contains(kts.Key))
				{
					var status = kts.Value as ValueStatus;
					if (status.IsShown)
					{
						status.IsShown = false;
						isChanged = true;
					}
				}
			}

			foreach (var texture in info.Textures)
			{
				var key = CreateKey(texture);

				if(keyToValues.ContainsKey(key))
				{
					var status = keyToValues[key] as ValueStatus;
					if(status.IsShown != texture.IsParam)
					{
						status.IsShown = texture.IsParam;
						isChanged = true;
					}
				}
				else
				{
					var status = new ValueStatus();
					var value = new Value.PathForImage(Resources.GetString("ImageFilter"), true);
					status.Key = key;
					status.Value = value;
					status.Name = texture.Name;
					status.Description = "";
					status.IsShown = texture.IsParam;
					keyToValues.Add(key, status);
					value.SetAbsolutePathDirectly(texture.DefaultPath);
					isChanged = true;
				}
			}

			foreach(var uniform in info.Uniforms)
			{
				var key = CreateKey(uniform);

				if (keyToValues.ContainsKey(key))
				{
					var status = keyToValues[key] as ValueStatus;
					if (!status.IsShown)
					{
						status.IsShown = true;
						isChanged = true;
					}
				}
				else
				{
					if(uniform.Type == 0)
					{
						var status = new ValueStatus();
						status.Key = key;
						status.Value = new Value.Float();
						status.Name = uniform.Name;
						status.Description = "";
						status.IsShown = true;
						keyToValues.Add(key, status);
						isChanged = true;
					}
					else
					{
						var status = new ValueStatus();
						status.Key = key;
						status.Value = new Value.Vector4D();
						status.Name = uniform.Name;
						status.Description = "";
						status.IsShown = true;
						keyToValues.Add(key, status);
						isChanged = true;
					}
				}
			}

			if(isChanged && OnChanged != null)
			{
				OnChanged(this, null);
			}
		}

		public List<Tuple35<ValueStatus, bool>> GetTextures(Utl.MaterialInformation info)
		{
			var ret = new List<Tuple35<ValueStatus, bool>>();

			foreach (var texture in info.Textures)
			{
				var key = CreateKey(texture);

				if(texture.IsValueTexture)
				{
					if (keyToValues.ContainsKey(key))
					{
						ret.Add(Tuple35.Create(keyToValues[key] as ValueStatus, true));
					}
					else
					{
						ret.Add(Tuple35.Create((ValueStatus)(null), true));
					}
				}
				else
				{
					if (keyToValues.ContainsKey(key))
					{
						ret.Add(Tuple35.Create(keyToValues[key] as ValueStatus, false));
					}
					else
					{
						ret.Add(Tuple35.Create((ValueStatus)(null), false));
					}
				}
			}

			return ret;
		}

		public List<ValueStatus> GetUniforms(Utl.MaterialInformation info)
		{
			var ret = new List<ValueStatus>();

			foreach (var uniform in info.Uniforms)
			{
				var key = CreateKey(uniform);
				if (keyToValues.ContainsKey(key))
				{
					ret.Add(keyToValues[key] as ValueStatus);
				}
				else
				{
					ret.Add(null);
				}
			}

			return ret;
		}

		public string CreateKey<T>(string name)
		{
			if(typeof(T) == typeof(Value.Float))
			{
				return name + "__TYPE__U0";
			}

			if (typeof(T) == typeof(Value.Vector4D))
			{
				return name + "__TYPE__U3";
			}

			if (typeof(T) == typeof(Value.PathForImage))
			{
				return name + "__TYPE__T";
			}

			throw new Exception();
		}
		public string CreateKey(Utl.MaterialInformation.UniformInformation info)
		{
			return info.Name + "__TYPE__U" + info.Type;
		}

		public string CreateKey(Utl.MaterialInformation.TextureInformation info)
		{
			return info.Name + "__TYPE__T";
		}

		public class ValueStatus
		{
			public string Key = string.Empty;
			public object Value = null;
			public string Name = string.Empty;
			public string Description = string.Empty;
			public bool IsShown = false;
		}

		public event ChangedValueEventHandler OnChanged;
	}

	public class RendererCommonValues
	{
#if MATERIAL_ENABLED
		[Selector(ID = 3)]
		[Name(language = Language.Japanese, value = "マテリアル")]
		[Name(language = Language.English, value = "Material")]
		public Value.Enum<MaterialType> Material
		{
			get;
			private set;
		}
#endif

		[Selected(ID = 3, Value = 0)]
		[Name(language = Language.Japanese, value = "色/歪み画像")]
		[Description(language = Language.Japanese, value = "色/歪みを表す画像")]
		[Name(language = Language.English, value = "Texture")]
		[Description(language = Language.English, value = "Image that represents color/distortion")]
		public Value.PathForImage ColorTexture
		{
			get;
			private set;
		}

#if MATERIAL_ENABLED
		[Selected(ID = 3, Value = (int)MaterialType.File)]
		[IO(Export = true)]
		public MaterialFileParameter MaterialFile
		{
			get;
			private set;
		}
#endif

		[Name(language = Language.Japanese, value = "ブレンド")]
		[Name(language = Language.English, value = "Blend")]
		public Value.Enum<AlphaBlendType> AlphaBlend { get; private set; }

		[Name(language = Language.Japanese, value = "フィルタ")]
		[Name(language = Language.English, value = "Filter")]
		public Value.Enum<FilterType> Filter { get; private set; }

		[Name(language = Language.Japanese, value = "外側")]
		[Name(language = Language.English, value = "Wrap")]
		public Value.Enum<WrapType> Wrap { get; private set; }

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

		[Name(language = Language.Japanese, value = "歪み")]
		[Name(language = Language.English, value = "Distortion")]
		public Value.Boolean Distortion { get; private set; }

		[Name(language = Language.Japanese, value = "歪み強度")]
		[Name(language = Language.English, value = "Distortion\nIntensity")]
		public Value.Float DistortionIntensity { get; private set; }

		internal RendererCommonValues()
		{
#if MATERIAL_ENABLED
			Material = new Value.Enum<MaterialType>(MaterialType.Default);
			MaterialFile = new MaterialFileParameter();
#endif
			ColorTexture = new Value.PathForImage(Resources.GetString("ImageFilter"), true, "");
			
			AlphaBlend = new Value.Enum<AlphaBlendType>(AlphaBlendType.Blend);
			Filter = new Value.Enum<FilterType>(FilterType.Linear);
			Wrap = new Value.Enum<WrapType>(WrapType.Repeat);

			FadeInType = new Value.Enum<FadeType>(FadeType.None);
			FadeInNone = new NoneParamater();
			FadeIn = new FadeInParamater();

			FadeOutType = new Value.Enum<FadeType>();
			FadeOutNone = new NoneParamater();
			FadeOut = new FadeOutParamater();

			UV = new Value.Enum<UVType>();

			UVDefault = new UVDefaultParamater();
			UVFixed = new UVFixedParamater();
			UVAnimation = new UVAnimationParamater();
			UVScroll = new UVScrollParamater();
			UVFCurve = new UVFCurveParamater();

			ZWrite = new Value.Boolean(false);
			ZTest = new Value.Boolean(true);

			ColorInheritType = new Value.Enum<ParentEffectType>(ParentEffectType.NotBind);

			Distortion = new Value.Boolean(false);
			DistortionIntensity = new Value.Float(1.0f, float.MaxValue, float.MinValue, 0.1f);
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

#if MATERIAL_ENABLED
		
		public enum MaterialType : int
		{
			[Name(value = "標準", language = Language.Japanese)]
			[Name(value = "Default", language = Language.English)]
			Default = 0,
			[Name(value = "ファイル", language = Language.Japanese)]
			[Name(value = "File", language = Language.English)]
			File = 128,
		}
#endif
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
	}
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class OptionValues
	{
		[Name(language = Language.Japanese, value = "グリッド色")]
		[Description(language = Language.Japanese, value = "グリッド色")]
		[Name(language = Language.English, value = "Grid Color")]
		[Description(language = Language.English, value = "Color of the grid")]
		[Undo(Undo = false)]
		public Value.Color GridColor
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "グリッドの表示")]
		[Description(language = Language.Japanese, value = "グリッドの表示非表示")]
		[Name(language = Language.English, value = "Grid Visibility")]
		[Description(language = Language.English, value = "Toggle the visibility of the grid")]
		[Undo(Undo = false)]
		public Value.Boolean IsGridShown
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "XYグリッドの表示")]
		[Description(language = Language.Japanese, value = "XYグリッドの表示非表示")]
		[Name(language = Language.English, value = "X-Y Grid Visibility")]
		[Description(language = Language.English, value = "Toggle the visibility of the grid along the X-Y axes")]
		[Undo(Undo = false)]
		public Value.Boolean IsXYGridShown
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "XZグリッドの表示")]
		[Description(language = Language.Japanese, value = "XZグリッドの表示非表示")]
		[Name(language = Language.English, value = "X-Z Grid Visibility")]
		[Description(language = Language.English, value = "Toggle the visibility of the grid along the X-Z axes")]
		[Undo(Undo = false)]
		public Value.Boolean IsXZGridShown
		{
			get;
			private set;
		}


		[Name(language = Language.Japanese, value = "YZグリッドの表示")]
		[Description(language = Language.Japanese, value = "YZグリッドの表示非表示")]
		[Name(language = Language.English, value = "Y-Z Grid Visibility")]
		[Description(language = Language.English, value = "Toggle the visibility of the grid along the Y-Z axes")]
		[Undo(Undo = false)]
		public Value.Boolean IsYZGridShown
		{
			get;
			private set;
		}


		[Name(language = Language.Japanese, value = "グリッドサイズ")]
		[Description(language = Language.Japanese, value = "表示しているグリッドの幅")]
		[Name(language = Language.English, value = "Grid Size")]
		[Description(language = Language.English, value = "Dimensions of the displayed grid")]
		[Undo(Undo = false)]
		public Value.Float GridLength
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "ライト方向")]
		[Description(language = Language.Japanese, value = "ディレクショナルライトの向き")]
		[Name(language = Language.English, value = "Light Direction")]
		[Description(language = Language.English, value = "Orientation of the directional light")]
		[Undo(Undo = false)]
		public Value.Vector3D LightDirection
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "ライト色")]
		[Description(language = Language.Japanese, value = "ライトのディフュージョン色")]
		[Name(language = Language.English, value = "Light Color")]
		[Description(language = Language.English, value = "Diffuse color of the light")]
		[Undo(Undo = false)]
		public Value.Color LightColor
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "アンビエント色")]
		[Description(language = Language.Japanese, value = "ライトのアンビエント色")]
		[Name(language = Language.English, value = "Ambient Color")]
		[Description(language = Language.English, value = "Ambient color of the light")]
		[Undo(Undo = false)]
		public Value.Color LightAmbientColor
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "出力時の拡大率")]
		[Description(language = Language.Japanese, value = "出力時の拡大率")]
		[Name(language = Language.English, value = "Output Magnification")]
		[Description(language = Language.English, value = "Output magnification")]
		[Undo(Undo = false)]
		public Value.Float Magnification
		{
			get;
			private set;
		}


		[Name(language = Language.Japanese, value = "出力FPS")]
		[Description(language = Language.Japanese, value = "出力FPS")]
		[Name(language = Language.English, value = "Output FPS")]
		[Description(language = Language.English, value = "Output FPS")]
		[Undo(Undo = false)]
		public Value.Enum<FPSType> FPS
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "座標系")]
		[Description(language = Language.Japanese, value = "座標系")]
		[Name(language = Language.English, value = "Coordinate System")]
		[Description(language = Language.English, value = "Coordinate system to use")]
		[Undo(Undo = false)]
		public Value.Enum<CoordinateType> Coordinate
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "背景色")]
		[Description(language = Language.Japanese, value = "背景色")]
		[Name(language = Language.English, value = "Background Color")]
		[Description(language = Language.English, value = "Background color")]
		[Undo(Undo = false)]
		public Value.Color BackgroundColor
		{
			get;
			private set;
		}


        // コンストラクタで初期化時、使用言語がまだ決められないので遅延初期化にする
        Lazy<Value.PathForImage> LasyBackgroundImage;

		[Name(language = Language.Japanese, value = "背景画像")]
		[Description(language = Language.Japanese, value = "背景画像")]
		[Name(language = Language.English, value = "Background Image")]
		[Description(language = Language.English, value = "Background image")]
		[Undo(Undo = false)]
		public Value.PathForImage BackgroundImage
		{
            get
            {
                return LasyBackgroundImage.Value;
            }
		}

		[Name(language = Language.Japanese, value = "カラースペース")]
		[Description(language = Language.Japanese, value = "カラースペース(再起動後に有効になります。)")]
		[Name(language = Language.English, value = "Color Space")]
		[Description(language = Language.English, value = "Color Space")]
		[Undo(Undo = false)]
		public Value.Enum<ColorSpaceType> ColorSpace
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "マウスの回転方向(X)")]
		[Description(language = Language.Japanese, value = "マウスの回転方向を逆にする。")]
		[Name(language = Language.English, value = "Mouse Rotation (X)")]
		[Description(language = Language.English, value = "Invert the rotation about the X-axis")]
		[Undo(Undo = false)]
		public Value.Boolean MouseRotInvX
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "マウスの回転方向(Y)")]
		[Description(language = Language.Japanese, value = "マウスの回転方向を逆にする。")]
		[Name(language = Language.English, value = "Mouse Rotation (Y)")]
		[Description(language = Language.English, value = "Invert the rotation about the Y-axis")]
		[Undo(Undo = false)]
		public Value.Boolean MouseRotInvY
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "マウスのスライド方向(X)")]
		[Description(language = Language.Japanese, value = "マウスのスライド方向を逆にする。")]
		[Name(language = Language.English, value = "Mouse Panning (X)")]
		[Description(language = Language.English, value = "Invert the pan direction about the X-axis")]
		[Undo(Undo = false)]
		public Value.Boolean MouseSlideInvX
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "マウスのスライド方向(Y)")]
		[Description(language = Language.Japanese, value = "マウスのスライド方向を逆にする。")]
		[Name(language = Language.English, value = "Mouse Panning (Y)")]
		[Description(language = Language.English, value = "Invert the pan direction about the Y-axis")]
		[Undo(Undo = false)]
		public Value.Boolean MouseSlideInvY
		{
			get;
			private set;
		}

        [Name(language = Language.Japanese, value = "言語設定")]
        [Description(language = Language.Japanese, value = "言語設定")]
        [Name(language = Language.English, value = "Language")]
        [Description(language = Language.English, value = "Langueage")]
        [Undo(Undo = false)]
        public Value.Enum<Language> GuiLanguage
        {
            get;
            private set;
        }
        
        public OptionValues()
		{
			BackgroundColor = new Value.Color(0, 0, 0, 255);
            LasyBackgroundImage = new Lazy<Value.PathForImage>(() => { return new Value.PathForImage(Properties.Resources.ImageFilter, false, ""); });
			GridColor = new Value.Color(255, 255, 255, 255);
			
			IsGridShown = new Value.Boolean(true);
			IsXYGridShown = new Value.Boolean(false);
			IsXZGridShown = new Value.Boolean(true);
			IsYZGridShown = new Value.Boolean(false);

			GridLength = new Value.Float(2, float.MaxValue, 0.1f);
			LightDirection = new Value.Vector3D(1, 1, 1, 1, -1, 1, -1, 1, -1, 0.1f, 0.1f, 0.1f);
			LightColor = new Value.Color(215, 215, 215, 255);
			LightAmbientColor = new Value.Color(40, 40, 40, 255);
			Magnification = new Value.Float(1, float.MaxValue, 0.00001f);
			FPS = new Value.Enum<FPSType>(FPSType._60FPS);
			Coordinate = new Value.Enum<CoordinateType>(CoordinateType.Right);

			ColorSpace = new Value.Enum<ColorSpaceType>(ColorSpaceType.GammaSpace);

			MouseRotInvX = new Value.Boolean(false);
			MouseRotInvY = new Value.Boolean(false);
			MouseSlideInvX = new Value.Boolean(false);
			MouseSlideInvY = new Value.Boolean(false);

            // Switch the language according to the OS settings
            var culture = System.Globalization.CultureInfo.CurrentCulture;
            if (culture.Name == "ja-JP")
            {
                GuiLanguage = new Value.Enum<Language>(Language.Japanese);
            }
            else
            {
                GuiLanguage = new Value.Enum<Language>(Language.English);
            }
		}

		public enum FPSType : int
		{
			[Name(value = "60FPS", language = Language.Japanese)]
			[Name(value = "60 FPS", language = Language.English)]
			_60FPS = 1,
			[Name(value = "30FPS", language = Language.Japanese)]
			[Name(value = "30 FPS", language = Language.English)]
			_30FPS = 2,
			[Name(value = "20FPS", language = Language.Japanese)]
			[Name(value = "20 FPS", language = Language.English)]
			_20FPS = 3,
			[Name(value = "15FPS", language = Language.Japanese)]
			[Name(value = "15 FPS", language = Language.English)]
			_15FPS = 4,
		}

		public enum CoordinateType : int
		{
			[Name(value = "右手系", language = Language.Japanese)]
			[Name(value = "Right-Handed", language = Language.English)]
			Right = 0,
			[Name(value = "左手系", language = Language.Japanese)]
			[Name(value = "Left-Handed", language = Language.English)]
			Left = 1,
		}

		public enum ColorSpaceType : int
		{
			[Name(value = "ガンマスペース", language = Language.Japanese)]
			[Name(value = "GammaSpace", language = Language.English)]
			GammaSpace = 0,
			[Name(value = "リニアスペース", language = Language.Japanese)]
			[Name(value = "LinearSpace", language = Language.English)]
			LinearSpace = 1,
		}
	}
}

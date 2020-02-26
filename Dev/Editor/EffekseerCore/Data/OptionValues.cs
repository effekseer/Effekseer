using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum FontType
	{
		[Name(value = "普通", language = Language.Japanese)]
		[Name(value = "Normal", language = Language.English)]
		Normal,

		[Name(value = "太い", language = Language.Japanese)]
		[Name(value = "Bold", language = Language.English)]
		Bold,
	}
	public class OptionValues
	{
		[Name(language = Language.Japanese, value = "描画モード")]
		[Description(language = Language.Japanese, value = "ビュワーのエフェクト描画モードの設定")]
		[Name(language = Language.English, value = "Render Mode")]
		[Description(language = Language.English, value = "Render Mode of effects")]
		[Undo(Undo = false)]
		public Value.Enum<RenderMode> RenderingMode
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "表示モード")]
		[Description(language = Language.Japanese, value = "ビュワーの表示モードの設定")]
		[Name(language = Language.English, value = "View Mode")]
		[Description(language = Language.English, value = "View Mode of effect viewer.")]
		[Undo(Undo = false)]
		public Value.Enum<ViewMode> ViewerMode
		{
			get;
			private set;
		}

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

		[Name(language = Language.Japanese, value = "出力時の拡大率")]
		[Description(language = Language.Japanese, value = "出力時の拡大率")]
		[Name(language = Language.English, value = "Output Magnification")]
		[Description(language = Language.English, value = "Output magnification")]
		[Undo(Undo = false)]
		[Shown(Shown = false)]
		public Value.Float ExternalMagnification
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

		[Name(language = Language.Japanese, value = "フォント")]
		[Description(language = Language.Japanese, value = "フォント")]
		[Name(language = Language.English, value = "Font")]
		[Description(language = Language.English, value = "Font")]
		[Undo(Undo = false)]
		public Value.Enum<FontType> Font
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "フォントサイズ")]
		[Description(language = Language.Japanese, value = "フォントサイズ")]
		[Name(language = Language.English, value = "Font size")]
		[Description(language = Language.English, value = "Font size")]
		[Undo(Undo = false)]
		public Value.Int FontSize
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

		[Name(language = Language.Japanese, value = "歪み方法")]
		[Description(language = Language.Japanese, value = "歪み方法")]
		[Name(language = Language.English, value = "Distortion method")]
		[Description(language = Language.English, value = "Distortion method")]
		[Undo(Undo = false)]
		public Value.Enum<DistortionMethodType> DistortionType
		{
			get;
			private set;
		}

		public OptionValues()
		{
			RenderingMode = new Value.Enum<RenderMode>(RenderMode.Normal);
			ViewerMode = new Value.Enum<ViewMode>(ViewMode._3D);
			GridColor = new Value.Color(255, 255, 255, 255);
			
			IsGridShown = new Value.Boolean(true);
			IsXYGridShown = new Value.Boolean(false);
			IsXZGridShown = new Value.Boolean(true);
			IsYZGridShown = new Value.Boolean(false);

			GridLength = new Value.Float(2, float.MaxValue, 0.1f);

			Magnification = new Value.Float(1, float.MaxValue, 0.00001f);
			ExternalMagnification = new Value.Float(1, float.MaxValue, 0.00001f);
			FPS = new Value.Enum<FPSType>(FPSType._60FPS);
			Coordinate = new Value.Enum<CoordinateType>(CoordinateType.Right);

			ColorSpace = new Value.Enum<ColorSpaceType>(ColorSpaceType.GammaSpace);

			MouseRotInvX = new Value.Boolean(false);
			MouseRotInvY = new Value.Boolean(false);
			MouseSlideInvX = new Value.Boolean(false);
			MouseSlideInvY = new Value.Boolean(false);

			DistortionType = new Value.Enum<DistortionMethodType>(DistortionMethodType.Current);

			Font = new Value.Enum<FontType>(FontType.Normal);
			FontSize = new Value.Int(16, 32, 8);

			// Switch the language according to the OS settings
			GuiLanguage = new Value.Enum<Language>(LanguageGetter.GetLanguage());
		}
		
		public enum RenderMode : int
		{
			[Name(value = "通常モード", language = Language.Japanese)]
			[Name(value = "Normal", language = Language.English)]
			Normal = 0,
			[Name(value = "ワイヤーフレーム", language = Language.Japanese)]
			[Name(value = "Wireframe", language = Language.English)]
			Wireframe = 1,
		}

		public enum ViewMode : int
		{
			[Name(value = "3D", language = Language.Japanese)]
			[Name(value = "3D", language = Language.English)]
			_3D = 0,
			[Name(value = "2D", language = Language.Japanese)]
			[Name(value = "2D", language = Language.English)]
			_2D = 1,
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

		public enum DistortionMethodType : int
		{
			[Name(value = "現行", language = Language.Japanese)]
			[Name(value = "Current", language = Language.English)]
			Current = 0,
			[Name(value = "1.20互換", language = Language.Japanese)]
			[Name(value = "1.20 Compatible", language = Language.English)]
			Effekseer120 = 1,
			[Name(value = "無効", language = Language.Japanese)]
			[Name(value = "Disabled", language = Language.English)]
			Disabled = 2,
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

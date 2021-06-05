using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using Effekseer.Utl;

namespace Effekseer.Data
{
	public class LanguageSelector
	{
		static LanguageSelector()
		{
			IO.ExtendSupportedType(typeof(LanguageSelector), SaveToElement, LoadFromElement);
		}

		public static XmlElement SaveToElement(XmlDocument doc, string element_name, object o, bool isClip)
		{
			var text = LanguageTable.Languages[LanguageTable.SelectedIndex];
			return doc.CreateTextElement(element_name, text);
		}

		public static void LoadFromElement(XmlElement e, object o, bool isClip)
		{
			var text = e.GetText();
			LanguageTable.SelectLanguage(text);
		}
	}

	public enum MouseMappingType
	{
		[Key(key = "MouseMappingType_Effekseer")]
		Effekseer = 0,
		[Key(key = "MouseMappingType_Unity")]
		Unity = 1,
		[Key(key = "MouseMappingType_Blender")]
		Blender = 2,
		[Key(key = "MouseMappingType_Maya")]
		Maya = 3,
	}

	public enum FontType
	{
		[Key(key ="FontType_Normal")]
		Normal,

		[Key(key = "FontType_Bold")]
		Bold,
	}
	public class OptionValues
	{
		[Key(key = "Options_RenderingMode")]
		[Undo(Undo = false)]
		public Value.Enum<RenderMode> RenderingMode
		{
			get;
			private set;
		}

		[Key(key = "Options_ViewerMode")]
		[Undo(Undo = false)]
		public Value.Enum<ViewMode> ViewerMode
		{
			get;
			private set;
		}

		[Key(key = "Options_GridColor")]
		[Undo(Undo = false)]
		public Value.Color GridColor
		{
			get;
			private set;
		}

		[Key(key = "Options_IsGridShown")]
		[Undo(Undo = false)]
		public Value.Boolean IsGridShown
		{
			get;
			private set;
		}

		[Key(key = "Options_IsXYGridShown")]
		[Undo(Undo = false)]
		public Value.Boolean IsXYGridShown
		{
			get;
			private set;
		}

		[Key(key = "Options_IsXZGridShown")]
		[Undo(Undo = false)]
		public Value.Boolean IsXZGridShown
		{
			get;
			private set;
		}

		[Key(key = "Options_IsYZGridShown")]
		[Undo(Undo = false)]
		public Value.Boolean IsYZGridShown
		{
			get;
			private set;
		}


		[Key(key = "Options_GridLength")]
		[Undo(Undo = false)]
		public Value.Float GridLength
		{
			get;
			private set;
		}

		[Key(key = "Options_Magnification")]
		[Undo(Undo = false)]
		public Value.Float Magnification
		{
			get;
			private set;
		}

		[Undo(Undo = false)]
		[Shown(Shown = false)]
		public Value.Float ExternalMagnification
		{
			get;
			private set;
		}


		[Key(key = "Options_FPS")]
		[Undo(Undo = false)]
		public Value.Enum<FPSType> FPS
		{
			get;
			private set;
		}

		[Key(key = "Options_Coordinate")]
		[Undo(Undo = false)]
		public Value.Enum<CoordinateType> Coordinate
		{
			get;
			private set;
		}

		[Key(key = "Options_ColorSpace")]
		[Undo(Undo = false)]
		public Value.Enum<ColorSpaceType> ColorSpace
		{
			get;
			private set;
		}

		[Key(key = "Options_Font")]
		[Undo(Undo = false)]
		public Value.Enum<FontType> Font
		{
			get;
			private set;
		}

		[Key(key = "Options_FontSize")]
		[Undo(Undo = false)]
		public Value.Int FontSize
		{
			get;
			private set;
		}

		[Key(key = "Options_MouseMappingType")]
		[Undo(Undo = false)]
		public Value.Enum<MouseMappingType> MouseMappingType
		{
			get;
			private set;
		}

		[Key(key = "Options_MouseRotInvX")]
		[Undo(Undo = false)]
		public Value.Boolean MouseRotInvX
		{
			get;
			private set;
		}

		[Key(key = "Options_MouseRotInvY")]
		[Undo(Undo = false)]
		public Value.Boolean MouseRotInvY
		{
			get;
			private set;
		}

		[Key(key = "Options_MouseSlideInvX")]
		[Undo(Undo = false)]
		public Value.Boolean MouseSlideInvX
		{
			get;
			private set;
		}

		[Key(key = "Options_MouseSlideInvY")]
		[Undo(Undo = false)]
		public Value.Boolean MouseSlideInvY
		{
			get;
			private set;
		}

		[Key(key = "Options_LanguageSelector")]
		[Undo(Undo = false)]
		public LanguageSelector LanguageSelector
		{
			get;
			private set;
		}

		[Key(key = "Options_DistortionType")]
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

			MouseMappingType = new Value.Enum<MouseMappingType>(Data.MouseMappingType.Effekseer);

			LanguageSelector = new LanguageSelector();
		}
		
		public enum RenderMode : int
		{
			[Key(key = "RenderMode_Normal")]
			Normal = 0,
			[Key(key = "RenderMode_Wireframe")]
			Wireframe = 1,
			[Key(key = "RenderMode_NormalWithWireframe")]
			NormalWithWireframe = 2,
			[Key(key = "RenderMode_Overdraw")]
			Overdraw = 3,

		}

		public enum ViewMode : int
		{
			[Key(key = "ViewMode_3D")]
			_3D = 0,
			[Key(key = "ViewMode_2D")]
			_2D = 1,
		}

		public enum FPSType : int
		{
			[Key(key = "FPSType_60FPS")]
			_60FPS = 1,
			[Key(key = "FPSType_30FPS")]
			_30FPS = 2,
			[Key(key = "FPSType_20FPS")]
			_20FPS = 3,
			[Key(key = "FPSType_15FPS")]
			_15FPS = 4,
		}

		public enum CoordinateType : int
		{
			[Key(key = "CoordinateType_Right")]
			Right = 0,
			[Key(key = "CoordinateType_Left")]
			Left = 1,
		}

		public enum DistortionMethodType : int
		{
			[Key(key = "DistortionMethodType_Current")]
			Current = 0,
			[Key(key = "DistortionMethodType_Effekseer120")]
			Effekseer120 = 1,
			[Key(key = "DistortionMethodType_Disabled")]
			Disabled = 2,
		}


		public enum ColorSpaceType : int
		{
			[Key(key = "ColorSpaceType_GammaSpace")]
			GammaSpace = 0,
			[Key(key = "ColorSpaceType_LinearSpace")]
			LinearSpace = 1,
		}
	}
}

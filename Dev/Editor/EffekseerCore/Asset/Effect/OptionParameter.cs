namespace Effekseer.Asset.Effect
{
	public partial class OptionParameter
	{
		public enum RenderMode
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
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class OptionParameter
	{
		public enum ViewMode
		{
			[Key(key = "ViewMode_3D")]
			_3D = 0,
			[Key(key = "ViewMode_2D")]
			_2D = 1,
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class OptionParameter
	{
		public enum FPSType
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
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class OptionParameter
	{
		public enum CoordinateType
		{
			[Key(key = "CoordinateType_Right")]
			Right = 0,
			[Key(key = "CoordinateType_Left")]
			Left = 1,
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class OptionParameter
	{
		public enum ColorSpaceType
		{
			[Key(key = "ColorSpaceType_GammaSpace")]
			GammaSpace = 0,
			[Key(key = "ColorSpaceType_LinearSpace")]
			LinearSpace = 1,
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class OptionParameter
	{
		public enum DistortionMethodType
		{
			[Key(key = "DistortionMethodType_Current")]
			Current = 0,
			[Key(key = "DistortionMethodType_Effekseer120")]
			Effekseer120 = 1,
			[Key(key = "DistortionMethodType_Disabled")]
			Disabled = 2,
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class OptionParameter
	{
		public enum FileViewMode
		{
			[Key(key = "FileBrowser_ViewMode_IconView")]
			IconView = 0,
			[Key(key = "FileBrowser_ViewMode_ListView")]
			ListView = 1,
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class OptionParameter
	{
		[Key(key = "Options_RenderingMode")]
		public Effekseer.Asset.Effect.OptionParameter.RenderMode RenderingMode = Effekseer.Asset.Effect.OptionParameter.RenderMode.Normal;

		[Key(key = "Options_ViewerMode")]
		public Effekseer.Asset.Effect.OptionParameter.ViewMode ViewerMode = Effekseer.Asset.Effect.OptionParameter.ViewMode._3D;

		[Key(key = "Options_GridColor")]
		public Effekseer.Asset.Color GridColor = new Effekseer.Asset.Color();

		[Key(key = "Options_IsGridShown")]
		public bool IsGridShown = true;

		[Key(key = "Options_IsXYGridShown")]
		public bool IsXYGridShown = false;

		[Key(key = "Options_IsXZGridShown")]
		public bool IsXZGridShown = true;

		[Key(key = "Options_IsYZGridShown")]
		public bool IsYZGridShown = false;

		[Key(key = "Options_GridLength")]
		public float GridLength = 2;

		[Key(key = "Options_Magnification")]
		public float Magnification = 1;

		public float ExternalMagnification = 1;

		[Key(key = "Options_FPS")]
		public Effekseer.Asset.Effect.OptionParameter.FPSType FPS = Effekseer.Asset.Effect.OptionParameter.FPSType._60FPS;

		[Key(key = "Options_Coordinate")]
		public Effekseer.Asset.Effect.OptionParameter.CoordinateType Coordinate = Effekseer.Asset.Effect.OptionParameter.CoordinateType.Right;

		[Key(key = "Options_ColorSpace")]
		public Effekseer.Asset.Effect.OptionParameter.ColorSpaceType ColorSpace = Effekseer.Asset.Effect.OptionParameter.ColorSpaceType.GammaSpace;

		[Key(key = "Options_Font")]
		public Effekseer.Asset.FontType Font = Effekseer.Asset.FontType.Normal;

		[Key(key = "Options_FontSize")]
		public int FontSize = 16;

		[Key(key = "Options_MouseMappingType")]
		public Effekseer.Asset.MouseMappingType MouseMappingType = Effekseer.Asset.MouseMappingType.Effekseer;

		[Key(key = "Options_MouseRotInvX")]
		public bool MouseRotInvX = false;

		[Key(key = "Options_MouseRotInvY")]
		public bool MouseRotInvY = false;

		[Key(key = "Options_MouseSlideInvX")]
		public bool MouseSlideInvX = false;

		[Key(key = "Options_MouseSlideInvY")]
		public bool MouseSlideInvY = false;

		[Key(key = "Options_LanguageSelector")]
		public Effekseer.Asset.LanguageSelector LanguageSelector = new LanguageSelector();


		[Key(key = "Options_DistortionType")]
		public Effekseer.Asset.Effect.OptionParameter.DistortionMethodType DistortionType = Effekseer.Asset.Effect.OptionParameter.DistortionMethodType.Current;

		[Key(key = "Options_FileBrowserViewMode")]
		public Effekseer.Asset.Effect.OptionParameter.FileViewMode FileBrowserViewMode = Effekseer.Asset.Effect.OptionParameter.FileViewMode.IconView;

		[Key(key = "Options_FileBrowserIconSize")]
		public int FileBrowserIconSize = 96;

		[Key(key = "Options_FloatFormatDigits")]
		public int FloatFormatDigits = 3;

		[Key(key = "Options_AutoSaveIntervalMin")]
		public int AutoSaveIntervalMin = 2;
	}
}


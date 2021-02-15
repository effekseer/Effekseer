using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.swig;
using Effekseer.Utl;

namespace Effekseer.GUI
{

	public class Manager : GUIManager
	{




		public static bool Initialize(int width, int height, swig.DeviceType deviceType)
		{
			dockTypes = new Type[]
			{
				typeof(Dock.ViewerController),
				typeof(Dock.NodeTreeView),
				typeof(Dock.CommonValues),
				typeof(Dock.LocationValues),
				typeof(Dock.LocationAbsValues),
				typeof(Dock.GenerationLocationValues),
				typeof(Dock.RotationValues),
				typeof(Dock.ScaleValues),
				typeof(Dock.DepthValues),
				typeof(Dock.RendererCommonValues),
				typeof(Dock.RendererValues),
				typeof(Dock.SoundValues),
				typeof(Dock.FCurves),
				typeof(Dock.ViewPoint),
				typeof(Dock.Recorder),
				typeof(Dock.Option),
				typeof(Dock.Environement),
				typeof(Dock.GlobalValues),
				typeof(Dock.BehaviorValues),
				typeof(Dock.Culling),
				typeof(Dock.Network),
				typeof(Dock.FileViewer),
				typeof(Dock.Dynamic),
				typeof(Dock.ProcedualModel),
				typeof(Dock.AdvancedRenderCommonValues),
			};

			if (!InitializeBase(width, height, deviceType, () => new GUI.Menu.MainMenu())) return false;

			return true;
		}


	}
}

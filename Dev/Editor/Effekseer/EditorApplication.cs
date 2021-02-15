using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer
{
	public class App : Application
	{
		protected override void OnInitialize()
		{
			System.OperatingSystem os = System.Environment.OSVersion;
			swig.DeviceType deviceType = swig.DeviceType.DirectX11;

			if (!(os.Platform == PlatformID.Win32NT ||
			os.Platform == PlatformID.Win32S ||
			os.Platform == PlatformID.Win32Windows ||
			os.Platform == PlatformID.WinCE))
			{
				deviceType = swig.DeviceType.OpenGL;
			}

			if (!GUI.Manager.Initialize(960, 540, deviceType))
			{
				return;
			}
		}

		protected override void OnUpdate()
		{
			GUI.Manager.Update();
		}

		protected override void OnTerminate()
		{
			GUI.Manager.Terminate();
			Process.MaterialEditor.Terminate();
		}
	}
}

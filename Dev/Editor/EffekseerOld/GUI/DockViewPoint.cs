using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Effekseer.GUI
{
	public partial class DockViewPoint : DockContent
	{
		public DockViewPoint()
		{
			InitializeComponent();
			
			txt_focusX.ReadMethod = () =>
				{
					if(GUIManager.DockViewer.Viewer != null) return GUIManager.DockViewer.Viewer.GetViewerParamater().FocusX;
					return 0.0f;
				};
			txt_focusX.WriteMethod = (value,wheel) =>
				{
					if (GUIManager.DockViewer.Viewer != null)
					{
						var param = GUIManager.DockViewer.Viewer.GetViewerParamater();
						param.FocusX = value;
						GUIManager.DockViewer.Viewer.SetViewerParamater(param);
					}
				};

			txt_focusY.ReadMethod = () =>
			{
				if (GUIManager.DockViewer.Viewer != null) return GUIManager.DockViewer.Viewer.GetViewerParamater().FocusY;
				return 0.0f;
			};
			txt_focusY.WriteMethod = (value, wheel) =>
			{
				if (GUIManager.DockViewer.Viewer != null)
				{
					var param = GUIManager.DockViewer.Viewer.GetViewerParamater();
					param.FocusY = value;
					GUIManager.DockViewer.Viewer.SetViewerParamater(param);
				}
			};

			txt_focusZ.ReadMethod = () =>
			{
				if (GUIManager.DockViewer.Viewer != null) return GUIManager.DockViewer.Viewer.GetViewerParamater().FocusZ;
				return 0.0f;
			};
			txt_focusZ.WriteMethod = (value, wheel) =>
			{
				if (GUIManager.DockViewer.Viewer != null)
				{
					var param = GUIManager.DockViewer.Viewer.GetViewerParamater();
					param.FocusZ = value;
					GUIManager.DockViewer.Viewer.SetViewerParamater(param);
				}
			};

			txt_angleX.ReadMethod = () =>
			{
				if (GUIManager.DockViewer.Viewer != null) return GUIManager.DockViewer.Viewer.GetViewerParamater().AngleX;
				return 0.0f;
			};
			txt_angleX.WriteMethod = (value, wheel) =>
			{
				if (GUIManager.DockViewer.Viewer != null)
				{
					var param = GUIManager.DockViewer.Viewer.GetViewerParamater();
					param.AngleX = value;
					GUIManager.DockViewer.Viewer.SetViewerParamater(param);
				}
			};

			txt_angleY.ReadMethod = () =>
			{
				if (GUIManager.DockViewer.Viewer != null) return GUIManager.DockViewer.Viewer.GetViewerParamater().AngleY;
				return 0.0f;
			};
			txt_angleY.WriteMethod = (value, wheel) =>
			{
				if (GUIManager.DockViewer.Viewer != null)
				{
					var param = GUIManager.DockViewer.Viewer.GetViewerParamater();
					param.AngleY = value;
					GUIManager.DockViewer.Viewer.SetViewerParamater(param);
				}
			};

			txt_distance.ReadMethod = () =>
			{
				if (GUIManager.DockViewer.Viewer != null) return GUIManager.DockViewer.Viewer.GetViewerParamater().Distance;
				return 0.0f;
			};
			txt_distance.WriteMethod = (value, wheel) =>
			{
				if (GUIManager.DockViewer.Viewer != null)
				{
					var param = GUIManager.DockViewer.Viewer.GetViewerParamater();
					param.Distance = value;
					GUIManager.DockViewer.Viewer.SetViewerParamater(param);
				}
			};

			txt_mag.ReadMethod = () =>
			{
				if (GUIManager.DockViewer.Viewer != null) return GUIManager.DockViewer.Viewer.GetViewerParamater().RateOfMagnification;
				return 0.0f;
			};
			txt_mag.WriteMethod = (value, wheel) =>
			{
				if (GUIManager.DockViewer.Viewer != null)
				{
					var param = GUIManager.DockViewer.Viewer.GetViewerParamater();
					param.RateOfMagnification = value;
					GUIManager.DockViewer.Viewer.SetViewerParamater(param);
				}
			};
			
			Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconViewPoint).GetHicon());
		}

		bool nowReloading = false;

		public void UpdateViewPoint()
		{
			nowReloading = true;

			if(!txt_focusX.Changed) txt_focusX.Reload();
			if (!txt_focusY.Changed) txt_focusY.Reload();
			if (!txt_focusZ.Changed) txt_focusZ.Reload();

			if (!txt_angleX.Changed) txt_angleX.Reload();
			if (!txt_angleY.Changed) txt_angleY.Reload();
			if (!txt_distance.Changed) txt_distance.Reload();

			if (!txt_mag.Changed) txt_mag.Reload();

			if (GUIManager.DockViewer.Viewer != null)
			{
				var param = GUIManager.DockViewer.Viewer.GetViewerParamater();

				if (!cb_type.DroppedDown)
				{
					if (param.IsPerspective) cb_type.SelectedIndex = 0;
					if (param.IsOrthographic) cb_type.SelectedIndex = 1;
				}
			}

			nowReloading = false;
		}

		private void cb_type_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (nowReloading) return;
			var param = GUIManager.DockViewer.Viewer.GetViewerParamater();
			param.IsPerspective = false;
			param.IsOrthographic = false;
			if (cb_type.SelectedIndex == 0) param.IsPerspective = true;
			if (cb_type.SelectedIndex == 1) param.IsOrthographic = true;
			GUIManager.DockViewer.Viewer.SetViewerParamater(param);
		}
	}
}

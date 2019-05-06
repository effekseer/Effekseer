using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class ViewPoint : DockPanel
	{
		bool isFiestUpdate = true;

		string id_f = string.Empty;
		string id_rx = string.Empty;
		string id_ry = string.Empty;
		string id_d = string.Empty;
		string id_s = string.Empty;
		string id_t = string.Empty;
        string id_cs = string.Empty;
        string id_ce = string.Empty;

        string[] viewTypes = new string[]
			{
			"Perspective",
            "Orthographic"
			};

		public ViewPoint()
		{
			Label = Resources.GetString("CameraSettings") + "###CameraSettings";

			id_f = "###" + Manager.GetUniqueID().ToString();
			id_rx = "###" + Manager.GetUniqueID().ToString();
			id_ry = "###" + Manager.GetUniqueID().ToString();
			id_d = "###" + Manager.GetUniqueID().ToString();
			id_s = "###" + Manager.GetUniqueID().ToString();
			id_t = "###" + Manager.GetUniqueID().ToString();
            id_cs = "###" + Manager.GetUniqueID().ToString();
            id_ce = "###" + Manager.GetUniqueID().ToString();

            Icon = Images.GetIcon("PanelViewPoint");
			IconSize = new swig.Vec2(24, 24);
			TabToolTip = Resources.GetString("CameraSettings");
		}


		public override void OnDisposed()
		{
		}

		protected override void UpdateInternal()
		{
			if (isFiestUpdate)
			{
			}

			var viewerParameter = Manager.Viewer.GetViewerParamater();

			var fx = viewerParameter.FocusX;
			var fy = viewerParameter.FocusY;
			var fz = viewerParameter.FocusZ;
			var f = new float[] { fx, fy, fz };

			var rx = viewerParameter.AngleX;
			var ry = viewerParameter.AngleY;

			var rx_ = new float[] { rx };
			var ry_ = new float[] { ry };

			var d = new float[] { viewerParameter.Distance };
			var s = new float[] { viewerParameter.RateOfMagnification };

            var cs = new float[] { viewerParameter.ClippingStart };
            var ce = new float[] { viewerParameter.ClippingEnd };

            if (Manager.NativeManager.DragFloat3(Resources.GetString("Viewpoint") + id_f, f))
			{
				viewerParameter.FocusX = f[0];
				viewerParameter.FocusY = f[1];
				viewerParameter.FocusZ = f[2];
			}

			if (Manager.NativeManager.DragFloat(Resources.GetString("XRotation") + id_rx, rx_))
			{
				viewerParameter.AngleX = rx_[0];
			}

			if (Manager.NativeManager.DragFloat(Resources.GetString("YRotation") + id_ry, ry_))
			{
				viewerParameter.AngleY = ry_[0];
			}

			if (Manager.NativeManager.DragFloat(Resources.GetString("PoVDistance") + id_d, d))
			{
				viewerParameter.Distance = d[0];
			}

			if (Manager.NativeManager.DragFloat(Resources.GetString("Zoom") + id_s, s))
			{
				viewerParameter.RateOfMagnification = s[0];
			}

            if (Manager.NativeManager.DragFloat(Resources.GetString("Clipping") + "\n" + Resources.GetString("Start") + id_cs, cs))
            {
                viewerParameter.ClippingStart = cs[0];
            }

            if (Manager.NativeManager.DragFloat(Resources.GetString("Clipping") + "\n" + Resources.GetString("End") + id_ce, ce))
            {
                viewerParameter.ClippingEnd = ce[0];
            }

            if (Manager.NativeManager.BeginCombo(Resources.GetString("CameraMode") + id_t, viewTypes[viewerParameter.IsPerspective ? 0 : 1], swig.ComboFlags.None))
			{
				if(Manager.NativeManager.Selectable(viewTypes[0]))
				{
					viewerParameter.IsPerspective = true;
					viewerParameter.IsOrthographic = false;
					Manager.NativeManager.SetItemDefaultFocus();
				}

				if (Manager.NativeManager.Selectable(viewTypes[1]))
				{
					viewerParameter.IsOrthographic = true;
					viewerParameter.IsPerspective = false;
					Manager.NativeManager.SetItemDefaultFocus();
				}

				Manager.NativeManager.EndCombo();
			}

            if (Manager.NativeManager.Button(Resources.GetString("Save") + "###btn1"))
            {
                var filter = "view";

                var result = swig.FileDialog.SaveDialog(filter, System.IO.Directory.GetCurrentDirectory());

                if (!string.IsNullOrEmpty(result))
                {
                    var filename = result;

                    if (System.IO.Path.GetExtension(filename) != "." + filter)
                    {
                        filename += "." + filter;
                    }

                    Data.ViewPoint viewPoint = new Data.ViewPoint();
                    viewPoint.FocusX = viewerParameter.FocusX;
                    viewPoint.FocusY = viewerParameter.FocusY;
                    viewPoint.FocusZ = viewerParameter.FocusZ;
                    viewPoint.Distance = viewerParameter.Distance;
                    viewPoint.AngleX = viewerParameter.AngleX;
                    viewPoint.AngleY = viewerParameter.AngleY;
                    viewPoint.ClippingStart = viewerParameter.ClippingStart;
                    viewPoint.ClippingEnd = viewerParameter.ClippingEnd;
                    viewPoint.CameraMode = viewerParameter.IsPerspective ? 0 : 1;
					viewPoint.RateOfMagnification = viewerParameter.RateOfMagnification;
                    viewPoint.Save(filename);
                }
            }

            if (Manager.NativeManager.Button(Resources.GetString("Load") + "###btn2"))
            {
                var filter = "view";

                var result = swig.FileDialog.OpenDialog(filter, System.IO.Directory.GetCurrentDirectory());

                if (!string.IsNullOrEmpty(result))
                {
                    var viewPoint = Data.ViewPoint.Load(result);
                    if(viewPoint != null)
                    {
                        viewerParameter.FocusX = viewPoint.FocusX;
                        viewerParameter.FocusY = viewPoint.FocusY;
                        viewerParameter.FocusZ = viewPoint.FocusZ;
                        viewerParameter.Distance = viewPoint.Distance;
                        viewerParameter.AngleX = viewPoint.AngleX;
                        viewerParameter.AngleY = viewPoint.AngleY;
                        viewerParameter.ClippingStart = viewPoint.ClippingStart;
                        viewerParameter.ClippingEnd = viewPoint.ClippingEnd;
                        viewerParameter.IsPerspective = viewPoint.CameraMode == 0;
						viewerParameter.IsOrthographic = !viewerParameter.IsPerspective;
						viewerParameter.RateOfMagnification = viewPoint.RateOfMagnification;
					}
                }
            }

            Manager.Viewer.SetViewerParamater(viewerParameter);
		}
	}
}

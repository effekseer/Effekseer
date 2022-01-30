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
			Label = Icons.PanelViewPoint + Resources.GetString("CameraSettings") + "###CameraSettings";

			id_f = "###" + Manager.GetUniqueID().ToString();
			id_rx = "###" + Manager.GetUniqueID().ToString();
			id_ry = "###" + Manager.GetUniqueID().ToString();
			id_d = "###" + Manager.GetUniqueID().ToString();
			id_s = "###" + Manager.GetUniqueID().ToString();
			id_t = "###" + Manager.GetUniqueID().ToString();
            id_cs = "###" + Manager.GetUniqueID().ToString();
            id_ce = "###" + Manager.GetUniqueID().ToString();

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

			var ctrl = Manager.Viewer.ViewPointController;

			var focusPosition = ctrl.GetFocusPosition();
			var f = new float[] { focusPosition.X, focusPosition.Y, focusPosition.Z };

			var rx = ctrl.GetAngleX();
			var ry = ctrl.GetAngleY();

			var rx_ = new float[] { rx };
			var ry_ = new float[] { ry };

			var d = new float[] { ctrl.GetDistance() };
			var s = new float[] { ctrl.RateOfMagnification };

            var cs = new float[] { ctrl.ClippingStart };
            var ce = new float[] { ctrl.ClippingEnd };

            if (Manager.NativeManager.DragFloat3(Resources.GetString("Viewpoint") + id_f, f))
			{
				ctrl.SetFocusPosition(new swig.Vector3F(f[0], f[1], f[2]));
			}

			if (Manager.NativeManager.DragFloat(Resources.GetString("XRotation") + id_rx, rx_))
			{
				ctrl.SetAngleX(rx_[0]);
			}

			if (Manager.NativeManager.DragFloat(Resources.GetString("YRotation") + id_ry, ry_))
			{
				ctrl.SetAngleY(ry_[0]);
			}

			if (Manager.NativeManager.DragFloat(Resources.GetString("PoVDistance") + id_d, d))
			{
				ctrl.SetDistance(d[0]);
			}

			if (Manager.NativeManager.DragFloat(Resources.GetString("Zoom") + id_s, s))
			{
				ctrl.RateOfMagnification = s[0];
			}

			if (Manager.NativeManager.DragFloat(Resources.GetString("Clipping") + "\n" + Resources.GetString("Start") + id_cs, cs))
            {
				ctrl.ClippingStart = cs[0];
			}

			if (Manager.NativeManager.DragFloat(Resources.GetString("Clipping") + "\n" + Resources.GetString("End") + id_ce, ce))
            {
				ctrl.ClippingEnd = ce[0];
			}

			if (Manager.NativeManager.BeginCombo(Resources.GetString("CameraMode") + id_t, viewTypes[ctrl.GetProjectionType() == swig.ProjectionType.Perspective ? 0 : 1], swig.ComboFlags.None))
			{
				if(Manager.NativeManager.Selectable(viewTypes[0]))
				{
					ctrl.SetProjectionType(swig.ProjectionType.Perspective);
					Manager.NativeManager.SetItemDefaultFocus();
				}

				if (Manager.NativeManager.Selectable(viewTypes[1]))
				{
					ctrl.SetProjectionType(swig.ProjectionType.Orthographic);
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

					var focusPositionSaved = ctrl.GetFocusPosition();

					Data.ViewPoint viewPoint = new Data.ViewPoint();
                    viewPoint.FocusX = focusPositionSaved.X;
                    viewPoint.FocusY = focusPositionSaved.Y;
                    viewPoint.FocusZ = focusPositionSaved.Z;
					viewPoint.Distance = ctrl.GetDistance();
					viewPoint.AngleX = ctrl.GetAngleX();
                    viewPoint.AngleY = ctrl.GetAngleY();
                    viewPoint.ClippingStart = ctrl.ClippingStart;
                    viewPoint.ClippingEnd = ctrl.ClippingEnd;
                    viewPoint.CameraMode = ctrl.GetProjectionType() == swig.ProjectionType.Perspective ? 0 : 1;
					viewPoint.RateOfMagnification = ctrl.RateOfMagnification;
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
						ctrl.SetFocusPosition(new swig.Vector3F(viewPoint.FocusX, viewPoint.FocusY, viewPoint.FocusZ));
						ctrl.SetDistance(viewPoint.Distance);
						ctrl.SetProjectionType(viewPoint.CameraMode == 0 ? swig.ProjectionType.Perspective : swig.ProjectionType.Orthographic);
                        ctrl.SetAngleX(viewPoint.AngleX);
                        ctrl.SetAngleY(viewPoint.AngleY);
						ctrl.ClippingStart = viewPoint.ClippingStart;
                        ctrl.ClippingEnd = viewPoint.ClippingEnd;
						ctrl.RateOfMagnification = viewPoint.RateOfMagnification;
					}
                }
			}
		}
	}
}

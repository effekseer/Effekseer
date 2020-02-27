using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Plugin
{
	class ExportglTF
	{
        public static string UniqueName
        {
            get
            {
                return "glTF";
            }
        }

        public static string Author
        {
            get
            {
                return "glTF";
            }
        }

        public static string Title
        {
            get
            {
                return Resources.GetString("GlTFFormat");
			}
        }

        public static string Description
        {
            get
            {
				return Resources.GetString("ExportAsGlTFFormat");
			}
        }

        public static string Filter
        {
            get
            {
				return Resources.GetString("GlTFFormatExample");
			}
        }

        public static void Call(string path)
        {
            var window = new Magnification();
            window.Show(path);
        }

        class Magnification : Effekseer.GUI.IRemovableControl
        {
            string title = string.Empty;
            string message = string.Empty;
            string id = "###magnification";

            bool opened = true;

            bool isFirstUpdate = true;

            string path = string.Empty;
            float[] mag = new float[1];

            public bool ShouldBeRemoved { get; private set; }

            public Magnification()
            {
                ShouldBeRemoved = false;
            }

            public void Show(string path)
            {
                
				this.title = Resources.GetString("Maginification");

				this.path = path;
                mag[0] = Effekseer.Core.Option.ExternalMagnification.GetValue();
                Effekseer.GUI.Manager.AddControl(this);
            }

            public void Update()
            {
                if (isFirstUpdate)
                {
                    Effekseer.GUI.Manager.NativeManager.OpenPopup(id);
                    isFirstUpdate = false;
                }

                if (Effekseer.GUI.Manager.NativeManager.BeginPopupModal(title + id, ref opened, Effekseer.swig.WindowFlags.AlwaysAutoResize))
                {
                    if (Effekseer.GUI.Manager.NativeManager.DragFloat(this.title, mag, 0.1f, 0, float.MaxValue))
                    {

                    }

                    var save_text = Resources.GetString("Save");

					if (Effekseer.GUI.Manager.NativeManager.Button(save_text))
                    {
                        var option = new Effekseer.Exporter.glTFExporterOption();
                        option.Scale = mag[0];
                        var exporter = new Effekseer.Exporter.glTFExporter();
                        exporter.Export(path, option);
                        Effekseer.Core.Option.ExternalMagnification.SetValue(mag[0]);
                        ShouldBeRemoved = true;
                    }

                    var cancel_text = Resources.GetString("Cancel");

					if (Effekseer.GUI.Manager.NativeManager.Button(cancel_text))
                    {
                        ShouldBeRemoved = true;
                    }

                    Effekseer.GUI.Manager.NativeManager.EndPopup();
                }
                else
                {
                    ShouldBeRemoved = true;
                }
            }
        }
    }
}

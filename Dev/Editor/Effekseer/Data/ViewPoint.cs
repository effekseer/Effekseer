using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace Effekseer.Data
{
    class ViewPoint
    {
        public float FocusX = 0.0f;
        public float FocusY = 0.0f;
        public float FocusZ = 0.0f;
        public float AngleX = 0.0f;
        public float AngleY = 0.0f;
        public float Distance = 0.0f;
        public float RateOfMagnification = 0.0f;
        public float ClippingStart = 0.0f;
        public float ClippingEnd = 0.0f;
        public int CameraMode = 0;

        public bool Save(string path)
        {
            string json = JsonConvert.SerializeObject(this, Formatting.Indented);

            try
            {
                System.IO.File.WriteAllText(path, json);
            }
            catch
            {
                return false;
            }

            return true;
        }

        public static ViewPoint Load(string path)
        {
            string json = string.Empty;

            try
            {
                json = System.IO.File.ReadAllText(path);
            }
            catch
            {
                return null;
            }

            var v = JsonConvert.DeserializeObject<ViewPoint>(json);

            return v;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Menu
{
    class MenuItem : IControl
    {
        string id = "";

        public string Label { get; set; } = string.Empty;

        public bool ShouldBeRemoved { get; private set; } = false;

        public Action Clicked;

        public MenuItem()
        {
            var rand = new Random();
            id = "###" + rand.Next(0xffff).ToString();
        }

        public void Update()
        {
            if (Manager.NativeManager.MenuItem(Label + id))
            {
                if(Clicked != null)
                {
                    Clicked();
                }
            }
        }
    }
}

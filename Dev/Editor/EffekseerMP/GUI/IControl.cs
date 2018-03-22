using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI
{
    interface IControl
    {
        void Update();
    }

    interface IRemovableControl : IControl
    {
        bool ShouldBeRemoved { get; }
    }

	interface IDroppableControl : IControl
	{
		void OnDropped(string path, ref bool handle);
	}

	interface IParameterControl : IControl
	{
		string Label { get; }

		bool EnableUndo { get; set; }

		void SetBinding(object o);

		void FixValue();

		void OnDisposed();
	}
}

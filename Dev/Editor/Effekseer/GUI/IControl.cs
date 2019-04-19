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

	class Control
	{
		public virtual void OnDropped(string path, ref bool handle) { }

		protected virtual void DispatchDropped(string path, ref bool handle)
		{
			if (handle) return;
			OnDropped(path, ref handle);
		}
	}

	class GroupControl : Control
	{
			

		protected override void DispatchDropped(string path, ref bool handle)
		{

			base.DispatchDropped(path, ref handle);
		}
	}


}

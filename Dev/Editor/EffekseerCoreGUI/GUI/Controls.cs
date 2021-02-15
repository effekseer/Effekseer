using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI
{
    public interface IControl
    {
        void Update();
    }

    public interface IRemovableControl : IControl
    {
        bool ShouldBeRemoved { get; }
    }

	interface IDroppableControl : IControl
	{
		void OnDropped(string path, ref bool handle);
	}

	interface IParameterControl : IControl
	{
		/// <summary>
		/// whether this parameter can be undo
		/// </summary>
		bool EnableUndo { get; set; }

		void SetBinding(object o);

		/// <summary>
		/// which is called when values must be fixed
		/// </summary>
		void FixValue();

		void OnDisposed();
	}


	class Control : IDroppableControl
	{
		public bool ShouldBeRemoved { get; protected set; } = false;

		public virtual void Update() { }

		public virtual void OnDisposed() { }

		public virtual void OnDropped(string path, ref bool handle) { }

		public virtual void DispatchDisposed()
		{
			OnDisposed();
		}

		public virtual void DispatchDropped(string path, ref bool handle)
		{
			if (handle) return;
			OnDropped(path, ref handle);
		}
	}

	class GroupControl : Control
	{
		public Utils.DelayedList<IControl> Controls { get; } = new Utils.DelayedList<IControl>();

		public override void DispatchDisposed()
		{
			foreach (var c in Controls.Internal.OfType<Control>())
			{
				c.DispatchDisposed();
			}

			OnDisposed();
		}

		public override void DispatchDropped(string path, ref bool handle)
		{
			if (handle) return;

			foreach (var c in Controls.Internal.OfType<Control>())
			{
				c.DispatchDropped(path, ref handle);
				if (handle) break;
			}

			base.DispatchDropped(path, ref handle);
		}
	}

}

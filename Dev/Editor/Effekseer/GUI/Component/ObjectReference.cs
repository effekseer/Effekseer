using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class ObjectReference<T> : Control, IParameterControl where T : class, Data.Value.INamedObject, new()
	{
		string id = "";

		object boundObject = null;

		Data.Value.ObjectCollection<T> collection;

		public bool EnableUndo { get; set; } = true;

		public ObjectReference(Data.Value.ObjectCollection<T> collection)
		{
			this.collection = collection;
			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void FixValue()
		{
		}

		public void SetBinding(object o)
		{
			boundObject = o;
		}

		public override void Update()
		{
			var b = (Data.Value.ObjectReference<T>)boundObject;
			var next = ObjectCollection.Select<T>("###ObjectReference", id, b.Value, false, collection);

			if (b.Value != next)
			{
				b.SetValue(next);
			}
		}
	}
}

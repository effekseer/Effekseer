using System;
using Effekseer.Data;

namespace Effekseer.GUI.Dock
{
	class ProcedualModel : DockPanel
	{
		readonly Component.ParameterList paramerterList = null;

		public ProcedualModel()
		{
			Label = Icons.PanelDynamicParams + Resources.GetString("ProcedualModel_Name") + "###ProcedualModel";

			paramerterList = new Component.ParameterList();

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;

			Read();

			TabToolTip = Resources.GetString("ProcedualModel_Name");
		}

		public void FixValues()
		{
			paramerterList.FixValues();
		}

		public override void OnDisposed()
		{
			FixValues();

			Core.OnAfterLoad -= OnAfterLoad;
			Core.OnAfterNew -= OnAfterLoad;
		}

		protected override void UpdateInternal()
		{
			float width = Manager.NativeManager.GetContentRegionAvail().X;

			Manager.NativeManager.PushItemWidth(width - Manager.NativeManager.GetTextLineHeight() * 5.5f);

			var nextParam = Component.ObjectCollection.Select("", "", Core.ProcedualModel.ProcedualModels.Selected, false, Core.ProcedualModel.ProcedualModels);

			if (Core.ProcedualModel.ProcedualModels.Selected != nextParam)
			{
				Core.ProcedualModel.ProcedualModels.Selected = nextParam;
			}

			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(Resources.GetString("DynamicAdd") + "###DynamicAdd"))
			{
				Core.ProcedualModel.ProcedualModels.New();
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(Resources.GetString("DynamicDelete") + "###DynamicDelete"))
			{
				Core.ProcedualModel.ProcedualModels.Delete(Core.ProcedualModel.ProcedualModels.Selected);
			}

			paramerterList.Update();
		}

		private void Read()
		{
			paramerterList.SetValue(Core.ProcedualModel.ProcedualModels);
		}

		private void OnAfterLoad(object sender, EventArgs e)
		{
			Read();
		}
	}
}


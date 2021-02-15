using System;
using Effekseer.Data;

namespace Effekseer.GUI.Dock
{
	class ProcedualModel : DockPanel
	{
		readonly Component.ParameterList paramerterList = null;
		Component.CopyAndPaste candp = null;

		public ProcedualModel()
		{
			Label = Icons.PanelDynamicParams + Resources.GetString("ProcedualModel_Name") + "###ProcedualModel";

			paramerterList = new Component.ParameterList();
			candp = new Component.CopyAndPaste("ProceduralModel", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnBeforeLoad += Core_OnBeforeLoad;
			Read();

			TabToolTip = Resources.GetString("ProcedualModel_Name");
		}

		public void FixValues()
		{
			paramerterList.FixValues();
		}

		object GetTargetObject()
		{
			return Core.ProcedualModel.ProcedualModels.Selected;
		}

		public override void OnDisposed()
		{
			FixValues();

			Core.OnAfterLoad -= OnAfterLoad;
			Core.OnAfterNew -= OnAfterLoad;
			Core.OnBeforeLoad -= Core_OnBeforeLoad;
		}

		protected override void UpdateInternal()
		{
			float width = GUIManager.NativeManager.GetContentRegionAvail().X;

			GUIManager.NativeManager.PushItemWidth(width - GUIManager.NativeManager.GetTextLineHeight() * 5.5f);

			var nextParam = Component.ObjectCollection.Select("", "", Core.ProcedualModel.ProcedualModels.Selected, false, Core.ProcedualModel.ProcedualModels);

			if (Core.ProcedualModel.ProcedualModels.Selected != nextParam)
			{
				Core.ProcedualModel.ProcedualModels.Selected = nextParam;
			}

			GUIManager.NativeManager.PopItemWidth();

			GUIManager.NativeManager.SameLine();

			if (GUIManager.NativeManager.Button(Resources.GetString("DynamicAdd") + "###DynamicAdd"))
			{
				Core.ProcedualModel.ProcedualModels.New();
			}

			GUIManager.NativeManager.SameLine();

			if (GUIManager.NativeManager.Button(Resources.GetString("DynamicDelete") + "###DynamicDelete"))
			{
				Core.ProcedualModel.ProcedualModels.Delete(Core.ProcedualModel.ProcedualModels.Selected);
			}

			candp.Update();
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

		private void Core_OnBeforeLoad(object sender, EventArgs e)
		{
			paramerterList.SetValue(null);
		}

	}
}


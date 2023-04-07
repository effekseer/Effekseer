

using Effekseer.GUI;
using Effekseer;
using Effekseer.GUI.Inspector;
using PartsTreeSystem;
using System.Reflection.Emit;

namespace Effekseer.DockPanels
{
	public class Dymamic : GUI.Dock.DockPanel
	{
		Inspector inspector;

		string compileResult = string.Empty;

		public Dymamic()
		{
			inspector = new Inspector();
			Label = Icons.PanelDynamicParams + "Inspector_DynamicParameter###Inspector_DynamicParameter";

			TabToolTip = Resources.GetString("DynamicParameter_Name");
			DocPage = "dynamicParameter.html";
		}

		protected override void UpdateInternal()
		{
			if (CoreContext.SelectedEffect.Context.NodeTree == null ||
				CoreContext.SelectedEffectNode == null)
			{
				return;
			}

			// TODO: Remove this when remove the previous version
			Manager.NativeManager.PushID(999);

			Manager.NativeManager.Text(Resources.GetString("DynamicInput"));

			// TODO insert inspector of DynamicInput here


			Manager.NativeManager.Separator();

			Manager.NativeManager.Text(Resources.GetString("DynamicEquation"));

			float width = Manager.NativeManager.GetContentRegionAvail().X;

			Manager.NativeManager.PushItemWidth(width - Manager.NativeManager.GetTextLineHeight() * 5.5f);

			// TODO insert combo of DynamicEquations here
			// this is a dummy
			if (Manager.NativeManager.BeginCombo("", "AAAAAA", swig.ComboFlags.None))
			{

				Manager.NativeManager.EndCombo();
			}


			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(Resources.GetString("DynamicAdd") + "###DynamicAdd"))
			{
				Core.Dynamic.Equations.New();
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(Resources.GetString("DynamicDelete") + "###DynamicDelete"))
			{
				Core.Dynamic.Equations.Delete(Core.Dynamic.Equations.Selected);
			}

			// TODO insert inspector of DynamicEquations here


			Manager.NativeManager.Button(Resources.GetString("Compile") + "###DynamicCompile");
			// show errors
			Manager.NativeManager.Text(compileResult);

			// TODO: Remove this when remove the previous version
			Manager.NativeManager.PopID();
		}

		public override void DispatchDropped(string path, ref bool handle)
		{
			if (CoreContext.SelectedEffect.Context.NodeTree == null || CoreContext.SelectedEffectNode == null)
			{
				return;
			}

			CoreContext.SelectedEffect.Context.CommandManager.StartEditFields(
				CoreContext.SelectedEffect.Asset.NodeTreeAsset,
				CoreContext.SelectedEffect.Context.NodeTree,
				CoreContext.SelectedEffectNode,
				CoreContext.Environment);

			if (inspector.Drop(path, CoreContext.SelectedEffect.Context,
				CoreContext.SelectedEffectNode,
				typeof(Asset.Effect.RotationParameter)))
			{
				handle = true;
			}

			CoreContext.SelectedEffect.Context.CommandManager.EndEditFields(
				CoreContext.SelectedEffectNode,
				CoreContext.Environment);


			base.DispatchDropped(path, ref handle);
		}
	}
}

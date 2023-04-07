using Effekseer.GUI;
using Effekseer;
using Effekseer.GUI.Inspector;
using PartsTreeSystem;
using System.Reflection.Emit;

namespace Effekseer.DockPanels
{
	public class CommonValues : GUI.Dock.DockPanel
	{
		Inspector inspector;

		public CommonValues()
		{
			inspector = new Inspector();
			Label = Icons.PanelCommon + "Inspector_CommonValues###Inspector_BasicSettings";

			TabToolTip = Resources.GetString("BasicSettings");
			DocPage = "common.html";
		}

		protected override void UpdateInternal()
		{
			if (CoreContext.SelectedEffect.Context.NodeTree == null ||
				CoreContext.SelectedEffectNode == null)
			{
				return;
			}

			CoreContext.SelectedEffect.Context.CommandManager.StartEditFields(
				CoreContext.SelectedEffect.Asset.NodeTreeAsset,
				CoreContext.SelectedEffect.Context.NodeTree,
				CoreContext.SelectedEffectNode,
				CoreContext.Environment);

			inspector.Update(CoreContext.SelectedEffect.Context,
				CoreContext.SelectedEffectNode,
				typeof(Asset.CommonParameter));

			CoreContext.SelectedEffect.Context.CommandManager.EndEditFields(
				CoreContext.SelectedEffectNode,
				CoreContext.Environment);

			// Fix edited results when values are not edited
			if (!Manager.NativeManager.IsAnyItemActive())
			{
				CoreContext.SelectedEffect.Context.CommandManager.SetFlagToBlockMergeCommands();
			}
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
				typeof(Asset.Effect.PositionParameter)))
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

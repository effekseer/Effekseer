using Effekseer.GUI;
using Effekseer;
using Effekseer.GUI.Inspector;
using PartsTreeSystem;

namespace Effekseer.DockPanels
{
	public class Option : GUI.Dock.DockPanel
	{
		Inspector inspector;

		// TODO: Move this to Effekseer.Core
		public Asset.Effect.OptionParameter OptionParameter { get; private set; } = new Asset.Effect.OptionParameter();


		public Option()
		{
			inspector = new Inspector();
			Label = Icons.PanelOptions + Resources.GetString("Inspector_Options") + "###Inspector_Options";
			TabToolTip = Resources.GetString("Options");
			DocPage = "options.html";
		}

		protected override void UpdateInternal()
		{
			// TODO: Remove this when remove the previous version
			Manager.NativeManager.PushID(999);

			inspector.Update(null, OptionParameter);

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

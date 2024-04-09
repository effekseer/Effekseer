using System;
namespace Effekseer.GUI.Dock
{
	class GlobalValues : DockPanel
	{
		BindableComponent.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public GlobalValues()
		{
			Label = Icons.PanelGlobal + MultiLanguageTextProvider.GetText("Global") + "###Global";
			DocPage = "global.html";

			paramerterList = new BindableComponent.ParameterList();
			paramerterList.SetType(typeof(Data.GlobalValues));

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Read();

			TabToolTip = MultiLanguageTextProvider.GetText("Global");
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
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}

		protected override void UpdateInternal()
		{
			if (isFiestUpdate)
			{
			}

			paramerterList.Update();
		}

		void Read()
		{
			paramerterList.SetValue(Core.Global);
		}

		void OnAfterLoad(object sender, EventArgs e)
		{
			Read();
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}
	}

}
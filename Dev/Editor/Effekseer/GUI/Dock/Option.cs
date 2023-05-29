using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class Option : DockPanel
	{
		BindableComponent.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public Option()
		{
			Label = Icons.PanelOptions + MultiLanguageTextProvider.GetText("Options") + "###Options";
			DocPage = "options.html";

			paramerterList = new BindableComponent.ParameterList();
			paramerterList.SetType(typeof(Data.OptionValues));

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;

			TabToolTip = MultiLanguageTextProvider.GetText("Options");
		}

		public void FixValues()
		{
			paramerterList.FixValues();
		}

		public override void OnDisposed()
		{
			FixValues();

			Core.OnAfterLoad -= OnAfter;
			Core.OnAfterNew -= OnAfter;
		}

		protected override void UpdateInternal()
		{
			if (isFiestUpdate)
			{
				paramerterList.SetValue(Core.Option);
				isFiestUpdate = false;

				Core.Option.ColorSpace.OnChanged += RestartValue_OnChanged;
				Core.Option.FontSize.OnChanged += FontSize_OnChanged;
				Core.Option.Font.OnChanged += FontSize_OnChanged;
				LanguageTable.OnLanguageChanged += RestartValue_OnChanged;
			}

			Manager.NativeManager.Separator();
			paramerterList.Update();
		}

		private void FontSize_OnChanged(object sender, ChangedValueEventArgs e)
		{
			Manager.UpdateFont();
		}

		private void RestartValue_OnChanged(object sender, ChangedValueEventArgs e_)
		{
			swig.GUIManager.show(MultiLanguageTextProvider.GetText("ChangeRestartValues_Message"), "Info", swig.DialogStyle.Info, swig.DialogButtons.OK);
		}

		void OnAfter(object sender, EventArgs e)
		{
			paramerterList.SetValue(Core.Option);
		}
	}
}
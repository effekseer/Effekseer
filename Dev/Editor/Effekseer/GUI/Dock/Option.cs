using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class Option : DockPanel
	{
		Component.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public Option()
		{
			Label = Resources.GetString("Options") + "###Options";
			paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.OptionValues));

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;

			Icon = Images.GetIcon("PanelOption");
			IconSize = new swig.Vec2(24, 24);
			TabToolTip = Resources.GetString("Options");
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
			if(isFiestUpdate)
			{
				paramerterList.SetValue(Core.Option);
				isFiestUpdate = false;

				Core.Option.GuiLanguage.OnChanged += GuiLanguage_OnChanged;
			}

			paramerterList.Update();
		}

		private void GuiLanguage_OnChanged(object sender, ChangedValueEventArgs e_)
		{
			var e = Core.Option.GuiLanguage.GetValue();

			if (e == Language.Japanese)
			{
                swig.GUIManager.show("次回のEffekseer起動時に適応します。", "Info", swig.DialogStyle.Info, swig.DialogButtons.OK);
				//var mb = new Dialog.MessageBox();
				//mb.Show("Info", "次回のEffekseer起動時に適応します。");
			}
			else if (e == Language.English)
			{
                swig.GUIManager.show("It adapts to the next startup", "Info", swig.DialogStyle.Info, swig.DialogButtons.OK);

				//var mb = new Dialog.MessageBox();
				//mb.Show("Info", "It adapts to the next startup");
			}
		}

		void OnAfter(object sender, EventArgs e)
		{
			paramerterList.SetValue(Core.Option);
		}
	}
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class Recorder : DockPanel
	{
		int startingFrame = 1;
		int endingFrame = 16;
		int freq = 1;
		int theNumberOfImageV = 4;
		bool nowReloading = false;
		int selectedTypeIndex = 0;
		int selectedAlphaIndex = 0;

		string[] selectedExportTypes = new string[]
		{
			"1",
			"2",
			"3",
			"4",
		};

		string[] selectedAlphaTypes = new string[]
		{
			"1",
			"2",
			"3",
		};

		public Recorder()
		{
			selectedExportTypes[0] = Resources.GetString("ExportAsSingleImage");
			selectedExportTypes[1] = Resources.GetString("ExportAsImages");
			selectedExportTypes[2] = Resources.GetString("ExportAsGifAnimation");
			selectedExportTypes[3] = Resources.GetString("ExportAsAvi");

			selectedAlphaTypes[0] = Resources.GetString("None");
			selectedAlphaTypes[1] = Resources.GetString("UseOriginalImage");
			selectedAlphaTypes[2] = Resources.GetString("GenerateAlpha");

			Label = Resources.GetString("Recorder") + "###Recorder";
		}

		protected override void UpdateInternal()
		{
			var viewerParameter = Manager.Viewer.GetViewerParamater();
			var w = new int [] { Manager.Viewer.GetViewerParamater().GuideWidth };
			var h = new int [] { Manager.Viewer.GetViewerParamater().GuideHeight };
			var showGuide = new bool[] { Manager.Viewer.GetViewerParamater().RendersGuide };

			var startingFrame_ = new int[] { startingFrame };
			var endingFrame_ = new int[] { endingFrame };
			var freq_ = new int[] { freq };
			var theNumberOfImageV_ = new int[] { theNumberOfImageV };
			var nowReloading_ = new bool[] { nowReloading };
			
			if(Manager.NativeManager.DragInt(Resources.GetString("Width") + "###w", w))
			{
				viewerParameter.GuideWidth = w[0];
			}

			if (Manager.NativeManager.DragInt(Resources.GetString("Height") + "###h", h))
			{
				viewerParameter.GuideHeight = h[0];
			}

			if (Manager.NativeManager.Checkbox(Resources.GetString("ShowGuide") + "###sg", showGuide))
			{
				viewerParameter.RendersGuide = showGuide[0];
			}

			if (Manager.NativeManager.DragInt(Resources.GetString("StartFrame") + "###sf", startingFrame_))
			{
				startingFrame = startingFrame_[0];
			}

			if (Manager.NativeManager.DragInt(Resources.GetString("EndFrame") + "###ef", endingFrame_))
			{
				endingFrame = endingFrame_[0];
			}

			if (Manager.NativeManager.DragInt(Resources.GetString("Frequency_Frame") + "###ff", freq_))
			{
				freq = freq_[0];
			}

			if(Manager.NativeManager.DragInt(Resources.GetString("XCount") + "###tn", theNumberOfImageV_))
			{
				theNumberOfImageV = theNumberOfImageV_[0];
			}

			if (Manager.NativeManager.BeginCombo(Resources.GetString("ExportedFrame")  + "###extype", selectedExportTypes[selectedTypeIndex], swig.ComboFlags.None))
			{
				for(int i = 0; i < selectedExportTypes.Length; i++)
				{
					if(Manager.NativeManager.Selectable(selectedExportTypes[i]))
					{
						selectedTypeIndex = i;
						Manager.NativeManager.SetItemDefaultFocus();
					}
				}

				Manager.NativeManager.EndCombo();
			}

			if (Manager.NativeManager.BeginCombo(Resources.GetString("MakeTransparent") + "###tr", selectedAlphaTypes[selectedAlphaIndex], swig.ComboFlags.None))
			{
				for (int i = 0; i < selectedAlphaTypes.Length; i++)
				{
					if (Manager.NativeManager.Selectable(selectedAlphaTypes[i]))
					{
						selectedTypeIndex = i;
						Manager.NativeManager.SetItemDefaultFocus();
					}
				}

				Manager.NativeManager.EndCombo();
			}

			Manager.Viewer.SetViewerParamater(viewerParameter);

			if(Manager.NativeManager.Button(Resources.GetString("Record") + "###btn"))
			{
				var during = endingFrame - startingFrame;
				if (during < 0)
				{
					var mb = new Dialog.MessageBox();
					mb.Show("Error", "出力フレームが存在しません。");
				}

				var count = during / freq + 1;
				var width = theNumberOfImageV;
				var height = count / width;
				if (height * width != count) height++;

				if (Manager.Viewer != null)
				{
					var filter = string.Empty;

					if (selectedTypeIndex == 0)
                    {
						filter = "png";
                    }
                    else if (selectedTypeIndex == 1)
                    {
						filter = "png";
                    }
                    else if (selectedTypeIndex == 2)
                    {
						filter = "gif";
                    }
                    else if (selectedTypeIndex == 3)
                    {
						filter = "avi";
                    }


                    var result = swig.FileDialog.SaveDialog(filter, System.IO.Directory.GetCurrentDirectory());

					if (string.IsNullOrEmpty(result)) return;
                                   
					var filename = result;

					if (System.IO.Path.GetExtension(filename) != "." + filter)
                    {
						filename += "." + filter;
                    }

					var viewer = Manager.Viewer;
					var param = Manager.Viewer.GetViewerParamater();

					//if (viewer.LoadEffectFunc != null)
					{
						viewer.LoadEffectFunc();
					}

					var tempDirectory = System.IO.Directory.GetCurrentDirectory();
					System.IO.Directory.SetCurrentDirectory(Program.StartDirectory);

					if (selectedTypeIndex == 0)
					{
						if (!viewer.Record(filename, count, width, startingFrame, freq, (swig.TransparenceType)selectedAlphaIndex))
						{
							var mb = new Dialog.MessageBox();
							mb.Show("Error", "保存に失敗しました。コンピューターのスペックが低い、もしくは設定に問題があります。");
						}
					}
					else if (selectedTypeIndex == 1)
					{
						if (!viewer.Record(filename, count, startingFrame, freq, (swig.TransparenceType)selectedAlphaIndex))
						{
							var mb = new Dialog.MessageBox();
                            mb.Show("Error", "保存に失敗しました。コンピューターのスペックが低い、もしくは設定に問題があります。");
						}
					}
					else if (selectedTypeIndex == 2)
					{
						if (!viewer.RecordAsGifAnimation(filename, count, startingFrame, freq, (swig.TransparenceType)selectedAlphaIndex))
						{
							var mb = new Dialog.MessageBox();
                            mb.Show("Error", "保存に失敗しました。コンピューターのスペックが低い、もしくは設定に問題があります。");
						}
					}
					else if (selectedTypeIndex == 3)
					{
						if (!viewer.RecordAsAVI(filename, count, startingFrame, freq, (swig.TransparenceType)selectedAlphaIndex))
						{
							var mb = new Dialog.MessageBox();
                            mb.Show("Error", "保存に失敗しました。コンピューターのスペックが低い、もしくは設定に問題があります。");
						}
					}

					System.IO.Directory.SetCurrentDirectory(tempDirectory);

				}
			}
		}
	}
}

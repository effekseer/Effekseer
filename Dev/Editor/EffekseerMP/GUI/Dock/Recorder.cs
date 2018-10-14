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

			Icon = Images.GetIcon("PanelRecorder");
			IconSize = new swig.Vec2(24, 24);
			TabToolTip = Resources.GetString("Recorder");
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

			var areaTitle = Resources.GetString("Resolution");
			var exportedFrameTitle = Resources.GetString("ExportedFrame");
			var typeTitle = Resources.GetString("Format");
			var optionsTitle = Resources.GetString("Options");
			
			// Recordingwindow
			Manager.NativeManager.BeginChild("##RecordRes", new swig.Vec2(0,120), true, swig.WindowFlags.MenuBar);
			if (Manager.NativeManager.BeginMenuBar())
			{
				if (Manager.NativeManager.BeginMenu(areaTitle + "##RecordResTitle"))
				{
					Manager.NativeManager.EndMenu();
				}

				Manager.NativeManager.EndMenuBar();
			}

			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnWidth(0, 120);

			Manager.NativeManager.Text(Resources.GetString("Width"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.DragInt("###w", w))
			{
				viewerParameter.GuideWidth = w[0];
			}

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("Height"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.DragInt("###h", h))
			{
				viewerParameter.GuideHeight = h[0];
			}

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("ShowGuide"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.Checkbox("###sg", showGuide))
			{
				viewerParameter.RendersGuide = showGuide[0];
			}

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChild();

			// Recordingwindow
			Manager.NativeManager.BeginChild("##OutputFrame", new swig.Vec2(0, 120), true, swig.WindowFlags.MenuBar);
			if (Manager.NativeManager.BeginMenuBar())
			{
				if (Manager.NativeManager.BeginMenu(exportedFrameTitle + "##OutputFrameTitle"))
				{
					Manager.NativeManager.EndMenu();
				}

				Manager.NativeManager.EndMenuBar();
			}

			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnWidth(0, 120);

			Manager.NativeManager.Text(Resources.GetString("StartFrame"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.DragInt("###sf", startingFrame_))
			{
				startingFrame = startingFrame_[0];
			}

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("EndFrame"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.DragInt("###ef", endingFrame_))
			{
				endingFrame = endingFrame_[0];
			}

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("Frequency_Frame"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.DragInt("###ff", freq_))
			{
				freq = freq_[0];
			}

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChild();

			// Recordingwindow
			Manager.NativeManager.BeginChild("##OutputType", new swig.Vec2(0, 100), true, swig.WindowFlags.MenuBar);
			if (Manager.NativeManager.BeginMenuBar())
			{
				if (Manager.NativeManager.BeginMenu(typeTitle + "##OutputTypeTitle"))
				{
					Manager.NativeManager.EndMenu();
				}

				Manager.NativeManager.EndMenuBar();
			}

			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnWidth(0, 120);

			Manager.NativeManager.Text(Resources.GetString("XCount"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.DragInt("###tn", theNumberOfImageV_))
			{
				theNumberOfImageV = theNumberOfImageV_[0];
			}

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("ExportedFrame"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.BeginCombo("###extype", selectedExportTypes[selectedTypeIndex], swig.ComboFlags.None))
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

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChild();

			// Recordingwindow
			Manager.NativeManager.BeginChild("##Option", new swig.Vec2(0, 60), true, swig.WindowFlags.MenuBar);
			if (Manager.NativeManager.BeginMenuBar())
			{
				if (Manager.NativeManager.BeginMenu(optionsTitle + "##OptionTitle"))
				{
					Manager.NativeManager.EndMenu();
				}

				Manager.NativeManager.EndMenuBar();
			}

			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnWidth(0, 120);

			Manager.NativeManager.Text(Resources.GetString("MakeTransparent"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.BeginCombo("###tr", selectedAlphaTypes[selectedAlphaIndex], swig.ComboFlags.None))
			{
				for (int i = 0; i < selectedAlphaTypes.Length; i++)
				{
					if (Manager.NativeManager.Selectable(selectedAlphaTypes[i]))
					{
						selectedAlphaIndex = i;
						Manager.NativeManager.SetItemDefaultFocus();
					}
				}

				Manager.NativeManager.EndCombo();
			}

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChild();

			Manager.Viewer.SetViewerParamater(viewerParameter);

			if(Manager.NativeManager.Button(Resources.GetString("Record") + "###btn"))
			{
				var during = endingFrame - startingFrame;
				if (during < 0)
				{
                    swig.GUIManager.show("出力フレームが存在しません。", "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
                        
					//var mb = new Dialog.MessageBox();
					//mb.Show("Error", "出力フレームが存在しません。");
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

					string errorMessage = string.Empty;

					if(Effekseer.Core.Language == Language.Japanese)
					{
						errorMessage = "保存に失敗しました。ファイルが他のアプリケーションで開かれている、もしくはスペックが足りません。";
					}
					else
					{
						errorMessage = "It failed to save. A file is opend by other application or lack of specification.";
					}

					if (selectedTypeIndex == 0)
					{
						if (!viewer.Record(filename, count, width, startingFrame, freq, (swig.TransparenceType)selectedAlphaIndex))
						{
                            swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
					}
					else if (selectedTypeIndex == 1)
					{
						if (!viewer.Record(filename, count, startingFrame, freq, (swig.TransparenceType)selectedAlphaIndex))
						{
                            swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
					}
					else if (selectedTypeIndex == 2)
					{
						if (!viewer.RecordAsGifAnimation(filename, count, startingFrame, freq, (swig.TransparenceType)selectedAlphaIndex))
						{
                            swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
					}
					else if (selectedTypeIndex == 3)
					{
						if (!viewer.RecordAsAVI(filename, count, startingFrame, freq, (swig.TransparenceType)selectedAlphaIndex))
						{
                            swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
					}

					System.IO.Directory.SetCurrentDirectory(tempDirectory);

				}
			}
		}
	}
}

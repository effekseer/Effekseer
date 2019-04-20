using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class Recorder : DockPanel
	{
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
            viewerParameter.GuideWidth = Core.Option.RecordingWidth.Value;
            viewerParameter.GuideHeight = Core.Option.RecordingHeight.Value;
            viewerParameter.RendersGuide = Core.Option.IsRecordingGuideShown;

            var w = new int [] { Manager.Viewer.GetViewerParamater().GuideWidth };
			var h = new int [] { Manager.Viewer.GetViewerParamater().GuideHeight };
			var showGuide = new bool[] { Manager.Viewer.GetViewerParamater().RendersGuide };

            var startingFrame_ = new int[] { Core.Option.RecordingStartingFrame.Value };
			var endingFrame_ = new int[] { Core.Option.RecordingEndingFrame.Value };
			var freq_ = new int[] { Core.Option.RecordingFrequency.Value };
			var theNumberOfImageH_ = new int[] { Core.Option.RecordingHorizontalCount.Value };

			var areaTitle = Resources.GetString("Resolution");
			var exportedFrameTitle = Resources.GetString("ExportedFrame");
			var typeTitle = Resources.GetString("Format");
			var optionsTitle = Resources.GetString("Options");

            selectedTypeIndex = Core.Option.RecordingExporter.GetValueAsInt();
            selectedAlphaIndex = Core.Option.RecordingTransparentMethod.GetValueAsInt();

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
                Core.Option.RecordingWidth.SetValueDirectly(w[0]);
            }

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("Height"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.DragInt("###h", h))
			{
				viewerParameter.GuideHeight = h[0];
                Core.Option.RecordingHeight.SetValueDirectly(h[0]);
            }

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("ShowGuide"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.Checkbox("###sg", showGuide))
			{
				viewerParameter.RendersGuide = showGuide[0];
                Core.Option.IsRecordingGuideShown.SetValueDirectly(showGuide[0]);
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
                Core.Option.RecordingStartingFrame.SetValueDirectly(startingFrame_[0]);
			}

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("EndFrame"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.DragInt("###ef", endingFrame_))
			{
                Core.Option.RecordingEndingFrame.SetValueDirectly(endingFrame_[0]);
			}

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("Frequency_Frame"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.DragInt("###ff", freq_))
			{
                Core.Option.RecordingFrequency.SetValueDirectly(freq_[0]);
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

			if (Manager.NativeManager.DragInt("###tn", theNumberOfImageH_))
			{
                Core.Option.RecordingHorizontalCount.SetValueDirectly(theNumberOfImageH_[0]);
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
                        Core.Option.RecordingExporter.SetValueDirectly((Data.RecordingExporterType)i);
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
                        Core.Option.RecordingTransparentMethod.SetValueDirectly((Data.RecordingTransparentMethodType)i);
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
				var during = Core.Option.RecordingEndingFrame.Value - Core.Option.RecordingStartingFrame.Value;
				if (during < 0)
				{
                    swig.GUIManager.show("出力フレームが存在しません。", "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
                        
					//var mb = new Dialog.MessageBox();
					//mb.Show("Error", "出力フレームが存在しません。");
				}

				var count = during / Core.Option.RecordingFrequency.Value + 1;
				var horizontalCount = Core.Option.RecordingHorizontalCount.Value;

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

                    var startingFrame = Core.Option.RecordingStartingFrame.Value;
                    var freq = Core.Option.RecordingFrequency.Value;

                    var recordingParameter = new swig.RecordingParameter();

                    recordingParameter.Count = count;
                    recordingParameter.HorizontalCount = horizontalCount;
                    recordingParameter.Freq = freq;
                    recordingParameter.OffsetFrame = startingFrame;
                    recordingParameter.Transparence = (swig.TransparenceType)selectedAlphaIndex;

					if (Effekseer.Core.Language == Language.Japanese)
					{
						errorMessage = "保存に失敗しました。ファイルが他のアプリケーションで開かれている、もしくはスペックが足りません。";
					}
					else
					{
						errorMessage = "It failed to save. A file is opend by other application or lack of specification.";
					}

					if (selectedTypeIndex == 0)
					{
						if (!viewer.RecordSpriteSheet(filename, recordingParameter))
						{
                            swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
					}
					else if (selectedTypeIndex == 1)
					{
						if (!viewer.RecordSprite(filename, recordingParameter))
						{
                            swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
					}
					else if (selectedTypeIndex == 2)
					{
						if (!viewer.RecordAsGifAnimation(filename, recordingParameter))
						{
                            swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
					}
					else if (selectedTypeIndex == 3)
					{
						if (!viewer.RecordAsAVI(filename, recordingParameter))
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

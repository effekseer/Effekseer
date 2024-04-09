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
		int selectedStorageTarget = 0;

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
			"4",
		};

		string[] selectedStorageTargets = new string[]
		{
			"1",
			"2",
		};

		MultiLanguageString scale_name = new MultiLanguageString("Recording_Scale_Name");
		MultiLanguageString scale_desc = new MultiLanguageString("Recording_Scale_Desc");

		swig.EffectRecorder effectRecorder = null;

		public Recorder()
		{
			selectedExportTypes[0] = MultiLanguageTextProvider.GetText("ExportAsSingleImage");
			selectedExportTypes[1] = MultiLanguageTextProvider.GetText("ExportAsImages");
			selectedExportTypes[2] = MultiLanguageTextProvider.GetText("ExportAsGifAnimation");
			selectedExportTypes[3] = MultiLanguageTextProvider.GetText("ExportAsAvi");

			System.OperatingSystem os = System.Environment.OSVersion;
			if ((os.Platform == PlatformID.Win32NT ||
			os.Platform == PlatformID.Win32S ||
			os.Platform == PlatformID.Win32Windows ||
			os.Platform == PlatformID.WinCE))
			{
				selectedExportTypes = selectedExportTypes.Concat(new[] { MultiLanguageTextProvider.GetText("ExportAsH264") }).ToArray();
			}

			selectedAlphaTypes[0] = MultiLanguageTextProvider.GetText("None");
			selectedAlphaTypes[1] = MultiLanguageTextProvider.GetText("UseOriginalImage");
			selectedAlphaTypes[2] = MultiLanguageTextProvider.GetText("GenerateAlpha");
			selectedAlphaTypes[3] = MultiLanguageTextProvider.GetText("GenerateAlpha2");

			selectedStorageTargets[0] = MultiLanguageTextProvider.GetText("StorageGlobal");
			selectedStorageTargets[1] = MultiLanguageTextProvider.GetText("StorageLocal");

			Label = Icons.PanelRecorder + MultiLanguageTextProvider.GetText("Recorder") + "###Recorder";
			DocPage = "record.html";

			TabToolTip = MultiLanguageTextProvider.GetText("Recorder");
		}

		public override void OnDisposed()
		{
			if (effectRecorder != null)
			{
				effectRecorder.End();
				effectRecorder.Dispose();
				effectRecorder = null;
			}

			base.OnDisposed();
		}

		protected override void UpdateInternal()
		{
			var mainRenderer = Manager.Viewer.EffectRenderer;
			mainRenderer.GuideWidth = Core.Recording.RecordingWidth.Value;
			mainRenderer.GuideHeight = Core.Recording.RecordingHeight.Value;
			mainRenderer.RendersGuide = Core.Recording.IsRecordingGuideShown;

			float dpiScale = Manager.DpiScale;
			var w = new int[] { mainRenderer.GuideWidth };
			var h = new int[] { mainRenderer.GuideHeight };
			var showGuide = new bool[] { mainRenderer.RendersGuide };

			var startingFrame_ = new int[] { Core.Recording.RecordingStartingFrame.Value };
			var endingFrame_ = new int[] { Core.Recording.RecordingEndingFrame.Value };
			var freq_ = new int[] { Core.Recording.RecordingFrequency.Value };
			var theNumberOfImageH_ = new int[] { Core.Recording.RecordingHorizontalCount.Value };

			var areaTitle = MultiLanguageTextProvider.GetText("Resolution");
			var exportedFrameTitle = MultiLanguageTextProvider.GetText("ExportedFrame");
			var typeTitle = MultiLanguageTextProvider.GetText("Format");
			var optionsTitle = MultiLanguageTextProvider.GetText("Options");

			selectedTypeIndex = Core.Recording.RecordingExporter.GetValueAsInt();
			selectedAlphaIndex = Core.Recording.RecordingTransparentMethod.GetValueAsInt();
			selectedStorageTarget = Core.Recording.RecordingStorageTarget.GetValueAsInt();

			// Recordingwindow
			{
				Manager.NativeManager.BeginChild("##RecordRes", new swig.Vec2(0, 140 * dpiScale), true, swig.WindowFlags.MenuBar);

				if (Manager.NativeManager.BeginMenuBar())
				{
					if (Manager.NativeManager.BeginMenu(areaTitle + "##RecordResTitle"))
					{
						Manager.NativeManager.EndMenu();
					}

					Manager.NativeManager.EndMenuBar();
				}

				Manager.NativeManager.Columns(2);
				Manager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

				Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("Width"));

				Manager.NativeManager.NextColumn();

				Manager.NativeManager.PushItemWidth(-1);
				if (Manager.NativeManager.DragInt("###w", w))
				{
					mainRenderer.GuideWidth = w[0];
					Core.Recording.RecordingWidth.SetValueDirectly(w[0]);
				}
				Manager.NativeManager.PopItemWidth();

				Manager.NativeManager.NextColumn();

				Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("Height"));

				Manager.NativeManager.NextColumn();

				Manager.NativeManager.PushItemWidth(-1);
				if (Manager.NativeManager.DragInt("###h", h))
				{
					mainRenderer.GuideHeight = h[0];
					Core.Recording.RecordingHeight.SetValueDirectly(h[0]);
				}
				Manager.NativeManager.PopItemWidth();

				Manager.NativeManager.NextColumn();

				Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("ShowGuide"));

				Manager.NativeManager.NextColumn();

				if (Manager.NativeManager.Checkbox("###sg", showGuide))
				{
					mainRenderer.RendersGuide = showGuide[0];
					Core.Recording.IsRecordingGuideShown.SetValueDirectly(showGuide[0]);
				}

				// Scale

				Manager.NativeManager.NextColumn();

				Manager.NativeManager.Text(scale_name.ToString());

				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.SetTooltip(scale_desc.ToString());
				}

				Manager.NativeManager.NextColumn();

				var recordingScale = new int[] { Core.Recording.RecordingScale.Value };
				if (Manager.NativeManager.DragInt("###RecScale", recordingScale))
				{
					Core.Recording.RecordingScale.SetValue(recordingScale[0]);
				}

				Manager.NativeManager.Columns(1);

				Manager.NativeManager.EndChild();
			}

			// Recordingwindow
			Manager.NativeManager.BeginChild("##OutputFrame", new swig.Vec2(0, 130 * dpiScale), true, swig.WindowFlags.MenuBar);
			if (Manager.NativeManager.BeginMenuBar())
			{
				if (Manager.NativeManager.BeginMenu(exportedFrameTitle + "##OutputFrameTitle"))
				{
					Manager.NativeManager.EndMenu();
				}

				Manager.NativeManager.EndMenuBar();
			}

			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

			Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("StartFrame"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.DragInt("###sf", startingFrame_))
			{
				Core.Recording.RecordingStartingFrame.SetValueDirectly(startingFrame_[0]);
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("EndFrame"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.DragInt("###ef", endingFrame_))
			{
				Core.Recording.RecordingEndingFrame.SetValueDirectly(endingFrame_[0]);
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("Frequency_Frame"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.DragInt("###ff", freq_))
			{
				Core.Recording.RecordingFrequency.SetValueDirectly(freq_[0]);
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChild();

			// ExportType
			float exportTypeChildHeight = (selectedTypeIndex == 0 ? 90 : 60) * dpiScale;
			Manager.NativeManager.BeginChild("##OutputType", new swig.Vec2(0, exportTypeChildHeight), true, swig.WindowFlags.MenuBar);
			if (Manager.NativeManager.BeginMenuBar())
			{
				if (Manager.NativeManager.BeginMenu(typeTitle + "##OutputTypeTitle"))
				{
					Manager.NativeManager.EndMenu();
				}

				Manager.NativeManager.EndMenuBar();
			}

			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

			Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("ExportedFrame"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.BeginCombo("###extype", selectedExportTypes[selectedTypeIndex], swig.ComboFlags.None))
			{
				for (int i = 0; i < selectedExportTypes.Length; i++)
				{
					if (Manager.NativeManager.Selectable(selectedExportTypes[i]))
					{
						selectedTypeIndex = i;
						Core.Recording.RecordingExporter.SetValueDirectly((Data.RecordingExporterType)i);
						Manager.NativeManager.SetItemDefaultFocus();
					}
				}

				Manager.NativeManager.EndCombo();
			}
			Manager.NativeManager.PopItemWidth();

			if (selectedTypeIndex == 0)
			{
				Manager.NativeManager.NextColumn();

				Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("XCount"));

				Manager.NativeManager.NextColumn();

				Manager.NativeManager.PushItemWidth(-1);
				if (Manager.NativeManager.DragInt("###tn", theNumberOfImageH_))
				{
					Core.Recording.RecordingHorizontalCount.SetValueDirectly(theNumberOfImageH_[0]);
				}
				Manager.NativeManager.PopItemWidth();
			}

			Manager.NativeManager.EndChild();

			// Recordingwindow
			Manager.NativeManager.BeginChild("##Option", new swig.Vec2(0, 60 * dpiScale), true, swig.WindowFlags.MenuBar);
			if (Manager.NativeManager.BeginMenuBar())
			{
				if (Manager.NativeManager.BeginMenu(optionsTitle + "##OptionTitle"))
				{
					Manager.NativeManager.EndMenu();
				}

				Manager.NativeManager.EndMenuBar();
			}

			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

			Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("MakeTransparent"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.BeginCombo("###tr", selectedAlphaTypes[selectedAlphaIndex], swig.ComboFlags.None))
			{
				for (int i = 0; i < selectedAlphaTypes.Length; i++)
				{
					if (Manager.NativeManager.Selectable(selectedAlphaTypes[i]))
					{
						selectedAlphaIndex = i;
						Core.Recording.RecordingTransparentMethod.SetValueDirectly((Data.RecordingTransparentMethodType)i);
						Manager.NativeManager.SetItemDefaultFocus();
					}
				}

				Manager.NativeManager.EndCombo();
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChild();

			// Storage
			Manager.NativeManager.BeginChild("##Storage", new swig.Vec2(0, 60 * dpiScale), true, swig.WindowFlags.MenuBar);
			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

			Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("StorageTarget"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.BeginCombo("###st", selectedStorageTargets[selectedStorageTarget], swig.ComboFlags.None))
			{
				for (int i = 0; i < selectedStorageTargets.Length; i++)
				{
					if (Manager.NativeManager.Selectable(selectedStorageTargets[i]))
					{
						selectedStorageTarget = i;
						Core.Recording.RecordingStorageTarget.SetValueDirectly((Data.RecordingStorageTargetTyoe)i);
						Manager.NativeManager.SetItemDefaultFocus();
					}
				}

				Manager.NativeManager.EndCombo();
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChild();

			mainRenderer.GuideWidth = Core.Recording.RecordingWidth.Value;
			mainRenderer.GuideHeight = Core.Recording.RecordingHeight.Value;
			mainRenderer.RendersGuide = Core.Recording.IsRecordingGuideShown;

			float buttonWidth = 100 * dpiScale;
			Manager.NativeManager.SetCursorPosX(Manager.NativeManager.GetContentRegionAvail().X / 2 - buttonWidth / 2);

			if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("Record") + "###btn", buttonWidth))
			{
				var during = Core.Recording.RecordingEndingFrame.Value - Core.Recording.RecordingStartingFrame.Value;
				if (during < 0)
				{
					swig.GUIManager.show("出力フレームが存在しません。", "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);

					//var mb = new Dialog.MessageBox();
					//mb.Show("Error", "出力フレームが存在しません。");
				}

				var count = during / Core.Recording.RecordingFrequency.Value + 1;
				var horizontalCount = Core.Recording.RecordingHorizontalCount.Value;

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
					else if (selectedTypeIndex == 4)
					{
						filter = "mp4";
					}

					var result = swig.FileDialog.SaveDialog(filter, System.IO.Directory.GetCurrentDirectory());

					if (string.IsNullOrEmpty(result)) return;

					var filename = result;

					if (System.IO.Path.GetExtension(filename) != "." + filter)
					{
						filename += "." + filter;
					}

					var viewer = Manager.Viewer;

					//if (viewer.LoadEffectFunc != null)
					{
						viewer.LoadEffectFunc();
					}

					var tempDirectory = System.IO.Directory.GetCurrentDirectory();
					System.IO.Directory.SetCurrentDirectory(Application.StartDirectory);

					Utils.Logger.Write(string.Format("SetCurrentDirectory : {0}", Application.StartDirectory));

					string errorMessage = string.Empty;

					var startingFrame = Core.Recording.RecordingStartingFrame.Value;
					var freq = Core.Recording.RecordingFrequency.Value;

					var recordingParameter = new swig.RecordingParameter();

					recordingParameter.Count = count;
					recordingParameter.HorizontalCount = horizontalCount;
					recordingParameter.Freq = freq;
					recordingParameter.OffsetFrame = startingFrame;
					recordingParameter.Transparence = (swig.TransparenceType)selectedAlphaIndex;
					recordingParameter.Scale = Core.Recording.RecordingScale.Value;

					errorMessage = MultiLanguageTextProvider.GetText("Error_FailedToSave");

					if (selectedTypeIndex == 0)
					{
						Utils.Logger.Write(string.Format("RecordSpriteSheet : {0}", filename));

						effectRecorder = viewer.RecordSpriteSheet(filename, recordingParameter);
					}
					else if (selectedTypeIndex == 1)
					{
						Utils.Logger.Write(string.Format("RecordSprite : {0}", filename));

						effectRecorder = viewer.RecordSprite(filename, recordingParameter);
					}
					else if (selectedTypeIndex == 2)
					{
						Utils.Logger.Write(string.Format("RecordAsGifAnimation : {0}", filename));

						effectRecorder = viewer.RecordAsGifAnimation(filename, recordingParameter);
					}
					else if (selectedTypeIndex == 3)
					{
						Utils.Logger.Write(string.Format("RecordAsAVI : {0}", filename));

						effectRecorder = viewer.RecordAsAVI(filename, recordingParameter);
					}
					else if (selectedTypeIndex == 4)
					{
						Utils.Logger.Write(string.Format("RecordAsH264 : {0}", filename));

						effectRecorder = viewer.RecordAsH264(filename, recordingParameter);
					}

					if (effectRecorder != null)
					{
						Manager.NativeManager.OpenPopup("###RecorderProgress");
					}
					else
					{
						swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);

						Utils.Logger.Write(string.Format("Record : Failed : {0}", errorMessage));
					}

					System.IO.Directory.SetCurrentDirectory(tempDirectory);

				}
			}

			Manager.NativeManager.SetNextWindowSize(320, 0, swig.Cond.Always);

			UpdateProgressDialog();
		}

		private void UpdateProgressDialog()
		{
			if (Manager.NativeManager.BeginPopupModal(MultiLanguageTextProvider.GetText("RecorderProgress") + "###RecorderProgress"))
			{
				float dpiScale = Manager.DpiScale;

				var viewer = Manager.Viewer;
				float progress;

				effectRecorder.Step(4);

				if (effectRecorder.IsCompleted())
				{
					effectRecorder.End();
					effectRecorder.Dispose();
					effectRecorder = null;

					progress = 1.0f;
					Manager.NativeManager.CloseCurrentPopup();
				}
				else
				{
					progress = effectRecorder.GetProgress();
				}

				Manager.NativeManager.PushItemWidth(-1);

				Manager.NativeManager.ProgressBar(progress, new swig.Vec2(-1, 0));

				Manager.NativeManager.PopItemWidth();

				Manager.NativeManager.Spacing();

				float buttonWidth = 100 * dpiScale;
				Manager.NativeManager.SetCursorPosX(Manager.NativeManager.GetContentRegionAvail().X / 2 - buttonWidth / 2 + 8 * dpiScale);
				if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("RecorderAbort"), buttonWidth))
				{
					effectRecorder.End();
					effectRecorder.Dispose();
					effectRecorder = null;
					Manager.NativeManager.CloseCurrentPopup();
				}

				Manager.NativeManager.EndPopup();
			}
		}
	}
}
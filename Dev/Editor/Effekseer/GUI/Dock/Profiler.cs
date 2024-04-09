using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Drawing;
using System.Security.Policy;

namespace Effekseer.GUI.Dock
{
	public class Profiler : DockPanel
	{
		private const int BufferLength = 5 * 60 * 60;
		private static readonly swig.Vec4 InstanceGraphColor = new swig.Vec4(0.8f, 0.8f, 0.8f, 1.0f);
		private static readonly swig.Vec4 CpuGraphColor = new swig.Vec4(0.4f, 0.65f, 0.8f, 1.0f);
		private static readonly swig.Vec4 GpuGraphColor = new swig.Vec4(0.8f, 0.4f, 0.65f, 1.0f);

		private struct Texts
		{
			public string Profiler = MultiLanguageTextProvider.GetText("Profiler");
			public string ProfilerStart = MultiLanguageTextProvider.GetText("Profiler_Start");
			public string ProfilerStop = MultiLanguageTextProvider.GetText("Profiler_Stop");
			public string ProfilerReset = MultiLanguageTextProvider.GetText("Profiler_Reset");
			public string ProfilerCpuUsage = MultiLanguageTextProvider.GetText("Profiler_CpuUsage");
			public string ProfilerGpuUsage = MultiLanguageTextProvider.GetText("Profiler_GpuUsage");
			public string ProfilerHandles = MultiLanguageTextProvider.GetText("Profiler_Handles");

			public string Network = MultiLanguageTextProvider.GetText("Network");
			public string NetworkConnected = MultiLanguageTextProvider.GetText("NetworkConnected");
			public string NetworkDisconnected = MultiLanguageTextProvider.GetText("NetworkDisconnected");

			public Texts() { }
		};
		private Texts texts = new Texts();

		class TimeValueChart
		{
			public string ID { get; private set; }
			public double[] TimeBuffer { get; private set; }
			public double[] ValueBuffer { get; private set; }
			public double[] ModifiedValueBuffer { get; private set; }
			public swig.Vec4 Color { get; set; }
			public double LatestValue { get; private set; } = 0.0;
			public int BufferOffset { get; private set; } = 0;
			public int FrameCount { get; private set; } = 0;

			public TimeValueChart(string idstr, int bufferLength, swig.Vec4 color, bool modifyValues)
			{
				ID = idstr;
				Color = color;
				TimeBuffer = new double[bufferLength];
				ValueBuffer = new double[bufferLength];
				if (modifyValues)
				{
					ModifiedValueBuffer = new double[bufferLength];
				}
			}

			public void Reset()
			{
				FrameCount = 0;
				BufferOffset = 0;
				LatestValue = 0.0;
				Array.Clear(TimeBuffer, 0, TimeBuffer.Length);
				Array.Clear(ValueBuffer, 0, ValueBuffer.Length);
				if (ModifiedValueBuffer != null)
				{
					Array.Clear(ModifiedValueBuffer, 0, ModifiedValueBuffer.Length);
				}
			}

			public void Push(double value)
			{
				LatestValue = value;
				TimeBuffer[BufferOffset] = FrameCount;
				ValueBuffer[BufferOffset] = LatestValue;

				FrameCount++;
				if (++BufferOffset >= ValueBuffer.Length)
				{
					BufferOffset = 0;
				}
			}

			public double GetMaxValue(int offset, int length)
			{
				var buffer = ModifiedValueBuffer ?? ValueBuffer;

				double maxValue = 0.0f;
				for (int i = 0; i < length; i++)
				{
					maxValue = Math.Max(maxValue, buffer[(offset + i) % buffer.Length]);
				}
				return maxValue;
			}

			public double GetMean(int offset, int length)
			{
				double sum = 0.0;
				for (int i = 0; i < length; i++)
				{
					int index = (offset + i) % ValueBuffer.Length;
					sum += ValueBuffer[index];
				}
				return sum / length;
			}

			public double GetStdDev(int offset, int length, double mean)
			{
				double stdDev = 0.0;
				for (int i = 0; i < length; i++)
				{
					int index = (offset + i) % ValueBuffer.Length;
					stdDev += Math.Pow(ValueBuffer[index] - mean, 2.0);
				}
				return Math.Sqrt(stdDev / length);
			}

			public void ModifyValues(int offset, int length, double mean, double stdDev)
			{
				for (int i = 0; i < length; i++)
				{
					int index = (offset + i) % ValueBuffer.Length;
					if (Math.Abs(ValueBuffer[index] - mean) <= 4 * stdDev)
					{
						ModifiedValueBuffer[index] = ValueBuffer[index];
					}
					else
					{
						ModifiedValueBuffer[index] = 0.0f;
					}
				}
			}

			public void DrawChart(swig.GUIManager gui)
			{
				var region = gui.GetContentRegionAvail();

				double minTime = FrameCount - region.X;
				double maxTime = FrameCount;

				int offset = (int)Math.Max(0, minTime);
				int length = (int)(maxTime - minTime);

				if (ModifiedValueBuffer != null)
				{
					double mean = GetMean(offset, length);
					double stdDev = GetStdDev(offset, length, mean);
					ModifyValues(offset, length, mean, stdDev);
				}
				var buffer = ModifiedValueBuffer ?? ValueBuffer;

				double minValue = 0.0;
				double maxValue = GetMaxValue(offset, length);

				if (gui.BeginPlot(ID, new swig.Vec2(-1, 140), swig.PlotFlags.NoInputs))
				{
					gui.SetupPlotAxis(swig.PlotAxis.X1, null, swig.PlotAxisFlags.None);
					gui.SetupPlotAxis(swig.PlotAxis.Y1, null, swig.PlotAxisFlags.None);
					gui.SetupPlotAxisLimits(swig.PlotAxis.X1, minTime, maxTime, swig.Cond.Always);
					gui.SetupPlotAxisLimits(swig.PlotAxis.Y1, minValue, maxValue, swig.Cond.Always);
					gui.SetNextPlotFillStyle(Color, 0.4f);
					gui.SetNextPlotLineStyle(Color, 1.0f);
					gui.PlotShaded(ID + "_Data", TimeBuffer, buffer, buffer.Length, 0.0, BufferOffset);
					gui.PlotLine(ID + "_Data", TimeBuffer, buffer, buffer.Length, BufferOffset);
					gui.EndPlot();
				}
			}
		}

		class EditorProfiler
		{
			public TimeValueChart CpuChart { get; private set; }
			public TimeValueChart GpuChart { get; private set; }

			public EditorProfiler()
			{
				CpuChart = new TimeValueChart("##CPUPlot", BufferLength, CpuGraphColor, true);
				GpuChart = new TimeValueChart("##GPUPlot", BufferLength, GpuGraphColor, true);
			}

			public void Reset()
			{
				CpuChart.Reset();
				GpuChart.Reset();
			}

			public void Update(swig.GUIManager gui, ref Texts texts)
			{
				CpuChart.Push(Manager.Viewer.EffectRenderer.GetCpuTime());
				GpuChart.Push(Manager.Viewer.EffectRenderer.GetGpuTime());

				if (gui.Button(texts.ProfilerReset))
				{
					Reset();
				}

				gui.Text(texts.ProfilerCpuUsage + ": " + CpuChart.LatestValue + "us");
				CpuChart.DrawChart(gui);

				gui.Text(texts.ProfilerGpuUsage + ": " + GpuChart.LatestValue + "us");
				GpuChart.DrawChart(gui);
			}
		};
		EditorProfiler editorProfiler = new EditorProfiler();

		class TargetProfiler
		{
			public class ManagerProfiler
			{
				public int Number { get; private set; }
				public TimeValueChart HandleCountChart { get; private set; }
				public TimeValueChart CpuTimeChart { get; private set; }
				public TimeValueChart GpuTimeChart { get; private set; }

				public ManagerProfiler(int number)
				{
					HandleCountChart = new TimeValueChart("##MgrHandlePlot_" + number, BufferLength, InstanceGraphColor, false);
					CpuTimeChart = new TimeValueChart("##MgrCpuPlot_" + number, BufferLength, CpuGraphColor, true);
					GpuTimeChart = new TimeValueChart("##MgrGpuPlot_" + number, BufferLength, GpuGraphColor, true);
				}

				public void Reset()
				{
					HandleCountChart.Reset();
					CpuTimeChart.Reset();
					GpuTimeChart.Reset();
				}

				public void Update(swig.GUIManager gui, ref Texts texts)
				{
					string titleLabel = string.Format("All Effects ({2}:{1})###AllEffects{0}", Number, HandleCountChart.LatestValue, texts.ProfilerHandles);
					if (gui.CollapsingHeader(titleLabel))
					{
						gui.Text(texts.ProfilerHandles + ": " + HandleCountChart.LatestValue);
						HandleCountChart.DrawChart(gui);
						gui.Text(texts.ProfilerCpuUsage + ": " + CpuTimeChart.LatestValue + "us");
						CpuTimeChart.DrawChart(gui);
						gui.Text(texts.ProfilerGpuUsage + ": " + GpuTimeChart.LatestValue + "us");
						GpuTimeChart.DrawChart(gui);
					}
				}

				public void PushData(swig.ProfileSample.Manager data)
				{
					HandleCountChart.Push(data.HandleCount);
					CpuTimeChart.Push(data.CpuTime);
					GpuTimeChart.Push(data.GpuTime);
				}
			}

			public class EffectProfiler
			{
				public string Key { get; private set; }
				public TimeValueChart HandleCountChart { get; private set; }
				public TimeValueChart GpuTimeChart { get; private set; }

				public EffectProfiler(string key)
				{
					Key = key;
					HandleCountChart = new TimeValueChart("##EfcHandlePlot_" + Key, BufferLength, InstanceGraphColor, false);
					GpuTimeChart = new TimeValueChart("##EfcGpuPlot_" + Key, BufferLength, GpuGraphColor, true);
				}

				public void Reset()
				{
					HandleCountChart.Reset();
					GpuTimeChart.Reset();
				}

				public void Update(swig.GUIManager gui, ref Texts texts)
				{
					string titleLabel = string.Format("{0} ({2}:{1})###Efc_{0}", Key, HandleCountChart.LatestValue, texts.ProfilerHandles);
					if (gui.CollapsingHeader(titleLabel))
					{
						gui.Text(texts.ProfilerHandles + ": " + HandleCountChart.LatestValue);
						HandleCountChart.DrawChart(gui);
						gui.Text(texts.ProfilerGpuUsage + ": " + GpuTimeChart.LatestValue + "us");
						GpuTimeChart.DrawChart(gui);
					}
				}

				public void PushData(swig.ProfileSample.Effect data)
				{
					HandleCountChart.Push(data.HandleCount);
					GpuTimeChart.Push(data.GpuTime);
				}
			}

			public List<ManagerProfiler> ManagerProfilers { get; private set; } = new List<ManagerProfiler>();
			public Dictionary<string, EffectProfiler> EffectProfilers { get; private set; } = new Dictionary<string, EffectProfiler>();
			public string FilterText { get; private set; }

			public void Reset()
			{
				foreach (var managerProfiler in ManagerProfilers)
				{
					managerProfiler.Reset();
				}
				EffectProfilers.Clear();
			}

			public void Update(swig.GUIManager gui, ref Texts texts)
			{
				if (gui.Button(texts.ProfilerReset))
				{
					Reset();
				}

				gui.SameLine();

				if (!Manager.Network.IsProfiling())
				{
					gui.BeginDisabled(!Manager.Network.IsConnected());

					if (gui.Button(texts.ProfilerStart))
					{
						if (!Manager.Network.IsConnected())
						{
							Manager.Network.Connect();
						}

						Manager.Network.StartProfiling();
					}

					gui.EndDisabled();
				}
				else
				{
					if (gui.Button(texts.ProfilerStop))
					{
						Manager.Network.StopProfiling();
					}
				}

				gui.SameLine();

				if (gui.IconButton(Icons.PanelNetwork))
				{
					Manager.SelectOrShowWindow(typeof(Dock.Network));
				}

				gui.SameLine();

				if (Manager.Network.IsConnected())
				{
					gui.Text(texts.NetworkConnected);
				}
				else
				{
					gui.Text(texts.NetworkDisconnected);
				}

				gui.PushItemWidth(-1);
				if (gui.InputTextWithHint("###ProfilerFilter", FilterText, "Filter..."))
				{
					FilterText = gui.GetInputTextResult().ToLower();
				}
				gui.PopItemWidth();

				while (Manager.Network.IsProfiling())
				{
					var profileSample = Manager.Network.ReadProfileSample();
					if (!profileSample.IsValid)
					{
						break;
					}

					while (ManagerProfilers.Count < profileSample.Managers.Count)
					{
						ManagerProfilers.Add(new ManagerProfiler(ManagerProfilers.Count));
					}
					while (ManagerProfilers.Count > profileSample.Managers.Count)
					{
						ManagerProfilers.RemoveAt(ManagerProfilers.Count - 1);
					}

					for (int i = 0; i < ManagerProfilers.Count; i++)
					{
						ManagerProfilers[i].PushData(profileSample.Managers[i]);
					}

					foreach (var effectProfileSample in profileSample.Effects)
					{
						if (!EffectProfilers.ContainsKey(effectProfileSample.Key))
						{
							EffectProfilers[effectProfileSample.Key] = new EffectProfiler(effectProfileSample.Key);
						}

						var effectProfiler = EffectProfilers[effectProfileSample.Key];
						effectProfiler.PushData(effectProfileSample);
					}
				}

				foreach (var managerProfiler in ManagerProfilers)
				{
					managerProfiler.Update(gui, ref texts);
				}
				foreach (var kv in EffectProfilers)
				{
					if (string.IsNullOrEmpty(FilterText) || kv.Key.ToLower().Contains(FilterText))
					{
						kv.Value.Update(gui, ref texts);
					}
				}
			}
		};
		TargetProfiler targetProfiler = new TargetProfiler();

		public Profiler()
		{
			Label = Icons.PanelProfiler + texts.Profiler + "###Profiler";
			DocPage = "profiler.html";
		}

		public override void OnDisposed()
		{
		}

		protected override void UpdateInternal()
		{
			var gui = Manager.NativeManager;

			if (gui.BeginTabBar("Mode"))
			{
				if (gui.BeginTabItem("Editor"))
				{
					editorProfiler.Update(gui, ref texts);
					gui.EndTabItem();
				}
				
				if (gui.BeginTabItem("Target"))
				{
					targetProfiler.Update(gui, ref texts);
					gui.EndTabItem();
				}

				gui.EndTabBar();
			}
		}
	}
}

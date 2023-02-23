using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Drawing;

namespace Effekseer.GUI.Dock
{
	public class Profiler : DockPanel
	{
		private const int BufferLength = 60 * 5;
		private static readonly swig.Vec4 InstanceGraphColor = new swig.Vec4(0.8f, 0.8f, 0.8f, 1.0f);
		private static readonly swig.Vec4 CpuGraphColor = new swig.Vec4(0.4f, 0.65f, 0.8f, 1.0f);
		private static readonly swig.Vec4 GpuGraphColor = new swig.Vec4(0.8f, 0.4f, 0.65f, 1.0f);

		private struct Texts
		{
			public string Profiler = Resources.GetString("Profiler");
			public string ProfilerStart = Resources.GetString("Profiler_Start");
			public string ProfilerStop = Resources.GetString("Profiler_Stop");
			public string ProfilerReset = Resources.GetString("Profiler_Reset");
			public string ProfilerCpuUsage = Resources.GetString("Profiler_CpuUsage");
			public string ProfilerGpuUsage = Resources.GetString("Profiler_GpuUsage");
			public string ProfilerHandles = Resources.GetString("Profiler_Handles");

			public string Network = Resources.GetString("Network");
			public string NetworkConnected = Resources.GetString("NetworkConnected");
			public string NetworkDisconnected = Resources.GetString("NetworkDisconnected");

			public Texts() { }
		};
		private Texts texts = new Texts();

		class TimeValueChart
		{
			public string ID { get; private set; }
			public double[] TimeBuffer { get; private set; }
			public double[] ValueBuffer { get; private set; }
			public swig.Vec4 Color { get; set; }
			public double MaxValue { get; set; } = 100.0;
			public double LatestValue { get; private set; } = 0.0;
			public int BufferOffset { get; private set; } = 0;
			public int FrameCount { get; private set; } = 0;

			public TimeValueChart(string idstr, int bufferLength, swig.Vec4 color)
			{
				ID = idstr;
				TimeBuffer = new double[bufferLength];
				ValueBuffer = new double[bufferLength];
				Color = color;
			}

			public void Reset()
			{
				FrameCount = 0;
				BufferOffset = 0;
				MaxValue = 100.0;
				LatestValue = 0.0;
				Array.Clear(TimeBuffer, 0, TimeBuffer.Length);
				Array.Clear(ValueBuffer, 0, ValueBuffer.Length);
			}

			public void Push(double value)
			{
				LatestValue = value;
				TimeBuffer[BufferOffset] = FrameCount;
				ValueBuffer[BufferOffset] = LatestValue;
				MaxValue = Math.Max(MaxValue, value);


				FrameCount++;
				if (++BufferOffset >= ValueBuffer.Length)
				{
					BufferOffset = 0;
				}
			}

			public void Update(swig.GUIManager gui)
			{
				double minTime = FrameCount - ValueBuffer.Length;
				double maxTime = FrameCount;

				if (gui.BeginPlot(ID, new swig.Vec2(-1, 140), swig.PlotFlags.NoInputs))
				{
					gui.SetupPlotAxis(swig.PlotAxis.X1, null, swig.PlotAxisFlags.None);
					gui.SetupPlotAxis(swig.PlotAxis.Y1, null, swig.PlotAxisFlags.None);
					gui.SetupPlotAxisLimits(swig.PlotAxis.X1, minTime, maxTime, swig.Cond.Always);
					gui.SetupPlotAxisLimits(swig.PlotAxis.Y1, 0, MaxValue, swig.Cond.Always);
					gui.SetNextPlotFillStyle(Color, 0.4f);
					gui.SetNextPlotLineStyle(Color, 1.0f);
					gui.PlotShaded(ID + "_Data", TimeBuffer, ValueBuffer, ValueBuffer.Length, 0.0, BufferOffset);
					gui.PlotLine(ID + "_Data", TimeBuffer, ValueBuffer, ValueBuffer.Length, BufferOffset);
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
				CpuChart = new TimeValueChart("##CPUPlot", BufferLength, CpuGraphColor);
				GpuChart = new TimeValueChart("##GPUPlot", BufferLength, GpuGraphColor);
			}

			public void Reset()
			{
				CpuChart.Reset();
				GpuChart.Reset();
			}

			public void Update(swig.GUIManager gui, ref Texts texts)
			{
				CpuChart.Push(Manager.Viewer.EffectRenderer.GetCPUTime());
				GpuChart.Push(Manager.Viewer.EffectRenderer.GetGPUTime());

				if (gui.Button(texts.ProfilerReset))
				{
					Reset();
				}

				gui.Text(texts.ProfilerCpuUsage + ": " + CpuChart.LatestValue + "us");
				CpuChart.Update(gui);

				gui.Text(texts.ProfilerGpuUsage + ": " + GpuChart.LatestValue + "us");
				GpuChart.Update(gui);
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
					HandleCountChart = new TimeValueChart("##MgrHandlePlot_" + number, BufferLength, InstanceGraphColor);
					HandleCountChart.MaxValue = 5;
					CpuTimeChart = new TimeValueChart("##MgrCpuPlot_" + number, BufferLength, CpuGraphColor);
					GpuTimeChart = new TimeValueChart("##MgrGpuPlot_" + number, BufferLength, GpuGraphColor);
				}

				public void Reset()
				{
					HandleCountChart.Reset();
					HandleCountChart.MaxValue = 5;
					CpuTimeChart.Reset();
					GpuTimeChart.Reset();
				}

				public void Update(swig.GUIManager gui, ref Texts texts)
				{
					string titleLabel = string.Format("Manager{0} ({2}:{1})###Manager{0}", Number, HandleCountChart.LatestValue, texts.ProfilerHandles);
					if (gui.CollapsingHeader(titleLabel))
					{
						gui.Text(texts.ProfilerHandles + ": " + HandleCountChart.LatestValue);
						HandleCountChart.Update(gui);
						gui.Text(texts.ProfilerCpuUsage + ": " + CpuTimeChart.LatestValue + "us");
						CpuTimeChart.Update(gui);
						gui.Text(texts.ProfilerCpuUsage + ": " + GpuTimeChart.LatestValue + "us");
						GpuTimeChart.Update(gui);
					}
				}

				public void PushData(swig.ProfileSample.Manager data)
				{
					HandleCountChart.Push(data.HandleCount);
					CpuTimeChart.Push(data.CPUTime);
					GpuTimeChart.Push(data.GPUTime);
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
					HandleCountChart = new TimeValueChart("##EfcHandlePlot_" + Key, BufferLength, InstanceGraphColor);
					HandleCountChart.MaxValue = 5;
					GpuTimeChart = new TimeValueChart("##EfcGpuPlot_" + Key, BufferLength, GpuGraphColor);
				}

				public void Reset()
				{
					HandleCountChart.Reset();
					HandleCountChart.MaxValue = 5;
					GpuTimeChart.Reset();
				}

				public void Update(swig.GUIManager gui, ref Texts texts)
				{
					string titleLabel = string.Format("{0} ({2}:{1})###Efc_{0}", Key, HandleCountChart.LatestValue, texts.ProfilerHandles);
					if (gui.CollapsingHeader(titleLabel))
					{
						gui.Text(texts.ProfilerHandles + ": " + HandleCountChart.LatestValue);
						HandleCountChart.Update(gui);
						gui.Text(texts.ProfilerGpuUsage + ": " + GpuTimeChart.LatestValue + "us");
						GpuTimeChart.Update(gui);
					}
				}

				public void PushData(swig.ProfileSample.Effect data)
				{
					HandleCountChart.Push(data.HandleCount);
					GpuTimeChart.Push(data.GPUTime);
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
				foreach (var kv in EffectProfilers)
				{
					kv.Value.Reset();
				}
			}

			public void Update(swig.GUIManager gui, ref Texts texts)
			{
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

				if (gui.Button(texts.Network))
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

				if (gui.Button(texts.ProfilerReset))
				{
					Reset();
				}

				gui.SameLine();

				gui.PushItemWidth(-1);
				if (gui.InputTextWithHint("###ProfilerFilter", FilterText, "Filter..."))
				{
					FilterText = gui.GetInputTextResult();
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
					kv.Value.Update(gui, ref texts);
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

			gui.Separator();

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

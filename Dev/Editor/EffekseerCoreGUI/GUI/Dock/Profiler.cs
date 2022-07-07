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

			public void Update(swig.GUIManager gui)
			{
				CpuChart.Push(Manager.Viewer.EffectRenderer.GetCPUTime());
				GpuChart.Push(Manager.Viewer.EffectRenderer.GetGPUTime());

				if (gui.Button("Reset"))
				{
					Reset();
				}

				gui.Text("CPU Usage: " + CpuChart.LatestValue + "us");
				CpuChart.Update(gui);

				gui.Text("GPU Usage: " + GpuChart.LatestValue + "us");
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

				public void Update(swig.GUIManager gui)
				{
					string titleLabel = string.Format("Manager{0} (Handles:{1})###Manager{0}", Number, HandleCountChart.LatestValue);
					if (gui.CollapsingHeader(titleLabel))
					{
						gui.Text("Handles: " + HandleCountChart.LatestValue);
						HandleCountChart.Update(gui);
						gui.Text("CPU Usage: " + CpuTimeChart.LatestValue + "us");
						CpuTimeChart.Update(gui);
						gui.Text("GPU Usage: " + GpuTimeChart.LatestValue + "us");
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

				public void Update(swig.GUIManager gui)
				{
					string titleLabel = string.Format("{0} (Handles:{1})###Efc_{0}", Key, HandleCountChart.LatestValue);
					if (gui.CollapsingHeader(titleLabel))
					{
						gui.Text("Handles: " + HandleCountChart.LatestValue);
						HandleCountChart.Update(gui);
						gui.Text("GPU Usage: " + GpuTimeChart.LatestValue + "us");
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

			public void Update(swig.GUIManager gui)
			{
				if (!Manager.Network.IsProfiling)
				{
					if (gui.Button("Start Profiling"))
					{
						if (!Manager.Network.IsConnected())
						{
							Manager.Network.Connect();
						}

						Manager.Network.StartProfiling();
					}
				}
				else
				{
					if (gui.Button("Stop Profiling"))
					{
						Manager.Network.StopProfiling();
					}
				}

				gui.SameLine();

				if (gui.Button("Network"))
				{
				}

				gui.SameLine();

				if (Manager.Network.IsConnected())
				{
					gui.Text(Resources.GetString("NetworkConnected"));
				}
				else
				{
					gui.Text(Resources.GetString("NetworkDisconnected"));
				}

				if (gui.Button("Reset"))
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

				while (Manager.Network.IsProfiling)
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
					managerProfiler.Update(gui);
				}
				foreach (var kv in EffectProfilers)
				{
					kv.Value.Update(gui);
				}
			}
		};
		TargetProfiler targetProfiler = new TargetProfiler();

		public Profiler()
		{
			Label = Icons.PanelProfiler + Resources.GetString("Profiler") + "###Profiler";
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
					editorProfiler.Update(gui);
					gui.EndTabItem();
				}
				
				if (gui.BeginTabItem("Target"))
				{
					targetProfiler.Update(gui);
					gui.EndTabItem();
				}

				gui.EndTabBar();
			}
		}
	}
}

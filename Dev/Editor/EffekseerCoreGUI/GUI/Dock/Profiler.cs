using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Drawing;

namespace Effekseer.GUI.Dock
{
	public class Profiler : DockPanel
	{
		class EditorProfiler
		{
			private readonly swig.Vec4 CpuGraphColor = new swig.Vec4(0.4f, 0.65f, 0.8f, 1.0f);
			private readonly swig.Vec4 GpuGraphColor = new swig.Vec4(0.8f, 0.4f, 0.65f, 1.0f);

			private const int BufferLength = 60 * 5;
			private double[] TimeLineBuffer = new double[BufferLength];
			private double[] CpuUsageBuffer = new double[BufferLength];
			private double[] GpuUsageBuffer = new double[BufferLength];
			private double CpuUsageMax = 100.0;
			private double GpuUsageMax = 100.0;
			private int BufferOffset = 0;
			private int FrameCount = 0;

			public void Reset()
			{
				FrameCount = 0;
				BufferOffset = 0;
				CpuUsageMax = 100.0;
				GpuUsageMax = 100.0;
				Array.Clear(TimeLineBuffer, 0, TimeLineBuffer.Length);
				Array.Clear(CpuUsageBuffer, 0, CpuUsageBuffer.Length);
				Array.Clear(GpuUsageBuffer, 0, GpuUsageBuffer.Length);
			}

			public void Update()
			{
				int cpuUsage = Manager.Viewer.EffectRenderer.GetCPUTime();
				int gpuUsage = Manager.Viewer.EffectRenderer.GetGPUTime();

				TimeLineBuffer[BufferOffset] = FrameCount;
				CpuUsageBuffer[BufferOffset] = cpuUsage;
				GpuUsageBuffer[BufferOffset] = gpuUsage;
				CpuUsageMax = Math.Max(CpuUsageMax, cpuUsage);
				GpuUsageMax = Math.Max(GpuUsageMax, gpuUsage);

				double timeMin = FrameCount - BufferLength;
				double timeMax = FrameCount;

				FrameCount++;
				if (++BufferOffset >= BufferLength)
				{
					BufferOffset = 0;
				}

				var gui = Manager.NativeManager;

				if (gui.Button("Reset"))
				{
					Reset();
				}

				gui.Text("CPU Usage: " + cpuUsage + "us");
				gui.BeginPlot("##CPUPlot", new swig.Vec2(-1, 140), swig.PlotFlags.NoInputs);
				gui.SetupPlotAxis(swig.PlotAxis.X1, null, swig.PlotAxisFlags.None);
				gui.SetupPlotAxis(swig.PlotAxis.Y1, null, swig.PlotAxisFlags.None);
				gui.SetupPlotAxisLimits(swig.PlotAxis.X1, timeMin, timeMax, swig.Cond.Always);
				gui.SetupPlotAxisLimits(swig.PlotAxis.Y1, 0, CpuUsageMax, swig.Cond.Always);
				gui.SetNextPlotFillStyle(CpuGraphColor, 0.4f);
				gui.SetNextPlotLineStyle(CpuGraphColor, 1.0f);
				gui.PlotShaded("##CPUPlotData", TimeLineBuffer, CpuUsageBuffer, BufferLength, 0.0, BufferOffset);
				gui.PlotLine("##CPUPlotData", TimeLineBuffer, CpuUsageBuffer, BufferLength, BufferOffset);
				gui.EndPlot();

				gui.Text("GPU Usage: " + gpuUsage + "us");
				gui.BeginPlot("##GPUPlot", new swig.Vec2(-1, 140), swig.PlotFlags.NoInputs);
				gui.SetupPlotAxis(swig.PlotAxis.X1, null, swig.PlotAxisFlags.None);
				gui.SetupPlotAxis(swig.PlotAxis.Y1, null, swig.PlotAxisFlags.None);
				gui.SetupPlotAxisLimits(swig.PlotAxis.X1, timeMin, timeMax, swig.Cond.Always);
				gui.SetupPlotAxisLimits(swig.PlotAxis.Y1, 0, GpuUsageMax, swig.Cond.Always);
				gui.SetNextPlotFillStyle(GpuGraphColor, 0.4f);
				gui.SetNextPlotLineStyle(GpuGraphColor, 1.0f);
				gui.PlotShaded("##GPUPlotData", TimeLineBuffer, GpuUsageBuffer, BufferLength, 0.0, BufferOffset);
				gui.PlotLine("##GPUPlotData", TimeLineBuffer, GpuUsageBuffer, BufferLength, BufferOffset);
				gui.EndPlot();
			}
		};
		EditorProfiler editorProfiler = new EditorProfiler();

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
					editorProfiler.Update();
					gui.EndTabItem();
				}
				//if (gui.BeginTabItem("Target"))
				//{
				//	gui.EndTabItem();
				//}

				gui.EndTabBar();
			}
		}
	}
}

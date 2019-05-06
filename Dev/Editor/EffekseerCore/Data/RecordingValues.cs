using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Data
{
	public class RecordingValues
	{
		[Undo(Undo = false)]
		[Shown(Shown = false)]
		public Value.Int RecordingWidth { get; private set; }

		[Undo(Undo = false)]
		[Shown(Shown = false)]
		public Value.Int RecordingHeight { get; private set; }

		[Undo(Undo = false)]
		[Shown(Shown = false)]
		[IO(Export = true, Import = true)]
		public Value.Boolean IsRecordingGuideShown { get; private set; }

		[Undo(Undo = false)]
		[Shown(Shown = false)]
		[IO(Export = true, Import = true)]
		public Value.Int RecordingStartingFrame { get; private set; }

		[Undo(Undo = false)]
		[Shown(Shown = false)]
		[IO(Export = true, Import = true)]
		public Value.Int RecordingEndingFrame { get; private set; }

		[Undo(Undo = false)]
		[Shown(Shown = false)]
		[IO(Export = true, Import = true)]
		public Value.Int RecordingFrequency { get; private set; }

		[Undo(Undo = false)]
		[Shown(Shown = false)]
		[IO(Export = true, Import = true)]
		public Value.Int RecordingHorizontalCount { get; private set; }

		[Undo(Undo = false)]
		[Shown(Shown = false)]
		[IO(Export = true, Import = true)]
		public Value.Enum<RecordingExporterType> RecordingExporter { get; private set; }

		[Undo(Undo = false)]
		[Shown(Shown = false)]
		[IO(Export = true, Import = true)]
		public Value.Enum<RecordingTransparentMethodType> RecordingTransparentMethod { get; private set; }

		[Undo(Undo = false)]
		[Shown(Shown = false)]
		[IO(Export = true, Import = true)]
		public Value.Enum<RecordingStorageTargetTyoe> RecordingStorageTarget { get; private set; }
		public RecordingValues()
		{
			RecordingWidth = new Value.Int(256);
			RecordingHeight = new Value.Int(256);
			IsRecordingGuideShown = new Value.Boolean(false);
			RecordingStartingFrame = new Value.Int(1);
			RecordingEndingFrame = new Value.Int(30);
			RecordingFrequency = new Value.Int(1);
			RecordingHorizontalCount = new Value.Int(4);
			RecordingExporter = new Value.Enum<RecordingExporterType>(Data.RecordingExporterType.Sprite);
			RecordingTransparentMethod = new Value.Enum<RecordingTransparentMethodType>(Data.RecordingTransparentMethodType.None);
			RecordingStorageTarget = new Value.Enum<RecordingStorageTargetTyoe>(Data.RecordingStorageTargetTyoe.Global);
		}
	}
}

using Effekseer.Data.Value;
using System.Collections.Generic;

namespace Effekseer.Data
{
	public class ExternalModelParameter
	{
		[Key(key = "EffectBehavior_ExternalModel_Path")]
		public Value.PathForModel Model { get; private set; }

		[Key(key = "EffectBehavior_ExternalModel_Translation")]
		public Value.Vector3D Translation { get; private set; }

		[Key(key = "EffectBehavior_ExternalModel_Rotation")]
		public Value.Vector3D Rotation { get; private set; }

		[Key(key = "EffectBehavior_ExternalModel_Scale")]
		public Value.Vector3D Scale { get; private set; }

		public ExternalModelParameter()
		{
			Model = new Value.PathForModel(null, new MultiLanguageString("ModelFilter"), false, string.Empty);
			Translation = new Value.Vector3D();
			Rotation = new Value.Vector3D();
			Scale = new Value.Vector3D(1.0f, 1.0f, 1.0f);
		}
	}

	public class ExternalModelValues : IEditableValueCollection
	{
		const string TreeNodeIdPrefix = "EffectBehavior_ExternalModel_";
		public const int SlotCount = 4;

		[IO(Export = true)]
		public ExternalModelParameter Model1 { get; private set; }

		[IO(Export = true)]
		public ExternalModelParameter Model2 { get; private set; }

		[IO(Export = true)]
		public ExternalModelParameter Model3 { get; private set; }

		[IO(Export = true)]
		public ExternalModelParameter Model4 { get; private set; }

		IReadOnlyList<ExternalModelParameter> _models;

		public ExternalModelValues()
		{
			Model1 = new ExternalModelParameter();
			Model2 = new ExternalModelParameter();
			Model3 = new ExternalModelParameter();
			Model4 = new ExternalModelParameter();

			_models = new[] { Model1, Model2, Model3, Model4 };

			foreach (var model in _models)
			{
				RegisterCallbacks(model);
			}
		}

		public EditableValue[] GetValues()
		{
			var values = new List<EditableValue>();

			for (int i = 0; i < _models.Count; i++)
			{
				var key = MultiLanguageTextProvider.GetText("EffectBehavior_ExternalModel_Path_Name");
				var editable = new EditableValue();
				editable.Value = _models[i];
				editable.Title = $"{key} {i + 1}";
				editable.IsUndoEnabled = true;
				editable.TreeNodeID = TreeNodeIdPrefix + (i + 1);
				editable.TreeNodeType = TreeNodeType.Small;
				values.Add(editable);
			}

			return values.ToArray();
		}

		public IEnumerable<ExternalModelParameter> GetModels()
		{
			return _models;
		}

		public void Reset()
		{
			foreach (var model in _models)
			{
				model.Model.SetAbsolutePathDirectly(string.Empty);
				model.Translation.X.SetValueDirectly(0.0f);
				model.Translation.Y.SetValueDirectly(0.0f);
				model.Translation.Z.SetValueDirectly(0.0f);
				model.Rotation.X.SetValueDirectly(0.0f);
				model.Rotation.Y.SetValueDirectly(0.0f);
				model.Rotation.Z.SetValueDirectly(0.0f);
				model.Scale.X.SetValueDirectly(1.0f);
				model.Scale.Y.SetValueDirectly(1.0f);
				model.Scale.Z.SetValueDirectly(1.0f);
			}
		}

		public event ChangedValueEventHandler OnChanged;

		void OnValueChanged(object sender, ChangedValueEventArgs e)
		{
			OnChanged?.Invoke(sender, e);
		}

		void RegisterCallbacks(ExternalModelParameter parameter)
		{
			parameter.Model.OnChanged += OnValueChanged;
			parameter.Translation.X.OnChanged += OnValueChanged;
			parameter.Translation.Y.OnChanged += OnValueChanged;
			parameter.Translation.Z.OnChanged += OnValueChanged;
			parameter.Rotation.X.OnChanged += OnValueChanged;
			parameter.Rotation.Y.OnChanged += OnValueChanged;
			parameter.Rotation.Z.OnChanged += OnValueChanged;
			parameter.Scale.X.OnChanged += OnValueChanged;
			parameter.Scale.Y.OnChanged += OnValueChanged;
			parameter.Scale.Z.OnChanged += OnValueChanged;
		}
	}
}

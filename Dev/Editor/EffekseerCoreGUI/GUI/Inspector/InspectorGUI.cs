using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;
using Effekseer.GUI.BindableComponent;
using Effekseer.Asset;

namespace Effekseer.GUI.Inspector
{
	class InspectorGuiResult
	{
		public bool isEdited;
		public object value;
	}

	// Œ^î•ñ‚ÆGui‚ğ•\¦‚·‚éŠÖ”‚ğ•R‚Ã‚¯‚éƒNƒ‰ƒX
	class InspectorGuiDictionary
	{
		private Dictionary<Type, Func<object, InspectorGuiState, InspectorGuiResult>> FuncDictionary { get; }

		private Dictionary<Type, Func<object, string, InspectorGuiState, InspectorGuiResult>> DropFuncDictionary { get; }

		public InspectorGuiDictionary()
		{
			// Œ^î•ñ‚ÆGui‚ğ•\¦‚·‚éŠÖ”‚ğ•R‚Ã‚¯‚é
			FuncDictionary = new Dictionary<Type, Func<object, InspectorGuiState, InspectorGuiResult>>
			{
				{ typeof(bool), Widgets.Widgets.GuiBool },
				{ typeof(int), Widgets.Widgets.GuiInt },
				{ typeof(Asset.Int), Widgets.Widgets.GuiInt },
				{ typeof(Asset.IntWithInfinite), Widgets.Widgets.GuiIntWithInfinite },
				{ typeof(Asset.IntWithRange), Widgets.Widgets.GuiIntWithRange },
				{ typeof(float), Widgets.Widgets.GuiFloat },
				{ typeof(Asset.Float), Widgets.Widgets.GuiFloat },
				{ typeof(Asset.FloatWithRange), Widgets.Widgets.GuiFloatWithRange },
				{ typeof(string), Widgets.Widgets.GuiString },
				{ typeof(Vector2I), Widgets.Widgets.GuiVector2I },
				{ typeof(Vector2F), Widgets.Widgets.GuiVector2F },
				{ typeof(Asset.Vector2WithRange), Widgets.Widgets.GuiVector2WithRange },
				{ typeof(Vector3F), Widgets.Widgets.GuiVector3F },
				{ typeof(Asset.Vector3), Widgets.Widgets.GuiVector3F },
				{ typeof(Vector3WithRange), Widgets.Widgets.GuiVector3WithRange },
				{ typeof(Vector4), Widgets.Widgets.GuiVector4F },
				{ typeof(System.Enum), Widgets.Widgets.GuiEnum },
				{ typeof(Asset.Gradient), Widgets.Gradient.GuiGradient },
				{ typeof(Asset.Color), Widgets.Color.Update },
				{ typeof(Asset.TextureAsset), Widgets.Texture.Update},
            };

			DropFuncDictionary = new Dictionary<Type, Func<object, string, InspectorGuiState, InspectorGuiResult>> {
				{ typeof(Asset.TextureAsset), Widgets.Texture.Dropped},
			};
		}

		public bool HasFunction(Type type)
		{
			return FuncDictionary.ContainsKey(type);
		}

		public bool HasDropFunction(Type type)
		{
			return DropFuncDictionary.ContainsKey(type);
		}

		public Func<object, InspectorGuiState, InspectorGuiResult> GetFunction(Type type)
		{
			return FuncDictionary[type];
		}

		public Func<object, string, InspectorGuiState, InspectorGuiResult> GetDropFunction(Type type)
		{
			return DropFuncDictionary[type];
		}
	}
}

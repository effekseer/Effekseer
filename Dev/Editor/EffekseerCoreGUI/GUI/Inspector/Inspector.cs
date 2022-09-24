using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;
using Effekseer.GUI.Component;

namespace Effekseer.GUI.Inspector
{
	public class InspectorPanel : Dock.DockPanel
	{
		InspectorEditable target;

		EditorState editorState;
		NodeTreeGroupContext context;

		public InspectorPanel()
		{
			Label = "Inspector###Inspector";

			editorState = new EditorState();

			editorState.Env = new Environment();
			editorState.PartsList = new PartsList();
			editorState.PartsList.Renew();

			context = new NodeTreeGroupContext();
			context.New(typeof(InspectorEditable), editorState);

			// Force select
			editorState.SelectedNode = context.NodeTree.Root as InspectorEditable;
			target = editorState.SelectedNode as InspectorEditable;
		}

		protected override void UpdateInternal()
		{
			context.CommandManager.StartEditFields(context.NodeTreeGroup, context.NodeTree, editorState.SelectedNode, editorState.Env);

			Inspector.Update(context, editorState, target);

			context.CommandManager.EndEditFields(editorState.SelectedNode, editorState.Env);

			// Fix edited results when values are not edited
			if (!Manager.NativeManager.IsAnyItemActive())
			{
				context.CommandManager.SetFlagToBlockMergeCommands();
			}
		}
	}

	// 型情報とGuiを表示する関数を紐づけるクラス
	class InspectorGuiDictionary
	{
		private Dictionary<Type, Func<object, string, FieldInfo, InspectorEditable, bool>> FuncDictionary { get; }

		public InspectorGuiDictionary()
		{
			// 型情報とGuiを表示する関数を紐づける
			FuncDictionary = new Dictionary<Type, Func<object, string, FieldInfo, InspectorEditable, bool>>
			{
				{ typeof(int), GuiInt },
				{ typeof(float), GuiFloat },
				{ typeof(string), GuiString },
				{ typeof(Vector3D), GuiVector3D },
			};
		}

		public bool HasFunction(Type type)
		{
			return FuncDictionary.ContainsKey(type);
		}

		public Func<object, string, FieldInfo, InspectorEditable, bool> GetFunction(Type type)
		{
			return FuncDictionary[type];
		}

		private bool GuiInt(object value, string name, FieldInfo field, InspectorEditable target)
		{
			// ver.1
#if false
			if (value is int i)
			{
				int[] v = new[] { i };
				if (Manager.NativeManager.DragInt(name + "###" + field.GetHashCode(), v, 1))
				{
					field.SetValue(target, v[0]);
					return true;
				}
			}
			return false;
#else
			// ver.2
			// SetValueを関数側で処理したいなら配列かどうかで分岐しないと無理
			// 引数のobjectは int or int[]　を前提とする
			if (value is int iValue)
			{
				int[] v = new[] { iValue };
				if (Manager.NativeManager.DragInt(name + "###" + field.Name, v, 1))
				{
					field.SetValue(target, v[0]);
					return true;
				}
			}
			else if (value is int[] iArray)
			{
				bool changed = false;
				for (int i = 0; i < iArray.Length; ++i)
				{
					int[] v = new[] { iArray[i] };
					if (Manager.NativeManager.DragInt(i + ": " + name + "###" + field.Name + i, v, 1))
					{
						iArray[i] = v[0];
						field.SetValue(target, iArray);
						changed = true;
					}
				}
				return changed;
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiInt");
			}
			return false;
#endif
		}
		private bool GuiInt2(object[] value, string name, FieldInfo field, InspectorEditable target)
		{
			for (int i = 0; i < value.Length; ++i)
			{
				if (value[i] is int iValue)
				{
					int[] v = new[] { iValue };
					if (Manager.NativeManager.DragInt(name + "###" + field.Name + i, v, 1))
					{
						field.SetValue(target, v);
						return true;
					}
				}
			}
			return false;
		}
		private bool GuiFloat(object value, string name, FieldInfo field, InspectorEditable target)
		{
			if (value is float fValue)
			{
				float[] v = new[] { fValue };
				if (Manager.NativeManager.DragFloat(name + "###" + field.Name, v, .1f))
				{
					field.SetValue(target, v[0]);
					return true;
				}
			}
			else if (value is float[] fArray)
			{
				bool changed = false;
				for (int i = 0; i < fArray.Length; ++i)
				{
					float[] v = new[] { fArray[i] };
					if (Manager.NativeManager.DragFloat(i + ": " + name + "###" + field.Name + i, v, .1f))
					{
						fArray[i] = v[0];
						field.SetValue(target, fArray);
						changed = true;
					}
				}
				return changed;
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiFloat");
			}
			return false;
		}
		private bool GuiString(object value, string name, FieldInfo field, InspectorEditable target)
		{
			if (value is string sValue)
			{
				if (Manager.NativeManager.InputText(name + "###" + field.Name, sValue))
				{
					field.SetValue(target, Manager.NativeManager.GetInputTextResult());
					return true;
				}
			}
			else if (value is string[] sArray)
			{
				bool changed = false;
				for (int i = 0; i < sArray.Length; ++i)
				{
					if(sArray[i] == null)
					{
						continue;
					}
					if (Manager.NativeManager.InputText(i + ": " + name + "###" + field.Name + i, sArray[i]))
					{
						sArray[i] = Manager.NativeManager.GetInputTextResult();
						field.SetValue(target, sArray);
						changed = true;
					}
				}
				return changed;
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiString");
			}
			return false;
		}

		private bool GuiVector3D(object value, string name, FieldInfo field, InspectorEditable target)
		{

			if (value is Vector3D vec3Value)
			{
				FieldInfo fieldInternalValue = vec3Value.GetType().GetField("internalValue", BindingFlags.NonPublic | BindingFlags.Instance);
				float[] internalValue = (float[])fieldInternalValue.GetValue(vec3Value);

				if (Manager.NativeManager.DragFloat3EfkEx(name + "###" + field.Name, internalValue, 1.0f,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					"X:" + Core.Option.GetFloatFormat(), "Y:" + Core.Option.GetFloatFormat(), "Z:" + Core.Option.GetFloatFormat()))
				{
					field.SetValue(target, vec3Value);
					return true;
				}
			}
			else if (value is Vector3D[] vec3Array)
			{
				bool changed = false;
				for (int i = 0; i < vec3Array.Length; ++i)
				{
					FieldInfo fieldInternalValue = vec3Array[i].GetType().GetField("internalValue", BindingFlags.NonPublic | BindingFlags.Instance);
					float[] internalValue = (float[])fieldInternalValue.GetValue(vec3Array[i]);

					if (Manager.NativeManager.DragFloat3EfkEx(name + "###" + field.Name + i, internalValue, 1.0f,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					"X:" + Core.Option.GetFloatFormat(), "Y:" + Core.Option.GetFloatFormat(), "Z:" + Core.Option.GetFloatFormat()))
					{
						changed = true;
					}
				}
				
				if (changed)
				{
					field.SetValue(target, vec3Array);
				}
				return changed;
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiVector3D");
			}

			return false;
		}
	}

	class Inspector
	{
		// Gui表示を登録する
		private static readonly InspectorGuiDictionary GuiDictionary = new InspectorGuiDictionary();

		public static void Update(NodeTreeGroupContext context, EditorState editorState, InspectorEditable target)
		{
			var commandManager = context.CommandManager;
			var nodeTreeGroup = context.NodeTreeGroup;
			var nodeTree = context.NodeTree;
			var partsList = editorState.PartsList;
			var env = editorState.Env;
			var nodeTreeGroupEditorProperty = context.EditorProperty;

			var getterSetters = new FieldGetterSetter[1];
			getterSetters[0] = new FieldGetterSetter();

			// エディタに表示する変数群
			var fields = target.GetType().GetFields();

			foreach (var field in fields)
			{
				getterSetters[0].Reset(target, field);
				var prop = context.EditorProperty.Properties.FirstOrDefault(_ => _.InstanceID == target.InstanceID);
				bool isValueChanged = false;
				if (prop != null)
				{
					// 配列の変更が取れないのは別インスタンスに上書きされてるから？
					// 配列のコピーではなく、配列の要素のコピーにすべき
					isValueChanged = prop.IsValueEdited(getterSetters.Select(_ => _.GetName()).ToArray());
				}

				var getterSetter = getterSetters.Last();
				var value = getterSetter.GetValue();
				var name = getterSetter.GetName();

				if (value == null)
				{
					// TODO : nullどうする？
					Manager.NativeManager.Text("null : " + field.GetType().ToString());
					continue;
				}

				if (isValueChanged)
				{
					name = "*" + name;
				}
				else
				{
					name = " " + name;
				}

				// 配列かリストの時、エレメントの型を取得する
				var valueType = value.GetType();
				if (valueType.IsArray)
				{
					valueType = valueType.GetElementType();
				}
				else if (valueType.IsGenericType && valueType.GetGenericTypeDefinition() == typeof(List<>))
				{
					valueType = valueType.GetGenericArguments()[0];
				}

				// TODO : 基底クラスNodeのpublic List<Node> Children = new List<Node>();が反応してる
				if (GuiDictionary.HasFunction(valueType))
				{
					var func = GuiDictionary.GetFunction(valueType);

					// 配列かどうかは関数内でやる
					if (func(value, name, field, target))
					{
						context.CommandManager.NotifyEditFields(target);
					}
				}
				else
				{
					Manager.NativeManager.Text("No Regist : " + value.GetType().ToString() + " " + name);
				}
			}
		}
	}

	class InspectorEditable : Node
	{
		public int Int1 = 0;
		public float Float1 = 0.0f;
		public string String1 = "text";

		// 配列
		// 配列を関数側で対応するのか、呼び出し側でどうにかするのか
		// 試した感じ呼び出し側で制御した方が楽そう
		public int[] IntArray = new int[2];
		public float[] FloatArray = new float[2];
		public string[] StringArray = new string[2] { "hoge", "fuga" };

		// 全てのコレクションに対しプログラムをする方法なかったっけ
		// TODO : List
		public List<int> ListInt1 = new List<int>{ 2, 3 };

		// TODO : Vector
		public Vector2D vector2 = new Vector2D();
		public Vector3D vector3 = new Vector3D();
		public Vector3D[] vector3Array = new Vector3D[2] { new Vector3D(), new Vector3D() };

		// TODO : string等、nullをどう扱うか
		public string String2;

		// TODO : 特殊な型
		Gradient Gradient1 = new Gradient();
		Dock.FCurves FCurves = new Dock.FCurves();

	}
}

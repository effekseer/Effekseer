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

	// �^����Gui��\������֐���R�Â���N���X
	class InspectorGuiDictionary
	{
		private Dictionary<Type, Func<object, string, FieldInfo, InspectorEditable, bool>> FuncDictionary { get; }

		public InspectorGuiDictionary()
		{
			// �^����Gui��\������֐���R�Â���
			FuncDictionary = new Dictionary<Type, Func<object, string, FieldInfo, InspectorEditable, bool>>
			{
				{ typeof(int), GuiInt },
				{ typeof(float), GuiFloat },
				{ typeof(string), GuiString },
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
			// SetValue���֐����ŏ����������Ȃ�z�񂩂ǂ����ŕ��򂵂Ȃ��Ɩ���
			// ������object�� int or int[]�@��O��Ƃ���
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
	}

	class Inspector
	{
		// Gui�\����o�^����
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

			// �G�f�B�^�ɕ\������ϐ��Q
			var fields = target.GetType().GetFields();

			foreach (var field in fields)
			{
				getterSetters[0].Reset(target, field);
				var prop = context.EditorProperty.Properties.FirstOrDefault(_ => _.InstanceID == target.InstanceID);
				bool isValueChanged = false;
				if (prop != null)
				{
					// �z��̕ύX�����Ȃ��͕̂ʃC���X�^���X�ɏ㏑������Ă邩��H
					// �z��̃R�s�[�ł͂Ȃ��A�z��̗v�f�̃R�s�[�ɂ��ׂ�
					isValueChanged = prop.IsValueEdited(getterSetters.Select(_ => _.GetName()).ToArray());
				}

				var getterSetter = getterSetters.Last();
				var value = getterSetter.GetValue();
				var name = getterSetter.GetName();

				if (value == null)
				{
					// TODO : null�ǂ�����H
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

				// �z�񂩃��X�g�̎��A�G�������g�̌^���擾����
				var valueType = value.GetType();
				if (valueType.IsArray)
				{
					valueType = valueType.GetElementType();
				}
				else if (valueType.IsGenericType && valueType.GetGenericTypeDefinition() == typeof(List<>))
				{
					valueType = valueType.GetGenericArguments()[0];
				}

				// TODO : ���N���XNode��public List<Node> Children = new List<Node>();���������Ă�
				if (GuiDictionary.HasFunction(valueType))
				{
					var func = GuiDictionary.GetFunction(valueType);

					// �z�񂩂ǂ����͊֐����ł��
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

		// �z��
		// �z����֐����őΉ�����̂��A�Ăяo�����łǂ��ɂ�����̂�
		// �����������Ăяo�����Ő��䂵�������y����
		public int[] IntArray = new int[2];
		public float[] FloatArray = new float[2];
		public string[] StringArray = new string[2] { "hoge", "fuga" };

		// �S�ẴR���N�V�����ɑ΂��v���O������������@�Ȃ���������
		// TODO : List
		public List<int> ListInt1 = new List<int>{ 2, 3 };

		// TODO : Vector
		public Vector2D vector2 = new Vector2D();
		public Vector3D vector3 = new Vector3D();

		// TODO : string���Anull���ǂ�������
		public string String2;

		// TODO : ����Ȍ^
		Gradient Gradient1 = new Gradient();
		Dock.FCurves FCurves = new Dock.FCurves();

	}
}

using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;
using Effekseer.GUI.BindableComponent;

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


	class InspectorGuiInfo
	{
		public InspectorGuiInfo()
		{
			Id = new string("###" + Manager.GetUniqueID().ToString());
		}

		public string Id { get; private set; } = "";

		// �t�B�[���h���z��Ȃǂł������Ƃ��A���̊e�v�f�ɂ��Ċi�[����
		public List<InspectorGuiInfo> subElement = new List<InspectorGuiInfo>();
	}

	class Inspector
	{
		// Gui�\����o�^����
		private static readonly InspectorGuiDictionary GuiDictionary = new InspectorGuiDictionary();
		
		private static List<InspectorGuiInfo> FieldGuiInfoList = new List<InspectorGuiInfo>();

		private static InspectorEditable LastTarget = null;

		private static void GenerateFieldGuiIds(InspectorEditable target)
		{
			if (FieldGuiInfoList == null)
			{
				FieldGuiInfoList = new List<InspectorGuiInfo>();
			}
			FieldGuiInfoList.Clear();

			var fields = target.GetType().GetFields();
			foreach (var field in fields)
			{
				FieldGuiInfoList.Add(new InspectorGuiInfo());
			}
		}

		private static void UpdateObject(NodeTreeGroupContext context, EditorState editorState, Node targetNode, object targetObject)
		{
			var getterSetters = new FieldGetterSetter[1];
			getterSetters[0] = new FieldGetterSetter();

			// �G�f�B�^�ɕ\������ϐ��Q
			var fields = targetObject.GetType().GetFields();

			for (int i = 0; i < fields.Length; ++i)
			{
				var field = fields[i];
				field.GetCustomAttributes();
				getterSetters[0].Reset(targetObject, field);
				//var prop = context.EditorProperty.Properties.FirstOrDefault(_ => _.InstanceID == target.InstanceID);
				bool isValueChanged = false;
				//if (prop != null)
				//{
				//	// �z��̕ύX�����Ȃ��͕̂ʃC���X�^���X�ɏ㏑������Ă邩��H
				//	// �z��̃R�s�[�ł͂Ȃ��A�z��̗v�f�̃R�s�[�ɂ��ׂ�
				//	isValueChanged = prop.IsValueEdited(getterSetters.Select(_ => _.GetName()).ToArray());
				//}

				var getterSetter = getterSetters.Last();
				var value = getterSetter.GetValue();
				var name = getterSetter.GetName();

				if (value == null)
				{
					// TODO : null�ǂ�����H
					//Manager.NativeManager.Text("null : " + field.GetType().ToString());
					continue;
				}

				var attr = (KeyAttribute)field.GetCustomAttribute(typeof(KeyAttribute));
				string description = string.Empty;
				if (attr != null)
				{
					string key = attr.key + "_Name";

					if (MultiLanguageTextProvider.HasKey(key))
					{
						name = MultiLanguageTextProvider.GetText(key);
					}

					key = attr.key + "_Desc";
					if (MultiLanguageTextProvider.HasKey(key))
					{
						description = MultiLanguageTextProvider.GetText(key);
					}
				}



				if (isValueChanged)
				{
					name = "*" + name;
				}
				else
				{
					name = " " + name;
				}

				// display name(left side of table)
				Manager.NativeManager.TableNextRow();
				Manager.NativeManager.TableNextColumn();
				// TODO : Separator�ŋ�؂�̂�Asset�Ȃǂ̒P�ʂɂ���
				// Make not separate first row
				if (Manager.NativeManager.TableGetRowIndex() >= 2)
				{
					Manager.NativeManager.Separator();
				}
				Manager.NativeManager.Text(name);
				// Description
				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.BeginTooltip();

					Manager.NativeManager.Text(name);
					Manager.NativeManager.Separator();
					Manager.NativeManager.Text(description);

					Manager.NativeManager.EndTooltip();
				}


				// �z�񂩃��X�g�̎��A�G�������g�̌^���擾����
				var valueType = value.GetType();
				bool isArray = valueType.IsArray;
				if (isArray)
				{
					valueType = valueType.GetElementType();
				}
				else if (valueType.IsGenericType && valueType.GetGenericTypeDefinition() == typeof(List<>))
				{
					valueType = valueType.GetGenericArguments()[0];
				}

				// display field(right side of table)
				Manager.NativeManager.TableNextColumn();
				// TODO : Separator�ŋ�؂�̂�Asset�Ȃǂ̒P�ʂɂ���
				// Make not separate first row
				if (Manager.NativeManager.TableGetRowIndex() >= 2)
				{
					Manager.NativeManager.Separator();
				}

				var guiFunctionKey = valueType;

				// the key about enums gui function is System.Enum
				if (valueType.IsEnum)
				{
					guiFunctionKey = typeof(System.Enum);
				}

				// TODO : make ignore "public List<Node> Children = new List<Node>();" node member.
				if (GuiDictionary.HasFunction(guiFunctionKey))
				{
					var func = GuiDictionary.GetFunction(guiFunctionKey);

					if (isArray)
					{
						Array arrayValue = (Array)value;

						// GuiId������Ȃ���΁A���ׂčĐ���
						// GenerateFieldGuiIds�̒��ł�肽�����A�^��񂩂�͗v�f����������Ȃ��̂ł����Ő����B
						if (arrayValue.GetLength(0) > FieldGuiInfoList[i].subElement.Count())
						{
							FieldGuiInfoList[i].subElement.Clear();

							foreach (var v in arrayValue)
							{
								FieldGuiInfoList[i].subElement.Add(new InspectorGuiInfo());
							}
						}

						int j = 0;
						bool isEdited = false;
						foreach (var v in arrayValue)
						{
							string id = FieldGuiInfoList[i].subElement[j].Id;
							var result = func(v, id);
							if (result.isEdited)
							{
								if (valueType.IsValueType)
								{
									arrayValue.SetValue(result.value, j);
								}
								isEdited = true;
							}
							++j;
						}
						if (isEdited)
						{
							field.SetValue(targetObject, arrayValue);
							context.CommandManager.NotifyEditFields(targetNode);
						}
					}
					else
					{
						string id = FieldGuiInfoList[i].Id;
						var result = func(value, id);
						if (result.isEdited)
						{
							field.SetValue(targetObject, result.value);
							context.CommandManager.NotifyEditFields(targetNode);
						}
					}
				}
				else
				{
					Manager.NativeManager.Text("No Regist : " + value.GetType().ToString() + " " + name);
				}
			}
		}

		public static void Update(NodeTreeGroupContext context, EditorState editorState, InspectorEditable targetNode)
		{
			var commandManager = context.CommandManager;
			var nodeTreeGroup = context.NodeTreeGroup;
			var nodeTree = context.NodeTree;
			var partsList = editorState.PartsList;
			var env = editorState.Env;
			var nodeTreeGroupEditorProperty = context.EditorProperty;



			// Generate field GUI IDs when target is selected or changed.
			// TODO : �����^�[�Q�b�g�ɑΉ��ł��Ă��Ȃ�
			if ((LastTarget == null && targetNode != null) || (targetNode.InstanceID != LastTarget.InstanceID))
			{
				GenerateFieldGuiIds(targetNode);
			}
			LastTarget = targetNode;
			if (Manager.NativeManager.BeginTable("Table", 2, 
				swig.TableFlags.Resizable |
				swig.TableFlags.BordersInnerV | swig.TableFlags.BordersOuterH |
				swig.TableFlags.SizingFixedFit | swig.TableFlags.SizingStretchProp |
				swig.TableFlags.NoSavedSettings))
			{
				// �A�C�e���̕����ő�ɐݒ�
				Manager.NativeManager.TableNextRow();
				Manager.NativeManager.TableSetColumnIndex(0);
				Manager.NativeManager.PushItemWidth(-1);
				Manager.NativeManager.TableSetColumnIndex(1);
				Manager.NativeManager.PushItemWidth(-1);

				UpdateObject(context, editorState, targetNode, targetNode);
			}
			Manager.NativeManager.EndTable();
			Manager.NativeManager.Separator();
		}
	}


	class InspectorEditable : Node
	{
		public string Name = string.Empty;

		//public EffectAsset.PositionParameter PositionParam = new EffectAsset.PositionParameter();
		//public EffectAsset.RotationParameter RotationParam = new EffectAsset.RotationParameter();


		// Attributes
		[Key(key = "Position_NurbsCurveParameter_Scale")]
		public float scale = 1.0f;
		[Key(key = "Position_FixedParamater_Location")]
		public Vector3F vector3f = new Vector3F();

		// Enum
		[EffectAsset.VisiblityController(ID = 100)]
		public ParamaterType Type = ParamaterType.Fixed;
		public enum ParamaterType : int
		{
			[Key(key = "Position_ParamaterType_Fixed")]
			Fixed = 0,
			[Key(key = "Position_ParamaterType_PVA")]
			PVA = 1,
			[Key(key = "Position_ParamaterType_Easing")]
			Easing = 2,
			[Key(key = "Position_ParamaterType_LocationFCurve")]
			LocationFCurve = 3,
			[Key(key = "Position_ParameterType_NurbsCurve")]
			NurbsCurve = 4,
			[Key(key = "Position_ParameterType_ViewOffset")]
			ViewOffset = 5,
		}



		// primitive types
		public int Int1 = 0;
		public float Float1 = 0.0f;
		public string String1 = "text";

		// �z��
		// �z����֐����őΉ�����̂��A�Ăяo�����łǂ��ɂ�����̂�
		// �����������Ăяo�����Ő��䂵�������y����
		public int[] IntArray = new int[2];
		public float[] FloatArray = new float[5];
		public string[] StringArray = new string[2] { "hoge", "fuga" };

		// �S�ẴR���N�V�����ɑ΂��v���O������������@�Ȃ���������
		// TODO : List
		public List<int> ListInt1 = new List<int> { 2, 3 };

		// TODO : Vector
		public Vector2D vector2 = new Vector2D();
		public Vector3D vector3 = new Vector3D();



		public Vector3D[] vector3Array = new Vector3D[2] { new Vector3D(), new Vector3D() };

		// TODO : string���Anull���ǂ�������
		public string String2;

		// TODO : ����Ȍ^
		Gradient Gradient1 = new Gradient();
		Dock.FCurves FCurves = new Dock.FCurves();

	}
}

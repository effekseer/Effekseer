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

		public InspectorPanel()
		{
			Label = "Inspector###Inspector";
		}

		protected override void UpdateInternal()
		{
			if (CoreContext.SelectedEffect.Context.NodeTree == null ||
				CoreContext.SelectedEffectNode == null)
			{
				return;
			}

			CoreContext.SelectedEffect.Context.CommandManager.StartEditFields(
				CoreContext.SelectedEffect.Asset.NodeTreeAsset,
				CoreContext.SelectedEffect.Context.NodeTree,
				CoreContext.SelectedEffectNode,
				CoreContext.Environment);

			Inspector.Update(CoreContext.SelectedEffect.Context,
				CoreContext.SelectedEffectNode);

			CoreContext.SelectedEffect.Context.CommandManager.EndEditFields(
				CoreContext.SelectedEffectNode,
				CoreContext.Environment);

			// Fix edited results when values are not edited
			if (!Manager.NativeManager.IsAnyItemActive())
			{
				CoreContext.SelectedEffect.Context.CommandManager.SetFlagToBlockMergeCommands();
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

		// フィールドが配列などであったとき、その各要素について格納する
		public List<InspectorGuiInfo> subElement = new List<InspectorGuiInfo>();
	}

	class Inspector
	{
		// Gui表示を登録する
		private static readonly InspectorGuiDictionary GuiDictionary = new InspectorGuiDictionary();
		
		private static List<InspectorGuiInfo> FieldGuiInfoList = new List<InspectorGuiInfo>();

		private static object LastTarget = null;

		private static Dictionary<int, object> VisiblityControllers = new Dictionary<int, object>();

		public Inspector()
		{
			//Core.OnAfterSelectNode += OnAfterSelect;
		}

		private static void UpdateVisiblityControllers(object target)
		{
			var fields = target.GetType().GetFields();
			foreach (var field in fields)
			{
				// visiblity controller attribute
				var attr = (EffectAsset.VisiblityControllerAttribute)field.GetCustomAttribute(typeof(EffectAsset.VisiblityControllerAttribute));
				if (attr != null)
				{
					VisiblityControllers[attr.ID] = field.GetValue(target);
				}
			}
		}

		private static void GenerateFieldGuiInfos(object target)
		{
			if (FieldGuiInfoList == null)
			{
				FieldGuiInfoList = new List<InspectorGuiInfo>();
			}
			FieldGuiInfoList.Clear();

			Func<object, InspectorGuiInfo> generate = null;
			generate = (object tgt) =>
			{
				InspectorGuiInfo info = new InspectorGuiInfo();
				if (tgt == null ||
					tgt.GetType().IsPrimitive ||
					tgt.GetType() == typeof(string) ||
					tgt.GetType().IsEnum)
				{
					return info;
				}
				var fields = tgt.GetType().GetFields();
				foreach (var field in fields)
				{
					// visiblity controller attribute
					UpdateVisiblityControllers(tgt);

					if (!GuiDictionary.HasFunction(field.GetType()) && !field.GetType().IsPrimitive)
					{
						info.subElement.Add(generate(field.GetValue(tgt)));
					}
				}
				return info;
			};
			var fields = target.GetType().GetFields();
			foreach (var field in fields)
			{
				FieldGuiInfoList.Add(generate(field.GetValue(target)));
			}
		}

		private static void UpdateObjectGuis(EffectAsset.EffectAssetEditorContext context
			, object targetNode, object targetObject, List<InspectorGuiInfo> guiInfos)
		{
			var getterSetters = new FieldGetterSetter[1];
			getterSetters[0] = new FieldGetterSetter();

			// エディタに表示する変数群
			var fields = targetObject.GetType().GetFields();
			UpdateVisiblityControllers(targetObject);

			bool isFirstField = true;


			for (int i = 0; i < fields.Length; ++i)
			{
				var field = fields[i];
				field.GetCustomAttributes();
				getterSetters[0].Reset(targetObject, field);
				//var prop = context.EditorProperty.Properties.FirstOrDefault(_ => _.InstanceID == target.InstanceID);
				bool isValueChanged = false;
				//if (prop != null)
				//{
				//	// 配列の変更が取れないのは別インスタンスに上書きされてるから？
				//	// 配列のコピーではなく、配列の要素のコピーにすべき
				//	isValueChanged = prop.IsValueEdited(getterSetters.Select(_ => _.GetName()).ToArray());
				//}

				var getterSetter = getterSetters.Last();
				var value = getterSetter.GetValue();
				var name = getterSetter.GetName();

				if (value == null)
				{
					// TODO : nullどうする？
					//Manager.NativeManager.Text("null : " + field.GetType().ToString());
					continue;
				}


				// 配列かリストの時、エレメントの型を取得する
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

				var guiFunctionKey = valueType;

				// the key about enums gui function is System.Enum
				if (valueType.IsEnum)
				{
					guiFunctionKey = typeof(System.Enum);
				}


				// key attrs
				string description = string.Empty;
				{
					var attr = (KeyAttribute)field.GetCustomAttribute(typeof(KeyAttribute));
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
				}
				
				// VisiblityControlledAttributes
				bool isVisible = true;
				{
					var attr = (EffectAsset.VisiblityControlledAttribute)field.GetCustomAttribute(typeof(EffectAsset.VisiblityControlledAttribute));
					if (attr != null)
					{
						if (VisiblityControllers.ContainsKey(attr.ID))
						{
							var controllerValue = (int)VisiblityControllers[attr.ID];
							isVisible = (attr.Value == controllerValue);
						}
					}
				}
				if (!isVisible)
				{
					continue;
				}


				//if (NodeParameters.Contains(value))
				if (!GuiDictionary.HasFunction(guiFunctionKey) &&
					value.GetType().GetFields().Length > 0 &&
					!value.GetType().IsEnum &&
					guiInfos[i].subElement.Count >= i + 1)
				{
					UpdateObjectGuis(context, targetNode, value, guiInfos[i].subElement);
					continue;
				}

				// display name(left side of table)
				Manager.NativeManager.TableNextRow();
				Manager.NativeManager.TableNextColumn();
				// TODO : Separatorで区切るのはAssetなどの単位にする
				// Make not separate first row
				bool isShowHorizonalSeparator = Manager.NativeManager.TableGetRowIndex() >= 2 && isFirstField;
				if (isShowHorizonalSeparator)
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


				// display field(right side of table)
				Manager.NativeManager.TableNextColumn();
				// TODO : Separatorで区切るのはAssetなどの単位にする
				if (isShowHorizonalSeparator)
				{
					Manager.NativeManager.Separator();
				}

				// TODO : make ignore "public List<Node> Children = new List<Node>();" node member.
				if (GuiDictionary.HasFunction(guiFunctionKey))
				{
					var func = GuiDictionary.GetFunction(guiFunctionKey);

					if (isArray)
					{
						Array arrayValue = (Array)value;

						// GuiIdが足りなければ、すべて再生成
						// GenerateFieldGuiIdsの中でやりたいが、型情報からは要素数が分からないのでここで生成。
						if (arrayValue.GetLength(0) > guiInfos[i].subElement.Count())
						{
							guiInfos[i].subElement.Clear();

							foreach (var v in arrayValue)
							{
								FieldGuiInfoList[i].subElement.Add(new InspectorGuiInfo());
							}
						}

						int j = 0;
						bool isEdited = false;
						foreach (var v in arrayValue)
						{
							string id = guiInfos[i].subElement[j].Id;
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
							context.CommandManager.NotifyEditFields((PartsTreeSystem.IInstance)targetNode);
						}
					}
					else
					{
						string id = guiInfos[i].Id;
						var result = func(value, id);
						if (result.isEdited)
						{
							field.SetValue(targetObject, result.value);
							context.CommandManager.NotifyEditFields((PartsTreeSystem.IInstance)targetNode);
						}
					}
				}
				else
				{
					Manager.NativeManager.Text("No Regist : " + value.GetType().ToString() + " " + name);
				}
				isFirstField = false;
			}
		}

		//public static void OnAfterSelect(object sender, EventArgs e)
		//{

		//	if (CoreContext.SelectedEffect.Context.NodeTree == null ||
		//		CoreContext.SelectedEffectNode == null)
		//	{
		//		return;
		//	}

		//	GenerateFieldGuiInfos(CoreContext.SelectedEffectNode);
		//}

		public static void Update(EffectAsset.EffectAssetEditorContext context, EffectAsset.Node targetNode)
		{
			// Generate field GUI IDs when the target is selected or changed.
			// TODO: this had better do at OnAfterSelect()
			if (targetNode?.InstanceID != ((EffectAsset.Node)LastTarget)?.InstanceID)
			{
				GenerateFieldGuiInfos(targetNode);
				LastTarget = targetNode;
				return;
			}
			LastTarget = targetNode;

			var commandManager = CoreContext.SelectedEffect.Context.CommandManager;
			var nodeTreeGroup = CoreContext.SelectedEffect.Asset.NodeTreeAsset;
			var nodeTree = CoreContext.SelectedEffect.Context.NodeTree;
			//var partsList = editorState.PartsList;
			var env = CoreContext.Environment;
			//var nodeTreeGroupEditorProperty = context.EditorProperty;



			if (Manager.NativeManager.BeginTable("Table", 2, 
				swig.TableFlags.Resizable |
				swig.TableFlags.BordersInnerV | swig.TableFlags.BordersOuterH |
				swig.TableFlags.SizingFixedFit | swig.TableFlags.SizingStretchProp |
				swig.TableFlags.NoSavedSettings))
			{
				// アイテムの幅を最大に設定
				Manager.NativeManager.TableNextRow();
				Manager.NativeManager.TableSetColumnIndex(0);
				Manager.NativeManager.PushItemWidth(-1);
				Manager.NativeManager.TableSetColumnIndex(1);
				Manager.NativeManager.PushItemWidth(-1);

				UpdateObjectGuis(context, targetNode, targetNode, FieldGuiInfoList);
			}
			Manager.NativeManager.EndTable();
			Manager.NativeManager.Separator();
		}
	}

	class InspectorEditable : Node
	{
		//[Key(key = "Node_Name")]
		//public string Name = string.Empty;

		public EffectAsset.CommonParameter CommonValues = new EffectAsset.CommonParameter();

		public EffectAsset.PositionParameter PositionParam = new EffectAsset.PositionParameter();

		public EffectAsset.RotationParameter RotationParam = new EffectAsset.RotationParameter();

		//public EffectAsset.Gradient GradientTest = new EffectAsset.Gradient();

		//public EffectAsset.Vector3WithRange Vector3WithRangeTest = new EffectAsset.Vector3WithRange();

		//public EffectAsset.TextureAsset TextureTest = null;


		//-------------------------------
#if false
		public EffectAsset.PositionParameter PositionParam = new EffectAsset.PositionParameter();
		public EffectAsset.RotationParameter RotationParam = new EffectAsset.RotationParameter();

		// Attributes
		[EffectAsset.VisiblityControlled(ID = 100, Value = 0)]
		[Key(key = "Position_NurbsCurveParameter_Scale")]
		public float scale = 1.0f;
		[EffectAsset.VisiblityControlled(ID = 100, Value = 1)]
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

		// 配列
		// 配列を関数側で対応するのか、呼び出し側でどうにかするのか
		// 試した感じ呼び出し側で制御した方が楽そう
		public int[] IntArray = new int[2];
		public float[] FloatArray = new float[5];
		public string[] StringArray = new string[2] { "hoge", "fuga" };

		// 全てのコレクションに対しプログラムをする方法なかったっけ
		// TODO : List
		public List<int> ListInt1 = new List<int> { 2, 3 };

		// TODO : Vector
		public Vector2D vector2 = new Vector2D();
		public Vector3D vector3 = new Vector3D();



		public Vector3D[] vector3Array = new Vector3D[2] { new Vector3D(), new Vector3D() };

		// TODO : string等、nullをどう扱うか
		public string String2;

		// TODO : 特殊な型
		Gradient Gradient1 = new Gradient();
		Dock.FCurves FCurves = new Dock.FCurves();
#endif
	}
}

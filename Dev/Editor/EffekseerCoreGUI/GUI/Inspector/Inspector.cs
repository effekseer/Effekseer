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

		public override void DispatchDropped(string path, ref bool handle)
		{
			if (CoreContext.SelectedEffect.Context.NodeTree == null || CoreContext.SelectedEffectNode == null)
			{
				return;
			}

			CoreContext.SelectedEffect.Context.CommandManager.StartEditFields(
				CoreContext.SelectedEffect.Asset.NodeTreeAsset,
				CoreContext.SelectedEffect.Context.NodeTree,
				CoreContext.SelectedEffectNode,
				CoreContext.Environment);

			if (Inspector.Drop(path, CoreContext.SelectedEffect.Context,
				CoreContext.SelectedEffectNode))
			{
				handle = true;
			}

			CoreContext.SelectedEffect.Context.CommandManager.EndEditFields(
				CoreContext.SelectedEffectNode,
				CoreContext.Environment);


			base.DispatchDropped(path, ref handle);
		}
	}

	class InspectorGuiState
	{
		public string Id { get; private set; }

		public object UserData { get; set; }

		public InspectorGuiState(string id)
		{
			Id = id;
		}
	}

	class InspectorGuiInfo
	{
		public InspectorGuiInfo()
		{
			State = new InspectorGuiState("###" + Manager.GetUniqueID().ToString());
			SubElements = new List<InspectorGuiInfo>();
		}

		public bool isRoot = false;

		public InspectorGuiState State { get; private set; }

		// Sub elements of gui(ex. list, class, struct)
		public List<InspectorGuiInfo> SubElements { get; private set; }
	}

	class Inspector
	{
		// functions to show gui
		private static readonly InspectorGuiDictionary GuiDictionary = new InspectorGuiDictionary();

		private static InspectorGuiInfo RootGuiInfo = new InspectorGuiInfo();

		private static object LastTarget = null;

		private static Dictionary<int, object> VisiblityControllers = new Dictionary<int, object>();

		public Inspector()
		{
			//Core.OnAfterSelectNode += OnAfterSelect;
			RootGuiInfo.isRoot = true;
		}

		private static void UpdateVisiblityControllers(object target)
		{
			if (target == null)
			{
				return;
			}

			var fields = target.GetType().GetFields();
			foreach (var field in fields)
			{
				// visiblity controller attribute
				var attr = (Asset.VisiblityControllerAttribute)field.GetCustomAttribute(typeof(Asset.VisiblityControllerAttribute));
				if (attr != null)
				{
					VisiblityControllers[attr.ID] = field.GetValue(target);
				}
			}
		}

		private static void GenerateFieldGuiInfos(object target, Type targetType = null)
		{
			RootGuiInfo.SubElements.Clear();

			InspectorGuiInfo generate(object tgt)
			{
				InspectorGuiInfo info = new InspectorGuiInfo();
				if (tgt == null)
				{
					return info;
				}
				var type = tgt.GetType();

				// TODO: when guiFuncs work are completed, this should be only "GuiDictionary.HasFunction(type)"
				bool hasGuiFunction = 
							type.IsEnum ||
							GuiDictionary.HasFunction(type);
				if (hasGuiFunction)
				{
					return info;
				}

				// fields in tgt
				var fieldsInTgt = tgt.GetType().GetFields();
				foreach (var fieldInTgt in fieldsInTgt)
				{
					// visiblity controller attribute
					UpdateVisiblityControllers(tgt);
					info.SubElements.Add(generate(fieldInTgt.GetValue(tgt)));

				}
				return info;
			};


			var fields = target.GetType().GetFields();
			foreach (var field in fields)
			{
				if (targetType != null && targetType != field.FieldType)
				{
					continue;
				}
				var tgt = field.GetValue(target);
				{
					UpdateVisiblityControllers(tgt);
				}

				RootGuiInfo.SubElements.Add(generate(tgt));
			}
		}

		private static void UpdateObjectGuis(Asset.EffectAssetEditorContext context
			, object targetNode, PartsTreeSystem.ElementGetterSetterArray elementGetterSetterArray, InspectorGuiInfo guiInfo)
		{
			var field = elementGetterSetterArray.FieldInfos.Last();

			bool isValueChanged = false;

			var prop = context.EditorProperty.Properties.FirstOrDefault(_ => _.InstanceID == ((Asset.Node)targetNode)?.InstanceID);
			if (prop != null)
			{
				isValueChanged = prop.IsValueEdited(elementGetterSetterArray.Names);
			}

			var value = elementGetterSetterArray.GetValue();
			var name = elementGetterSetterArray.GetName();

			// value type of element
			var valueType = field != null ? field.FieldType : value.GetType();
			bool isList = valueType is IList;
			if (isList)
			{
				valueType = valueType.GetElementType();
			}
			else if (valueType.IsGenericType && valueType.GetGenericTypeDefinition() == typeof(List<>))
			{
				valueType = valueType.GetGenericArguments()[0];
			}

			var guiFunctionKey = valueType;

			// when valueType is enum, the key is System.Enum 
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
			{
				bool isVisible = true;
				var attr = (Asset.VisiblityControlledAttribute)field.GetCustomAttribute(typeof(Asset.VisiblityControlledAttribute));
				if (attr != null)
				{
					if (VisiblityControllers.ContainsKey(attr.ID))
					{
						var controllerField = VisiblityControllers[attr.ID];
						int controllerValue = 0;

						if (controllerField.GetType().IsEnum)
						{

							controllerValue = (int)controllerField;
						}
						else if (controllerField.GetType() == typeof(bool))
						{
							controllerValue = (bool)controllerField ? 1 : 0;
						}

						isVisible = (attr.Value == controllerValue);
					}
				}
				if (!isVisible)
				{
					return;
				}
			}


			// update subfields
			if (!GuiDictionary.HasFunction(guiFunctionKey))
			{
				var subFields = valueType.GetFields();
				int i = 0;
				foreach (var f in subFields)
				{
					if (
						value.GetType().GetFields().Length > 0 &&
						guiInfo.SubElements.Count > i
					)
					{
						UpdateVisiblityControllers(value);
						elementGetterSetterArray.Push(value, f);
						UpdateObjectGuis(context, targetNode, elementGetterSetterArray, guiInfo.SubElements[i]);
						elementGetterSetterArray.Pop();
					}
					++i;
				}
			}


			Manager.NativeManager.TableNextRow();

			// name column(left side of table)
			Manager.NativeManager.TableNextColumn();

			// TODO: use grouping attributes
			bool isShowHorizonalSeparator = Manager.NativeManager.TableGetRowIndex() >= 2;
			if (isShowHorizonalSeparator)
			{
				Manager.NativeManager.Separator();
			}

			// TODO: for debugging. this should be delete.
			//name = (isValueChanged ? "*" : "") + name + " " + guiInfo.Id;

			Manager.NativeManager.Text(name);

			// tooltip for description
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

			if (isShowHorizonalSeparator)
			{
				Manager.NativeManager.Separator();
			}

			//bool opened = Manager.NativeManager.CollapsingHeader(label);

			// TODO : ignore "public List<Node> Children = new List<Node>();" node member.
			if (GuiDictionary.HasFunction(guiFunctionKey))
			{
				if (isValueChanged)
				{
					Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.Border, 0x77ffff11);
					Manager.NativeManager.PushStyleVar(swig.ImGuiStyleVarFlags.FrameBorderSize, 1);
				}

				var func = GuiDictionary.GetFunction(guiFunctionKey);

				if (isList)
				{
					IList arrayValue = (IList)value;

					// generate/regenerate subeElements when there are not enougth GuiIDs.
					// generate it here because the number of elements can't read in GenerateFieldGuiIds
					if (arrayValue.Count > guiInfo.SubElements.Count())
					{
						guiInfo.SubElements.Clear();

						foreach (var v in arrayValue)
						{
							guiInfo.SubElements.Add(new InspectorGuiInfo());
						}
					}

					bool isEdited = false;
					for (int j = 0; j < arrayValue.Count; j++)
					{
						var v = arrayValue[j];
						elementGetterSetterArray.Push(arrayValue, j);
						var result = func(v, guiInfo.SubElements[j].State);
						if (result.isEdited)
						{
							if (valueType.IsValueType)
							{
								elementGetterSetterArray.SetValue(result.value);
							}
							isEdited = true;
						}
						elementGetterSetterArray.Pop();
						++j;
					}
					if (isEdited)
					{
						field.SetValue(targetNode, arrayValue);
						context.CommandManager.NotifyEditFields((PartsTreeSystem.IInstance)targetNode);
					}
				}
				else
				{
					var result = func(value, guiInfo.State);
					if (result.isEdited)
					{
						elementGetterSetterArray.SetValue(result.value);
						context.CommandManager.NotifyEditFields((PartsTreeSystem.IInstance)targetNode);
					}

				}

				if (isValueChanged)
				{
					Manager.NativeManager.PopStyleColor();
					Manager.NativeManager.PopStyleVar();
				}
			}
			else
			{
				if (value != null)
				{
					Manager.NativeManager.Text("No Regist : " + value.GetType().ToString() + " " + name);
				}
				else
				{
					Manager.NativeManager.Text("None : " + " " + name);
				}
			}
		}

		/// <summary>
		/// TODO : Refactor
		/// </summary>
		/// <param name="path"></param>
		/// <param name="context"></param>
		/// <param name="targetNode"></param>
		/// <param name="elementGetterSetterArray"></param>
		/// <param name="guiInfo"></param>
		/// <returns></returns>
		private static bool DropObjectGuis(string path, Asset.EffectAssetEditorContext context
			, object targetNode, PartsTreeSystem.ElementGetterSetterArray elementGetterSetterArray, InspectorGuiInfo guiInfo)
		{
			var field = elementGetterSetterArray.FieldInfos.Last();
			var value = elementGetterSetterArray.GetValue();

			// value type of element
			var valueType = field != null ? field.FieldType : value.GetType();
			bool isList = valueType is IList;
			if (isList)
			{
				valueType = valueType.GetElementType();
			}
			else if (valueType.IsGenericType && valueType.GetGenericTypeDefinition() == typeof(List<>))
			{
				valueType = valueType.GetGenericArguments()[0];
			}

			var guiFunctionKey = valueType;

			// when valueType is enum, the key is System.Enum 
			if (valueType.IsEnum)
			{
				guiFunctionKey = typeof(System.Enum);
			}

			// VisiblityControlledAttributes
			{
				bool isVisible = true;
				var attr = (Asset.VisiblityControlledAttribute)field.GetCustomAttribute(typeof(Asset.VisiblityControlledAttribute));
				if (attr != null)
				{
					if (VisiblityControllers.ContainsKey(attr.ID))
					{
						var controllerValue = (int)VisiblityControllers[attr.ID];
						isVisible = (attr.Value == controllerValue);
					}
				}
				if (!isVisible)
				{
					return false;
				}
			}

			// update subfields
			var subFields = valueType.GetFields();
			bool shownSubFields = false;
			foreach (var (f, i) in subFields.Select((_, i) => Tuple.Create(_, i)))
			{
				if (!GuiDictionary.HasDropFunction(guiFunctionKey) &&
				value.GetType().GetFields().Length > 0 &&
				!value.GetType().IsEnum &&
				guiInfo.SubElements.Count > i)
				{
					UpdateVisiblityControllers(value);
					elementGetterSetterArray.Push(value, f);
					bool editted = DropObjectGuis(path, context, targetNode, elementGetterSetterArray, guiInfo.SubElements[i]);
					elementGetterSetterArray.Pop();
					shownSubFields = true;

					if (editted)
					{
						return true;
					}
				}
			}
			if (shownSubFields)
			{
				return false;
			}

			if (GuiDictionary.HasDropFunction(guiFunctionKey))
			{
				var func = GuiDictionary.GetDropFunction(guiFunctionKey);

				if (isList)
				{
					IList arrayValue = (IList)value;

					// generate/regenerate subElements when there are not enougth GuiIDs.
					// generate it here because the number of elements can't read in GenerateFieldGuiIds
					if (arrayValue.Count > guiInfo.SubElements.Count())
					{
						guiInfo.SubElements.Clear();

						foreach (var v in arrayValue)
						{
							guiInfo.SubElements.Add(new InspectorGuiInfo());
						}
					}

					bool isEdited = false;
					for (int j = 0; j < arrayValue.Count; j++)
					{
						var v = arrayValue[j];

						elementGetterSetterArray.Push(arrayValue, j);
						var result = func(v, path, guiInfo.SubElements[j].State);
						if (result.isEdited)
						{
							if (valueType.IsValueType)
							{
								elementGetterSetterArray.SetValue(result.value);
							}
							isEdited = true;
						}
						elementGetterSetterArray.Pop();
						++j;
					}
					if (isEdited)
					{
						field.SetValue(targetNode, arrayValue);
						context.CommandManager.NotifyEditFields((PartsTreeSystem.IInstance)targetNode);
						return true;
					}
				}
				else
				{
					var result = func(value, path, guiInfo.State);
					if (result.isEdited)
					{
						elementGetterSetterArray.SetValue(result.value);
						context.CommandManager.NotifyEditFields((PartsTreeSystem.IInstance)targetNode);
						return true;
					}

				}
			}

			return false;
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

		public static void Update(Asset.EffectAssetEditorContext context, Asset.Node targetNode, Type targetType = null)
		{
			// Generate field GUI IDs when the target is selected or changed.
			// TODO: this had better do at OnAfterSelect()
			//if (targetNode?.InstanceID != ((Asset.Node)LastTarget)?.InstanceID)
			{
				GenerateFieldGuiInfos(targetNode, targetType);
				LastTarget = targetNode;
				//return;
			}
			LastTarget = targetNode;


			if (Manager.NativeManager.BeginTable("Table", 2,
				swig.TableFlags.Resizable |
				swig.TableFlags.BordersInnerV | swig.TableFlags.BordersOuterH |
				swig.TableFlags.SizingFixedFit | swig.TableFlags.SizingStretchProp |
				swig.TableFlags.NoSavedSettings))
			{
				// set width of table max
				Manager.NativeManager.TableNextRow();
				Manager.NativeManager.TableSetColumnIndex(0);
				Manager.NativeManager.PushItemWidth(-1);
				Manager.NativeManager.TableSetColumnIndex(1);
				Manager.NativeManager.PushItemWidth(-1);


				var fields = targetNode.GetType().GetFields();
				int i = 0;
				var elementGetterSetterArray = new PartsTreeSystem.ElementGetterSetterArray();
				foreach (var field in fields)
				{
					if (targetType != null && targetType != field.FieldType)
					{
						continue;
					}

					elementGetterSetterArray.Push(targetNode, field);
					UpdateVisiblityControllers(targetNode);
					UpdateObjectGuis(context, targetNode, elementGetterSetterArray, RootGuiInfo.SubElements[i]);
					elementGetterSetterArray.Pop();
					++i;
				}
			}
			Manager.NativeManager.EndTable();
			Manager.NativeManager.Separator();
		}

		public static bool Drop(string path, Asset.EffectAssetEditorContext context, Asset.Node targetNode, Type targetType = null)
		{
			var fields = targetNode.GetType().GetFields();
			int i = 0;
			var elementGetterSetterArray = new PartsTreeSystem.ElementGetterSetterArray();
			foreach (var field in fields)
			{
				if (targetType != null && targetType != field.FieldType)
				{
					continue;
				}

				elementGetterSetterArray.Push(targetNode, field);
				UpdateVisiblityControllers(targetNode);
				var eddited = DropObjectGuis(path, context, targetNode, elementGetterSetterArray, RootGuiInfo.SubElements[i]);
				elementGetterSetterArray.Pop();
				++i;

				if (eddited)
				{
					return true;
				}
			}

			return false;
		}
	}
}

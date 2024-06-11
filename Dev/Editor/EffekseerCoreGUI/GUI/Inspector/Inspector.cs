using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace Effekseer.GUI.Inspector
{
	public class InspectorPanel : Dock.DockPanel
	{
		Inspector inspector;

		public InspectorPanel()
		{
			inspector = new Inspector();
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

			inspector.Update(CoreContext.SelectedEffect.Context,
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

			if (inspector.Drop(path, CoreContext.SelectedEffect.Context,
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

	/// <summary>
	/// State about widget.<br/>
	/// This can be accessed from inspector widgets.
	/// </summary>
	class WidgetState
	{
		public string Id { get; private set; }

		public object UserData { get; set; }


		public List<Attribute> Attriubutes { get; private set; }

		public WidgetState(string id)
		{
			Id = id;
			Attriubutes = new List<Attribute>();
		}
	}


	/// <summary>
	/// Information about the target of the widget.<br/>
	/// Subelements(ex. class, struct, collection) are held as a tree structure by the inspector.
	/// </summary>
	class WidgetInfo
	{
		public WidgetInfo()
		{
			State = new WidgetState("###" + Manager.GetUniqueID().ToString());
			Children = new List<WidgetInfo>();
		}

		public bool isRoot = false;

		// Visibility control
		public bool IsVisibilityController { get; private set; } = false;
		public int VisibilityControlID { get; private set; } = -1;
		public int VisibilityControlledValue { get; private set; } = -1;

		public WidgetState State { get; private set; }

		// symbol of the field
		public string Name { get; set; }

		// data from attributes
		public string Key { get; private set; } = string.Empty;
		public string Description
		{
			get {
				if (multiLanguageDescription != null)
				{
					return multiLanguageDescription.Value;
				}
				return string.Empty;
			}
		}
		private MultiLanguageString multiLanguageDescription = null;
		public bool EnableDescription { get; private set; } = true;
		public string Label { 
			get {
				if (multiLanguageLabel != null)
				{
					return multiLanguageLabel.Value;
				}
				return string.Empty;
			}
		}
		private MultiLanguageString multiLanguageLabel = null;
		public int VisibilityControllID { get; set; } = -1;

		// Subelements of widget(ex. list, class, collection)
		public List<WidgetInfo> Children { get; private set; }

		public void SetupFromAttributes()
		{
			// KeyAttribute
			{
				Key = string.Empty;
				var attr = (KeyAttribute)State.Attriubutes.FirstOrDefault(_ => _ is KeyAttribute);
				if (attr != null)
				{
					Key = attr.key + "_Name";

					if (MultiLanguageTextProvider.HasKey(Key))
					{
						multiLanguageLabel = new MultiLanguageString(Key);
					}
					else if (MultiLanguageTextProvider.HasKey(attr.key))
					{
						EnableDescription = false;
						multiLanguageLabel = new MultiLanguageString(attr.key);
					}

					Key = attr.key + "_Desc";
					if (MultiLanguageTextProvider.HasKey(Key))
					{
						multiLanguageDescription = new MultiLanguageString(Key);
					}
					else if (MultiLanguageTextProvider.HasKey(attr.key))
					{
						//multiLanguageDescription = new MultiLanguageString(attr.key);
					}
				}
			}
			// VisiblityControllerAttributes
			IsVisibilityController = false;
			{
				var attr = (Asset.VisiblityControllerAttribute)State.Attriubutes.FirstOrDefault(_ => _ is Asset.VisiblityControllerAttribute);
				if (attr != null)
				{
					IsVisibilityController = true;
					VisibilityControlID = attr.ID;
				}
			}
			// VisiblityControlledAttributes
			{
				VisibilityControllID = -1;
				var attr = (Asset.VisiblityControlledAttribute)State.Attriubutes.FirstOrDefault(_ => _ is Asset.VisiblityControlledAttribute);
				if (attr != null)
				{
					VisibilityControllID = attr.ID;
				}
			}
		}
	}

	public class Inspector
	{
		// Function to process the widgets
		private readonly InspectorWidgetDictionary WidgetDictionary = new InspectorWidgetDictionary();

		private WidgetInfo RootWidgetInfo = new WidgetInfo();

		private object LastTarget = null;

		private Dictionary<int, object> VisiblityControllers = new Dictionary<int, object>();

		public Inspector()
		{
			RootWidgetInfo.isRoot = true;
		}

		/// <summary>
		/// Collect visiblity controllers state of fields in target.
		/// </summary>
		/// <param name="target"></param>
		private void UpdateVisiblityControllers(object target)
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


		/// <summary>
		/// Generate WidgetInfo(recursively)
		/// </summary>
		/// <param name="targetValue">Instance that generate info</param>
		/// <param name="targetFieldInfo">FieldInfo that generate info</param>
		/// <returns></returns>
		WidgetInfo GenerateWidgetInfo(object targetValue, FieldInfo targetFieldInfo)
		{
			WidgetInfo info = new WidgetInfo();

			var targetType = targetFieldInfo.FieldType;
			info.Name = targetFieldInfo.Name + "(type: " + targetType.Name + ")";

			var attributes = targetFieldInfo.GetCustomAttributes();
			foreach (var attr in attributes)
			{
				info.State.Attriubutes.Add(attr);
			}
			info.SetupFromAttributes();

			bool hasWidgetiFunction =
						targetType.IsEnum ||    // Enum is not supported by HasFunction
						WidgetDictionary.HasFunction(targetType);
			if (hasWidgetiFunction)
			{
				return info;
			}

			// Subelements(fields in target)
			var fieldsInTgt = targetType.GetFields();
			foreach (var fieldInTgt in fieldsInTgt)
			{
				// VisiblityController attribute
				if (targetValue != null)
				{
					UpdateVisiblityControllers(targetValue);
				}

				info.Children.Add(GenerateWidgetInfo(
					fieldInTgt != null ? fieldInTgt.GetValue(targetValue) : null,
					fieldInTgt));
			}
			return info;
		}

		/// <summary>
		/// Generate fieldWidgetInfo recursively for each field.<br/>
		/// Existing fieldWidgetInfo will be cleared.
		/// </summary>
		/// <param name="target">Target node or target object</param>
		/// <param name="targetType">Type of field in target. If this is null, all fields in target will be generated.</param>
		private void InitializeWidgetInfo(object target, Type targetType = null)
		{
			RootWidgetInfo.Children.Clear();
			RootWidgetInfo.State.Attriubutes.Clear();

			var fields = target.GetType().GetFields();
			foreach (var field in fields)
			{
				if (targetType != null && targetType != field.FieldType)
				{
					continue;
				}
				var targetValue = field.GetValue(target);
				{
					UpdateVisiblityControllers(targetValue);
				}
				RootWidgetInfo.Children.Add(GenerateWidgetInfo(targetValue, field));
			}
		}


		/// <summary>
		/// End current table with settings for inspector
		/// </summary>
		private void EndTable()
		{
			Manager.NativeManager.EndTable();
		}

		/// <summary>
		/// Begin table with settings for inspector
		/// </summary>
		private bool BeginTable()
		{
			var regionAvail = Manager.NativeManager.GetContentRegionAvail();
			if (Manager.NativeManager.BeginTable("Table", 2,
				swig.TableFlags.BordersInnerV | swig.TableFlags.SizingFixedFit | swig.TableFlags.SizingStretchProp |
				swig.TableFlags.NoSavedSettings))
			{
				// Set columns width
				Manager.NativeManager.TableSetupColumn("", swig.TableColumnFlags.WidthFixed, (int)(regionAvail.X * 0.3f));
				Manager.NativeManager.TableSetupColumn("", swig.TableColumnFlags.WidthStretch);
				// Set controls width to maximum
				Manager.NativeManager.TableNextRow();
				Manager.NativeManager.TableSetColumnIndex(0);
				Manager.NativeManager.PushItemWidth(-1);
				Manager.NativeManager.TableSetColumnIndex(1);
				Manager.NativeManager.PushItemWidth(-1);

				return true;
			}

			return false;
		}


		/// <summary>
		/// Update labels and widgets recursively for each field.
		/// </summary>
		/// <param name="context">EditorContext</param>
		/// <param name="targetNode">Node that is being inspected.<br/>
		/// If it's null, also possible to inspect objects that do not belong to Node.</param>
		/// <param name="targetObject">Object to be inspected. Processed in this function.</param>
		/// <param name="elementGetterSetterArray"></param>
		/// <param name="widgetInfo">Widget info about targetObject</param>
		private void UpdateParameterGrid(
			Asset.EffectAssetEditorContext context,
			Asset.Node targetNode, object targetObject,
			PartsTreeSystem.ElementGetterSetterArray elementGetterSetterArray, WidgetInfo widgetInfo)
		{
			var field = elementGetterSetterArray.FieldInfos.Last();

			bool isValueChanged = false;


			if (targetNode != null)
			{
				var prop = context.EditorProperty.Properties.FirstOrDefault(_ => _.InstanceID == targetNode?.InstanceID);
				if (prop != null)
				{
					isValueChanged = prop.IsValueEdited(elementGetterSetterArray.Names);
				}
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

			var widgetFunctionKey = valueType;

			// when valueType is enum, the key is System.Enum 
			if (valueType.IsEnum)
			{
				widgetFunctionKey = typeof(System.Enum);
			}


			// data in attributes
			string description = widgetInfo.Description;
			bool enableDescription = widgetInfo.EnableDescription;
			string labelStr = widgetInfo.Label;
			bool isVisibilityController = widgetInfo.IsVisibilityController;
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
							controllerValue = (bool)controllerField ? 0 : 1;
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
			if (!WidgetDictionary.HasFunction(widgetFunctionKey))
			{
				// Toggle mode
				if (value is Asset.IToggleMode toggleMode)
				{
					// End the current table for display a toggle
					EndTable();

					var toggleId = "###" + widgetInfo.State.Id + "_toggle";
					var collapsingHeaderLabel = labelStr + "###" + widgetInfo.State.Id + "_label";
					bool enabled = toggleMode.Enabled;
					bool opened = Manager.NativeManager.CollapsingHeaderWithToggle(collapsingHeaderLabel, swig.TreeNodeFlags.None, toggleId, ref enabled);

					// If toggle state is changed
					if (enabled != toggleMode.Enabled)
					{
						toggleMode.Enabled = enabled;
					}

					if (opened && !enabled)
					{
						string message = MultiLanguageTextProvider.GetText("TreeDisabled_Message");
						Manager.NativeManager.TextWrapped(message);
					}

					// Begin the table again
					BeginTable();

					if (!opened || !enabled)
					{
						return;
					}
				}


				var subFields = valueType.GetFields();
				int i = 0;
				foreach (var f in subFields)
				{
					if (value != null && !isList && widgetInfo.Children.Count > i)
					{
						UpdateVisiblityControllers(value);
						elementGetterSetterArray.Push(value, f);
						UpdateParameterGrid(context, targetNode, targetObject, elementGetterSetterArray, widgetInfo.Children[i]);
						elementGetterSetterArray.Pop();
					}
					++i;
				}

				return;
			}


			bool isShowHorizonalSeparator = isVisibilityController;

			Manager.NativeManager.TableNextRow();

			// name column(left side of table)
			Manager.NativeManager.TableNextColumn();
			// for debugging. this should be delete.
			//name = (isValueChanged ? "*" : "") + name + " " + guiInfo.Id;

			if (isShowHorizonalSeparator)
			{
				Manager.NativeManager.Separator();
			}

			Manager.NativeManager.Text(labelStr);

			// tooltip for description
			if (enableDescription && Manager.NativeManager.IsItemHovered())
			{
				Manager.NativeManager.BeginTooltip();

				Manager.NativeManager.Text(labelStr);
				Manager.NativeManager.Separator();
				Manager.NativeManager.Text(description);

				Manager.NativeManager.EndTooltip();
			}

			// show field widget(right side of table)
			Manager.NativeManager.TableNextColumn();

			if (isShowHorizonalSeparator)
			{
				Manager.NativeManager.Separator();
			}

			// TODO : ignore "public List<Node> Children = new List<Node>();" node member.
			if (WidgetDictionary.HasFunction(widgetFunctionKey))
			{
				if (isValueChanged)
				{
					Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.Border, 0x77ffff11);
					Manager.NativeManager.PushStyleVar(swig.ImGuiStyleVarFlags.FrameBorderSize, 1);
				}

				var func = WidgetDictionary.GetFunction(widgetFunctionKey);

				if (isList)
				{
					IList arrayValue = (IList)value;

					// generate/regenerate subeElements when there are not enougth WidgetIDs.
					// generate it here because the number of elements can't read in GenerateFieldWidgetIds
					if (arrayValue.Count > widgetInfo.Children.Count())
					{
						widgetInfo.Children.Clear();

						foreach (var v in arrayValue)
						{
							widgetInfo.Children.Add(new WidgetInfo());
						}
					}

					bool isEdited = false;
					for (int j = 0; j < arrayValue.Count; j++)
					{
						var v = arrayValue[j];
						elementGetterSetterArray.Push(arrayValue, j);
						var result = func(v, widgetInfo.Children[j].State);
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
						field.SetValue(targetObject, arrayValue);

						var t = targetObject.GetType();
						if (targetNode != null)
						{
							context?.CommandManager.NotifyEditFields(targetNode);
						}
						else
						{
							context?.CommandManager.NotifyEditFields((PartsTreeSystem.Asset)targetObject);
						}
					}
				}
				else
				{
					var result = func(value, widgetInfo.State);
					if (result.isEdited)
					{
						elementGetterSetterArray.SetValue(result.value);
						if (targetNode != null)
						{
							context?.CommandManager.NotifyEditFields(targetNode);
						}
						else
						{
							context?.CommandManager.NotifyEditFields((PartsTreeSystem.Asset)targetObject);
						}
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
		/// <param name="targetObject"></param>
		/// <param name="elementGetterSetterArray"></param>
		/// <param name="widgetInfo"></param>
		/// <returns></returns>
		private bool DropParameterGrid(string path, Asset.EffectAssetEditorContext context
			, object targetObject, PartsTreeSystem.ElementGetterSetterArray elementGetterSetterArray, WidgetInfo widgetInfo)
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

			var widgetFunctionKey = valueType;

			// when valueType is enum, the key is System.Enum 
			if (valueType.IsEnum)
			{
				widgetFunctionKey = typeof(System.Enum);
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
			if (!(WidgetDictionary.HasFunction(widgetFunctionKey)))
			{
				var subFields = valueType.GetFields();
				int i = 0;
				bool edited = false;
				foreach (var f in subFields)
				{
					if (value != null && !isList && widgetInfo.Children.Count > i)
					//if (!isList && widgetInfo.SubElements.Count > i)
					{
						UpdateVisiblityControllers(value);
						elementGetterSetterArray.Push(value, f);
						edited = DropParameterGrid(path, context, targetObject, elementGetterSetterArray, widgetInfo.Children[i]);
						elementGetterSetterArray.Pop();
					}
					++i;
				}

				return edited;
			}

			if (WidgetDictionary.HasDropFunction(widgetFunctionKey))
			{
				var func = WidgetDictionary.GetDropFunction(widgetFunctionKey);

				if (isList)
				{
					IList arrayValue = (IList)value;

					// generate/regenerate subElements when there are not enougth WidgetIDs.
					// generate it here because the number of elements can't read in GenerateWidgetIds
					if (arrayValue.Count > widgetInfo.Children.Count())
					{
						widgetInfo.Children.Clear();

						foreach (var v in arrayValue)
						{
							widgetInfo.Children.Add(new WidgetInfo());
						}
					}

					bool isEdited = false;
					bool isHovered = false;
					for (int j = 0; j < arrayValue.Count; j++)
					{
						var v = arrayValue[j];

						elementGetterSetterArray.Push(arrayValue, j);
						var state = widgetInfo.Children[j].State;
						var result = func(v, path, state);
						if (result.isEdited)
						{
							if (valueType.IsValueType)
							{
								elementGetterSetterArray.SetValue(result.value);
							}
							isEdited = true;
						}
						if (result.isHovered)
						{
							isHovered = true;
						}
						elementGetterSetterArray.Pop();
						++j;
					}
					if (isEdited)
					{
						field.SetValue(targetObject, arrayValue);
						context.CommandManager.NotifyEditFields((PartsTreeSystem.IInstance)targetObject);
					}
					return isHovered;
				}
				else
				{
					var result = func(value, path, widgetInfo.State);
					if (result.isEdited)
					{
						elementGetterSetterArray.SetValue(result.value);
						context.CommandManager.NotifyEditFields((PartsTreeSystem.IInstance)targetObject);
					}
					return result.isHovered;
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

		//	GenerateFieldGuiInfo(CoreContext.SelectedEffectNode);
		//}

		public void Update(Asset.EffectAssetEditorContext context, Asset.Node targetNode, object target)
		{
			// Generate field Widget IDs when the target is selected or changed.
			// TODO: this had better do at OnAfterSelect()
			if (targetNode?.InstanceID != ((Asset.Node)LastTarget)?.InstanceID)
			{
				InitializeWidgetInfo(target);
				LastTarget = targetNode;
				return;
			}
			LastTarget = targetNode;

			if (BeginTable())
			{
				var fields = target.GetType().GetFields();
				int i = 0;
				var elementGetterSetterArray = new PartsTreeSystem.ElementGetterSetterArray();
				foreach (var field in fields)
				{
					elementGetterSetterArray.Push(target, field);
					UpdateVisiblityControllers(target);
					UpdateParameterGrid(context, targetNode, target, elementGetterSetterArray, RootWidgetInfo.Children[i]);
					elementGetterSetterArray.Pop();
					++i;
				}
			}
			EndTable();
		}

		public void Update(Asset.EffectAssetEditorContext context, Asset.Node targetNode, Type targetType = null)
		{
			// Generate field Widget IDs when the target is selected or changed.
			// TODO: this had better do at OnAfterSelect()
			if (targetNode?.InstanceID != ((Asset.Node)LastTarget)?.InstanceID)
			{
				InitializeWidgetInfo(targetNode, targetType);
				LastTarget = targetNode;
				return;
			}
			LastTarget = targetNode;

			if (BeginTable())
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
					UpdateParameterGrid(context, targetNode, targetNode, elementGetterSetterArray, RootWidgetInfo.Children[i]);
					elementGetterSetterArray.Pop();
					++i;
				}
			}
			EndTable();
		}


		public void Update(Asset.EffectAssetEditorContext context, PartsTreeSystem.Asset targetAsset)
		{
			// Generate field Widget IDs when the target is selected or changed.
			// TODO: this had better do at OnAfterSelect()
			if (targetAsset != null && !ReferenceEquals(targetAsset, LastTarget))
			{
				InitializeWidgetInfo(targetAsset);
				LastTarget = targetAsset;
				return;
			}
			LastTarget = targetAsset;

			if (BeginTable())
			{

				var fields = targetAsset.GetType().GetFields();
				int i = 0;
				var elementGetterSetterArray = new PartsTreeSystem.ElementGetterSetterArray();
				foreach (var field in fields)
				{
					elementGetterSetterArray.Push(targetAsset, field);
					UpdateVisiblityControllers(targetAsset);
					UpdateParameterGrid(context, null, targetAsset, elementGetterSetterArray, RootWidgetInfo.Children[i]);
					elementGetterSetterArray.Pop();
					++i;
				}
			}
			EndTable();
		}

		public bool Drop(string path, Asset.EffectAssetEditorContext context, Asset.Node targetNode, Type targetType = null)
		{
			// skip if WidgetInfo is not yet generated
			if (RootWidgetInfo.Children.Count <= 0)
			{
				return false;
			}

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
				var eddited = DropParameterGrid(path, context, targetNode, elementGetterSetterArray, RootWidgetInfo.Children[i]);
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

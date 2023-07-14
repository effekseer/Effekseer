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
	/// This can be accessed from inspector widgets.<br/>
	/// Basically, this has data only for the current widget.
	/// </summary>
	class InspectorGuiState
	{
		public string Id { get; private set; }

		public object UserData { get; set; }


		public List<Attribute> Attriubutes { get; private set; }

		public InspectorGuiState(string id)
		{
			Id = id;
			Attriubutes = new List<Attribute>();
		}
	}


	/// <summary>
	/// Information about field and subelements.
	/// Subelements are held as a tree structure by the inspector.
	/// </summary>
	class InspectorGuiInfo
	{
		public InspectorGuiInfo()
		{
			State = new InspectorGuiState("###" + Manager.GetUniqueID().ToString());
			SubElements = new List<InspectorGuiInfo>();
		}

		public bool isRoot = false;

		public InspectorGuiState State { get; private set; }

		public string Name { get; set; }

		// Sub elements of gui(ex. list, class, struct)
		public List<InspectorGuiInfo> SubElements { get; private set; }
	}

	public class Inspector
	{
		// Function to process the widgets
		private readonly InspectorGuiDictionary GuiDictionary = new InspectorGuiDictionary();

		private InspectorGuiInfo RootGuiInfo = new InspectorGuiInfo();

		private object LastTarget = null;

		private Dictionary<int, object> VisiblityControllers = new Dictionary<int, object>();

		public Inspector()
		{
			RootGuiInfo.isRoot = true;
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
		/// Generate InspectorGuiInfo(recursively)
		/// </summary>
		/// <param name="targetValue">Instance that generate info</param>
		/// <param name="targetFieldInfo">FieldInfo that generate info</param>
		/// <returns></returns>
		InspectorGuiInfo GenerateInspectorGuiInfo(object targetValue, FieldInfo targetFieldInfo)
		{
			InspectorGuiInfo info = new InspectorGuiInfo();

			var targetType = targetFieldInfo.FieldType;
			info.Name = targetFieldInfo.Name + "(type: " + targetType.Name + ")";

			var attributes = targetFieldInfo.GetCustomAttributes();
			foreach (var attr in attributes)
			{
				info.State.Attriubutes.Add(attr);
			}

			bool hasGuiFunction =
						targetType.IsEnum ||	// Enum is not supported by HasFunction
						GuiDictionary.HasFunction(targetType);
			if (hasGuiFunction)
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

				info.SubElements.Add(GenerateInspectorGuiInfo(
					fieldInTgt != null ? fieldInTgt.GetValue(targetValue) : null, 
					fieldInTgt));
			}
			return info;
		}

		/// <summary>
		/// Generate fieldGuiInfo recursively for each field.<br/>
		/// Existing fieldGuiInfo will be cleared.
		/// </summary>
		/// <param name="target">Target node or target object</param>
		/// <param name="targetType">Type of field in target. If this is null, all fields in target will be generated.</param>
		private void InitializeFieldGuiInfo(object target, Type targetType = null)
		{
			RootGuiInfo.SubElements.Clear();
			RootGuiInfo.State.Attriubutes.Clear();

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
				RootGuiInfo.SubElements.Add(GenerateInspectorGuiInfo(targetValue, field));
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
				swig.TableFlags.BordersInnerV |
				swig.TableFlags.SizingFixedFit | swig.TableFlags.SizingStretchProp |
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
		/// Update the inspector gui recursively for each field.
		/// </summary>
		/// <param name="context">EditorContext</param>
		/// <param name="targetNode">Node that is being inspected.<br/>
		/// If it's null, also possible to inspect objects that do not belong to Node.</param>
		/// <param name="targetObject">Object to be inspected. Processed in this function.</param>
		/// <param name="elementGetterSetterArray"></param>
		/// <param name="guiInfo">GuiInfo about targetObject</param>
		private void UpdateObjectGuis(
			Asset.EffectAssetEditorContext context, 
			Asset.Node targetNode, object targetObject, 
			PartsTreeSystem.ElementGetterSetterArray elementGetterSetterArray, InspectorGuiInfo guiInfo)
		{
			var field = elementGetterSetterArray.FieldInfo.Last();

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

			var guiFunctionKey = valueType;

			// when valueType is enum, the key is System.Enum 
			if (valueType.IsEnum)
			{
				guiFunctionKey = typeof(System.Enum);
			}


			// key attrs
			string description = string.Empty;
			bool enableDescription = true;
			string labelStr = string.Empty;
			{
				var attr = (KeyAttribute)field.GetCustomAttribute(typeof(KeyAttribute));
				if (attr != null)
				{
					string key = attr.key + "_Name";

					if (MultiLanguageTextProvider.HasKey(key))
					{
						labelStr = MultiLanguageTextProvider.GetText(key);
					}
					else if (MultiLanguageTextProvider.HasKey(attr.key))
					{
						enableDescription = false;
						labelStr = MultiLanguageTextProvider.GetText(attr.key);
					}

					key = attr.key + "_Desc";
					if (MultiLanguageTextProvider.HasKey(key))
					{
						description = MultiLanguageTextProvider.GetText(key);
					}
					else if (MultiLanguageTextProvider.HasKey(attr.key))
					{
						//description = MultiLanguageTextProvider.HasKey(attr.key);
					}
				}
			}
			// VisiblityControllerAttributes
			bool isVisibilityController = false;
			{
				var attr = (Asset.VisiblityControlledAttribute)field.GetCustomAttribute(typeof(Asset.VisiblityControlledAttribute));
				if (attr != null)
				{
					isVisibilityController = true;
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
			if (!GuiDictionary.HasFunction(guiFunctionKey))
			{
				// Toggle mode
				if (value is Asset.IToggleMode toggleMode)
				{
					// End the current table for display a toggle
					EndTable();

					var toggleId = "###" + guiInfo.State.Id + "_toggle";
					var collapsingHeaderLabel = labelStr + "###" + guiInfo.State.Id + "_label";
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
					if (value != null && !isList && guiInfo.SubElements.Count > i)
					{
						UpdateVisiblityControllers(value);
						elementGetterSetterArray.Push(value, f);
						UpdateObjectGuis(context, targetNode, targetObject, elementGetterSetterArray, guiInfo.SubElements[i]);
						elementGetterSetterArray.Pop();
					}
					++i;
				}

				return;
			}


			Manager.NativeManager.TableNextRow();

			// name column(left side of table)
			Manager.NativeManager.TableNextColumn();

			bool isShowHorizonalSeparator = isVisibilityController;
			if (isShowHorizonalSeparator)
			{
				Manager.NativeManager.Separator();
			}

			// for debugging. this should be delete.
			//name = (isValueChanged ? "*" : "") + name + " " + guiInfo.Id;

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
					var result = func(value, guiInfo.State);
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
		/// <param name="guiInfo"></param>
		/// <returns></returns>
		private bool DropObjectGuis(string path, Asset.EffectAssetEditorContext context
			, object targetObject, PartsTreeSystem.ElementGetterSetterArray elementGetterSetterArray, InspectorGuiInfo guiInfo)
		{
			var field = elementGetterSetterArray.FieldInfo.Last();
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
			if (!(GuiDictionary.HasFunction(guiFunctionKey)))
			{
				var subFields = valueType.GetFields();
				int i = 0;
				bool edited = false;
				foreach (var f in subFields)
				{
					if (value != null && !isList && guiInfo.SubElements.Count > i)
					//if (!isList && guiInfo.SubElements.Count > i)
					{
						UpdateVisiblityControllers(value);
						elementGetterSetterArray.Push(value, f);
						edited = DropObjectGuis(path, context, targetObject, elementGetterSetterArray, guiInfo.SubElements[i]);
						elementGetterSetterArray.Pop();
					}
					++i;
				}

				return edited;
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
						field.SetValue(targetObject, arrayValue);
						context.CommandManager.NotifyEditFields((PartsTreeSystem.IInstance)targetObject);
						return true;
					}
				}
				else
				{
					var result = func(value, path, guiInfo.State);
					if (result.isEdited)
					{
						elementGetterSetterArray.SetValue(result.value);
						context.CommandManager.NotifyEditFields((PartsTreeSystem.IInstance)targetObject);
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

		//	GenerateFieldGuiInfo(CoreContext.SelectedEffectNode);
		//}

		public void Update(Asset.EffectAssetEditorContext context, Asset.Node targetNode, object target)
		{
			// Generate field GUI IDs when the target is selected or changed.
			// TODO: this had better do at OnAfterSelect()
			if (targetNode?.InstanceID != ((Asset.Node)LastTarget)?.InstanceID)
			{
				InitializeFieldGuiInfo(target);
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
					UpdateObjectGuis(context, targetNode, target, elementGetterSetterArray, RootGuiInfo.SubElements[i]);
					elementGetterSetterArray.Pop();
					++i;
				}
			}
			EndTable();
		}

		public void Update(Asset.EffectAssetEditorContext context, Asset.Node targetNode, Type targetType = null)
		{
			// Generate field GUI IDs when the target is selected or changed.
			// TODO: this had better do at OnAfterSelect()
			if (targetNode?.InstanceID != ((Asset.Node)LastTarget)?.InstanceID)
			{
				InitializeFieldGuiInfo(targetNode, targetType);
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
					UpdateObjectGuis(context, targetNode, targetNode, elementGetterSetterArray, RootGuiInfo.SubElements[i]);
					elementGetterSetterArray.Pop();
					++i;
				}
			}
			EndTable();
		}


		public void Update(Asset.EffectAssetEditorContext context, PartsTreeSystem.Asset targetAsset)
		{
			// Generate field GUI IDs when the target is selected or changed.
			// TODO: this had better do at OnAfterSelect()
			if (targetAsset != null && !ReferenceEquals(targetAsset, LastTarget))
			{
				InitializeFieldGuiInfo(targetAsset);
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
					UpdateObjectGuis(context, null, targetAsset, elementGetterSetterArray, RootGuiInfo.SubElements[i]);
					elementGetterSetterArray.Pop();
					++i;
				}
			}
			EndTable();
		}

		public bool Drop(string path, Asset.EffectAssetEditorContext context, Asset.Node targetNode, Type targetType = null)
		{
			// skip if GuiInfo is not yet generated
			if (RootGuiInfo.SubElements.Count <= 0)
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

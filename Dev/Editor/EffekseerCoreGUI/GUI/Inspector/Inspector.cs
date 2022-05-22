using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;

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

	class Inspector
	{
		public static void Update(NodeTreeGroupContext context, EditorState editorState, InspectorEditable target)
		{
			var commandManager = context.CommandManager;
			var nodeTreeGroup = context.NodeTreeGroup;
			var nodeTree = context.NodeTree;
			var partsList = editorState.PartsList;
			var env = editorState.Env;
			var nodeTreeGroupEditorProperty = context.EditorProperty;

			var fields = target.GetType().GetFields();

			var getterSetters = new FieldGetterSetter[1];
			getterSetters[0] = new FieldGetterSetter();

			foreach (var field in fields)
			{
				getterSetters[0].Reset(target, field);
				var prop = context.EditorProperty.Properties.FirstOrDefault(_ => _.InstanceID == target.InstanceID);
				bool isValueChanged = false;
				if (prop != null)
				{
					isValueChanged = prop.IsValueEdited(getterSetters.Select(_ => _.GetName()).ToArray());
				}

				var getterSetter = getterSetters.Last();
				var value = getterSetter.GetValue();
				var name = getterSetter.GetName();

				if (isValueChanged)
				{
					name = "*" + name;
				}
				else
				{
					name = " " + name;
				}

				if (value is int i)
				{
					var iarray = new[] { i };

					if (Manager.NativeManager.DragInt(name + "###" + field.Name, iarray, 1))
					{
						field.SetValue(target, iarray[0]);
						context.CommandManager.NotifyEditFields(target);
					}
				}
			}
		}
	}

	class InspectorEditable : Node
	{
		public int Int1;
		public int Int2;
	}
}

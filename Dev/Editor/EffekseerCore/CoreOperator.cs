using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Utils;
using System.Xml;
using Effekseer.Data;
using Effekseer.IO;

namespace Effekseer
{
	public class CoreOperator
	{
		public static void New()
		{
			CoreContext.SelectedEffectNode = null;
			CoreContext.SelectedEffect = null;

			var newEffect = new Asset.EffectAsset();
			newEffect.New(CoreContext.Environment);
			CoreData.EffectAssets = new Asset.EffectAsset[] { newEffect };

			var context = new CoreEffectContext();
			context.Asset = newEffect;
			context.Context = newEffect.CreateEditorContext(CoreContext.Environment);
			CoreContext.SelectedEffect = context;
		}

		public static string CopyNode()
		{
			if (CoreContext.SelectedEffect == null)
			{
				return string.Empty;
			}

			return CoreContext.SelectedEffect.Asset.NodeTreeAsset.Copy(CoreContext.SelectedEffectNode.InstanceID, CoreContext.Environment);
		}

		public static void PasteNode(string data)
		{
			if (CoreContext.SelectedEffect == null)
			{
				return;
			}

			CoreContext.SelectedEffect.Context.CommandManager.PushMergingBlock();

			var id = CoreContext.SelectedEffect.Context.CommandManager.AddNode(
			CoreContext.SelectedEffect.Asset.NodeTreeAsset,
			CoreContext.SelectedEffect.Context.NodeTree,
			CoreContext.SelectedEffectNode.InstanceID,
			typeof(Asset.ParticleNode),
			CoreContext.Environment);


			CoreContext.SelectedEffect.Context.CommandManager.Paste(
			CoreContext.SelectedEffect.Asset.NodeTreeAsset,
			CoreContext.SelectedEffect.Context.NodeTree,
			id,
			data,
			CoreContext.Environment);

			CoreContext.SelectedEffect.Context.CommandManager.PopMergingBlock();

			CoreContext.SelectedEffectNode = null;
		}

		public static void OverwriteNode(string data)
		{
			if (CoreContext.SelectedEffect == null)
			{
				return;
			}

			CoreContext.SelectedEffect.Context.CommandManager.Paste(
			CoreContext.SelectedEffect.Asset.NodeTreeAsset,
			CoreContext.SelectedEffect.Context.NodeTree,
			CoreContext.SelectedEffectNode.InstanceID,
			data,
			CoreContext.Environment);
			CoreContext.SelectedEffectNode = null;
		}

		public static void AddNode()
		{
			if (CoreContext.SelectedEffect == null)
			{
				return;
			}

			CoreContext.SelectedEffect.Context.CommandManager.AddNode(
			CoreContext.SelectedEffect.Asset.NodeTreeAsset,
			CoreContext.SelectedEffect.Context.NodeTree,
			CoreContext.SelectedEffectNode.InstanceID,
			typeof(Asset.ParticleNode),
			CoreContext.Environment);
		}

		public static void RemoveNode()
		{
			if (CoreContext.SelectedEffect == null)
			{
				return;
			}

			CoreContext.SelectedEffect.Context.CommandManager.RemoveNode(
			CoreContext.SelectedEffect.Asset.NodeTreeAsset,
			CoreContext.SelectedEffect.Context.NodeTree,
			CoreContext.SelectedEffectNode.InstanceID,
			CoreContext.Environment);
			CoreContext.SelectedEffectNode = null;
		}

		public static void Undo()
		{
			if (CoreContext.SelectedEffect == null)
			{
				return;
			}

			CoreContext.SelectedEffect.Context.Undo();
		}

		public static void Redo()
		{
			if (CoreContext.SelectedEffect == null)
			{
				return;
			}

			CoreContext.SelectedEffect.Context.Redo();
		}
	}
}
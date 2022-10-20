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

			var newEffect = new EffectAsset.EffectAsset();
			newEffect.New(CoreContext.Environment);
			CoreData.EffectAssets = new EffectAsset.EffectAsset[] { newEffect };

			var context = new CoreEffectContext();
			context.Asset = newEffect;
			context.Context = newEffect.CreateEditorContext(CoreContext.Environment);
			CoreContext.SelectedEffect = context;
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
			typeof(EffectAsset.ParticleNode),
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
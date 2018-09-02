using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utl;

namespace Effekseer.Binary
{
	public class Exporter
	{
		const int Version = 13;

		/// <summary>
		/// エフェクトデータの出力
		/// </summary>
		/// <returns></returns>
		public static byte[] Export(float magnification = 1.0f)
		{
			List<byte[]> data = new List<byte[]>();

			// ヘッダ
			data.Add(Encoding.UTF8.GetBytes("SKFE"));

			// バージョン
			data.Add(BitConverter.GetBytes(Version));

			// テクスチャ名称一覧取得
            SortedSet<string> textures = new SortedSet<string>();

			SortedSet<string> normalTextures = new SortedSet<string>();

			SortedSet<string> distortionTextures = new SortedSet<string>();

            // ウェーブ名称一覧取得
            SortedSet<string> waves = new SortedSet<string>();

			// モデル名称一覧取得
			SortedSet<string> models = new SortedSet<string>();

			Action<Data.NodeBase> get_textures = null;
			get_textures = (node) =>
				{
					if (node is Data.Node)
					{
						var _node = node as Data.Node;

						if (!_node.IsRendered)
						{
						}
						else
						{
							{
								var relative_path = _node.RendererCommonValues.ColorTexture.RelativePath;
								if (relative_path != string.Empty)
								{
									if(_node.RendererCommonValues.Distortion.Value)
									{
										if (!distortionTextures.Contains(relative_path))
										{
											distortionTextures.Add(relative_path);
										}
									}
									else
									{
										if (!textures.Contains(relative_path))
										{
											textures.Add(relative_path);
										}
									}
								}
							}

							{
								var relative_path = _node.DrawingValues.Model.NormalTexture.RelativePath;
								if (relative_path != string.Empty)
								{
									if (!normalTextures.Contains(relative_path))
									{
										normalTextures.Add(relative_path);
									}
								}
							}
						}
					}

					for (int i = 0; i < node.Children.Count; i++)
					{
						get_textures(node.Children[i]);
					}
				};

			get_textures(Core.Root);

            Dictionary<string, int> texture_and_index = new Dictionary<string, int>();
            {
                int index = 0;
                foreach (var texture in textures)
                {
                    texture_and_index.Add(texture, index);
                    index++;
                }
            }

			Dictionary<string, int> normalTexture_and_index = new Dictionary<string, int>();
			{
				int index = 0;
				foreach (var texture in normalTextures)
				{
					normalTexture_and_index.Add(texture, index);
					index++;
				}
			}

			Dictionary<string, int> distortionTexture_and_index = new Dictionary<string, int>();
			{
				int index = 0;
				foreach (var texture in distortionTextures)
				{
					distortionTexture_and_index.Add(texture, index);
					index++;
				}
			}

            Action<Data.NodeBase> get_waves = null;
            get_waves = (node) =>
            {
                if (node is Data.Node)
                {
                    var _node = node as Data.Node;

                    if (_node.SoundValues.Type.GetValue() == Data.SoundValues.ParamaterType.None)
                    {
                    }
                    else if (_node.SoundValues.Type.GetValue() == Data.SoundValues.ParamaterType.Use)
                    {
                        var relative_path = _node.SoundValues.Sound.Wave.RelativePath;
                        if (relative_path != string.Empty)
                        {
                            if (!waves.Contains(relative_path))
                            {
                                waves.Add(relative_path);
                            }
                        }
                    }
                }

                for (int i = 0; i < node.Children.Count; i++)
                {
                    get_waves(node.Children[i]);
                }
            };

            get_waves(Core.Root);

            Dictionary<string, int> wave_and_index = new Dictionary<string, int>();
            {
                int index = 0;
                foreach (var wave in waves)
                {
                    wave_and_index.Add(wave, index);
                    index++;
                }
            }

			Action<Data.NodeBase> get_models = null;
			get_models = (node) =>
			{
				if (node is Data.Node)
				{
					var _node = node as Data.Node;

					if (_node.IsRendered && _node.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Model)
					{
						var relative_path = _node.DrawingValues.Model.Model.RelativePath;

                        if (!string.IsNullOrEmpty(relative_path))
                        {
							if(string.IsNullOrEmpty(System.IO.Path.GetDirectoryName(relative_path)))
							{
								relative_path = System.IO.Path.GetFileNameWithoutExtension(relative_path) + ".efkmodel";
							}
							else
							{
								relative_path = System.IO.Path.GetDirectoryName(relative_path) + "/" + System.IO.Path.GetFileNameWithoutExtension(relative_path) + ".efkmodel";
							}

							if (relative_path != string.Empty)
                            {
                                if (!models.Contains(relative_path))
                                {
                                    models.Add(relative_path);
                                }
                            }
                        }
                       
					}

					if (_node.GenerationLocationValues.Type.Value == Data.GenerationLocationValues.ParameterType.Model)
					{
						var relative_path = _node.GenerationLocationValues.Model.Model.RelativePath;

                        if (!string.IsNullOrEmpty(relative_path))
                        {
							if (string.IsNullOrEmpty(System.IO.Path.GetDirectoryName(relative_path)))
							{
								relative_path = System.IO.Path.GetFileNameWithoutExtension(relative_path) + ".efkmodel";
							}
							else
							{
								relative_path = System.IO.Path.GetDirectoryName(relative_path) + "/" + System.IO.Path.GetFileNameWithoutExtension(relative_path) + ".efkmodel";
							}

							if (relative_path != string.Empty)
                            {
                                if (!models.Contains(relative_path))
                                {
                                    models.Add(relative_path);
                                }
                            }
                        }
					}
				}

				for (int i = 0; i < node.Children.Count; i++)
				{
					get_models(node.Children[i]);
				}
			};

			get_models(Core.Root);

			Dictionary<string, int> model_and_index = new Dictionary<string, int>();
			{
				int index = 0;
				foreach (var model in models)
				{
					model_and_index.Add(model, index);
					index++;
				}
			}

			// get all nodes
			var nodes = new List<Data.Node>();

			Action<Data.NodeBase> get_nodes = null;
			get_nodes = (node) =>
			{
				if (node is Data.Node)
				{
					var _node = node as Data.Node;
					nodes.Add(_node);
				}

				for (int i = 0; i < node.Children.Count; i++)
				{
					get_nodes(node.Children[i]);
				}
			};

			get_nodes(Core.Root);

			var snode2ind = nodes.
				Select((v, i) => Tuple.Create(v, i)).
				OrderBy(_ => _.Item1.DepthValues.DrawingPriority.Value * 255 + _.Item2).
				Select((v, i) => Tuple.Create(v.Item1, i)).ToList();

				// ファイルにテクスチャ一覧出力
				data.Add(BitConverter.GetBytes(texture_and_index.Count));
			foreach (var texture in texture_and_index)
			{
				var path = Encoding.Unicode.GetBytes(texture.Key);
				data.Add(((path.Count() + 2) / 2).GetBytes());
				data.Add(path);
				data.Add(new byte[] { 0, 0 });
			}

			data.Add(BitConverter.GetBytes(normalTexture_and_index.Count));
			foreach (var texture in normalTexture_and_index)
			{
				var path = Encoding.Unicode.GetBytes(texture.Key);
				data.Add(((path.Count() + 2) / 2).GetBytes());
				data.Add(path);
				data.Add(new byte[] { 0, 0 });
			}

			data.Add(BitConverter.GetBytes(distortionTexture_and_index.Count));
			foreach (var texture in distortionTexture_and_index)
			{
				var path = Encoding.Unicode.GetBytes(texture.Key);
				data.Add(((path.Count() + 2) / 2).GetBytes());
				data.Add(path);
				data.Add(new byte[] { 0, 0 });
			}

            // ファイルにウェーブ一覧出力
            data.Add(BitConverter.GetBytes(wave_and_index.Count));
            foreach (var wave in wave_and_index)
            {
                var path = Encoding.Unicode.GetBytes(wave.Key);
                data.Add(((path.Count() + 2) / 2).GetBytes());
                data.Add(path);
                data.Add(new byte[] { 0, 0 });
            }

			// ファイルにモデル一覧出力
			data.Add(BitConverter.GetBytes(model_and_index.Count));
			foreach (var model in model_and_index)
			{
				var path = Encoding.Unicode.GetBytes(model.Key);
				data.Add(((path.Count() + 2) / 2).GetBytes());
				data.Add(path);
				data.Add(new byte[] { 0, 0 });
			}

			// Export the number of nodes
			data.Add(BitConverter.GetBytes(snode2ind.Count));

			var renderPriorityThreshold = snode2ind.Where(_ => _.Item1.DepthValues.DrawingPriority.Value < 0).Count();
			data.Add(BitConverter.GetBytes(renderPriorityThreshold));

			// Export magnification
			data.Add(BitConverter.GetBytes(magnification));

			// Export default seed
			int randomSeed = Core.Global.RandomSeed.Value;
			data.Add(BitConverter.GetBytes(randomSeed));

			// カリングを出力
			data.Add(BitConverter.GetBytes((int)Core.Culling.Type.Value));

			if (Core.Culling.Type.Value == Data.EffectCullingValues.ParamaterType.Sphere)
			{
				data.Add(BitConverter.GetBytes(Core.Culling.Sphere.Radius));
				data.Add(BitConverter.GetBytes(Core.Culling.Sphere.Location.X));
				data.Add(BitConverter.GetBytes(Core.Culling.Sphere.Location.Y));
				data.Add(BitConverter.GetBytes(Core.Culling.Sphere.Location.Z));
			}
			
			// ノード情報出力
			Action<Data.NodeRoot> outout_rootnode = null;
			Action<Data.Node> outout_node = null;

			outout_rootnode = (n) =>
				{
					data.Add(((int)NodeType.Root).GetBytes());
					data.Add(n.Children.Count.GetBytes());
					for (int i = 0; i < n.Children.Count; i++)
					{
						outout_node(n.Children[i]);
					}
				};

			outout_node = (n) =>
			{
				List<byte[]> node_data = new List<byte[]>();

				var isRenderParamExported = n.IsRendered.GetValue();

				for (int i = 0; i < n.Children.Count; i++)
				{
					var nc = n.Children[i];
					var v = nc.RendererCommonValues.ColorInheritType.GetValue();
					if (v == Data.ParentEffectType.Already || v == Data.ParentEffectType.WhenCreating)
					{
						isRenderParamExported = true;
						break;
					}
				}

				if (!isRenderParamExported)
				{
					data.Add(((int)NodeType.None).GetBytes());
				}
				else if (n.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.None)
				{
					data.Add(((int)NodeType.None).GetBytes());
				}
				else if (n.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Sprite)
				{
					data.Add(((int)NodeType.Sprite).GetBytes());
				}
				else if (n.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Ribbon)
				{
					data.Add(((int)NodeType.Ribbon).GetBytes());
				}
				else if (n.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Ring)
				{
					data.Add(((int)NodeType.Ring).GetBytes());
				}
				else if (n.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Model)
				{
					data.Add(((int)NodeType.Model).GetBytes());
				}
				else if (n.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Track)
				{
					data.Add(((int)NodeType.Track).GetBytes());
				}
				else
				{
					throw new Exception();
				}

				// Whether to draw the node.
				if (n.IsRendered)
				{
					int v = 1;
					node_data.Add(BitConverter.GetBytes(v));
				}
				else
				{
					int v = 0;
					node_data.Add(BitConverter.GetBytes(v));
				}

				// render order
				{
					var s = snode2ind.FirstOrDefault(_ => _.Item1 == n);
					if(s.Item1 != null)
					{
						node_data.Add(BitConverter.GetBytes(s.Item2));
					}
					else
					{
						node_data.Add(BitConverter.GetBytes(-1));
					}
				}

				node_data.Add(CommonValues.GetBytes(n.CommonValues));
				node_data.Add(LocationValues.GetBytes(n.LocationValues, n.CommonValues.ScaleEffectType));
				node_data.Add(LocationAbsValues.GetBytes(n.LocationAbsValues, n.CommonValues.ScaleEffectType));
				node_data.Add(RotationValues.GetBytes(n.RotationValues));
				node_data.Add(ScaleValues.GetBytes(n.ScalingValues, n.CommonValues.ScaleEffectType));
				node_data.Add(GenerationLocationValues.GetBytes(n.GenerationLocationValues, n.CommonValues.ScaleEffectType, model_and_index));

				// Export depth
                node_data.Add(n.DepthValues.DepthOffset.Value.GetBytes());
				node_data.Add(BitConverter.GetBytes(n.DepthValues.IsScaleChangedDependingOnDepthOffset.Value ? 1 : 0));
				node_data.Add(BitConverter.GetBytes(n.DepthValues.IsDepthOffsetChangedDependingOnParticleScale.Value ? 1 : 0));
				node_data.Add(((int)n.DepthValues.ZSort.Value).GetBytes());
				node_data.Add(n.DepthValues.DrawingPriority.Value.GetBytes());
				node_data.Add(n.DepthValues.SoftParticle.Value.GetBytes());

                node_data.Add(RendererCommonValues.GetBytes(n.RendererCommonValues, texture_and_index, distortionTexture_and_index));

				if (isRenderParamExported)
				{
					node_data.Add(RendererValues.GetBytes(n.DrawingValues, texture_and_index, normalTexture_and_index, model_and_index));
				}
				else
				{
					node_data.Add(RendererValues.GetBytes(null, texture_and_index, normalTexture_and_index, model_and_index));
				}

				data.Add(node_data.ToArray().ToArray());

				data.Add(SoundValues.GetBytes(n.SoundValues, wave_and_index));

				data.Add(n.Children.Count.GetBytes());
				for (int i = 0; i < n.Children.Count; i++)
				{
					outout_node(n.Children[i]);
				}
			};

			outout_rootnode(Core.Root);

			return data.ToArray().ToArray();
		}
	}
}

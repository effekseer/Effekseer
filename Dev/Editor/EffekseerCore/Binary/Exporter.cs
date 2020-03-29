using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utl;

namespace Effekseer.Binary
{
	public enum ExporterVersion
	{
		Ver1500 = 1500,
		Ver1600 = 1600,

#if __EFFEKSEER_BUILD_VERSION16__
		Latest = 1600,
#else
		Latest = 1500,
#endif
	}

	public class Exporter
	{
		public HashSet<string> UsedTextures = new HashSet<string>();

		public HashSet<string> UsedNormalTextures = new HashSet<string>();

		public HashSet<string> UsedDistortionTextures = new HashSet<string>();

		public HashSet<string> Sounds = new HashSet<string>();

		public HashSet<string> Models = new HashSet<string>();

		public HashSet<string> Materials = new HashSet<string>();

		/// <summary>
		/// Export effect data
		/// </summary>
		/// <returns></returns>
		public byte[] Export(float magnification = 1.0f, ExporterVersion exporterVersion = ExporterVersion.Latest)
		{
			List<byte[]> data = new List<byte[]>();

			// ヘッダ
			data.Add(Encoding.UTF8.GetBytes("SKFE"));

			// Version
			data.Add(BitConverter.GetBytes((int)exporterVersion));

			// reset texture names
			UsedTextures = new HashSet<string>();

			UsedNormalTextures = new HashSet<string>();

			UsedDistortionTextures = new HashSet<string>();

            Sounds = new HashSet<string>();

			Models = new HashSet<string>();

			Materials = new HashSet<string>();

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
							if(_node.RendererCommonValues.Material.Value == Data.RendererCommonValues.MaterialType.Default)
							{
								var relative_path = _node.RendererCommonValues.ColorTexture.RelativePath;
								if (relative_path != string.Empty)
								{
									if (!UsedTextures.Contains(relative_path))
									{
										UsedTextures.Add(relative_path);
									}
								}

#if __EFFEKSEER_BUILD_VERSION16__
								var alpha_relative_path = _node.RendererCommonValues.AlphaTextureParam.Texture.RelativePath;
								if (_node.RendererCommonValues.EnableAlphaTexture && alpha_relative_path != string.Empty)
								{
									if (!UsedTextures.Contains(alpha_relative_path))
									{
										UsedTextures.Add(alpha_relative_path);
									}
								}
#endif
							}
							if (_node.RendererCommonValues.Material.Value == Data.RendererCommonValues.MaterialType.BackDistortion )
							{
								var relative_path = _node.RendererCommonValues.ColorTexture.RelativePath;
								if (relative_path != string.Empty)
								{
									if (!UsedDistortionTextures.Contains(relative_path))
									{
										UsedDistortionTextures.Add(relative_path);
									}
								}

#if __EFFEKSEER_BUILD_VERSION16__
								var alpha_relative_path = _node.RendererCommonValues.AlphaTextureParam.Texture.RelativePath;
								if (_node.RendererCommonValues.EnableAlphaTexture && alpha_relative_path != string.Empty)
								{
									if (!UsedDistortionTextures.Contains(alpha_relative_path))
									{
										UsedDistortionTextures.Add(alpha_relative_path);
									}
								}
#endif
							}
							if (_node.RendererCommonValues.Material.Value == Data.RendererCommonValues.MaterialType.Lighting)
							{
								var path1 = _node.RendererCommonValues.ColorTexture.RelativePath;
								if (path1 != string.Empty)
								{
									if (!UsedTextures.Contains(path1))
									{
										UsedTextures.Add(path1);
									}
								}

								var path2 = _node.RendererCommonValues.NormalTexture.RelativePath;
								if (path2 != string.Empty)
								{
									if (!UsedNormalTextures.Contains(path2))
									{
										UsedNormalTextures.Add(path2);
									}
								}

#if __EFFEKSEER_BUILD_VERSION16__
								var path3 = _node.RendererCommonValues.AlphaTextureParam.Texture.RelativePath;
								if (_node.RendererCommonValues.EnableAlphaTexture && path3 != string.Empty)
								{
									if (!UsedTextures.Contains(path3))
									{
										UsedTextures.Add(path3);
									}
								}
#endif
							}
							else if (_node.RendererCommonValues.Material.Value == Data.RendererCommonValues.MaterialType.File)
							{
								var materialInfo = Core.ResourceCache.LoadMaterialInformation(_node.RendererCommonValues.MaterialFile.Path.AbsolutePath);
								if (materialInfo == null)
								{
									materialInfo = new MaterialInformation();
								}

								var textures = _node.RendererCommonValues.MaterialFile.GetTextures(materialInfo).Where(_ => _.Item1 != null);

								foreach (var texture in textures)
								{
									var relative_path = (texture.Item1.Value as Data.Value.PathForImage).RelativePath;
									if (relative_path != string.Empty)
									{
										if (texture.Item2.Type == TextureType.Value)
										{
											if (!UsedNormalTextures.Contains(relative_path))
											{
												UsedNormalTextures.Add(relative_path);
											}
										}
										else
										{
											if (!UsedTextures.Contains(relative_path))
											{
												UsedTextures.Add(relative_path);
											}
										}
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
                foreach (var texture in UsedTextures.ToList().OrderBy(_ => _))
                {
                    texture_and_index.Add(texture, index);
                    index++;
                }
            }

			Dictionary<string, int> normalTexture_and_index = new Dictionary<string, int>();
			{
				int index = 0;
				foreach (var texture in UsedNormalTextures.ToList().OrderBy(_ => _))
				{
					normalTexture_and_index.Add(texture, index);
					index++;
				}
			}

			Dictionary<string, int> distortionTexture_and_index = new Dictionary<string, int>();
			{
				int index = 0;
				foreach (var texture in UsedDistortionTextures.ToList().OrderBy(_ => _))
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
                            if (!Sounds.Contains(relative_path))
                            {
                                Sounds.Add(relative_path);
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
                foreach (var wave in Sounds.ToList().OrderBy(_ => _))
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
								relative_path = System.IO.Path.ChangeExtension(relative_path, ".efkmodel");
							}

							if (relative_path != string.Empty)
                            {
                                if (!Models.Contains(relative_path))
                                {
                                    Models.Add(relative_path);
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
								relative_path = System.IO.Path.ChangeExtension(relative_path, ".efkmodel");
							}

							if (relative_path != string.Empty)
                            {
                                if (!Models.Contains(relative_path))
                                {
                                    Models.Add(relative_path);
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
				foreach (var model in Models.ToList().OrderBy(_ => _))
				{
					model_and_index.Add(model, index);
					index++;
				}
			}

			Action<Data.NodeBase> get_materials = null;
			get_materials = (node) =>
			{
				if (node is Data.Node)
				{
					var _node = node as Data.Node;

					if (_node.RendererCommonValues.Material.Value == Data.RendererCommonValues.MaterialType.File)
					{
						var relative_path = _node.RendererCommonValues.MaterialFile.Path.RelativePath;
						if (relative_path != string.Empty)
						{
							if (!Materials.Contains(relative_path))
							{
								Materials.Add(relative_path);
							}
						}
					}
				}

				for (int i = 0; i < node.Children.Count; i++)
				{
					get_materials(node.Children[i]);
				}
			};

			get_materials(Core.Root);

			Dictionary<string, int> material_and_index = new Dictionary<string, int>();
			{
				int index = 0;
				foreach (var wave in Materials.ToList().OrderBy(_ => _))
				{
					material_and_index.Add(wave, index);
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
				Select((v, i) => Tuple35.Create(v, i)).
				OrderBy(_ => _.Item1.DepthValues.DrawingPriority.Value * 255 + _.Item2).
				Select((v, i) => Tuple35.Create(v.Item1, i)).ToList();

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

			// export materials to a file
			data.Add(BitConverter.GetBytes(material_and_index.Count));
			foreach (var material in material_and_index)
			{
				var path = Encoding.Unicode.GetBytes(material.Key);
				data.Add(((path.Count() + 2) / 2).GetBytes());
				data.Add(path);
				data.Add(new byte[] { 0, 0 });
			}

			// export dynamic parameters
			data.Add(BitConverter.GetBytes(Core.Dynamic.Inputs.Values.Count));
			foreach (var value in Core.Dynamic.Inputs.Values)
			{
				float value_ = value.Input.Value;
				data.Add(BitConverter.GetBytes(value_));
			}

			data.Add(BitConverter.GetBytes(Core.Dynamic.Equations.Values.Count));

			var compiler = new InternalScript.Compiler();

			foreach (var value in Core.Dynamic.Equations.Values)
			{
				var cx = compiler.Compile(value.Code.Value);

				var cs = new []{ cx };

				foreach(var c in cs)
				{
					if(c.Bytecode != null)
					{
						data.Add(BitConverter.GetBytes((int)c.Bytecode.Length));
						data.Add(c.Bytecode);
					}
					else
					{
						data.Add(BitConverter.GetBytes((int)0));
					}
				}
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

				node_data.Add(((float)n.DepthValues.SuppressionOfScalingByDepth.Value).GetBytes());

				if (n.DepthValues.DepthClipping.Infinite)
				{
					node_data.Add((float.MaxValue).GetBytes());
				}
				else
				{
					node_data.Add(((float)n.DepthValues.DepthClipping.Value.Value).GetBytes());
				}
				
				node_data.Add(((int)n.DepthValues.ZSort.Value).GetBytes());
				node_data.Add(n.DepthValues.DrawingPriority.Value.GetBytes());
				node_data.Add(n.DepthValues.SoftParticle.Value.GetBytes());
				node_data.Add(RendererCommonValues.GetBytes(n.RendererCommonValues, texture_and_index, normalTexture_and_index, distortionTexture_and_index, material_and_index));


				if (isRenderParamExported)
				{
					node_data.Add(RendererValues.GetBytes(n.DrawingValues, texture_and_index, normalTexture_and_index, model_and_index));
				}
				else
				{
					node_data.Add(RendererValues.GetBytes(null, texture_and_index, normalTexture_and_index, model_and_index));
				}

#if __EFFEKSEER_BUILD_VERSION16__
				if (Version >= 1600)
				{
					node_data.Add(AlphaCrunchValues.GetBytes(n.AlphaCrunchValues));
				}
#endif

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

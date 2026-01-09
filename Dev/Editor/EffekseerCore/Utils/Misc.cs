using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Effekseer.Utils
{
	public class Misc
	{
		/// <summary>
		/// https://stackoverflow.com/questions/1646807/quick-and-simple-hash-code-combinations
		/// </summary>
		/// <param name="hashCodes"></param>
		/// <returns></returns>
		public static int CombineHashCodes(IEnumerable<int> hashCodes)
		{
			int hash = 5381;

			foreach (var hashCode in hashCodes)
				hash = ((hash << 5) + hash) ^ hashCode;

			return hash;
		}

		public static string GetRelativePath(string basePath, string path)
		{
			Func<string, string> escape = (string s) =>
			{
				return s.Replace("%", "%25");
			};

			Func<string, string> unescape = (string s) =>
			{
				return s.Replace("%25", "%");
			};

			Uri basepath = new Uri(escape(basePath));
			Uri targetPath = new Uri(escape(path));
			return unescape(Uri.UnescapeDataString(basepath.MakeRelativeUri(targetPath).ToString()));
		}
		public static string GetAbsolutePath(string basePath, string path)
		{
			if (string.IsNullOrEmpty(path))
			{
				return string.Empty;
			}

			var basePath_ecs = new Uri(basePath, UriKind.Relative);
			var path_ecs = new Uri(path, UriKind.Relative);
			var basePath_slash = BackSlashToSlash(basePath_ecs.ToString());
			var basePath_uri = new Uri(basePath_slash, UriKind.Absolute);
			var path_uri = new Uri(path_ecs.ToString(), UriKind.Relative);
			var targetPath = new Uri(basePath_uri, path_uri);
			var ret = targetPath.LocalPath.ToString();
			return ret;
		}

		public static string BackSlashToSlash(string input)
		{
			return input.Replace("\\", "/");
		}

		public static bool IsValidPath(string path)
		{
			foreach (char c in Path.GetInvalidPathChars())
			{
				if (path.IndexOf(c) >= 0) return false;
			}

			path = Path.GetFileName(path);

			foreach (char c in Path.GetInvalidFileNameChars())
			{
				if (path.IndexOf(c) >= 0) return false;
			}

			return true;
		}

		public static bool IsFullPath(string path)
		{
			var correctFullPath = BackSlashToSlash(Path.GetFullPath(path));
			return correctFullPath == BackSlashToSlash(path);
		}

		public static ResourcePaths FindResourcePaths(Data.NodeBase node, Binary.ExporterVersion exporterVersion, ResourcePaths resources = null)
		{
			if (resources == null)
			{
				resources = new ResourcePaths();
			}

			if (node is Data.Node)
			{
				var _node = node as Data.Node;

				if (IsRenderedNode(_node))
				{
					// Enumulate used texture image paths
					if (_node.RendererCommonValues.Material.Value == Data.RendererCommonValues.MaterialType.Default)
					{
						var relative_path = _node.RendererCommonValues.ColorTexture.RelativePath;
						if (relative_path != string.Empty)
						{
							resources.ColorTextures.Add(_node.RendererCommonValues.ColorTexture);
						}

						if (exporterVersion >= Binary.ExporterVersion.Ver16Alpha1)
						{
							var alpha_relative_path = _node.AdvancedRendererCommonValuesValues.AlphaTextureParam.Texture.RelativePath;
							if (_node.AdvancedRendererCommonValuesValues.AlphaTextureParam.Enabled && alpha_relative_path != string.Empty)
							{
								resources.ColorTextures.Add(_node.AdvancedRendererCommonValuesValues.AlphaTextureParam.Texture);
							}

							var uvDistortion_relative_path = _node.AdvancedRendererCommonValuesValues.UVDistortionTextureParam.Texture.RelativePath;
							if (_node.AdvancedRendererCommonValuesValues.UVDistortionTextureParam.Enabled && uvDistortion_relative_path != string.Empty)
							{
								resources.ColorTextures.Add(_node.AdvancedRendererCommonValuesValues.UVDistortionTextureParam.Texture);
							}

							var blend_relative_path = _node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendTextureParam.Texture.RelativePath;
							if (_node.AdvancedRendererCommonValuesValues.BlendTextureParams.Enabled && blend_relative_path != string.Empty)
							{
								resources.ColorTextures.Add(_node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendTextureParam.Texture);

								var blend_alpha_relative_path = _node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendAlphaTextureParam.Texture.RelativePath;
								if (_node.AdvancedRendererCommonValuesValues.BlendTextureParams.EnableBlendAlphaTexture &&
									blend_alpha_relative_path != string.Empty)
								{
									resources.ColorTextures.Add(_node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendAlphaTextureParam.Texture);
								}

								var blend_uv_distortion_relative_path = _node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendUVDistortionTextureParam.Texture.RelativePath;
								if (_node.AdvancedRendererCommonValuesValues.BlendTextureParams.EnableBlendUVDistortionTexture &&
									blend_uv_distortion_relative_path != string.Empty)
								{
									resources.ColorTextures.Add(_node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendUVDistortionTextureParam.Texture);
								}
							}
						}
					}
					if (_node.RendererCommonValues.Material.Value == Data.RendererCommonValues.MaterialType.BackDistortion)
					{
						var relative_path = _node.RendererCommonValues.ColorTexture.RelativePath;
						if (relative_path != string.Empty)
						{
							resources.DistortionTextures.Add(_node.RendererCommonValues.ColorTexture);
						}

						if (exporterVersion >= Binary.ExporterVersion.Ver16Alpha1)
						{
							var alpha_relative_path = _node.AdvancedRendererCommonValuesValues.AlphaTextureParam.Texture.RelativePath;
							if (_node.AdvancedRendererCommonValuesValues.AlphaTextureParam.Enabled && alpha_relative_path != string.Empty)
							{
								resources.DistortionTextures.Add(_node.AdvancedRendererCommonValuesValues.AlphaTextureParam.Texture);
							}

							var uvDistortion_relative_path = _node.AdvancedRendererCommonValuesValues.UVDistortionTextureParam.Texture.RelativePath;
							if (_node.AdvancedRendererCommonValuesValues.UVDistortionTextureParam.Enabled && uvDistortion_relative_path != string.Empty)
							{
								resources.DistortionTextures.Add(_node.AdvancedRendererCommonValuesValues.UVDistortionTextureParam.Texture);
							}

							var blend_relative_path = _node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendTextureParam.Texture.RelativePath;
							if (_node.AdvancedRendererCommonValuesValues.BlendTextureParams.Enabled && blend_relative_path != string.Empty)
							{
								resources.DistortionTextures.Add(_node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendTextureParam.Texture);

								var blend_alpha_relative_path = _node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendAlphaTextureParam.Texture.RelativePath;
								if (_node.AdvancedRendererCommonValuesValues.BlendTextureParams.EnableBlendAlphaTexture &&
									blend_alpha_relative_path != string.Empty)
								{
									resources.DistortionTextures.Add(_node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendAlphaTextureParam.Texture);
								}

								var blend_uv_distortion_relative_path = _node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendUVDistortionTextureParam.Texture.RelativePath;
								if (_node.AdvancedRendererCommonValuesValues.BlendTextureParams.EnableBlendUVDistortionTexture &&
									blend_uv_distortion_relative_path != string.Empty)
								{
									resources.DistortionTextures.Add(_node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendUVDistortionTextureParam.Texture);
								}
							}
						}
					}
					if (_node.RendererCommonValues.Material.Value == Data.RendererCommonValues.MaterialType.Lighting)
					{
						var path1 = _node.RendererCommonValues.ColorTexture.RelativePath;
						if (path1 != string.Empty)
						{
							resources.ColorTextures.Add(_node.RendererCommonValues.ColorTexture);
						}

						var path2 = _node.RendererCommonValues.NormalTexture.RelativePath;
						if (path2 != string.Empty)
						{
							resources.NormalTextures.Add(_node.RendererCommonValues.NormalTexture);
						}

						if (exporterVersion >= Binary.ExporterVersion.Ver16Alpha1)
						{
							var alpha_relative_path = _node.AdvancedRendererCommonValuesValues.AlphaTextureParam.Texture.RelativePath;
							if (_node.AdvancedRendererCommonValuesValues.AlphaTextureParam.Enabled && alpha_relative_path != string.Empty)
							{
								resources.ColorTextures.Add(_node.AdvancedRendererCommonValuesValues.AlphaTextureParam.Texture);
							}

							var uvDistortion_relative_path = _node.AdvancedRendererCommonValuesValues.UVDistortionTextureParam.Texture.RelativePath;
							if (_node.AdvancedRendererCommonValuesValues.UVDistortionTextureParam.Enabled && uvDistortion_relative_path != string.Empty)
							{
								resources.ColorTextures.Add(_node.AdvancedRendererCommonValuesValues.UVDistortionTextureParam.Texture);
							}

							var blend_relative_path = _node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendTextureParam.Texture.RelativePath;
							if (_node.AdvancedRendererCommonValuesValues.BlendTextureParams.Enabled && blend_relative_path != string.Empty)
							{
								resources.ColorTextures.Add(_node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendTextureParam.Texture);

								var blend_alpha_relative_path = _node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendAlphaTextureParam.Texture.RelativePath;
								if (_node.AdvancedRendererCommonValuesValues.BlendTextureParams.EnableBlendAlphaTexture &&
									blend_alpha_relative_path != string.Empty)
								{
									resources.ColorTextures.Add(_node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendAlphaTextureParam.Texture);
								}

								var blend_uv_distortion_relative_path = _node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendUVDistortionTextureParam.Texture.RelativePath;
								if (_node.AdvancedRendererCommonValuesValues.BlendTextureParams.EnableBlendUVDistortionTexture &&
									blend_uv_distortion_relative_path != string.Empty)
								{
									resources.ColorTextures.Add(_node.AdvancedRendererCommonValuesValues.BlendTextureParams.BlendUVDistortionTextureParam.Texture);
								}
							}
						}
					}
					if (_node.RendererCommonValues.Material.Value == Data.RendererCommonValues.MaterialType.File)
					{
						string material_path = _node.RendererCommonValues.MaterialFile.Path.AbsolutePath;
						if (material_path != string.Empty)
						{
							resources.Materials.Add(_node.RendererCommonValues.MaterialFile.Path);

							var materialInfo = Core.ResourceCache.LoadMaterialInformation(material_path);

							var textures = _node.RendererCommonValues.MaterialFile.GetTextures(materialInfo).Where(_ => _.Item1 != null);

							foreach (var texture in textures)
							{
								var pathImage = texture.Item1.Value as Data.Value.PathForImage;
								var relative_path = pathImage.RelativePath;
								if (relative_path != string.Empty)
								{
									if (texture.Item2.Type == Utils.TextureType.Value)
									{
										resources.NormalTextures.Add(pathImage);
									}
									else
									{
										resources.ColorTextures.Add(pathImage);
									}
								}
							}
						}
					}

					// Enumulate used sound wave paths
					if (_node.SoundValues.Type.GetValue() == Data.SoundValues.ParamaterType.Use)
					{
						var relative_path = _node.SoundValues.Sound.Wave.RelativePath;
						if (relative_path != string.Empty)
						{
							resources.Sounds.Add(_node.SoundValues.Sound.Wave);
						}
					}

					// Enumulate used model data path
					if (_node.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Model)
					{
						var relative_path = _node.DrawingValues.Model.Model.RelativePath;

						if (relative_path != string.Empty)
						{
							resources.Models.Add(_node.DrawingValues.Model.Model);
						}
					}

					// Enumulate used material file path
					if (_node.RendererCommonValues.Material.Value == Data.RendererCommonValues.MaterialType.File)
					{
						var relative_path = _node.RendererCommonValues.MaterialFile.Path.RelativePath;
						if (relative_path != string.Empty)
						{
							resources.Materials.Add(_node.RendererCommonValues.MaterialFile.Path);
						}
					}
				}

				// Enumulate used model data path
				if (_node.GenerationLocationValues.Type.Value == Data.GenerationLocationValues.ParameterType.Model)
				{
					var relative_path = _node.GenerationLocationValues.Model.Model.RelativePath;

					if (relative_path != string.Empty)
					{
						resources.Models.Add(_node.GenerationLocationValues.Model.Model);
					}
				}

				// Enumerate used curve paths
				if (_node.LocationValues.Type == Data.LocationValues.ParamaterType.NurbsCurve)
				{
					var relative_path = _node.LocationValues.NurbsCurve.FilePath.RelativePath;

					if (relative_path != string.Empty)
					{
						resources.Curves.Add(_node.LocationValues.NurbsCurve.FilePath);
					}
				}
			}

			for (int i = 0; i < node.Children.Count; i++)
			{
				FindResourcePaths(node.Children[i], exporterVersion, resources);
			}

			return resources;
		}

		static bool IsRenderedNode(Data.Node node)
		{
			var rendered = node.IsRendered.Value && node.DrawingValues.Type.Value != Data.RendererValues.ParamaterType.None;
			var hasSound = node.SoundValues.Type.GetValue() == Data.SoundValues.ParamaterType.Use;
			return rendered || hasSound;
		}

		static bool IsRenderedNodeGroup(Data.Node node)
		{
			if (IsRenderedNode(node))
				return true;

			foreach (var n in node.Children.Internal)
			{
				if (IsRenderedNodeGroup(n))
					return true;
			}

			return false;
		}
	}

	public class ResourcePaths
	{
		public enum Type
		{
			ColorTexture,
			NormalTexture,
			DistortionTexture,
			Model,
			Sound,
			Material,
			Curve,
		}

		public List<Data.Value.Path>[] All { get; private set; }
		public List<Data.Value.Path> ColorTextures { get { return All[(int)Type.ColorTexture]; } }
		public List<Data.Value.Path> NormalTextures { get { return All[(int)Type.NormalTexture]; } }
		public List<Data.Value.Path> DistortionTextures { get { return All[(int)Type.DistortionTexture]; } }
		public List<Data.Value.Path> Models { get { return All[(int)Type.Model]; } }
		public List<Data.Value.Path> Sounds { get { return All[(int)Type.Sound]; } }
		public List<Data.Value.Path> Materials { get { return All[(int)Type.Material]; } }
		public List<Data.Value.Path> Curves { get { return All[(int)Type.Curve]; } }

		public ResourcePaths()
		{
			All = new List<Data.Value.Path>[Enum.GetNames(typeof(Type)).Length];
			for (int i = 0; i < All.Length; i++)
			{
				All[i] = new List<Data.Value.Path>();
			}
		}
	}
}

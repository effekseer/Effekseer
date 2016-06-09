using UnityEngine;
using UnityEditor;
using System;
using System.IO;
using System.Collections;

public class EffekseerAssetPostProcessor : AssetPostprocessor
{
	static void OnPostprocessAllAssets(
		string[] importedAssets,
		string[] deletedAssets,
		string[] movedAssets,
		string[] movedFromPaths)
	{
		// インポートするときに.efkを.bytesにリネームする
		foreach(string assetPath in importedAssets) {
			if (Path.GetExtension(assetPath) == ".efk") {
				File.Move(assetPath, Path.ChangeExtension(assetPath, ".bytes"));
			}
			if (Path.GetExtension(assetPath) == ".efkmodel") {
				File.Move(assetPath, assetPath + ".bytes");
			}
		}
	}

	static bool isInEffekseerDirectory(string path)
	{
		return path.IndexOf("Effekseer") >= 0 || 
			   path.IndexOf("effekseer") >= 0;
	}

	void OnPreprocessTexture()
	{
		// パスでチェック
		if (!isInEffekseerDirectory(assetPath)) {
			return;
		}

		TextureImporter ti = assetImporter as TextureImporter;
		//ti.maxTextureSize = 4096;
		ti.isReadable = true;
		ti.mipmapEnabled = false;
		ti.textureFormat = TextureImporterFormat.ARGB32;
	}

	void OnPostprocessTexture(Texture2D texture)
	{
		// パスでチェック
		if (!isInEffekseerDirectory(assetPath)) {
			return;
		}

		// 上下反転させる
		Color32[] srcPixels = texture.GetPixels32();
		Color32[] dstPixels = new Color32[srcPixels.Length];
		int width = texture.width, height = texture.height;
		int srcIndex = 0, dstIndex = dstPixels.Length - width;
		for (int i = 0; i < height; i++) {
			Array.Copy(srcPixels, srcIndex, dstPixels, dstIndex, width);
			srcIndex += width;
			dstIndex -= width;
		}
		texture.SetPixels32(dstPixels);
		texture.Apply();
	}
}

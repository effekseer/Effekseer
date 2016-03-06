using UnityEngine;
using UnityEditor;
using System.IO;
using System.Collections;

public class EffekseerAssetPostProcessor : AssetPostprocessor
{
	static void OnPostprocessAllAssets(
		string[] importedAssets,
		string[] deletedAssets,
		string[] movedAssets,
		string[] movedFromPaths )
	{
		// インポートするときに.efkを.bytesにリネームする
		foreach(string assetPath in importedAssets) {
			if (Path.GetExtension(assetPath) == ".efk") {
				File.Move(assetPath, Path.ChangeExtension(assetPath, ".bytes"));
			}
		}
	}
}

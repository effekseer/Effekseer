using UnityEngine;
using System;
using System.Collections;

/// <summary>
/// エフェクトの発生源
/// </summary>
public class EffekseerEmitter : MonoBehaviour
{
	/// <summary>
	/// エフェクト名
	/// </summary>
	public string effectName;

	/// <summary>
	/// Start時に再生開始するかどうか
	/// </summary>
	public bool playOnStart = false;

	/// <summary>
	/// ループ再生するかどうか
	/// </summary>
	public bool loop = false;

	/// <summary>
	/// 保持しているハンドル
	/// </summary>
	private EffekseerHandle? handle;
	
	/// <summary>
	/// エフェクトを再生
	/// </summary>
	/// <param name="name">エフェクト名</param>
	public void Play(string name)
	{
		effectName = name;
		Play();
	}
	
	/// <summary>
	/// 設定されているエフェクトを再生
	/// </summary>
	public void Play()
	{
		handle = EffekseerSystem.PlayEffect(effectName, transform.position);
		UpdateTransform();
	}
	
	/// <summary>
	/// 再生中のエフェクトの停止
	/// </summary>
	public void Stop()
	{
		if (handle.HasValue) {
			handle.Value.Stop();
			handle = null;
		}
	}
	
	/// <summary>
	/// 再生中のエフェクトが存在しているか
	/// </summary>
	public bool exists
	{
		get {
			return handle.HasValue && handle.Value.exists;
		}
	}
	
	#region Internal Implimentation

	void Start()
	{
		EffekseerSystem.LoadEffect(effectName);
		if (playOnStart) {
			Play();
		}
	}
	
	void OnDestroy()
	{
		Stop();
	}
	
	void Update()
	{
		if (handle.HasValue) {
			if (handle.Value.exists) {
				UpdateTransform();
			} else if (loop) {
				Play();
			} else {
				Stop();
			}
		}
	}

	void UpdateTransform() {
		if (handle.HasValue) {
			handle.Value.SetLocation(transform.position);
			handle.Value.SetRotation(transform.rotation);
			handle.Value.SetScale(transform.localScale);
		}
	}

	#endregion
}

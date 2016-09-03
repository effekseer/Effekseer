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
		var h = EffekseerSystem.PlayEffect(effectName, transform.position);
		h.SetRotation(transform.rotation);
		h.SetScale(transform.localScale);
		handle = h;
	}
	
	/// <summary>
	/// 再生中のエフェクトを停止
	/// 全てのエフェクトが瞬時に消える
	/// </summary>
	public void Stop()
	{
		if (handle.HasValue) {
			handle.Value.Stop();
			handle = null;
		}
	}
	
	/// <summary>
	/// 再生中のエフェクトのルートノードだけを停止
	/// ルートノードを削除したことで子ノード生成が停止され寿命で徐々に消える
	/// </summary>
	public void StopRoot()
	{
		if (handle.HasValue) {
			handle.Value.StopRoot();
			handle = null;
		}
	}
	
	/// <summary>
	/// 再生中のエフェクトのターゲット位置を設定
	/// </summary>
	/// <param name="targetLocation">ターゲット位置</param>
	public void SetTargetLocation(Vector3 targetLocation)
	{
		if (handle.HasValue) {
			handle.Value.SetTargetLocation(targetLocation);
		}
	}
	
	/// <summary>
	/// ポーズ設定
	/// <para>true:  停止中。Updateで更新しない</para>
	/// <para>false: 再生中。Updateで更新する</para>
	/// </summary>
	public bool paused
	{
		set {
			if (handle.HasValue) {
				var h = handle.Value;
				h.paused = value;
			}
		}
		get {
			return handle.HasValue && handle.Value.paused;
		}
	}
	
	/// <summary>
	/// 表示設定
	/// <para>true:  表示ON。Drawで描画する</para>
	/// <para>false: 表示OFF。Drawで描画しない</para>
	/// </summary>
	public bool shown
	{
		set {
			if (handle.HasValue) {
				var h = handle.Value;
				h.shown = value;
			}
		}
		get {
			return handle.HasValue && handle.Value.shown;
		}
	}
	
	/// <summary>
	/// 再生中のエフェクトが存在しているか
	/// <para>true:  存在している</para>
	/// <para>false: 再生終了で破棄。もしくはStopで停止された</para>
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
		if (!String.IsNullOrEmpty(effectName)) {
			EffekseerSystem.LoadEffect(effectName);
			if (playOnStart) {
				Play();
			}
		}
	}
	
	void OnDestroy()
	{
		Stop();
	}
	
	void Update()
	{
		if (handle.HasValue) {
			var h = handle.Value;
			if (h.exists) {
				h.SetLocation(transform.position);
				h.SetRotation(transform.rotation);
				h.SetScale(transform.localScale);
			} else if (loop) {
				Play();
			} else {
				Stop();
			}
		}
	}

	#endregion
}

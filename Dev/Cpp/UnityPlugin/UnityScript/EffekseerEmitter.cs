using UnityEngine;
using System;
using System.Collections;

public class EffekseerEmitter : MonoBehaviour {
	
	public string effectName;
	public bool playOnStart = false;
	public bool loop = false;
	private EffekseerHandle? handle;
	
	public void Play(string name)
	{
		effectName = name;
		Play();
	}
	
	public void Play()
	{
		handle = EffekseerSystem.PlayEffect(effectName, transform.position);
	}
	
	public void Stop()
	{
		if (handle.HasValue) {
			handle.Value.Stop();
		}
	}
	
	public bool isPlaying
	{
		get {
			return handle.HasValue && handle.Value.exists;
		}
	}
	
	void Start()
	{
		EffekseerSystem.LoadEffect(effectName);
		if (playOnStart) {
			Play();
		}
	}
	
	void OnDisable()
	{
		Stop();
	}
	
	void Update()
	{
		if (handle.HasValue) {
			if (handle.Value.exists) {
				handle.Value.SetLocation(transform.position);
				handle.Value.SetRotation(transform.rotation);
				handle.Value.SetScale(transform.localScale);
			} else if (loop) {
				Play();
			} else {
				handle.Value.Stop();
			}
		}
	}
}

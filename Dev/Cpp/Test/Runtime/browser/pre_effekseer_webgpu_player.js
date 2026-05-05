if (typeof Module === 'undefined') {
	var Module = {};
}

function effekseerGetBrowserPlayerParams() {
	var params = typeof URLSearchParams !== 'undefined' && typeof location !== 'undefined'
		? new URLSearchParams(location.search)
		: null;
	var get = function(name, fallback) {
		return params && params.get(name) ? params.get(name) : fallback;
	};

	return {
		effect: get('effect', '/TestData/Effects/10/SimpleLaser.efk'),
		width: get('width', '640'),
		height: get('height', '360'),
		loop: get('loop', '1'),
		loopFrame: get('loopFrame', '180'),
		x: get('x', '0'),
		y: get('y', '0'),
		z: get('z', '0')
	};
}

var effekseerBrowserPlayerParams = effekseerGetBrowserPlayerParams();

Module.arguments = Module.arguments || [
	'--effect=' + effekseerBrowserPlayerParams.effect,
	'--width=' + effekseerBrowserPlayerParams.width,
	'--height=' + effekseerBrowserPlayerParams.height,
	'--loop=' + effekseerBrowserPlayerParams.loop,
	'--loop-frame=' + effekseerBrowserPlayerParams.loopFrame,
	'--x=' + effekseerBrowserPlayerParams.x,
	'--y=' + effekseerBrowserPlayerParams.y,
	'--z=' + effekseerBrowserPlayerParams.z
];

Module.preRun = Module.preRun || [];
Module.preRun.push(function() {
	var canvas = Module.canvas || (typeof document !== 'undefined' ? document.getElementById('canvas') : null);
	if (canvas) {
		var width = Number(effekseerBrowserPlayerParams.width) || 640;
		var height = Number(effekseerBrowserPlayerParams.height) || 360;
		canvas.width = width;
		canvas.height = height;
		canvas.style.width = width + 'px';
		canvas.style.height = height + 'px';
	}

	var dependency = 'effekseer-webgpu-player-device';
	addRunDependency(dependency);

	(async function() {
		if (!navigator.gpu) {
			throw new Error('navigator.gpu is not available. Serve over localhost/https and use a WebGPU-capable browser.');
		}

		var adapter = await navigator.gpu.requestAdapter();
		if (!adapter) {
			throw new Error('Failed to request a WebGPU adapter.');
		}

		var optionalFeatures = ['float32-filterable', 'texture-formats-tier2'];
		var requiredFeatures = optionalFeatures.filter(function(feature) {
			return adapter.features && adapter.features.has(feature);
		});

		Module.preinitializedWebGPUDevice = await adapter.requestDevice({
			requiredFeatures: requiredFeatures
		});
		Module.preinitializedWebGPUDevice.addEventListener('uncapturederror', function(event) {
			var message = event.error && event.error.message ? event.error.message : String(event.error);
			console.error('EFFEKSEER_WEBGPU_ERROR', message);
		});
		removeRunDependency(dependency);
	})().catch(function(error) {
		console.error('EFFEKSEER_PLAYER_FAIL', error && error.message ? error.message : String(error));
		removeRunDependency(dependency);
	});
});

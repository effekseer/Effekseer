if (typeof Module === 'undefined') {
	var Module = {};
}

Module.arguments = Module.arguments || (function() {
	var args = ['--webgpu', '--filter=Runtime.WebGPUBrowser.*'];
	if (typeof URLSearchParams !== 'undefined' && typeof location !== 'undefined') {
		var params = new URLSearchParams(location.search);
		var filter = params.get('filter');
		if (filter) {
			args[1] = '--filter=' + filter;
		}
	}
	return args;
})();

Module.preRun = Module.preRun || [];
Module.preRun.push(function() {
	var dependency = 'effekseer-webgpu-device';
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
			Module.llgiLastWebGPUError = event.error && event.error.message ? event.error.message : String(event.error);
			console.error('EFFEKSEER_WEBGPU_ERROR', Module.llgiLastWebGPUError);
		});
		removeRunDependency(dependency);
	})().catch(function(error) {
		Module.llgiTestResult = {
			status: 'failed',
			message: error && error.message ? error.message : String(error)
		};
		console.error('EFFEKSEER_TEST_FAIL', Module.llgiTestResult.message);
		removeRunDependency(dependency);
	});
});

Module.onAbort = function(reason) {
	Module.llgiTestResult = {
		status: 'failed',
		message: reason ? String(reason) : 'aborted'
	};
	console.error('EFFEKSEER_TEST_FAIL', Module.llgiTestResult.message);
};

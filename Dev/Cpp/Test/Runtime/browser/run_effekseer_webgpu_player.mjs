import http from 'node:http';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import {spawn} from 'node:child_process';

const args = process.argv.slice(2);
const htmlPath = args[0];
const effectPath = args[1] && !args[1].startsWith('--') ? args[1] : '/TestData/Effects/10/SimpleLaser.efk';
const optionArgs = args.slice(args[1] && !args[1].startsWith('--') ? 2 : 1);

if (!htmlPath) {
	console.error('Usage: node run_effekseer_webgpu_player.mjs <EffekseerWebGPUBrowserPlayer.html> [/TestData/Effects/10/SimpleLaser.efk] [--width=640] [--height=360] [--loopFrame=180]');
	process.exit(2);
}

const resolvedHtmlPath = path.resolve(htmlPath);
if (!fs.existsSync(resolvedHtmlPath)) {
	console.error(`Not found: ${resolvedHtmlPath}`);
	process.exit(2);
}

const root = path.dirname(resolvedHtmlPath);
const htmlFile = path.basename(resolvedHtmlPath);
const executablePath = findBrowserExecutable();

function findBrowserExecutable() {
	const candidates = [
		process.env.CHROME_PATH,
		process.env.EDGE_PATH,
		'C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe',
		'C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe',
		'C:\\Program Files\\Microsoft\\Edge\\Application\\msedge.exe',
		'C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe',
	].filter(Boolean);

	for (const candidate of candidates) {
		if (fs.existsSync(candidate)) {
			return candidate;
		}
	}

	console.error('A WebGPU-capable Chrome or Edge executable is required.');
	console.error('Set CHROME_PATH or EDGE_PATH if it is installed in a custom location.');
	process.exit(2);
}

function contentType(filePath) {
	if (filePath.endsWith('.html')) return 'text/html';
	if (filePath.endsWith('.js')) return 'application/javascript';
	if (filePath.endsWith('.wasm')) return 'application/wasm';
	if (filePath.endsWith('.data')) return 'application/octet-stream';
	return 'application/octet-stream';
}

const server = http.createServer((request, response) => {
	const requestUrl = new URL(request.url, 'http://127.0.0.1');
	if (requestUrl.pathname === '/favicon.ico') {
		response.writeHead(204);
		response.end();
		return;
	}

	const relativePath = decodeURIComponent(requestUrl.pathname === '/' ? `/${htmlFile}` : requestUrl.pathname);
	const filePath = path.resolve(root, `.${relativePath}`);
	const relativeFromRoot = path.relative(root, filePath);

	if (relativeFromRoot.startsWith('..') || path.isAbsolute(relativeFromRoot) || !fs.existsSync(filePath) || !fs.statSync(filePath).isFile()) {
		response.writeHead(404);
		response.end('Not found');
		return;
	}

	response.writeHead(200, {
		'Content-Type': contentType(filePath),
		'Cache-Control': 'no-store, max-age=0',
		'Pragma': 'no-cache',
		'Expires': '0',
	});
	fs.createReadStream(filePath).pipe(response);
});

await new Promise((resolve) => server.listen(0, '127.0.0.1', resolve));
const {port} = server.address();

const params = new URLSearchParams();
params.set('effect', effectPath);
for (const option of optionArgs) {
	if (!option.startsWith('--')) {
		continue;
	}
	const separator = option.indexOf('=');
	if (separator > 2) {
		params.set(option.substring(2, separator), option.substring(separator + 1));
	}
}

const url = `http://127.0.0.1:${port}/${htmlFile}?${params.toString()}`;
const userDataDir = fs.mkdtempSync(path.join(os.tmpdir(), 'effekseer-webgpu-player-'));
console.log(url);

const browser = spawn(executablePath, [
	`--user-data-dir=${userDataDir}`,
	'--no-first-run',
	'--no-default-browser-check',
	'--enable-unsafe-webgpu',
	'--ignore-gpu-blocklist',
	'--use-angle=d3d11',
	url,
], {stdio: 'inherit'});

function shutdown() {
	server.close();
	fs.rmSync(userDataDir, {recursive: true, force: true, maxRetries: 10, retryDelay: 100});
}

process.on('SIGINT', () => {
	browser.kill();
	shutdown();
	process.exit(130);
});

browser.on('exit', (code) => {
	shutdown();
	process.exit(code || 0);
});

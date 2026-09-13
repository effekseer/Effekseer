# Shared backend tests

`TestPlatforms.h` enumerates the compiled renderers. Test scenarios are shared;
backend names select separate registrations and output files. GPU readback tests
use the LLGI subset (DX12, Vulkan, Metal, WebGPU).

Run a subset with `TestCpp --filter=<regular-expression>`. The following families
are opt-in (`FilterOnly`), so enabling another backend does not add long-running
GPU tests to the default test run.

| Family | Checks |
| --- | --- |
| `Runtime.Lifecycle.*` | Initialize, update without effects, play, update an effect, and terminate |
| `Runtime.RenderingSmoke.*` | Model color, material UV, or a representative set of screenshot cases |
| `Runtime.TexturelessDistortionVisibility.*` | LLGI readback confirms distortion changes a non-periodic background |
| `Runtime.TextureFormats.*` | Load and render DDS/TGA effects and save screenshots |
| `Runtime.RenderingTransformScreenshot.*` | Coordinate transforms, flips, lighting, and coordinate-system variants |
| `ToolRuntime.*` | Shared registration of the existing supported environment/platform tests |

For example:

```text
TestCpp --filter=Runtime.Lifecycle.*.DX12
TestCpp --filter=Runtime.TexturelessDistortionVisibility.Vulkan
TestCpp --filter=Runtime.RenderingTransformScreenshot.Metal
```

Screenshot tests produce review artifacts; they do not assert pixel equality
between different backends. Transform tests write separate
`screenshot_comparison_sources/manifest_<backend>.csv` files. Cases requiring GPU
particles report a skip when the initialized manager has no GPU particle system.
Backend initialization failures remain failures, rather than silently passing.

LLGI platform tests count error-level logs and fail after the test function
returns, including platform teardown. Warnings remain informational. The test
logger is installed for device creation and reinstalled after renderer creation
(renderer initialization installs its own logger). Errors inside renderer
creation itself are not covered by this log counter.
Run `python Script/compose_screenshot_comparisons.py <output-directory>` to
compose all backend manifests; an old `manifest.csv` is used only when no
backend manifests exist.

The WebGPU compiled-material/DLL test remains in `BasicRenderingWebGPU.cpp`.
Its compiler integration is backend-specific. The material rendering scenarios
are shared in `RenderingSmoke.cpp` and `BasicRendering.cpp`.

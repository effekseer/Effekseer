How to test
==========

Run the tests
-------------

After building, you can run the included unit test suite from the repository root with:

```
python3 -m unittest Script/test_screenshot.py -v
```

This command checks generated screenshots against the reference data and reports
the results.

Generate screenshot comparison images
-------------------------------------

Screenshot rendering tests write PNG files beside `TestCpp.exe`. The following
example builds a Windows test binary with Vulkan enabled, runs the basic
rendering screenshot tests for DirectX 11, Vulkan, DirectX 12, and OpenGL, then
generates CSV reports and comparison sheets.

Install the Python dependency once:

```
python -m pip install pillow
```

Build the test executable:

```
cmake -S . -B build_vulkan -DBUILD_VIEWER=ON -DBUILD_TEST=ON -DBUILD_EXAMPLES=ON -DBUILD_VULKAN=ON
cmake --build ./build_vulkan --config Release --target TestCpp
```

Run the screenshot tests from the test output directory:

```
cd build_vulkan/Dev/Cpp/Test/Release
TestCpp.exe --filter=Runtime\.BasicRendering\.DX11
TestCpp.exe --filter=Runtime\.BasicRendering\.Vulkan
TestCpp.exe --filter=Runtime\.BasicRendering\.DX12
TestCpp.exe --filter=Runtime\.BasicRendering\.GL
cd ../../../../../..
```

To compare one case across backends, use the case-first aliases:

```
cd build_vulkan/Dev/Cpp/Test/Release
TestCpp.exe --filter=Runtime\.BasicRendering\.Case\.SimpleLaser\.(DX11|Vulkan|DX12|GL)
cd ../../../../../..
```

The C++ basic rendering tests keep writing the compatibility files in the test
output directory, such as `SimpleLaser_DX11.png`. They also copy the same
screenshots into backend-specific directories:

- `screenshots_by_backend/DX11/*.png`
- `screenshots_by_backend/Vulkan/*.png`
- `screenshots_by_backend/DX12/*.png`
- `screenshots_by_backend/GL/*.png`

Each backend directory contains a `manifest.csv` file with the case name,
screenshot name, backend, original flat path, and backend-specific path.

Generate the comparison artifacts:

```
python Script/generate_screenshot_comparison.py build_vulkan/Dev/Cpp/Test/Release --baseline-dir TestData/Tests/Windows
```

The script groups files by backend suffixes such as `_DX11.png`,
`_Vulkan.png`, `_DX12.png`, and `_GL.png`. It writes:

- `comparison_visuals/diff_report.csv` for all backend-to-backend pixel diffs.
- `comparison_visuals/backend_pair_summary.csv` for per-backend-pair summary metrics.
- `comparison_visuals/baseline_report.csv` when `--baseline-dir` is specified.
- `comparison_visuals/backend_pair_summary.png` for a backend-pair matrix.
- `comparison_visuals/top_backend_differences.png` for the largest differences.
- `comparison_visuals/all_backend_comparisons.png` for all cases.
- `comparison_visuals/pairs/*.png` for one sheet per backend pair, sorted by difference.
- `comparison_visuals/cases/*.png` for one sheet per test case.

For backend-focused review, start with `backend_pair_summary.png`, then open the
matching file under `pairs/`, such as `DX11_vs_Vulkan.png` or
`Vulkan_vs_DX12.png`, to inspect the cases with the largest differences first.

DirectX 12 screenshot tests require a local machine or self-hosted runner with a
usable DirectX 12 hardware adapter. GitHub-hosted Windows runners may not be
able to run them.

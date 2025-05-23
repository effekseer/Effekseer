
# Profiling

We use easy profiler 2.1.

https://github.com/yse/easy_profiler

Download a GUI tool from Releases of easy profiler

Compile effekseer with -D BUILD_WITH_EASY_PROFILER in Cmake

Run Test

--filter=Performance.CheckPerformance

effekseer_profile.prof is exported

Load it with easy profiler tool

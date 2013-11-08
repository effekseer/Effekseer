set PATH=C:\Windows\Microsoft.NET\Framework\v4.0.30319;%PATH%

msbuild Effekseer.sln /p:Configuration=Release /t:Clean,Build
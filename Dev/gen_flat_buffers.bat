
for %%q in (FlatBuffers\*.fbs) do (
    echo %%q
    Cpp\3rdParty\flatbuffers\bin\flatc.exe --cpp --cpp-std c++11 --scoped-enums -o Cpp\Effekseer\Effekseer\FB\ %%q
    Cpp\3rdParty\flatbuffers\bin\flatc.exe --csharp --gen-onefile --gen-object-api -o Editor\EffekseerCore\FB\ %%q
)

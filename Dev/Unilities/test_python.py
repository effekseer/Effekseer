import sys
import os
from python import EfkEfcProperty

f = open('../../TestData/Effects/15/BasicRenderSettings_Blend.efkefc', 'rb')
buf = f.read()

result = EfkEfcProperty.loadEfkEfcInformation(buf)

print(result.dependentFiles)
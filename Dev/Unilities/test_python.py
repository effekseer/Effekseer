import sys
import os
from typing import List
from python import EfkEfcProperty


def test_property(path: str, files: List[str]):
    f = open(path, 'rb')
    buf = f.read()
    result = EfkEfcProperty.loadEfkEfcInformation(buf)
    included_files = [df.path for df in result.dependentFiles]
    included_files.sort()
    files.sort()

    if files != included_files:
        raise "Not equal"

test_property('../../TestData/Effects/15/BasicRenderSettings_Blend.efkefc',
              ['../Textures/Particle03.png'])

test_property('../../TestData/Effects/15/Material_WorldPositionOffset.efkefc',
              ['../Materials/WorldPositionOffset.efkmat', '../Models/Sphare_Resized.efkmodel', '../Textures/Normal1.png', '../Textures/TestPattern.png'])

test_property('../../TestData/Effects/16/Curve01.efkefc',
              ['../Curves/Curve1.efkcurve'])

test_property('../../TestData/Effects/16/SoftParticle01.efkefc',
              ['../Materials/DepthFade.efkmat', '../Models/Sphare_Resized.efkmodel'])

test_property('../../TestData/Effects/17_Dependency/BasicRenderSettings_Blend.efkefc',
              ['../Textures/Particle03.png'])

test_property('../../TestData/Effects/17_Dependency/Material_WorldPositionOffset.efkefc',
              ['../Materials/WorldPositionOffset.efkmat', '../Models/Sphare_Resized.efkmodel', '../Textures/Normal1.png', '../Textures/TestPattern.png'])

test_property('../../TestData/Effects/17_Dependency/Curve01.efkefc',
              ['../Curves/Curve1.efkcurve'])

test_property('../../TestData/Effects/17_Dependency/SoftParticle01.efkefc',
              ['../Materials/DepthFade.efkmat', '../Models/Sphare_Resized.efkmodel'])

from enum import Enum
import struct


class BinaryReader:
    def __init__(self, buffer : "bytes"):
        self.buffer = buffer
        self.offset = 0

    def returnBack(self, size):
        self.offset -= size

    def getInt32(self):
        ret = struct.unpack_from('i', self.buffer, self.offset)[0]
        self.offset += 4
        return ret

    def getChunk4(self):
        ret = bytes([self.buffer[self.offset + 0], self.buffer[self.offset + 1],
               self.buffer[self.offset + 2], self.buffer[self.offset + 3]]).decode("utf-8")
        self.offset += 4
        return ret

    def getStringAsUTF16(self):
        size = self.getInt32()
        ret = self.buffer[self.offset:self.offset + size * 2].decode("utf-16")
        ret = ret.strip('\0')
        self.offset += (size * 2)
        return ret

    def isEoF(self):
        return self.offset >= len(self.buffer)

    def skip(self, num):
        self.offset += num


class FileType(Enum):
    Effect = 0
    Texture = 1
    Sound = 2
    Model = 3
    Material = 4
    Curve = 5
    Other = 255


class DependentFile:
    def __init__(self):
        self.fileType = FileType.Other
        self.flag = 0
        self.path = ""


class EfkEfcProperty:
    def __init__(self):
        self.version = 0
        self.dependentFiles = []


def loadEfkEfcInformation(buffer):
    reader = BinaryReader(buffer)
    header = reader.getChunk4()
    version = reader.getInt32()

    while (not reader.isEoF()):
        chunk = reader.getChunk4()
        chunkSize = reader.getInt32()

        if chunk == "INFO":
            info = EfkEfcProperty()

            version = reader.getInt32()
            if (version < 1500):
                reader.returnBack(4)
                version = 0

            info.version = version

            if version < 1700:
                def readStringArray(reader):
                    ret = []

                    count = reader.getInt32()
                    for i in range(count):
                        str = reader.getStringAsUTF16()
                        ret.append(str)
                    return ret

                colorImages = readStringArray(reader)
                normalImages = readStringArray(reader)
                distortionImages = readStringArray(reader)
                models = readStringArray(reader)
                sounds = readStringArray(reader)
                materials = []
                curves = []

                if version >= 1500:
                    materials = readStringArray(reader)

                if version >= 1610:
                    curves = readStringArray(reader)

                def containsRGB(path):
                    return path in colorImages

                def containLinear(path):
                    return path in normalImages or path in distortionImages

                images = []
                for i in range(len(colorImages)):
                    if not colorImages[i] in images:
                        images.append(colorImages[i])

                for i in range(len(normalImages)):
                    if not normalImages[i] in images:
                        images.append(normalImages[i])

                for i in range(len(distortionImages)):
                    if not distortionImages[i] in images:
                        images.append(distortionImages[i])

                for i in range(len(images)):
                    df = DependentFile()
                    df.fileType = FileType.Texture
                    if containsRGB(images[i]):
                        df.flag += 1

                    if containLinear(images[i]):
                        df.flag += 2

                    df.path = images[i]
                    info.dependentFiles.append(df)

                for i in range(len(models)):
                    df = DependentFile()
                    df.fileType = FileType.Model
                    df.path = models[i]
                    info.dependentFiles.append(df)

                for i in range(len(sounds)):
                    df = DependentFile()
                    df.fileType = FileType.Sound
                    df.path = sounds[i]
                    info.dependentFiles.append(df)

                for i in range(len(materials)):
                    df = DependentFile()
                    df.fileType = FileType.Material
                    df.path = materials[i]
                    info.dependentFiles.append(df)

                for i in range(len(curves)):
                    df = DependentFile()
                    df.fileType = FileType.Curve
                    df.path = curves[i]
                    info.dependentFiles.append(df)
            else:
                count = reader.getInt32()
                for i in range(count):
                    df = DependentFile()
                    df.fileType = reader.getInt32()
                    df.flag = reader.getInt32()
                    df.path = reader.getStringAsUTF16()
                    info.dependentFiles.append(df)

            return info
        else:
            reader.skip(chunkSize)

    return None

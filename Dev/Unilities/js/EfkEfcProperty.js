




class BinaryReader {
    constructor(buffer) {
        this.buffer = buffer;
        this.offset = 0;
    }

    returnBack(size) {
        this.offset -= size;
    }

    getInt32() {
        var ret = new DataView(this.buffer, this.offset, 4).getInt32(0, true);
        this.offset += 4;
        return ret;
    }

    getChunk4() {
        var sliced = new Uint8Array(this.buffer.slice(this.offset, this.offset + 4));
        var ret = String.fromCharCode(sliced[0], sliced[1], sliced[2], sliced[3]);
        this.offset += 4;
        return ret;
    }

    getStringAsUTF16() {
        var size = new DataView(this.buffer, this.offset, 4).getInt32(0, true);
        this.offset += 4;
        var sliced = new Uint8Array(this.buffer.slice(this.offset, this.offset + (size * 2 - 2)));

        var str = "";
        for (var i = 0; i < sliced.byteLength / 2; i++) {
            str += String.fromCharCode(sliced[i * 2] + sliced[i * 2 + 1] * 256);
        }

        this.offset += (size * 2);
        return str;
    }

    isEoF() {
        return this.offset >= this.buffer.byteLength;
    }

    skip(num) {
        this.offset += num;
    }
}

const FileType = { Effect: 0, Texture: 1, Sound: 2, Model: 3, Material: 4, Curve: 5, Other: 255 };

class DependentFile {
    constructor() {
        this.fileType = 0;
        this.flag = 0;
        this.path = "";
    }
}

class EfkEfcProperty {
    constructor() {
        this.version = 0
        this.dependentFiles = []
    }
}

function loadEfkEfcInformation(buffer) {

    var reader = new BinaryReader(buffer);
    var header = reader.getChunk4();
    var version = reader.getInt32();

    while (!reader.isEoF()) {
        var chunk = reader.getChunk4();
        var chunkSize = reader.getInt32();

        if (chunk == "INFO") {
            var info = new EfkEfcProperty();

            version = reader.getInt32();
            if (version >= 1500) {
            }
            else {
                reader.returnBack(4);
                version = 0;
            }

            info.version = version;

            if (version < 1700) {
                function readStringArray(reader) {
                    var ret = []

                    var count = reader.getInt32();
                    for (var i = 0; i < count; i++) {
                        var str = reader.getStringAsUTF16();
                        ret.push(str)
                    }

                    return ret;
                }

                var colorImages = readStringArray(reader);
                var normalImages = readStringArray(reader);
                var distortionImages = readStringArray(reader);
                var models = readStringArray(reader);
                var sounds = readStringArray(reader);
                var materials = [];
                var curves = [];

                if (version >= 1500) {
                    materials = readStringArray(reader);
                }

                if (version >= 1610) {
                    curves = readStringArray(reader);
                }

                function containsRGB(path) {
                    return colorImages.includes(path);
                }

                function containLinear(path) {
                    return normalImages.includes(path) || distortionImages.includes(path);
                }

                var images = [];
                for (var i = 0; i < colorImages.length; i++) {
                    if (!images.includes(colorImages[i])) {
                        images.push(colorImages[i]);
                    }
                }

                for (var i = 0; i < normalImages.length; i++) {
                    if (!images.includes(normalImages[i])) {
                        images.push(normalImages[i]);
                    }
                }

                for (var i = 0; i < distortionImages.length; i++) {
                    if (!images.includes(distortionImages[i])) {
                        images.push(distortionImages[i]);
                    }
                }

                for (var i = 0; i < images.length; i++) {
                    var df = new DependentFile();
                    df.fileType = FileType.Texture;
                    if (containsRGB(images[i])) {
                        df.flag += 1;
                    }
                    if (containLinear(images[i])) {
                        df.flag += 2;
                    }
                    df.path = images[i];
                    info.dependentFiles.push(df);
                }

                for (var i = 0; i < models.length; i++) {
                    var df = new DependentFile();
                    df.fileType = FileType.Model;
                    df.path = models[i];
                    info.dependentFiles.push(df);
                }

                for (var i = 0; i < sounds.length; i++) {
                    var df = new DependentFile();
                    df.fileType = FileType.Sound;
                    df.path = sounds[i];
                    info.dependentFiles.push(df);
                }

                for (var i = 0; i < materials.length; i++) {
                    var df = new DependentFile();
                    df.fileType = FileType.Material;
                    df.path = materials[i];
                    info.dependentFiles.push(df);
                }

                for (var i = 0; i < curves.length; i++) {
                    var df = new DependentFile();
                    df.fileType = FileType.Curve;
                    df.path = curves[i];
                    info.dependentFiles.push(df);
                }
            }
            else {
                var count = reader.getInt32();
                for (var i = 0; i < count; i++) {
                    var df = new DependentFile();
                    df.fileType = reader.getInt32();
                    df.flag = reader.getInt32();
                    df.path = reader.getStringAsUTF16();
                    info.dependentFiles.push(df);
                }
            }

            return info;
        }
        else {
            reader.skip(chunkSize);
        }
    }

    return null;
}
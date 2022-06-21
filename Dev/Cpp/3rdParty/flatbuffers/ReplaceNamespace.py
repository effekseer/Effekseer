import glob

def main():
    file_list = glob.glob("include/flatbuffers/*.h", recursive=False)

    for filename in file_list:
        print(filename)
        with open(filename) as infile:
            lines = infile.readlines()

            begin_index = index_any(lines, [
                "namespace flatbuffers {\n",
                "namespace flexbuffers {\n",
                "namespace reflection {\n",
            ])
            assert begin_index >= 0
            lines.insert(begin_index, "namespace Data {\n")
            lines.insert(begin_index, "namespace Effekseer {\n")

            end_index = index_any(lines, [
                "}  // namespace flatbuffers\n",
                "}  // namespace flexbuffers\n",
                "}  // namespace reflection\n",
            ])
            assert end_index >= 0
            lines.insert(end_index + 1, "}  // namespace Data\n")
            lines.insert(end_index + 2, "}  // namespace Effekseer\n")

            with open(filename, mode="w", newline="\n") as outfile:
                outfile.writelines(lines)

def index_any(lines: str, key_texts: list) -> int:
    for key in key_texts:
        if key in lines:
            return lines.index(key)
    return -1

if __name__ == "__main__":
    main()

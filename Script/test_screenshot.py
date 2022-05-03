import sys
import unittest
import os
import time
import shutil
import filecmp
import base64
import glob
import json


class ScreenShotTest(unittest.TestCase):
    def test_screenshots(self):
        generated_file_paths = glob.glob('build/Dev/Cpp/Test/Release/*.png')

        success = True

        for path in generated_file_paths:
            name = os.path.basename(path)
            test_data_path = 'TestData/Tests/Windows/' + name

            if os.path.exists(test_data_path):
                is_same = filecmp.cmp(test_data_path, path)
                if not is_same:
                    print(f'{name} is not equal.')
                    success = False
            else:
                print(f'{test_data_path} is not found.')
                success = False

        self.assertTrue(success)

if __name__ == '__main__':
    unittest.main()

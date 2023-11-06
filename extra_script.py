import os
import glob
import shutil
Import('env')

FRAMEWORK_DIR = env.PioPlatform().get_package_dir("framework-arduinoteensy")
print(FRAMEWORK_DIR)
teensy_dir = os.path.join(FRAMEWORK_DIR, 'cores', 'teensy4')
print(teensy_dir)

# copy everything from overrides/teensy4/* to framework dir
for file in glob.glob(os.path.join('overrides', 'teensy4', '*')):
    shutil.copy(file, teensy_dir)

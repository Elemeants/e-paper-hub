import subprocess

try:
    revision = (
        subprocess.check_output(["git", "describe", "--tags"])
        .strip()
        .decode("utf-8")
    )
    dirty = subprocess.check_output(['git', 'diff', '--stat']).strip().__len__() > 0

    version = f'"{revision}{"-d" if dirty else ""}"'
except:
    version = '"UNKNOWN"'

print("'-DFIRMWARE_VERSION=%s'" % version)

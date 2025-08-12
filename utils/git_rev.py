import subprocess

def get_git_tag() -> str:
    return (
        subprocess.check_output(["git", "describe", "--tags"])
        .strip()
        .decode("utf-8")
    )

def get_git_is_dirty() -> bool:
    return subprocess.check_output(['git', 'diff', '--stat']).strip().__len__() > 0

def get_git_revision_short_hash() -> str:
    return subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).decode('ascii').strip()

try:
    tag = get_git_tag()
    sha = get_git_revision_short_hash()
    dirty = get_git_is_dirty()

    version = f'"{tag}-{sha}{"-d" if dirty else ""}"'
except:
    version = '"UNKNOWN"'

print("'-DFIRMWARE_VERSION=%s'" % version)

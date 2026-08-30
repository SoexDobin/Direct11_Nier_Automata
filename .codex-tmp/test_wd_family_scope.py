import importlib
import sys
from pathlib import Path


ADDON = "nier2blender2nier"
EXTENSION_ROOT = Path(r"C:\Users\a9018\AppData\Roaming\Blender Foundation\Blender\4.5\extensions\user_default")
CPK_ROOT = Path(r"D:\NierRes\cpk_unpacked")

sys.path.insert(0, str(EXTENSION_ROOT))
dat_importer = importlib.import_module(ADDON + ".dat_dtt.importer.datImportOperator")
util = importlib.import_module(ADDON + ".utils.util")


def find_complete_dtt(data_name, group_name):
    for dtt_path in sorted((CPK_ROOT / data_name / group_name).glob("*.dtt")):
        if dat_importer._find_companion_archive(str(dtt_path), ".dat").is_file():
            return dtt_path
    raise AssertionError(f"No complete DAT/DTT pair: {data_name}/{group_name}")


cases = (
    ("data004", "wd1", {"ga0001", "ga0002"}),
    ("data004", "wd2", {"ga0001", "ga0002"}),
    ("data014", "wd1", {"ga0000"}),
    ("data014", "wd2", {"ga0000"}),
    ("data014", "wd3", {"ga0000"}),
    ("data014", "wd4", {"ga0000"}),
    ("data014", "wd5", {"ga0000"}),
    ("data100", "wd5", set()),
)

for data_name, group_name, expected_stems in cases:
    dtt_path = find_complete_dtt(data_name, group_name)
    dat_path = dat_importer._find_companion_archive(str(dtt_path), ".dat")
    pairs = dat_importer._world_shared_archive_pairs(str(dtt_path))
    pair_stems = {dtt.stem.lower() for dat, dtt in pairs}
    assert pair_stems == expected_stems, (dtt_path, pair_stems, expected_stems)

    texture_dir = dtt_path.parent / "nier2blender_extracted" / dtt_path.name / "textures"
    util._getWorldSharedTextureDirs.cache_clear()
    shared_dirs = util._getWorldSharedTextureDirs(str(texture_dir))
    shared_dir_stems = {
        Path(path).parent.name.removesuffix(".dtt").lower()
        for path in shared_dirs
    }
    assert shared_dir_stems == expected_stems, (texture_dir, shared_dir_stems, expected_stems)

    print(
        "CASE_OK",
        f"{data_name}/{group_name}/{dtt_path.name}",
        f"DAT={dat_path.parent.parent.name}/{dat_path.parent.name}/{dat_path.name}",
        f"WDA={sorted(pair_stems)}",
    )

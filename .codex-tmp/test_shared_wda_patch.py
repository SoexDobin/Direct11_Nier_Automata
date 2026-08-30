import importlib
from pathlib import Path


ADDON = "bl_ext.user_default.nier2blender2nier"
TARGET = Path(r"D:\NierRes\cpk_unpacked\data004\wd1\g11021.dtt")

dat_importer = importlib.import_module(ADDON + ".dat_dtt.importer.datImportOperator")
util = importlib.import_module(ADDON + ".utils.util")
wta_module = importlib.import_module(ADDON + ".wta_wtp.importer.wta")
wmb_module = importlib.import_module(ADDON + ".wmb.importer.wmb")

companion = dat_importer._find_companion_archive(str(TARGET), ".dat")
expected_companion = Path(r"D:\NierRes\cpk_unpacked\data003\wd1\g11021.dat")
assert companion == expected_companion, (companion, expected_companion)

pairs = dat_importer._world_shared_archive_pairs(str(TARGET))
pair_names = [(dat_path.name, dtt_path.name) for dat_path, dtt_path in pairs]
assert {("ga0001.dat", "ga0001.dtt"), ("ga0002.dat", "ga0002.dtt")} <= set(pair_names), pair_names
assert all(dat_name[:-4] == dtt_name[:-4] for dat_name, dtt_name in pair_names), pair_names

result = dat_importer.importDtt(True, str(TARGET))
assert result == {"FINISHED"}, result

local_extract = TARGET.parent / "nier2blender_extracted"
local_wmb = local_extract / "g11021.dtt" / "g11021.wmb"
local_wta = local_extract / "g11021.dat" / "g11021.wta"
local_wtp = local_extract / "g11021.dtt" / "g11021.wtp"
local_texture_dir = local_extract / "g11021.dtt" / "textures"
for required_path in (local_wmb, local_wta, local_wtp, local_texture_dir):
    assert required_path.exists(), required_path


def read_identifiers(wta_path):
    with open(wta_path, "rb") as wta_file:
        return set(wta_module.WTA(wta_file).wtaTextureIdentifier)


local_ids = read_identifiers(local_wta)
shared_ids = set()
for dat_path, dtt_path in pairs:
    shared_wta = dat_path.parent / "nier2blender_extracted" / (dat_path.stem + ".dat") / (dat_path.stem + ".wta")
    shared_texture_dir = dtt_path.parent / "nier2blender_extracted" / (dtt_path.stem + ".dtt") / "textures"
    identifiers = read_identifiers(shared_wta)
    assert identifiers
    assert all((shared_texture_dir / (identifier + ".dds")).is_file() for identifier in identifiers)
    shared_ids.update(identifiers)

wmb = wmb_module.WMB3(str(local_wmb), True)
referenced_ids = {
    identifier
    for material in wmb.materialArray
    for identifier in material.textureArray.values()
}
shared_only_references = sorted((referenced_ids & shared_ids) - local_ids)
shared_available_not_local = sorted(shared_ids - local_ids)
assert shared_available_not_local, "wda has no textures outside the g11021 local WTA"

util._getWorldSharedTextureDirs.cache_clear()
resolved = {
    identifier: util.getTexture(str(local_texture_dir), identifier)
    for identifier in shared_available_not_local
}
missing = [identifier for identifier, path in resolved.items() if path is None]
assert not missing, missing
assert all("\\wda\\" in str(path).lower() for path in resolved.values()), resolved

print("SHARED_WDA_PATCH_OK")
print("COMPANION", companion)
print("SHARED_PAIRS", pair_names)
print("LOCAL_TEXTURE_IDS", len(local_ids))
print("SHARED_TEXTURE_IDS", len(shared_ids))
print("G11021_REFERENCED_IDS", len(referenced_ids))
print("G11021_SHARED_ONLY_REFERENCES", len(shared_only_references), shared_only_references[:10])
print("WDA_TEXTURES_NOT_IN_G11021_LOCAL_WTA", len(shared_available_not_local), shared_available_not_local[:10])
print("SHARED_TEXTURE_DIRS", util._getWorldSharedTextureDirs(str(local_texture_dir)))

import json
import sys
from collections import Counter, defaultdict
from pathlib import Path


TOOLS_ROOT = Path(r"D:\NierRes\tools")
ADDON = "NieR2Blender2NieR_DragDrop"
DAT_DIR = Path(r"D:\NierRes\cpk_unpacked\data003\wd1\nier2blender_extracted\g11021.dat")
OUTPUT = Path(r"C:\Users\a9018\Desktop\Direct11_Nier_Automata\.codex-tmp\g11021_col_lay_analysis.json")

sys.path.insert(0, str(TOOLS_ROOT))

from NieR2Blender2NieR_DragDrop.col.importer.col import Col
from NieR2Blender2NieR_DragDrop.lay.importer.lay import Lay


with (DAT_DIR / "g11021.col").open("rb") as file:
    col = Col(file)

col_names = defaultdict(lambda: {
    "mesh_records": 0,
    "batches": 0,
    "vertices": 0,
    "triangles": 0,
    "collision_types": Counter(),
    "modifiers": Counter(),
    "surface_types": Counter(),
})

for mesh in col.meshes:
    name = col.nameGroups.names[mesh.nameIndex]
    record = col_names[name]
    record["mesh_records"] += 1
    record["batches"] += len(mesh.batches)
    record["vertices"] += sum(batch.vertexCount for batch in mesh.batches)
    record["triangles"] += sum(len(batch.indices) for batch in mesh.batches)
    record["collision_types"][str(mesh.collisionType)] += 1
    record["modifiers"][str(mesh.modifier)] += 1
    record["surface_types"][str(mesh.surfaceType)] += 1

col_name_records = []
for name, record in sorted(col_names.items()):
    col_name_records.append({
        "name": name,
        "mesh_records": record["mesh_records"],
        "batches": record["batches"],
        "vertices": record["vertices"],
        "triangles": record["triangles"],
        "collision_types": dict(record["collision_types"]),
        "modifiers": dict(record["modifiers"]),
        "surface_types": dict(record["surface_types"]),
    })

with (DAT_DIR / "Layout.lay").open("rb") as file:
    lay = Lay(file)

lay_assets = []
for asset in lay.assets:
    lay_assets.append({
        "name": asset.name,
        "position": asset.position,
        "rotation": asset.rotation,
        "scale": asset.scale,
        "unknown_index": asset.unknownIndex,
        "instance_count": asset.instanceCount,
        "instances": [
            {
                "position": instance.position,
                "rotation": instance.rotation,
                "scale": instance.scale,
            }
            for instance in asset.instances
        ],
    })

analysis = {
    "col": {
        "name_count": col.header.nameCount,
        "mesh_count": col.header.meshCount,
        "tree_node_count": col.header.colTreeNodesCount,
        "name_records": col_name_records,
    },
    "lay": {
        "model_list_count": lay.header.modelListCount,
        "asset_count": lay.header.assetsCount,
        "instance_count": lay.header.instancesCount,
        "model_ids": [
            {"dir_hex": entry.dir.hex(), "id_hex": entry.id.hex()}
            for entry in lay.modelList
        ],
        "assets": lay_assets,
    },
}

OUTPUT.write_text(json.dumps(analysis, ensure_ascii=False, indent=2), encoding="utf-8")

print(f"OUTPUT={OUTPUT}")
print(
    "COL",
    f"NAMES={col.header.nameCount}",
    f"MESHES={col.header.meshCount}",
    f"TREE_NODES={col.header.colTreeNodesCount}",
    f"BATCHES={sum(record['batches'] for record in col_name_records)}",
)
for record in col_name_records:
    print("COL_NAME", json.dumps(record, ensure_ascii=False))
print(
    "LAY",
    f"MODELS={lay.header.modelListCount}",
    f"ASSETS={lay.header.assetsCount}",
    f"INSTANCES={lay.header.instancesCount}",
)
for asset in lay_assets:
    print("LAY_ASSET", asset["name"], f"INSTANCES={asset['instance_count']}")

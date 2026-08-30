import csv
import json
import re
from collections import defaultdict
from pathlib import Path


ROOT = Path(r"C:\Users\a9018\Desktop\Direct11_Nier_Automata\.codex-tmp")
FBX_ANALYSIS = ROOT / "city_of_ruin_entry_analysis.json"
SOURCE_ANALYSIS = ROOT / "g11021_col_lay_analysis.json"
MATERIALS_JSON = Path(r"C:\Users\a9018\Desktop\Direct11_Nier_Automata\Client\bin\resources\Models\World\CityOfRuinEntry\materials.json")
CSV_OUTPUT = ROOT / "CityOfRuinEntry_MeshInventory.csv"
MD_OUTPUT = ROOT / "CityOfRuinEntry_MeshNameAnalysis.md"


def base_name(name):
    return re.sub(r"\.\d{3}$", "", name)


def classify(name):
    base = base_name(name)
    if base == "HexGround":
        return "COL", "Static collision batch", "렌더 제외; COL metadata를 보존해 충돌·Nav 입력으로 사용"
    if "-LOD1" in base:
        return "WMB", "Internal LOD1 visual", "대응 LOD0 submesh와 묶어 거리 전환"
    if base == "g11021_DistantView":
        return "WMB", "Distant backdrop visual", "근거리 구조물과 분리해 별도 가시거리·컬링 적용"
    if base == "ground":
        return "WMB", "Ground visual", "시각 메시로 유지; 충돌·Nav는 COL을 우선"
    if base == "nolowmap_debris":
        return "WMB", "Debris visual", "디테일 메시 그룹으로 분리하고 짧은 가시거리 후보"
    if base == "MtRobot9":
        return "WMB", "Large prop visual", "독립 공간 chunk 또는 원거리 그룹 후보"
    if base.startswith("g11021_build"):
        return "WMB", "Building visual", "공간 chunk·material 단위 정적 배치"
    if base == "g11021_highway":
        return "WMB", "Highway visual", "공간 chunk·material 단위 정적 배치"
    return "WMB", "Unclassified visual", "수동 검토"


fbx = json.loads(FBX_ANALYSIS.read_text(encoding="utf-8"))
source = json.loads(SOURCE_ANALYSIS.read_text(encoding="utf-8"))
material_sidecar = json.loads(MATERIALS_JSON.read_text(encoding="utf-8"))
material_hashes = {
    value.upper()
    for material in material_sidecar.values()
    for value in material["Textures"].values()
}
material_bindings = sum(len(material["Textures"]) for material in material_sidecar.values())

rows = []
groups = defaultdict(list)
for index, obj in enumerate(fbx["objects"]):
    source_class, semantic, action = classify(obj["name"])
    mesh = obj["mesh"]
    row = {
        "index": index,
        "mesh_name": obj["name"],
        "base_name": base_name(obj["name"]),
        "source_class": source_class,
        "semantic_group": semantic,
        "vertices": mesh["vertices"],
        "triangles": mesh["triangles"],
        "material": ";".join(value or "" for value in mesh["materials"]),
        "uv_layers": ";".join(mesh["uv_layers"]),
        "color_attributes": ";".join(mesh["color_attributes"]),
        "min_x": mesh["local_bounds"]["min"][0],
        "min_y": mesh["local_bounds"]["min"][1],
        "min_z": mesh["local_bounds"]["min"][2],
        "max_x": mesh["local_bounds"]["max"][0],
        "max_y": mesh["local_bounds"]["max"][1],
        "max_z": mesh["local_bounds"]["max"][2],
        "geometry_hash": mesh["translation_normalized_geometry_hash"],
        "engine_action": action,
    }
    rows.append(row)
    groups[row["base_name"]].append(row)

with CSV_OUTPUT.open("w", encoding="utf-8", newline="") as file:
    writer = csv.DictWriter(file, fieldnames=list(rows[0]))
    writer.writeheader()
    writer.writerows(rows)

lines = [
    "---",
    "type: source",
    "date: 2026-08-30",
    "status: active",
    "source: Client/bin/resources/Models/World/CityOfRuinEntry/CityOfRuinEntry.fbx; g11021.col; Layout.lay",
    "owner: user",
    r"repo_path: C:\Users\a9018\Desktop\Direct11_Nier_Automata",
    "aliases:",
    "  - CityOfRuinEntry FBX Mesh Inventory",
    "  - g11021 Mesh Name Analysis",
    "---",
    "",
    "# CityOfRuinEntry FBX 전체 메시 이름 분석",
    "",
    "## Summary",
    "",
    f"- 입력 FBX: `{fbx['source']}` ({fbx['source_bytes']:,} bytes)",
    f"- 전체 메시: {fbx['mesh_count']:,}개, 정점 {fbx['mesh_totals']['vertices']:,}개, 삼각형 {fbx['mesh_totals']['triangles']:,}개",
    f"- 공간 범위: min `{fbx['world_bounds']['min']}`, max `{fbx['world_bounds']['max']}`",
    "- FBX에는 Collection, parent hierarchy, Empty, collection instance, 공유 Mesh datablock이 남아 있지 않다. 모든 메시가 루트에 평탄화되고 배치 transform이 vertex 좌표에 구워져 있다.",
    f"- `HexGround` 401개는 원본 `g11021.col`의 name group 1개, mesh/batch 401개와 정확히 대응한다. 원본 COL은 {source['col']['name_records'][0]['triangles']:,} triangles지만 FBX에는 {sum(row['triangles'] for row in rows if row['source_class'] == 'COL'):,} triangles만 남아 있어 FBX 왕복 중 34개 삼각형이 탈락했다.",
    "- `HexGround`를 제외한 515개는 재질·UV4·vertex color를 가진 WMB 시각 메시다. 이 중 명시적 `-LOD1` 3개를 제외하면 512개로, g11021 wd1의 문서상 object 수와 일치한다.",
    f"- 원본 `Layout.lay`에는 model {source['lay']['model_list_count']}종, asset record {source['lay']['asset_count']}개, 추가 instance {source['lay']['instance_count']}개가 있다. FBX에는 `-Instance` Empty나 LAY collection이 하나도 없으므로 LAY 배치 데이터는 포함되지 않았다.",
    "- WMB 시각 메시 515개는 모두 `UVMap1~4`와 `Col` vertex color를 갖지만 현재 ModelConverter/VTXMESH는 UV0 하나만 저장하고 vertex color를 버린다.",
    "- FBX는 material name 50개를 보존했지만 image texture node가 확인된 material은 `kaj_tree_leaf_aaa1_1` 하나뿐이고, 같은 `5C912AB1.dds` 참조만 중복되어 있다. 따라서 FBX embedded material은 입력 계약으로 사용할 수 없다.",
    f"- 같은 폴더의 `materials.json`은 FBX 50개 material name과 정확히 1:1 일치하며 texture binding {material_bindings}개, unique hash {len(material_hashes)}개를 보존한다. local g11021 및 같은 data family WDA cache 대조 결과 128개가 실제 DDS로 resolve되고 `1FBC0984`, `4E9C16F4`, `7FD4929A` 3개는 공통 Env/Irradiance, Light/Mask, DetailNormal placeholder 성격으로 별도 엔진 default binding이 필요하다.",
    "",
    "## Classification Evidence",
    "",
    "- `COL`: raw `g11021.col` parser 결과와 name·mesh count·vertex count가 일치하는 `HexGround`.",
    "- `WMB Internal LOD1`: 이름에 `-LOD1`이 명시된 3개 시각 메시.",
    "- `WMB Distant`: `g11021_DistantView` 16개.",
    "- 나머지 WMB: 512개 원본 object 집합에 속하는 시각 메시. 이름은 object 의미를 나타내지만 LAY instance는 아니다.",
    "- Blender `.001` suffix는 동일 이름 충돌을 피하기 위한 export/import suffix이며 원본 instance ID가 아니다.",
    "",
    "## Base Name Groups",
    "",
    "| Base name | Count | Source | Semantic | Vertices | Triangles | Unique geometry | Materials |",
    "|---|---:|---|---|---:|---:|---:|---:|",
]

for name, members in sorted(groups.items(), key=lambda item: (-len(item[1]), item[0])):
    lines.append(
        "| {name} | {count} | {source_class} | {semantic} | {vertices:,} | {triangles:,} | {unique_geometry} | {materials} |".format(
            name=name,
            count=len(members),
            source_class=members[0]["source_class"],
            semantic=members[0]["semantic_group"],
            vertices=sum(member["vertices"] for member in members),
            triangles=sum(member["triangles"] for member in members),
            unique_geometry=len({member["geometry_hash"] for member in members}),
            materials=len({member["material"] for member in members if member["material"]}),
        )
    )

lines.extend([
    "",
    "## Raw COL Summary",
    "",
    f"- names: {source['col']['name_count']}, mesh records: {source['col']['mesh_count']}, COL tree nodes: {source['col']['tree_node_count']}",
    "- `HexGround` collision types: `127` 297개, `255` 5개, `3` 68개, 미등록값 `95` 4개와 `16` 27개.",
    "- modifiers: `0` 394개, transparent wall `2` 7개.",
    "- surfaces: rock `2` 159개, concrete `0` 100개, metal solid `3` 55개, rubble2 `16` 48개, grass2 `9` 32개, metal grate `5` 4개, gravel1 `13` 3개.",
    "",
    "## Raw LAY Placements",
    "",
    "Asset record 자체도 하나의 배치 transform이며 `instance_count`는 그 뒤에 추가되는 복제 수다. 따라서 총 placement는 26 + 372 = 398개다.",
    "",
    "| Asset name | Model key | Base placement | Extra instances | Total placements |",
    "|---|---|---:|---:|---:|",
])

for asset in source["lay"]["assets"]:
    lines.append(
        f"| {asset['name']} | {asset['name'][:6]} | 1 | {asset['instance_count']} | {asset['instance_count'] + 1} |"
    )

lines.extend([
    "",
    "## Full FBX Mesh Inventory",
    "",
    "916개 이름을 Blender import 순서대로 모두 보존한다. 좌표 범위와 geometry hash까지 포함한 기계 판독용 전체 필드는 companion CSV를 사용한다.",
    "",
    "| # | Mesh name | Base name | Source | Semantic | Vertices | Triangles | Material | UV | Color |",
    "|---:|---|---|---|---|---:|---:|---|---|---|",
])

for row in rows:
    lines.append(
        f"| {row['index']} | {row['mesh_name']} | {row['base_name']} | {row['source_class']} | {row['semantic_group']} | {row['vertices']} | {row['triangles']} | {row['material'] or '-'} | {row['uv_layers'] or '-'} | {row['color_attributes'] or '-'} |"
    )

lines.extend([
    "",
    "## Links",
    "",
    "- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/CityOfRuinEntry_월드_임포트_계획]]",
    "- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata]]",
])

MD_OUTPUT.write_text("\n".join(lines) + "\n", encoding="utf-8")
print(f"CSV={CSV_OUTPUT}")
print(f"MARKDOWN={MD_OUTPUT}")
print(f"ROWS={len(rows)} BASE_GROUPS={len(groups)}")

import csv
import json
import os
import re
from collections import Counter, defaultdict


ROOT = os.path.dirname(os.path.abspath(__file__))
SOURCE = os.path.join(ROOT, "city_blends_deep_analysis.json")
CSV_PATH = os.path.join(ROOT, "CityOfRuin_BLEND_ObjectInventory.csv")
ENTRY_MESH_CSV_PATH = os.path.join(ROOT, "CityOfRuinEntry_BLEND_MeshInventory.csv")
MD_PATH = os.path.join(ROOT, "CityOfRuin_BLEND_Analysis.md")


def blender_base(name):
    return re.sub(r"\.\d{3}$", "", name)


def semantic_wmb_base(name, lod_level):
    value = blender_base(name)
    if lod_level == -1:
        value = re.sub(r"[-_]SHADOW$", "", value, flags=re.IGNORECASE)
    elif lod_level:
        value = re.sub(rf"[-_]LOD{lod_level}$", "", value, flags=re.IGNORECASE)
    return value


def lay_asset_key(name):
    value = blender_base(name)
    return re.sub(r"-Instance$", "", value)


def lay_model(asset_name):
    value = blender_base(asset_name)
    match = re.match(r"^(.*)_\d+$", value)
    return match.group(1) if match else value


def identity_transform(obj):
    loc = obj["location"]
    scale = obj["scale"]
    euler = obj["rotation_euler"]
    return (
        all(abs(v) < 1e-6 for v in loc)
        and all(abs(v - 1.0) < 1e-6 for v in scale)
        and all(abs(v) < 1e-6 for v in euler)
    )


def tree_integrity(meshes, tree_objects, suffix, object_node_property=None):
    nodes = {}
    malformed = []
    for obj in tree_objects:
        match = re.match(rf"^(\d+)_(-?\d+)_(-?\d+)_{suffix}$", obj["name"])
        if not match:
            malformed.append(obj["name"])
            continue
        index, left, right = map(int, match.groups())
        nodes[index] = {"left": left, "right": right, "object": obj}
    referenced = []
    leaf_assignment = {}
    branch_errors = []
    for index, node in nodes.items():
        left, right = node["left"], node["right"]
        if left >= 0:
            referenced.append(left)
            if left not in nodes:
                branch_errors.append((index, left))
        if right >= 0:
            referenced.append(right)
            if right not in nodes:
                branch_errors.append((index, right))
        if left == -1 and right == -1:
            for mesh_index in node["object"]["id_properties"].get("meshIndices", []):
                leaf_assignment.setdefault(mesh_index, []).append(index)
    covered = set(leaf_assignment)
    expected = set(range(len(meshes)))
    property_mismatches = []
    if object_node_property:
        for mesh_index, mesh in enumerate(meshes):
            expected_node = mesh["id_properties"].get(object_node_property)
            actual_nodes = leaf_assignment.get(mesh_index, [])
            if expected_node not in actual_nodes:
                property_mismatches.append((mesh_index, mesh["name"], expected_node, actual_nodes))
    return {
        "nodes": len(nodes),
        "leaves": sum(1 for node in nodes.values() if node["left"] == -1 and node["right"] == -1),
        "root_candidates": sorted(set(nodes) - set(referenced)),
        "missing_mesh_indices": sorted(expected - covered),
        "out_of_range_mesh_indices": sorted(covered - expected),
        "duplicate_mesh_indices": {key: value for key, value in leaf_assignment.items() if len(value) > 1},
        "property_mismatches": property_mismatches,
        "branch_errors": branch_errors,
        "malformed_names": malformed,
        "display_types": dict(Counter(obj["empty_display_type"] for obj in tree_objects)),
    }


def domain_for(obj):
    collections = set(obj["collections"])
    if "COL" in collections:
        return "COL_MESH"
    if "col_colTreeNodes" in collections:
        return "COL_TREE"
    if "lay_layAssets" in collections:
        return "LAY_ASSET"
    if "lay_layInstances" in collections:
        return "LAY_INSTANCE"
    if "wmb_colTreeNodes" in collections:
        return "WMB_TREE"
    if obj["type"] == "MESH" and obj["id_properties"].get("mesh_group_props"):
        return "WMB_MESH"
    return "OTHER"


def summarize_file(entry):
    objects = entry["objects"]
    object_by_name = {obj["name"]: obj for obj in objects}
    collection_by_name = {collection["name"]: collection for collection in entry["collections"]}
    domains = defaultdict(list)
    for obj in objects:
        domains[domain_for(obj)].append(obj)

    wmb_collection = next(
        collection for collection in entry["collections"]
        if collection["name"].startswith("g") and collection["parents"] == ["WMB"]
    )
    wmb = [object_by_name[name] for name in wmb_collection["direct_object_order"]]
    col = [object_by_name[name] for name in collection_by_name["COL"]["direct_object_order"]]
    lay_assets = domains["LAY_ASSET"]
    lay_instances = domains["LAY_INSTANCE"]

    lod = defaultdict(lambda: {"objects": 0, "vertices": 0, "triangles": 0, "bases": set(), "materials": set()})
    suffix_mismatches = []
    groups = defaultdict(Counter)
    group_triangles = defaultdict(Counter)
    for obj in wmb:
        props = obj["id_properties"]["mesh_group_props"]
        level = props["lod_level"]
        base = semantic_wmb_base(obj["name"], level)
        mesh = obj["mesh"]
        item = lod[level]
        item["objects"] += 1
        item["vertices"] += mesh["vertices"]
        item["triangles"] += mesh["triangles"] or 0
        item["bases"].add(base)
        item["materials"].update(m for m in mesh["materials"] if m)
        groups[base][level] += 1
        group_triangles[base][level] += mesh["triangles"] or 0
        name_has = re.search(r"[-_]LOD(\d+)(?:\.\d{3})?$", obj["name"], re.IGNORECASE)
        if re.search(r"[-_]SHADOW(?:\.\d{3})?$", obj["name"], re.IGNORECASE):
            named_level = -1
        else:
            named_level = int(name_has.group(1)) if name_has else 0
        if named_level != level:
            suffix_mismatches.append((obj["name"], named_level, level))

    lod_patterns = Counter(tuple(sorted(level_counts)) for level_counts in groups.values())
    col_types = Counter()
    col_surfaces = Counter()
    col_modifiers = Counter()
    for obj in col:
        props = obj["id_properties"]["col_mesh_props"]
        col_types[props["col_type"]] += 1
        col_surfaces[props["surface_type"]] += 1
        col_modifiers[props["modifier"]] += 1

    asset_keys = {obj["name"] for obj in lay_assets}
    placements = Counter({key: 1 for key in asset_keys})
    orphan_instances = Counter()
    for obj in lay_instances:
        key = lay_asset_key(obj["name"])
        placements[key] += 1
        if key not in asset_keys:
            orphan_instances[key] += 1
    models = Counter()
    for asset, count in placements.items():
        models[lay_model(asset)] += count

    mesh_attribute_shapes = Counter()
    for obj in wmb:
        mesh = obj["mesh"]
        signature = (
            tuple(mesh["uv_layers"]),
            tuple(attr["name"] for attr in mesh["color_attributes"]),
        )
        mesh_attribute_shapes[signature] += 1

    material_vertex_color = []
    for mat in entry["materials"]:
        attrs = [node for node in mat["nodes"] if node["type"] == "VERTEX_COLOR"]
        if attrs:
            material_vertex_color.append(mat["name"])

    wmb_tree_objects = [
        object_by_name[name] for name in collection_by_name["wmb_colTreeNodes"]["direct_object_order"]
    ]
    col_tree_objects = [
        object_by_name[name] for name in collection_by_name["col_colTreeNodes"]["direct_object_order"]
    ]
    wmb_tree = tree_integrity(wmb, wmb_tree_objects, "wmb", "colTreeNodeIndex")
    col_tree = tree_integrity(col, col_tree_objects, "col")

    return {
        "domains": {key: len(value) for key, value in sorted(domains.items())},
        "lod": lod,
        "lod_patterns": lod_patterns,
        "lod_groups": groups,
        "lod_group_triangles": group_triangles,
        "suffix_mismatches": suffix_mismatches,
        "col_types": col_types,
        "col_surfaces": col_surfaces,
        "col_modifiers": col_modifiers,
        "lay_placements": placements,
        "lay_models": models,
        "lay_orphans": orphan_instances,
        "mesh_attribute_shapes": mesh_attribute_shapes,
        "material_vertex_color": material_vertex_color,
        "wmb_tree": wmb_tree,
        "col_tree": col_tree,
        "non_identity": Counter(domain_for(obj) for obj in objects if not identity_transform(obj)),
        "constraints": sum(len(obj["constraints"]) for obj in objects),
        "modifiers": sum(len(obj["modifiers"]) for obj in objects),
        "parents": sum(1 for obj in objects if obj["parent"]),
        "collection_instances": sum(1 for obj in objects if obj["instance_collection"]),
    }


def inventory_rows(data):
    for entry in data["files"]:
        filename = os.path.basename(entry["file"])
        for obj in entry["objects"]:
            domain = domain_for(obj)
            ids = obj["id_properties"]
            wmb_props = ids.get("mesh_group_props", {})
            col_props = ids.get("col_mesh_props", {})
            mesh = obj.get("mesh", {})
            level = wmb_props.get("lod_level", "")
            semantic_base = semantic_wmb_base(obj["name"], level) if domain == "WMB_MESH" else ""
            asset_key = lay_asset_key(obj["name"]) if domain in {"LAY_ASSET", "LAY_INSTANCE"} else ""
            yield {
                "blend_file": filename,
                "domain": domain,
                "object_name": obj["name"],
                "type": obj["type"],
                "data_name": obj["data_name"] or "",
                "collections": "|".join(obj["collections"]),
                "semantic_base": semantic_base,
                "lod_name": wmb_props.get("lod_name", ""),
                "lod_level": level,
                "vertices": mesh.get("vertices", ""),
                "triangles": mesh.get("triangles", ""),
                "materials": "|".join(m for m in mesh.get("materials", []) if m),
                "uv_layers": "|".join(mesh.get("uv_layers", [])),
                "color_attributes": "|".join(attr["name"] for attr in mesh.get("color_attributes", [])),
                "modifiers": "|".join(mod["type"] for mod in obj["modifiers"]),
                "parent": obj["parent"] or "",
                "instance_type": obj["instance_type"],
                "instance_collection": obj["instance_collection"] or "",
                "lay_asset_key": asset_key,
                "lay_model": lay_model(asset_key) if asset_key else "",
                "boneSetIndex": ids.get("boneSetIndex", ""),
                "colTreeNodeIndex": ids.get("colTreeNodeIndex", ""),
                "unknownWorldDataIndex": ids.get("unknownWorldDataIndex", ""),
                "col_type": col_props.get("col_type", ""),
                "surface_type": col_props.get("surface_type", ""),
                "col_modifier": col_props.get("modifier", ""),
                "location": json.dumps(obj["location"], separators=(",", ":")),
                "rotation_euler": json.dumps(obj["rotation_euler"], separators=(",", ":")),
                "scale": json.dumps(obj["scale"], separators=(",", ":")),
            }


def main():
    with open(SOURCE, "r", encoding="utf-8") as stream:
        data = json.load(stream)

    rows = list(inventory_rows(data))
    with open(CSV_PATH, "w", encoding="utf-8-sig", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)
    entry_mesh_rows = [
        row for row in rows
        if row["blend_file"] == "CityOfRuinEntry.blend" and row["type"] == "MESH"
    ]
    with open(ENTRY_MESH_CSV_PATH, "w", encoding="utf-8-sig", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(entry_mesh_rows)

    lines = [
        "# CityOfRuin `.blend` 구조 분석",
        "",
        "- 근거: Blender 4.5.13에서 세 파일을 직접 열어 Collection, object, custom property, modifier, constraint, material node를 조사함.",
        "- `LOD`는 이름이 아니라 `mesh_group_props.lod_level` / `lod_name`을 기준으로 집계함.",
        "- CSV에는 세 파일의 모든 object 이름과 분류/속성을 기록함.",
        "",
    ]
    for entry in data["files"]:
        name = os.path.basename(entry["file"])
        summary = summarize_file(entry)
        lines.extend([f"## {name}", "", f"- domain: `{summary['domains']}`"])
        for level, item in sorted(summary["lod"].items()):
            label = "SHADOW" if level == -1 else f"LOD{level}"
            lines.append(
                f"- {label}: objects {item['objects']}, semantic bases {len(item['bases'])}, "
                f"vertices {item['vertices']:,}, triangles {item['triangles']:,}, materials {len(item['materials'])}"
            )
        lines.extend(["", "| Semantic mesh group | Object count by pass | Triangle count by pass |", "|---|---:|---:|"])
        for base in sorted(summary["lod_groups"], key=str.lower):
            count_parts = []
            tri_parts = []
            for level, count in sorted(summary["lod_groups"][base].items()):
                label = "SHADOW" if level == -1 else f"LOD{level}"
                count_parts.append(f"{label}={count}")
                tri_parts.append(f"{label}={summary['lod_group_triangles'][base][level]:,}")
            lines.append(f"| `{base}` | {', '.join(count_parts)} | {', '.join(tri_parts)} |")
        lines.extend([
            "",
            f"- LOD level-set patterns: `{dict(summary['lod_patterns'])}`",
            f"- LOD name/property mismatches: {len(summary['suffix_mismatches'])}",
            f"- COL type: `{dict(summary['col_types'])}`",
            f"- COL surface: `{dict(summary['col_surfaces'])}`",
            f"- COL modifier field: `{dict(summary['col_modifiers'])}`",
            f"- LAY model placements: `{dict(summary['lay_models'])}`",
            f"- LAY orphan instance keys: `{dict(summary['lay_orphans'])}`",
            f"- WMB spatial tree: nodes {summary['wmb_tree']['nodes']}, leaves {summary['wmb_tree']['leaves']}, "
            f"root {summary['wmb_tree']['root_candidates']}, missing/out-of-range/duplicate/property-mismatch/branch-error "
            f"{len(summary['wmb_tree']['missing_mesh_indices'])}/{len(summary['wmb_tree']['out_of_range_mesh_indices'])}/"
            f"{len(summary['wmb_tree']['duplicate_mesh_indices'])}/{len(summary['wmb_tree']['property_mismatches'])}/"
            f"{len(summary['wmb_tree']['branch_errors'])}",
            f"- COL spatial tree: nodes {summary['col_tree']['nodes']}, leaves {summary['col_tree']['leaves']}, "
            f"root {summary['col_tree']['root_candidates']}, missing/out-of-range/duplicate/branch-error "
            f"{len(summary['col_tree']['missing_mesh_indices'])}/{len(summary['col_tree']['out_of_range_mesh_indices'])}/"
            f"{len(summary['col_tree']['duplicate_mesh_indices'])}/{len(summary['col_tree']['branch_errors'])}",
            f"- non-identity transforms by domain: `{dict(summary['non_identity'])}`",
            f"- Blender modifiers/constraints/parents/collection instances: "
            f"{summary['modifiers']}/{summary['constraints']}/{summary['parents']}/{summary['collection_instances']}",
            f"- WMB mesh UV/color signatures: `{dict(summary['mesh_attribute_shapes'])}`",
            f"- materials using Vertex Color node: `{summary['material_vertex_color']}`",
            "",
        ])

    with open(MD_PATH, "w", encoding="utf-8", newline="\n") as stream:
        stream.write("\n".join(lines))
    print(MD_PATH)
    print(CSV_PATH)
    print(ENTRY_MESH_CSV_PATH)


if __name__ == "__main__":
    main()

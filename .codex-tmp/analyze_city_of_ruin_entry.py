import json
import hashlib
import re
import struct
from collections import Counter, defaultdict
from pathlib import Path

import bpy
from mathutils import Vector


SOURCE = Path(r"C:\Users\a9018\Desktop\Direct11_Nier_Automata\Client\bin\resources\Models\World\CityOfRuinEntry\CityOfRuinEntry.fbx")
OUTPUT = Path(r"C:\Users\a9018\Desktop\Direct11_Nier_Automata\.codex-tmp\city_of_ruin_entry_analysis.json")


def json_value(value):
    if isinstance(value, (str, int, float, bool)) or value is None:
        return value
    try:
        return list(value)
    except TypeError:
        return str(value)


def classify_name(name):
    upper = name.upper()
    tags = []
    for tag, patterns in {
        "COL": (r"(^|[^A-Z])COL([^A-Z]|$)", r"COLLISION"),
        "LAY": (r"(^|[^A-Z])LAY([^A-Z]|$)", r"LAYOUT"),
        "LOD": (r"(^|[^A-Z])LOD[0-9]*([^A-Z]|$)",),
        "INSTANCE": (r"INSTANCE", r"(^|[^A-Z])INST([^A-Z]|$)"),
        "NAV": (r"(^|[^A-Z])NAV([^A-Z]|$)",),
    }.items():
        if any(re.search(pattern, upper) for pattern in patterns):
            tags.append(tag)
    return tags or ["OTHER"]


bpy.ops.wm.read_factory_settings(use_empty=True)
result = bpy.ops.import_scene.fbx(filepath=str(SOURCE), use_custom_normals=True)
if "FINISHED" not in result:
    raise RuntimeError(f"FBX import failed: {result}")

objects = list(bpy.data.objects)
meshes = [obj for obj in objects if obj.type == "MESH"]
type_counts = Counter(obj.type for obj in objects)
tag_counts = Counter()
tag_samples = defaultdict(list)
prefix_counts = Counter()
mesh_data_users = Counter(obj.data.name for obj in meshes if obj.data)
shared_mesh_data = {
    name: count for name, count in mesh_data_users.items() if count > 1
}

world_min = Vector((float("inf"),) * 3)
world_max = Vector((float("-inf"),) * 3)
records = []

for obj in objects:
    tags = classify_name(obj.name)
    for tag in tags:
        tag_counts[tag] += 1
        if len(tag_samples[tag]) < 30:
            tag_samples[tag].append(obj.name)
    prefix = re.split(r"[_\-.: ]", obj.name, maxsplit=1)[0]
    prefix_counts[prefix] += 1

    record = {
        "name": obj.name,
        "type": obj.type,
        "parent": obj.parent.name if obj.parent else None,
        "children": [child.name for child in obj.children],
        "collections": [collection.name for collection in obj.users_collection],
        "location": list(obj.location),
        "rotation_euler": list(obj.rotation_euler),
        "scale": list(obj.scale),
        "dimensions": list(obj.dimensions),
        "instance_type": obj.instance_type,
        "instance_collection": obj.instance_collection.name if obj.instance_collection else None,
        "custom_properties": {
            key: json_value(obj[key]) for key in obj.keys() if key != "_RNA_UI"
        },
        "tags": tags,
    }
    if obj.type == "MESH" and obj.data:
        mesh = obj.data
        mesh.calc_loop_triangles()
        local_min = Vector((float("inf"),) * 3)
        local_max = Vector((float("-inf"),) * 3)
        for vertex in mesh.vertices:
            local_min.x = min(local_min.x, vertex.co.x)
            local_min.y = min(local_min.y, vertex.co.y)
            local_min.z = min(local_min.z, vertex.co.z)
            local_max.x = max(local_max.x, vertex.co.x)
            local_max.y = max(local_max.y, vertex.co.y)
            local_max.z = max(local_max.z, vertex.co.z)
        geometry_hash = hashlib.sha256()
        for vertex in mesh.vertices:
            normalized = vertex.co - local_min
            geometry_hash.update(struct.pack("<3i", *(round(value * 10000) for value in normalized)))
        for polygon in mesh.polygons:
            geometry_hash.update(struct.pack("<I", len(polygon.vertices)))
            geometry_hash.update(struct.pack(f"<{len(polygon.vertices)}I", *polygon.vertices))
        record["mesh"] = {
            "data_name": mesh.name,
            "data_users": mesh.users,
            "vertices": len(mesh.vertices),
            "edges": len(mesh.edges),
            "polygons": len(mesh.polygons),
            "triangles": len(mesh.loop_triangles),
            "uv_layers": [layer.name for layer in mesh.uv_layers],
            "color_attributes": [attribute.name for attribute in mesh.color_attributes],
            "materials": [material.name if material else None for material in mesh.materials],
            "local_bounds": {"min": list(local_min), "max": list(local_max)},
            "translation_normalized_geometry_hash": geometry_hash.hexdigest(),
        }
        for corner in obj.bound_box:
            point = obj.matrix_world @ Vector(corner)
            world_min.x = min(world_min.x, point.x)
            world_min.y = min(world_min.y, point.y)
            world_min.z = min(world_min.z, point.z)
            world_max.x = max(world_max.x, point.x)
            world_max.y = max(world_max.y, point.y)
            world_max.z = max(world_max.z, point.z)
    records.append(record)

collections = []
for collection in bpy.data.collections:
    collections.append({
        "name": collection.name,
        "parent_names": [candidate.name for candidate in bpy.data.collections if collection.name in candidate.children],
        "object_count": len(collection.objects),
        "objects": [obj.name for obj in collection.objects],
        "custom_properties": {
            key: json_value(collection[key]) for key in collection.keys() if key != "_RNA_UI"
        },
    })

materials = []
for material in bpy.data.materials:
    nodes = []
    if material.use_nodes and material.node_tree:
        for node in material.node_tree.nodes:
            node_record = {"name": node.name, "type": node.type, "label": node.label}
            if node.type == "TEX_IMAGE" and getattr(node, "image", None):
                node_record["image"] = node.image.name
                node_record["image_filepath"] = node.image.filepath
            nodes.append(node_record)
    materials.append({
        "name": material.name,
        "users": material.users,
        "use_nodes": material.use_nodes,
        "custom_properties": {
            key: json_value(material[key]) for key in material.keys() if key != "_RNA_UI"
        },
        "nodes": nodes,
    })

images = [{"name": image.name, "filepath": image.filepath, "source": image.source} for image in bpy.data.images]

analysis = {
    "source": str(SOURCE),
    "source_bytes": SOURCE.stat().st_size,
    "object_count": len(objects),
    "object_type_counts": dict(type_counts),
    "mesh_count": len(meshes),
    "mesh_totals": {
        "vertices": sum(len(obj.data.vertices) for obj in meshes),
        "polygons": sum(len(obj.data.polygons) for obj in meshes),
        "triangles": sum(len(obj.data.loop_triangles) for obj in meshes),
    },
    "name_tag_counts": dict(tag_counts),
    "name_tag_samples": dict(tag_samples),
    "name_prefix_counts": dict(prefix_counts.most_common()),
    "shared_mesh_data": shared_mesh_data,
    "world_bounds": {"min": list(world_min), "max": list(world_max)},
    "collection_count": len(collections),
    "collections": collections,
    "material_count": len(materials),
    "materials": materials,
    "images": images,
    "objects": records,
}

OUTPUT.write_text(json.dumps(analysis, ensure_ascii=False, indent=2), encoding="utf-8")

print(f"ANALYSIS_OUTPUT={OUTPUT}")
print(f"OBJECTS={len(objects)} TYPES={dict(type_counts)} MESHES={len(meshes)}")
print(f"MESH_TOTALS={analysis['mesh_totals']}")
print(f"TAGS={dict(tag_counts)}")
print(f"COLLECTIONS={len(collections)} MATERIALS={len(materials)} IMAGES={len(images)}")
print(f"SHARED_MESH_DATABLOCKS={len(shared_mesh_data)}")
print(f"BOUNDS_MIN={list(world_min)} BOUNDS_MAX={list(world_max)}")

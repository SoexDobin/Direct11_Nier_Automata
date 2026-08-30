import bpy
import json
import os
from pathlib import Path


def matrix_rows(matrix):
    return [[float(value) for value in row] for row in matrix]


def vector_values(value):
    return [float(component) for component in value]


def object_record(obj):
    modifiers = []
    for modifier in obj.modifiers:
        record = {
            "name": modifier.name,
            "type": modifier.type,
        }
        if modifier.type == "ARMATURE":
            record["object"] = modifier.object.name if modifier.object else None
            record["use_vertex_groups"] = bool(modifier.use_vertex_groups)
            record["use_bone_envelopes"] = bool(modifier.use_bone_envelopes)
        modifiers.append(record)

    vertex_groups = []
    if obj.type == "MESH":
        weights_by_group = {group.index: [] for group in obj.vertex_groups}
        for vertex in obj.data.vertices:
            for membership in vertex.groups:
                weights_by_group.setdefault(membership.group, []).append(
                    float(membership.weight)
                )
        for group in obj.vertex_groups:
            weights = weights_by_group.get(group.index, [])
            vertex_groups.append(
                {
                    "name": group.name,
                    "vertex_count": len(weights),
                    "min_weight": min(weights) if weights else None,
                    "max_weight": max(weights) if weights else None,
                    "mean_weight": (
                        sum(weights) / len(weights) if weights else None
                    ),
                }
            )

    animation_data = obj.animation_data
    return {
        "name": obj.name,
        "type": obj.type,
        "parent": obj.parent.name if obj.parent else None,
        "parent_type": obj.parent_type,
        "parent_bone": obj.parent_bone,
        "children": [child.name for child in obj.children],
        "matrix_world": matrix_rows(obj.matrix_world),
        "matrix_parent_inverse": matrix_rows(obj.matrix_parent_inverse),
        "location": vector_values(obj.location),
        "rotation_mode": obj.rotation_mode,
        "rotation_euler": vector_values(obj.rotation_euler),
        "scale": vector_values(obj.scale),
        "modifiers": modifiers,
        "vertex_groups": vertex_groups,
        "action": (
            animation_data.action.name
            if animation_data and animation_data.action
            else None
        ),
    }


def armature_record(obj):
    bones = []
    for bone in obj.data.bones:
        bones.append(
            {
                "name": bone.name,
                "parent": bone.parent.name if bone.parent else None,
                "children": [child.name for child in bone.children],
                "use_connect": bool(bone.use_connect),
                "head_local": vector_values(bone.head_local),
                "tail_local": vector_values(bone.tail_local),
                "matrix_local": matrix_rows(bone.matrix_local),
            }
        )

    animation_data = obj.animation_data
    return {
        "object": obj.name,
        "data": obj.data.name,
        "parent": obj.parent.name if obj.parent else None,
        "children": [child.name for child in obj.children],
        "display_type": obj.data.display_type,
        "action": (
            animation_data.action.name
            if animation_data and animation_data.action
            else None
        ),
        "bones": bones,
    }


scene = bpy.context.scene
objects = list(scene.objects)
payload = {
    "pid": os.getpid(),
    "blend_filepath": bpy.data.filepath,
    "scene": scene.name,
    "active_object": (
        bpy.context.view_layer.objects.active.name
        if bpy.context.view_layer.objects.active
        else None
    ),
    "selected_objects": [obj.name for obj in bpy.context.selected_objects],
    "objects": [object_record(obj) for obj in objects],
    "armatures": [armature_record(obj) for obj in objects if obj.type == "ARMATURE"],
    "actions": [action.name for action in bpy.data.actions],
}

output = Path(
    r"C:\Users\a9018\Desktop\Direct11_Nier_Automata\.codex-tmp"
) / f"blender_hierarchy_{os.getpid()}.json"
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text(json.dumps(payload, ensure_ascii=False, indent=2), encoding="utf-8")
print(f"CODEX_HIERARCHY_REPORT={output}")

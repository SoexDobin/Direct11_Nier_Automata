import bpy
import json
import math
from pathlib import Path


scene = bpy.context.scene
armature = scene.objects["wp0220"]
mesh = next(obj for obj in scene.objects if obj.type == "MESH")
pose_bone = armature.pose.bones[0]
depsgraph = bpy.context.evaluated_depsgraph_get()


def evaluated_world_positions():
    evaluated_object = mesh.evaluated_get(depsgraph)
    evaluated_mesh = evaluated_object.to_mesh()
    try:
        world = evaluated_object.matrix_world
        return [world @ vertex.co for vertex in evaluated_mesh.vertices]
    finally:
        evaluated_object.to_mesh_clear()


original_basis = pose_bone.matrix_basis.copy()
before = evaluated_world_positions()

pose_bone.rotation_mode = "XYZ"
pose_bone.rotation_euler.z += math.radians(15.0)
bpy.context.view_layer.update()
after = evaluated_world_positions()

displacements = [
    (after_position - before_position).length
    for before_position, after_position in zip(before, after)
]

pose_bone.matrix_basis = original_basis
bpy.context.view_layer.update()

report = {
    "mesh": mesh.name,
    "armature": armature.name,
    "bone": pose_bone.name,
    "vertex_count": len(displacements),
    "moved_vertices": sum(value > 1e-6 for value in displacements),
    "max_displacement": max(displacements) if displacements else 0.0,
    "pose_restored": all(
        abs(pose_bone.matrix_basis[row][column] - original_basis[row][column]) < 1e-6
        for row in range(4)
        for column in range(4)
    ),
}

output = Path(
    r"C:\Users\a9018\Desktop\Direct11_Nier_Automata\.codex-tmp\wp0220_deformation_verification.json"
)
output.write_text(
    json.dumps(report, ensure_ascii=False, indent=2),
    encoding="utf-8",
)
print(f"CODEX_WP0220_DEFORMATION_REPORT={output}")

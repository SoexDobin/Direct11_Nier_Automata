import bpy
import json
from datetime import datetime
from pathlib import Path


OUTPUT_DIR = Path(
    r"C:\Users\a9018\Desktop\Direct11_Nier_Automata\.codex-tmp"
)
OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
before_path = OUTPUT_DIR / f"wp0220_before_hierarchy_fix_{stamp}.blend"
after_path = OUTPUT_DIR / f"wp0220_hierarchy_fixed_{stamp}.blend"
report_path = OUTPUT_DIR / "wp0220_hierarchy_repair.json"

scene = bpy.context.scene
armature = scene.objects.get("wp0220")
if armature is None or armature.type != "ARMATURE":
    raise RuntimeError("Expected one ARMATURE object named 'wp0220'.")

meshes = [obj for obj in scene.objects if obj.type == "MESH"]
if len(meshes) != 1:
    raise RuntimeError(
        f"Expected exactly one mesh in the wp0220 scene, found {len(meshes)}."
    )
mesh = meshes[0]

if len(armature.data.bones) != 1:
    raise RuntimeError(
        "Expected the wp0220 armature to contain exactly one rigid root bone."
    )
bone = armature.data.bones[0]

if mesh.parent is not None:
    raise RuntimeError(
        f"Expected mesh '{mesh.name}' to be unparented before repair; "
        f"found parent '{mesh.parent.name}'."
    )
if any(modifier.type == "ARMATURE" for modifier in mesh.modifiers):
    raise RuntimeError(
        f"Mesh '{mesh.name}' already has an Armature modifier; aborting to avoid "
        "overwriting an unknown setup."
    )
if len(mesh.vertex_groups) != 0:
    raise RuntimeError(
        f"Mesh '{mesh.name}' already has vertex groups; aborting to avoid "
        "overwriting weights."
    )

save_before = bpy.ops.wm.save_as_mainfile(
    filepath=str(before_path),
    check_existing=False,
    copy=True,
)
if "FINISHED" not in save_before:
    raise RuntimeError("Could not create the pre-repair Blender backup copy.")

world_before = mesh.matrix_world.copy()
mesh.parent = armature
mesh.parent_type = "OBJECT"
mesh.parent_bone = ""
mesh.matrix_parent_inverse = armature.matrix_world.inverted()
mesh.matrix_world = world_before

modifier = mesh.modifiers.new(name="Armature", type="ARMATURE")
modifier.object = armature
modifier.use_vertex_groups = True
modifier.use_bone_envelopes = False

vertex_group = mesh.vertex_groups.new(name=bone.name)
vertex_indices = list(range(len(mesh.data.vertices)))
vertex_group.add(vertex_indices, 1.0, "REPLACE")

bpy.context.view_layer.update()

if mesh.parent != armature:
    raise RuntimeError("Parent assignment verification failed.")
if modifier.object != armature:
    raise RuntimeError("Armature modifier target verification failed.")
if len(vertex_indices) != len(mesh.data.vertices):
    raise RuntimeError("Vertex assignment count verification failed.")

weighted_vertices = 0
min_weight = 1.0
max_weight = 0.0
for vertex in mesh.data.vertices:
    try:
        weight = vertex_group.weight(vertex.index)
    except RuntimeError:
        continue
    weighted_vertices += 1
    min_weight = min(min_weight, float(weight))
    max_weight = max(max_weight, float(weight))

if weighted_vertices != len(mesh.data.vertices):
    raise RuntimeError(
        f"Only {weighted_vertices}/{len(mesh.data.vertices)} vertices were weighted."
    )
if min_weight != 1.0 or max_weight != 1.0:
    raise RuntimeError(
        f"Rigid weight verification failed: min={min_weight}, max={max_weight}."
    )

layout_workspace = bpy.data.workspaces.get("Layout")
if layout_workspace is not None and bpy.context.window is not None:
    bpy.context.window.workspace = layout_workspace

save_after = bpy.ops.wm.save_as_mainfile(
    filepath=str(after_path),
    check_existing=False,
    copy=True,
)
if "FINISHED" not in save_after:
    raise RuntimeError("Could not create the repaired Blender backup copy.")

report = {
    "armature": armature.name,
    "bone": bone.name,
    "mesh": mesh.name,
    "mesh_parent": mesh.parent.name if mesh.parent else None,
    "armature_modifier": modifier.name,
    "armature_modifier_target": modifier.object.name if modifier.object else None,
    "vertex_group": vertex_group.name,
    "vertex_count": len(mesh.data.vertices),
    "weighted_vertices": weighted_vertices,
    "min_weight": min_weight,
    "max_weight": max_weight,
    "world_transform_preserved": all(
        abs(world_before[row][column] - mesh.matrix_world[row][column]) < 1e-6
        for row in range(4)
        for column in range(4)
    ),
    "before_backup": str(before_path),
    "fixed_backup": str(after_path),
}
report_path.write_text(
    json.dumps(report, ensure_ascii=False, indent=2),
    encoding="utf-8",
)
print(f"CODEX_WP0220_REPAIR_REPORT={report_path}")

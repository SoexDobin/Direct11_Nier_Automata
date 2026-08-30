import bpy
import json
import os
import sys
from collections import Counter, defaultdict


def plain(value, depth=0):
    if depth > 3:
        return repr(value)
    if value is None or isinstance(value, (bool, int, float, str)):
        return value
    if hasattr(value, "keys"):
        try:
            return {str(key): plain(value[key], depth + 1) for key in value.keys()}
        except Exception:
            pass
    if hasattr(value, "to_list"):
        try:
            return [plain(v, depth + 1) for v in value.to_list()]
        except Exception:
            pass
    if isinstance(value, (list, tuple)):
        return [plain(v, depth + 1) for v in value]
    if hasattr(value, "name"):
        return {"type": type(value).__name__, "name": value.name}
    try:
        return [plain(v, depth + 1) for v in value]
    except Exception:
        return repr(value)


def id_properties(owner):
    result = {}
    try:
        for key in owner.keys():
            if key == "_RNA_UI":
                continue
            try:
                result[key] = plain(owner[key])
            except Exception as exc:
                result[key] = f"<error: {exc}>"
    except Exception:
        pass
    return result


def rna_simple_properties(owner, excluded=None):
    excluded = set(excluded or ()) | {"rna_type"}
    result = {}
    try:
        props = owner.bl_rna.properties
    except Exception:
        return result
    for prop in props:
        ident = prop.identifier
        if ident in excluded or prop.is_readonly:
            continue
        try:
            value = getattr(owner, ident)
        except Exception:
            continue
        if isinstance(value, (bool, int, float, str)):
            result[ident] = value
        elif hasattr(value, "to_list"):
            try:
                vals = value.to_list()
                if len(vals) <= 16:
                    result[ident] = plain(vals)
            except Exception:
                pass
        elif hasattr(value, "name") and value is not None:
            result[ident] = {"type": type(value).__name__, "name": value.name}
    return result


def modifier_record(mod):
    record = {
        "name": mod.name,
        "type": mod.type,
        "show_viewport": mod.show_viewport,
        "show_render": mod.show_render,
        "show_in_editmode": mod.show_in_editmode,
        "show_on_cage": mod.show_on_cage,
        "id_properties": id_properties(mod),
        "settings": rna_simple_properties(
            mod,
            excluded={"name", "type", "show_viewport", "show_render", "show_in_editmode", "show_on_cage"},
        ),
    }
    if mod.type == "NODES" and getattr(mod, "node_group", None):
        group = mod.node_group
        group_record = {
            "name": group.name,
            "id_properties": id_properties(group),
            "nodes": [],
            "links": [],
            "interface": [],
        }
        for node in group.nodes:
            group_record["nodes"].append({
                "name": node.name,
                "label": node.label,
                "bl_idname": node.bl_idname,
                "type": node.type,
                "id_properties": id_properties(node),
            })
        for link in group.links:
            group_record["links"].append({
                "from_node": link.from_node.name,
                "from_socket": link.from_socket.name,
                "to_node": link.to_node.name,
                "to_socket": link.to_socket.name,
            })
        try:
            for item in group.interface.items_tree:
                item_record = {
                    "name": item.name,
                    "item_type": item.item_type,
                    "identifier": getattr(item, "identifier", None),
                    "in_out": getattr(item, "in_out", None),
                    "socket_type": getattr(item, "socket_type", None),
                }
                if hasattr(item, "default_value"):
                    item_record["default_value"] = plain(item.default_value)
                group_record["interface"].append(item_record)
        except Exception as exc:
            group_record["interface_error"] = repr(exc)
        record["node_group"] = group_record
    return record


def custom_property_group(owner, attr_name):
    try:
        group = getattr(owner, attr_name)
    except Exception:
        return None
    if group is None:
        return None
    data = rna_simple_properties(group)
    ids = id_properties(group)
    if ids:
        data["id_properties"] = ids
    return data or None


def mesh_record(mesh):
    try:
        mesh.calc_loop_triangles()
        triangles = len(mesh.loop_triangles)
    except Exception:
        triangles = None
    return {
        "name": mesh.name,
        "users": mesh.users,
        "vertices": len(mesh.vertices),
        "edges": len(mesh.edges),
        "polygons": len(mesh.polygons),
        "triangles": triangles,
        "materials": [material.name if material else None for material in mesh.materials],
        "uv_layers": [uv.name for uv in mesh.uv_layers],
        "color_attributes": [
            {"name": attr.name, "domain": attr.domain, "data_type": attr.data_type}
            for attr in mesh.color_attributes
        ],
        "attributes": [
            {"name": attr.name, "domain": attr.domain, "data_type": attr.data_type}
            for attr in mesh.attributes
        ],
        "shape_keys": [block.name for block in mesh.shape_keys.key_blocks] if mesh.shape_keys else [],
        "id_properties": id_properties(mesh),
    }


def object_record(obj):
    record = {
        "name": obj.name,
        "type": obj.type,
        "data_name": getattr(obj.data, "name", None),
        "collections": sorted(collection.name for collection in obj.users_collection),
        "parent": obj.parent.name if obj.parent else None,
        "parent_type": obj.parent_type,
        "children": sorted(child.name for child in obj.children),
        "instance_type": obj.instance_type,
        "instance_collection": obj.instance_collection.name if obj.instance_collection else None,
        "location": plain(obj.location),
        "rotation_mode": obj.rotation_mode,
        "rotation_euler": plain(obj.rotation_euler),
        "rotation_quaternion": plain(obj.rotation_quaternion),
        "scale": plain(obj.scale),
        "matrix_world": [plain(row) for row in obj.matrix_world],
        "hide_viewport": obj.hide_viewport,
        "hide_render": obj.hide_render,
        "hide_get": obj.hide_get(),
        "display_type": obj.display_type,
        "empty_display_type": obj.empty_display_type if obj.type == "EMPTY" else None,
        "empty_display_size": obj.empty_display_size if obj.type == "EMPTY" else None,
        "dimensions": plain(obj.dimensions),
        "visible_camera": obj.visible_camera,
        "visible_shadow": obj.visible_shadow,
        "id_properties": id_properties(obj),
        "constraints": [
            {
                "name": constraint.name,
                "type": constraint.type,
                "settings": rna_simple_properties(constraint, excluded={"name", "type"}),
                "id_properties": id_properties(constraint),
            }
            for constraint in obj.constraints
        ],
        "modifiers": [modifier_record(modifier) for modifier in obj.modifiers],
    }
    for group_name in (
        "col_mesh_props",
        "collisionTools",
        "wmb_mesh",
        "wmb_mesh_props",
        "wmb_object",
        "lay_asset",
        "lay_instance",
    ):
        group = custom_property_group(obj, group_name)
        if group is not None:
            record[group_name] = group
    if obj.type == "MESH":
        record["mesh"] = mesh_record(obj.data)
    return record


def collection_parents():
    parents = defaultdict(list)
    for parent in bpy.data.collections:
        for child in parent.children:
            parents[child.name].append(parent.name)
    for scene in bpy.data.scenes:
        for child in scene.collection.children:
            parents[child.name].append(f"SCENE:{scene.name}")
    return parents


def collection_record(collection, parents):
    return {
        "name": collection.name,
        "parents": sorted(set(parents.get(collection.name, []))),
        "children": sorted(child.name for child in collection.children),
        "direct_objects": sorted(obj.name for obj in collection.objects),
        "direct_object_order": [obj.name for obj in collection.objects],
        "direct_object_count": len(collection.objects),
        "all_object_count": len(collection.all_objects),
        "hide_viewport": collection.hide_viewport,
        "hide_render": collection.hide_render,
        "color_tag": collection.color_tag,
        "instance_offset": plain(collection.instance_offset),
        "id_properties": id_properties(collection),
    }


def material_record(material):
    result = {
        "name": material.name,
        "users": material.users,
        "use_nodes": material.use_nodes,
        "blend_method": getattr(material, "surface_render_method", None),
        "id_properties": id_properties(material),
        "nodes": [],
        "images": [],
    }
    if material.use_nodes and material.node_tree:
        images = set()
        for node in material.node_tree.nodes:
            node_record = {
                "name": node.name,
                "label": node.label,
                "type": node.type,
                "bl_idname": node.bl_idname,
                "id_properties": id_properties(node),
            }
            if hasattr(node, "image") and node.image:
                node_record["image"] = node.image.name
                images.add(node.image.name)
            result["nodes"].append(node_record)
        result["images"] = sorted(images)
    return result


def analyze(filepath):
    bpy.ops.wm.open_mainfile(filepath=filepath)
    parents = collection_parents()
    objects = [object_record(obj) for obj in bpy.data.objects]
    collections = [collection_record(collection, parents) for collection in bpy.data.collections]

    object_types = Counter(obj["type"] for obj in objects)
    modifier_types = Counter(
        modifier["type"] for obj in objects for modifier in obj["modifiers"]
    )
    id_key_counts = Counter(
        key for obj in objects for key in obj["id_properties"].keys()
    )
    lod_values = Counter()
    for obj in objects:
        for key, value in obj["id_properties"].items():
            if "lod" in key.lower():
                lod_values[f"{key}={value}"] += 1

    shared_meshes = []
    for mesh in bpy.data.meshes:
        object_users = sorted(obj.name for obj in bpy.data.objects if obj.data == mesh)
        if len(object_users) > 1:
            shared_meshes.append({"mesh": mesh.name, "users": object_users})

    collection_instances = [
        {
            "object": obj["name"],
            "collection": obj["instance_collection"],
            "collections": obj["collections"],
            "id_properties": obj["id_properties"],
        }
        for obj in objects
        if obj["instance_collection"]
    ]

    term_hits = []
    terms = ("lod", "instance", "col", "lay", "wmb", "batch", "collision")
    for category, records in (("object", objects), ("collection", collections)):
        for record in records:
            haystack = json.dumps(record, ensure_ascii=False).lower()
            hits = sorted(term for term in terms if term in haystack)
            if hits:
                term_hits.append({"category": category, "name": record["name"], "terms": hits})

    return {
        "file": filepath,
        "file_size": os.path.getsize(filepath),
        "blender_version": bpy.app.version_string,
        "scenes": [
            {
                "name": scene.name,
                "root_children": sorted(child.name for child in scene.collection.children),
                "objects": len(scene.objects),
                "frame_start": scene.frame_start,
                "frame_end": scene.frame_end,
                "unit_settings": rna_simple_properties(scene.unit_settings),
                "id_properties": id_properties(scene),
            }
            for scene in bpy.data.scenes
        ],
        "view_layers": [
            {"name": layer.name, "objects": len(layer.objects), "id_properties": id_properties(layer)}
            for scene in bpy.data.scenes for layer in scene.view_layers
        ],
        "summary": {
            "objects": len(objects),
            "object_types": dict(sorted(object_types.items())),
            "collections": len(collections),
            "meshes": len(bpy.data.meshes),
            "materials": len(bpy.data.materials),
            "images": len(bpy.data.images),
            "node_groups": len(bpy.data.node_groups),
            "modifier_types": dict(sorted(modifier_types.items())),
            "object_id_property_keys": dict(sorted(id_key_counts.items())),
            "lod_property_values": dict(sorted(lod_values.items())),
            "collection_instances": len(collection_instances),
            "shared_mesh_datablocks": len(shared_meshes),
        },
        "collections": collections,
        "objects": objects,
        "shared_meshes": shared_meshes,
        "collection_instances": collection_instances,
        "materials": [material_record(mat) for mat in bpy.data.materials],
        "images": [
            {
                "name": image.name,
                "filepath": image.filepath,
                "filepath_raw": image.filepath_raw,
                "source": image.source,
                "size": list(image.size),
                "packed": image.packed_file is not None,
                "users": image.users,
                "id_properties": id_properties(image),
            }
            for image in bpy.data.images
        ],
        "node_groups": [
            {
                "name": group.name,
                "type": group.bl_idname,
                "users": group.users,
                "id_properties": id_properties(group),
                "nodes": [
                    {"name": node.name, "type": node.type, "bl_idname": node.bl_idname}
                    for node in group.nodes
                ],
            }
            for group in bpy.data.node_groups
        ],
        "term_hits": term_hits,
    }


def main():
    separator = "--"
    args = sys.argv[sys.argv.index(separator) + 1:] if separator in sys.argv else []
    if len(args) < 2:
        raise SystemExit("Usage: blender --background --python analyze_city_blends.py -- OUTPUT INPUT...")
    output = os.path.abspath(args[0])
    files = [os.path.abspath(path) for path in args[1:]]
    result = {"files": [analyze(path) for path in files]}
    with open(output, "w", encoding="utf-8", newline="\n") as stream:
        json.dump(result, stream, ensure_ascii=False, indent=2)
    for item in result["files"]:
        print("CITY_BLEND_SUMMARY", os.path.basename(item["file"]), json.dumps(item["summary"], ensure_ascii=False))
    print("CITY_BLEND_OUTPUT", output)


if __name__ == "__main__":
    main()

import os
from pathlib import Path

import bpy
from bpy.props import StringProperty
from bpy_extras.io_utils import ImportHelper

from ...consts import DAT_EXTENSIONS
from ...col.exporter.col_ui_manager import enableCollisionTools
from ...utils.visibilitySwitcher import enableVisibilitySelector
from ...utils.util import setExportFieldsFromImportFile


def _find_companion_archive(filepath, extension):
    source_path = Path(filepath)
    local_candidate = source_path.with_suffix(extension)
    if local_candidate.is_file():
        return local_candidate

    source_group = source_path.parent
    data_dir = source_group.parent
    cpk_root = data_dir.parent
    if not data_dir.name.lower().startswith('data') or not cpk_root.is_dir():
        return local_candidate

    for sibling_data_dir in sorted(cpk_root.iterdir(), key=lambda path: path.name.lower()):
        if not sibling_data_dir.is_dir() or not sibling_data_dir.name.lower().startswith('data'):
            continue
        candidate = sibling_data_dir / source_group.name / (source_path.stem + extension)
        if candidate.is_file():
            return candidate
    return local_candidate


def _world_shared_archive_pairs(filepath):
    source_path = Path(filepath)
    source_group = source_path.parent
    if not source_group.name.lower().startswith('wd') or source_group.name.lower() == 'wda':
        return []

    data_dir = source_group.parent
    cpk_root = data_dir.parent
    if not data_dir.name.lower().startswith('data') or not cpk_root.is_dir():
        return []

    companion_path = _find_companion_archive(filepath, '.dat')
    family_data_dirs = []
    for archive_path in (companion_path, source_path):
        archive_data_dir = archive_path.parent.parent
        if archive_data_dir.name.lower().startswith('data') and archive_data_dir not in family_data_dirs:
            family_data_dirs.append(archive_data_dir)

    shared_dat_by_stem = {}
    shared_dtt_by_stem = {}
    for family_data_dir in family_data_dirs:
        shared_dir = family_data_dir / 'wda'
        if not shared_dir.is_dir():
            continue
        for dat_path in sorted(shared_dir.glob('*.dat'), key=lambda path: path.name.lower()):
            shared_dat_by_stem.setdefault(dat_path.stem.lower(), dat_path)
        for dtt_path in sorted(shared_dir.glob('*.dtt'), key=lambda path: path.name.lower()):
            shared_dtt_by_stem.setdefault(dtt_path.stem.lower(), dtt_path)

    return [
        (shared_dat_by_stem[stem], shared_dtt_by_stem[stem])
        for stem in sorted(shared_dat_by_stem.keys() & shared_dtt_by_stem.keys())
    ]


def _has_all_extracted_textures(wta_path, texture_dir):
    from ...wta_wtp.importer.wta import WTA

    if not texture_dir.is_dir():
        return False
    with open(wta_path, 'rb') as wta_file:
        identifiers = WTA(wta_file).wtaTextureIdentifier
    return bool(identifiers) and all((texture_dir / (identifier + '.dds')).is_file() for identifier in identifiers)


def _prepare_world_shared_textures(filepath, dat_unpacker):
    from ...wta_wtp.importer import wtpImportOperator

    for dat_path, dtt_path in _world_shared_archive_pairs(filepath):
        dat_extract_dir = dat_path.parent / 'nier2blender_extracted' / (dat_path.stem + '.dat')
        dtt_extract_dir = dtt_path.parent / 'nier2blender_extracted' / (dtt_path.stem + '.dtt')
        wta_path = dat_extract_dir / (dat_path.stem + '.wta')
        wtp_path = dtt_extract_dir / (dtt_path.stem + '.wtp')
        texture_dir = dtt_extract_dir / 'textures'

        if not wta_path.is_file():
            dat_unpacker.main(str(dat_path), str(dat_extract_dir), str(dat_path))
        if not wtp_path.is_file():
            dat_unpacker.main(str(dtt_path), str(dtt_extract_dir), str(dtt_path))

        if not wta_path.is_file() or not wtp_path.is_file():
            print(f'WARNING: Shared world texture pair is incomplete: {dat_path.name} / {dtt_path.name}')
            continue
        if not _has_all_extracted_textures(wta_path, texture_dir):
            extracted_count = wtpImportOperator.extractFromWta(str(wta_path), str(wtp_path), str(texture_dir))
            print(f'[+] Extracted {extracted_count} shared world textures from {dtt_path.name}')
        else:
            print(f'[+] Reusing shared world textures from {texture_dir}')

def importDtt(only_extract, filepath, import_mesh_indices = False):
    head = os.path.split(filepath)[0]
    tail = os.path.split(filepath)[1]
    tailless_tail = tail[:-4]
    dat_filepath = str(_find_companion_archive(filepath, '.dat'))
    extract_dir = os.path.join(head, 'nier2blender_extracted')
    from . import dat_unpacker
    if os.path.isfile(dat_filepath):
        dat_unpacker.main(dat_filepath, os.path.join(extract_dir, tailless_tail + '.dat'), dat_filepath)   # dat
        if os.path.dirname(dat_filepath) != head:
            print(f'[+] Using split DAT companion: {dat_filepath}')
    else:
        print('DAT not found. Only extracting DTT. (No materials, collisions or layouts will automatically be imported)')

    extracted_files = dat_unpacker.main(filepath, os.path.join(extract_dir, tailless_tail + '.dtt'), filepath)       # dtt

    wmb_filepath = os.path.join(extract_dir, tailless_tail + '.dtt', tailless_tail + '.wmb')
    if not os.path.exists(wmb_filepath):
        wmb_filepath = os.path.join(extract_dir, tailless_tail + '.dat', tailless_tail + '.wmb')                     # if not in dtt, then must be in dat

    # WTA/WTP
    wtaPath = os.path.join(extract_dir, tailless_tail + '.dat', tailless_tail + '.wta')
    wtpPath = os.path.join(extract_dir, tailless_tail + '.dtt', tailless_tail + '.wtp')
    if os.path.isfile(wtaPath) and os.path.isfile(wtpPath):
        texturesExtractDir = os.path.join(extract_dir, tailless_tail + '.dtt', "textures")
        from ...wta_wtp.importer import wtpImportOperator
        wtpImportOperator.extractFromWta(wtaPath, wtpPath, texturesExtractDir)

    _prepare_world_shared_textures(filepath, dat_unpacker)

    if only_extract:
        return {'FINISHED'}

    setExportFieldsFromImportFile(filepath, True)
    enableVisibilitySelector()

    # WMB
    from ...wmb.importer import wmb_importer
    wmb_importer.main(only_extract, wmb_filepath, import_mesh_indices)

    # COL
    col_filepath = os.path.join(extract_dir, tailless_tail + '.dat', tailless_tail + '.col')
    if os.path.isfile(col_filepath):
        from ...col.importer import col_importer
        col_importer.main(col_filepath)
        enableCollisionTools()

    # LAY
    lay_filepath = os.path.join(extract_dir, tailless_tail + '.dat', 'Layout.lay')
    if os.path.isfile(lay_filepath):
        from ...lay.importer import lay_importer
        lay_importer.main(lay_filepath)

    return {'FINISHED'}

class ImportNierDtt(bpy.types.Operator, ImportHelper):
    '''Load a Nier:Automata DTT (and DAT) File.'''
    bl_idname = "import_scene.dtt_data"
    bl_label = "Import DTT (and DAT) Data"
    bl_options = {'PRESET'}
    filename_ext = ".dtt"
    filter_glob: StringProperty(default="*.dtt", options={'HIDDEN'})

    reset_blend: bpy.props.BoolProperty(name="Reset Blender Scene on Import", default=True)
    bulk_import: bpy.props.BoolProperty(name="Bulk Import All DTT/DATs In Folder (Experimental)", default=False)
    only_extract: bpy.props.BoolProperty(name="Only Extract DTT/DAT Contents. (Experimental)", default=False)
    import_mesh_indices: bpy.props.BoolProperty(name="Import Mesh Group Indices (Bayonetta 3)", default=False)

    def execute(self, context):
        from ...wmb.importer import wmb_importer
        if self.reset_blend and not self.only_extract:
            wmb_importer.reset_blend()
        if self.bulk_import:
            folder = os.path.split(self.filepath)[0]
            for filename in os.listdir(folder):
                if filename[-4:] == '.dtt':
                    try:
                        filepath = os.path.join(folder, filename)
                        importDtt(self.only_extract, filepath, self.import_mesh_indices)
                    except:
                        print('ERROR: FAILED TO IMPORT', filename)
            return {'FINISHED'}

        else:
            return importDtt(self.only_extract, self.filepath, self.import_mesh_indices)

class ImportNierDat(bpy.types.Operator, ImportHelper):
    '''Load a Nier:Automata DAT File.'''
    bl_idname = "import_scene.dat_data"
    bl_label = "Import DAT Data"
    bl_options = {'PRESET'}
    filename_ext = ".dat"
    filter_glob: StringProperty(default=";".join([f"*{ext}" for ext in DAT_EXTENSIONS]), options={'HIDDEN'})

    reset_blend: bpy.props.BoolProperty(name="Reset Blender Scene on Import", default=True)
    bulk_import: bpy.props.BoolProperty(name="Bulk Import All DTT/DATs In Folder (Experimental)", default=False)
    only_extract: bpy.props.BoolProperty(name="Only Extract DTT/DAT Contents. (Experimental)", default=False)

    def doImport(self, onlyExtract, filepath):
        head = os.path.split(filepath)[0]
        tail = os.path.split(filepath)[1]
        ext = tail[-4:]
        tailless_tail = tail[:-4]
        dat_filepath = os.path.join(head, tailless_tail + ext)
        extract_dir = os.path.join(head, 'nier2blender_extracted')
        from . import dat_unpacker
        if os.path.isfile(dat_filepath):
            dat_unpacker.main(dat_filepath, os.path.join(extract_dir, tailless_tail + ext), dat_filepath)   # dat

        if onlyExtract:
            return {'FINISHED'}

        setExportFieldsFromImportFile(filepath, True)

        # COL
        col_filepath = os.path.join(extract_dir, tailless_tail + '.dat', tailless_tail + '.col')
        if os.path.isfile(col_filepath):
            from ...col.importer import col_importer
            col_importer.main(col_filepath)
            enableCollisionTools()

        # LAY
        lay_filepath = os.path.join(extract_dir, tailless_tail + '.dat', 'Layout.lay')
        if os.path.isfile(lay_filepath):
            from ...lay.importer import lay_importer
            lay_importer.main(lay_filepath)

        return {'FINISHED'}

    def execute(self, context):
        from ...wmb.importer import wmb_importer
        if self.reset_blend and not self.only_extract:
            wmb_importer.reset_blend()
        if self.bulk_import:
            folder = os.path.split(self.filepath)[0]
            for filename in os.listdir(folder):
                if filename[-4:] in DAT_EXTENSIONS:
                    try:
                        filepath = os.path.join(folder, filename)
                        self.doImport(self.only_extract, filepath)
                    except:
                        print('ERROR: FAILED TO IMPORT', filename)
            return {'FINISHED'}

        else:
            return self.doImport(self.only_extract, self.filepath)

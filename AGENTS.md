# Direct11_Nier_Automata Agent Rules

## Project Identity

- Repository: `C:\Users\a9018\Desktop\Direct11_Nier_Automata`
- Solution: `NieRAutomata.sln`
- Vault root: `C:\Users\a9018\Desktop\Project_Vault`
- Vault project hub: `AI-Sessions/wiki/projects/Direct11_Nier_Automata.md`
- External resource workspace: `D:\NierRes`
- Resource research notebook: `https://notebook.google.com/notebook/88617913-333a-42b4-82bb-6eeb6bac60ea`

This repository uses the Project Vault as its durable project memory. This file defines repository-local entry and safety rules; the Vault root `AGENTS.md` remains the source of truth for Vault routing, save, index, log, and lint rules.

## Mandatory Entry Flow

Follow the Vault root AGENTS.md Context Entry and Reuse policy: first project entry restores this file, Git state, the hub and relevant evidence; same-task follow-ups reuse applicable context and recheck affected changes only. Check descendant AGENTS for selected source paths. Missing project registration still requires clarification.

## Repository Scope

- `Engine/`: DirectX 11 engine runtime and shared systems
- `Client/`: game/client implementation
- `Editor/`: editor application
- `Launcher/`: launcher application
- `ModelConverter/`: model conversion tool
- `EngineSDK/`: exported engine headers and libraries
- `ProjectSetting/`: project-wide settings and Editor-authored project assets
  - `ProjectSetting/Scene/`: Scene/Level hierarchy documents
  - `ProjectSetting/Prefab/`: Prefab documents
  - future Editor-authored asset types use their own `ProjectSetting/<AssetType>/` folder when a real writer/reader exists
  - Editor-only project UI/layout settings use `ProjectSetting/Editor/`; do not mix them with runtime Scene/Prefab documents

Do not treat generated files, copied SDK outputs, project data, or external resources as ordinary source code without first checking their ownership and generation flow.

## Build and Verification

- Solution entry: `NieRAutomata.sln`. VS2022 Debug x64 command:
  `MSBuild.exe NieRAutomata.sln /t:Engine,Client,Editor,Launcher /p:Configuration=Debug /p:Platform=x64 /m /v:minimal`
- For Codex build validation, request elevated execution for this solution-level command on the first attempt. The sandbox previously denied access to the installed Windows SDK; the same command succeeded after approval. Keep the request limited to this build command and repository working directory, and retain normal approval review. Do not retry the known failing sandbox build first or broaden approval to unrelated commands.
- Direct Engine/default/Engine.vcxproj or Client/default/Client.vcxproj builds are unsupported because their steps require `$(SolutionDir)`.
- Do not claim current build success until the exact command/configuration/platform succeeds. Keep validation evidence in the active Vault task; update this file only when the supported build procedure changes.
- The latest recorded AnimationPreset build/runtime checks passed on 2026-09-10, but actual game-view motion/input and MRT/offscreen resize remain pending; no playable-slice completion claim.
- Authoring gates can rewrite product Scene/Prefab data. Read the active task's failure/validation section before selecting regression commands. Preserve GUIDs and non-target authored properties.
- Do not use plXXXX.fbx for routine ModelConverter regression: conversion may exceed 20 minutes. Use small weapon fixtures; full resources require the requested full-resource scope.
- Historical validation and pre-refactor instructions are preserved in the project's `handoffs/컨텍스트_개편_이전_진입문서_2026-09-10.md` in the Vault.

## Safety and Existing State

- Preserve the existing deletion of `.vscode/settings.json` unless the user explicitly asks to restore or replace it.
- Do not overwrite or modify original CPK data under `D:\NierRes\data`.
- Keep large extracted resources and generated extraction output out of this repository and the Vault.
- Prefer existing tools under `D:\NierRes` before proposing downloads or installations.
- Do not edit generated RTTR files or `EngineSDK` copies until their source/generation relationship is identified.

## NotebookLM Resource Reference

- Use the project resource notebook through the NotebookLM MCP when resource work requires discovering, identifying, or validating information that is not already available from explicit user-provided paths or locally verifiable files.
- Routine resource operations with known inputs and known procedures do not require a NotebookLM query. Use it as a discovery and reference aid, not as a mandatory gate for every resource task.
- Use notebook ID `direct11-nier-automata`, registered from `https://notebook.google.com/notebook/88617913-333a-42b4-82bb-6eeb6bac60ea`.
- Treat NotebookLM answers as reference context, not as a replacement for checking repository source, extracted files, tool output, and build or runtime evidence.
- If a needed NotebookLM lookup is unavailable or unauthenticated, report that limitation and continue with locally verifiable evidence unless the missing reference context is required for a safe decision.

## Architecture Change Discipline

- This project is primarily an architecture refactoring project. Prefer correcting or extending existing types and functions so their current owners enforce the required behavior.
- Adding a variable or function is allowed when a concrete invariant or caller requires it; this is not a blanket prohibition. Reuse or extend the existing API first, and keep any addition local to the owner whose responsibility it completes.
- Optimize the existing `Game` Singleton, manager lifecycle, `GameObject`, `ObjectManager`, `PrototypeManager`, Serializer, and template API boundaries before adding architecture.
- Do not interpret “expand when needed” as standing permission to add classes, services, contexts, registries, keys, transactions, or adapters. Future extensibility alone is not sufficient evidence.
- When a plan does not prescribe the implementation, inspect the current code and choose the smallest change that satisfies the requirement and preserves ownership.
- Before adding a new architectural unit, identify the current bug or caller, why the existing owner cannot safely handle it, the new owner and lifetime, the path it replaces, and the verification method. Do not add it when net complexity is not justified.
- The durable project rule is `AI-Sessions/wiki/projects/Direct11_Nier_Automata/rules/기존_구조_우선_설계_규칙.md` in the Project Vault.

## Durable Project Memory

Save only reusable decisions, failed approaches, risks, architecture notes, task summaries, handoffs, and project-specific rules that pass the Vault save filter.

Route project-owned pages under:

`AI-Sessions/wiki/projects/Direct11_Nier_Automata/`

Use its `dev-tasks/`, `design/`, `decisions/`, `errors/`, `handoffs/`, `rules/`, and `sources/` subfolders as defined by the Vault root rules. After meaningful Vault work, update the project hub, `index.md`, and append to `log.md` when required.

## Text Encoding

Treat maintained Markdown, configuration, source, and plain-text files as UTF-8 unless the format requires otherwise. Preserve existing BOM and newline style. After editing Korean text, re-read the affected content and verify filenames, frontmatter, and Wiki links remain intact.

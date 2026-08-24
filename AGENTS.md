# Direct11_Nier_Automata Agent Rules

## Project Identity

- Repository: `C:\Users\a9018\Desktop\Direct11_Nier_Automata`
- Solution: `NieRAutomata.sln`
- Vault root: `C:\Users\a9018\Desktop\Project_Vault`
- Vault project hub: `AI-Sessions/wiki/projects/Direct11_Nier_Automata.md`
- External resource workspace: `D:\NierRes`

This repository uses the Project Vault as its durable project memory. This file defines repository-local entry and safety rules; the Vault root `AGENTS.md` remains the source of truth for Vault routing, save, index, log, and lint rules.

## Mandatory Entry Flow

Before any project-specific analysis, recommendation, implementation plan, code edit, build, verification, or review:

1. Run `git status --short` and preserve unrelated user changes.
2. Read this file.
3. Read the Vault root `AGENTS.md`, `index.md`, and `log.md`.
4. Read the Vault project hub `AI-Sessions/wiki/projects/Direct11_Nier_Automata.md`.
5. Search the hub, recent relevant log entries, and `AI-Sessions/wiki/projects/Direct11_Nier_Automata/` for request keywords, aliases, affected systems, and Wiki links.
6. Read relevant decisions, errors, design notes, dev-task summaries, handoffs, and project rules before acting.
7. Report the preflight files and searches for non-trivial work or code changes.

If the Vault root or project hub is missing, unreadable, or ambiguous, stop before substantive project work and ask the user.

## Repository Scope

- `Engine/`: DirectX 11 engine runtime and shared systems
- `Client/`: game/client implementation
- `Editor/`: editor application
- `Launcher/`: launcher application
- `ModelConverter/`: model conversion tool
- `EngineSDK/`: exported engine headers and libraries
- `ProjectSetting/`: project and scene data

Do not treat generated files, copied SDK outputs, project data, or external resources as ordinary source code without first checking their ownership and generation flow.

## Build and Verification

- The solution entry point is `NieRAutomata.sln`.
- Verified on 2026-08-24 with Visual Studio 2022, Debug|x64:
  - Engine: `MSBuild.exe NieRAutomata.sln /t:Engine /p:Configuration=Debug /p:Platform=x64 /m /v:minimal`
  - Client (after Engine): `MSBuild.exe Client\default\Client.vcxproj /p:Configuration=Debug /p:Platform=x64 /m /v:minimal`
- Building `Engine\default\Engine.vcxproj` directly is unsupported because its pre-build event requires the solution-level `$(SolutionDir)` value.
- Do not claim build success until the exact Visual Studio/MSBuild command, configuration, and platform have run successfully.
- When a build command is verified, record it in both this file and the Vault project hub.

## Safety and Existing State

- Preserve the existing deletion of `.vscode/settings.json` unless the user explicitly asks to restore or replace it.
- Do not overwrite or modify original CPK data under `D:\NierRes\data`.
- Keep large extracted resources and generated extraction output out of this repository and the Vault.
- Prefer existing tools under `D:\NierRes` before proposing downloads or installations.
- Do not edit generated RTTR files or `EngineSDK` copies until their source/generation relationship is identified.

## Durable Project Memory

Save only reusable decisions, failed approaches, risks, architecture notes, task summaries, handoffs, and project-specific rules that pass the Vault save filter.

Route project-owned pages under:

`AI-Sessions/wiki/projects/Direct11_Nier_Automata/`

Use its `dev-tasks/`, `design/`, `decisions/`, `errors/`, `handoffs/`, `rules/`, and `sources/` subfolders as defined by the Vault root rules. After meaningful Vault work, update the project hub, `index.md`, and append to `log.md` when required.

## Text Encoding

Treat maintained Markdown, configuration, source, and plain-text files as UTF-8 unless the format requires otherwise. Preserve existing BOM and newline style. After editing Korean text, re-read the affected content and verify filenames, frontmatter, and Wiki links remain intact.

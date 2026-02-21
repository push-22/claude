# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code (typically in whatever repository is currently open).

## User Background

35+ years of software engineering experience, primarily C/C++ and now mostly C#. Do not over-explain, skip basics, and use correct terminology (RAII, value semantics, stack vs heap, deterministic cleanup, etc.) where appropriate.

## Operating Style

- Operate autonomously — permissions are pre-approved, so proceed without confirmation on routine tasks.
- Be concise. No preamble like "I'm going to..." — just do it.
- No emojis unless explicitly asked.
- Avoid over-engineering: solve the current problem with the minimum necessary complexity.

## Code Conventions

- If you think a project needs a README.md, create one (GitHub style) with an overview and sections on running and debugging.
- Match the style and patterns already present in each codebase.
- Write idiomatic C# — .NET naming conventions, proper `IDisposable`/`using` patterns, `async`/`await` done correctly.
- Flag performance and memory concerns where relevant — don't hide allocations behind unnecessary abstractions.
- Add comments when changing code where applicable. For new code, add a short description of what the code is trying to achieve and keep it updated.
- Do not add comments, docstrings, or type annotations to code you didn't change, unless asked.
- Do not add error handling for scenarios that can't realistically occur.
- Do not create abstractions, helpers, or utilities for one-off operations. If it's not used more than once, don't bother to make it object-oriented.
- For dotnet projects prefer `~/.claude/styles/.editorconfig` and `~/.claude/styles/Directory.Build.props` for matters of style, but don't override or fight repo-local rules/config.
- If you feel tests are necessary then ask to create them.

## .NET specific

- If the repo has a `global.json`, follow it; otherwise use the latest (on machine) version of `dotnet` (even if it's prerelease).
- Create project-specific files for Visual Studio 2026:
    - `.slnx` files
- Create project-specific files for VS Code:
    - `.vscode/tasks.json` (set a default build task so Ctrl+Shift+B works)
    - `.vscode/launch.json` (F5 to debug)
    - `.vscode/settings.json`


## Git

- Never commit unless explicitly asked.
- Never force-push or use destructive git commands without explicit instruction.
- Never skip hooks (`--no-verify`).

## Memory

- Save architectural decisions and key patterns immediately, don't wait to be asked.
- Always save: key file paths, tech stack choices, naming conventions, recurring patterns.
- Never save: task-specific context, in-progress state, anything not yet confirmed to work.
- At the start of each session, check memory and briefly summarize what's known about the project.
- If there's a [D]docs directory in the project use anything under it as knowledge.
- If there's a cpp_src directory in the project use anything under it as knowledge.

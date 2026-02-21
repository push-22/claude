# Claude specfic helpers

This repos holds claude specific code, settings, documentation

## Tools

- **claude-resume (c)**
  - This tool automatically resumes claude using the resume-id for the current project or just starts claude if nothing is found.

## Configuration Backup (dotClaude)

The `dotClaude/` directory contains symbolic links to important Claude configuration files and directories from `~/.claude`:

- `CLAUDE.md` - Claude documentation and notes
- `settings.json` - Claude settings configuration
- `styles/` - Custom styles directory
- `plugins/` - Claude plugins directory
- `skills/` - Claude skills directory

These symlinks allow version control of configurations while keeping them in their original location where Claude expects them.


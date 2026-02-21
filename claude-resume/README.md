# claude-resume

Finds the most recent Claude Code session for the current directory and runs `claude --resume <session_id>` automatically.

Claude stores sessions as JSONL files under `~/.claude/projects/<munged_path>/`. This tool locates the newest one and launches it without you needing to copy/paste session IDs.

## Building

### macOS / Linux

```bash
make
sudo make install   # copies to /usr/local/bin
```

### Windows (MSVC)

```cmd
cl /O2 /W3 claude-resume.c /Fe:claude-resume.exe
```

### Windows (MinGW / gcc)

```bash
gcc -std=c11 -O2 -Wall -o claude-resume.exe claude-resume.c
```

## Usage

Run from the project directory you want to resume:

```bash
claude-resume
```

To verify the session directory it's searching (useful if nothing is found):

```bash
claude-resume --show-path
```

## Windows path format

Claude's project directory name is derived from the current working path. On Windows the expected format is `C--Users-foo-bar` (colon dropped, backslashes replaced with dashes). If `--show-path` shows a directory that doesn't exist, check what directories are actually present under `%USERPROFILE%\.claude\projects\` and adjust `munge_path()` in `claude-resume.c` accordingly.

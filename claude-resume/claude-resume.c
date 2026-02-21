/*
 * claude-resume: find the most recent Claude Code session for the current
 * directory and exec `claude --resume <session_id>`.
 *
 * Claude stores sessions as JSONL files under:
 *   Unix:    $HOME/.claude/projects/<munged_path>/
 *   Windows: %USERPROFILE%\.claude\projects\<munged_path>\
 *
 * Path munging:
 *   Unix:    /Users/foo/bar  ->  -Users-foo-bar   (/ becomes -)
 *   Windows: C:\Users\foo    ->  C--Users-foo     (: dropped, \ becomes -)
 *
 * If the Windows path format differs on your machine, run with --show-path
 * to see what directory is being searched, then adjust munge_path() below.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <direct.h>
#  include <process.h>
#  ifndef PATH_MAX
#    define PATH_MAX 4096
#  endif
static char *get_home(void)                    { return getenv("USERPROFILE"); }
static char *get_cwd_buf(char *buf, size_t n)  { return _getcwd(buf, (int)n); }
#else
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
static char *get_home(void)                    { return getenv("HOME"); }
static char *get_cwd_buf(char *buf, size_t n)  { return getcwd(buf, n); }
#endif

/* Convert a filesystem path to Claude's project directory name. */
static void munge_path(const char *path, char *out, size_t out_size)
{
    size_t j = 0;
    for (size_t i = 0; path[i] && j < out_size - 1; i++) {
#ifdef _WIN32
        if      (path[i] == '\\') out[j++] = '-';
        else if (path[i] == ':')  { /* drop colon after drive letter */ }
        else                      out[j++] = path[i];
#else
        out[j++] = (path[i] == '/') ? '-' : path[i];
#endif
    }
    out[j] = '\0';
}

/* Find the most recently modified *.jsonl in dir, write basename to session_id. */
static int find_newest_session(const char *dir, char *session_id, size_t id_size)
{
#ifdef _WIN32
    char pattern[PATH_MAX];
    snprintf(pattern, sizeof(pattern), "%s\\*.jsonl", dir);

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return 0;

    FILETIME best = {0, 0};
    char best_name[MAX_PATH] = {0};

    do {
        if (CompareFileTime(&fd.ftLastWriteTime, &best) > 0) {
            best = fd.ftLastWriteTime;
            strncpy(best_name, fd.cFileName, sizeof(best_name) - 1);
        }
    } while (FindNextFileA(h, &fd));
    FindClose(h);

    if (!best_name[0]) return 0;
    char *dot = strrchr(best_name, '.');
    if (dot) *dot = '\0';
    strncpy(session_id, best_name, id_size - 1);
    session_id[id_size - 1] = '\0';
    return 1;

#else
    DIR *d = opendir(dir);
    if (!d) return 0;

    time_t best_time = 0;
    char best_name[PATH_MAX] = {0};
    struct dirent *entry;

    while ((entry = readdir(d)) != NULL) {
        size_t len = strlen(entry->d_name);
        if (len < 6 || strcmp(entry->d_name + len - 6, ".jsonl") != 0)
            continue;

        char full[PATH_MAX];
        snprintf(full, sizeof(full), "%s/%s", dir, entry->d_name);

        struct stat st;
        if (stat(full, &st) != 0) continue;

        if (st.st_mtime > best_time) {
            best_time = st.st_mtime;
            strncpy(best_name, entry->d_name, sizeof(best_name) - 1);
        }
    }
    closedir(d);

    if (!best_name[0]) return 0;
    char *dot = strrchr(best_name, '.');
    if (dot) *dot = '\0';
    strncpy(session_id, best_name, id_size - 1);
    session_id[id_size - 1] = '\0';
    return 1;
#endif
}

int main(int argc, char *argv[])
{
    int show_path = (argc > 1 && strcmp(argv[1], "--show-path") == 0);

    char *home = get_home();
    if (!home) { fprintf(stderr, "Cannot determine home directory\n"); return 1; }

    char cwd[PATH_MAX];
    if (!get_cwd_buf(cwd, sizeof(cwd))) {
        fprintf(stderr, "Cannot get current directory\n");
        return 1;
    }

    char munged[PATH_MAX];
    munge_path(cwd, munged, sizeof(munged));

    char project_dir[PATH_MAX];
#ifdef _WIN32
    snprintf(project_dir, sizeof(project_dir), "%s\\.claude\\projects\\%s", home, munged);
#else
    snprintf(project_dir, sizeof(project_dir), "%s/.claude/projects/%s", home, munged);
#endif

    if (show_path) {
        printf("Looking in: %s\n", project_dir);
        return 0;
    }

    char session_id[256];
    const char *args[4];
    if (find_newest_session(project_dir, session_id, sizeof(session_id))) {
        args[0] = "claude"; args[1] = "--resume"; args[2] = session_id; args[3] = NULL;
    } else {
        /* No prior session — start fresh. */
        args[0] = "claude"; args[1] = NULL;
    }

#ifdef _WIN32
    return _execvp("claude", args);
#else
    execvp("claude", (char *const *)args);
    perror("execvp");
    return 1;
#endif
}

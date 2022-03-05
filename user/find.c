#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// copied from grep.c for matching regular expression
int matchhere(char *, char *);
int matchstar(int, char *, char *);

int match(char *re, char *text)
{
    if (re[0] == '^')
        return matchhere(re + 1, text);
    do
    { // must look at empty string
        if (matchhere(re, text))
            return 1;
    } while (*text++ != '\0');
    return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
    if (re[0] == '\0')
        return 1;
    if (re[1] == '*')
        return matchstar(re[0], re + 2, text);
    if (re[0] == '$' && re[1] == '\0')
        return *text == '\0';
    if (*text != '\0' && (re[0] == '.' || re[0] == *text))
        return matchhere(re + 1, text + 1);
    return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
    do
    { // a * matches zero or more instances
        if (matchhere(re, text))
            return 1;
    } while (*text != '\0' && (*text++ == c || c == '.'));
    return 0;
}

// copied from ls.c to get filename
char *fmtname(char *path)
{
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    buf[strlen(p)] = 0; // mark the ending
    return buf;
}

void find_file(char *dir, char *file)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(dir, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", dir);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", dir);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_FILE:
        printf("Error: %s is not a directory\n", dir);
        break;

    case T_DIR:
        if (strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("ls: dir too long\n");
            break;
        }
        strcpy(buf, dir);
        p = buf + strlen(buf);
        *p++ = '/';
        // buf = "dir/"
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {

            if (de.inum == 0)
                continue;
            if ((strcmp(de.name, ".") == 0) || (strcmp(de.name, "..") == 0))
                continue;

            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)
            {
                printf("find: cannot stat %s\n", buf);
                continue;
            }

            if (match(file, fmtname(buf)) && (st.type == T_FILE))
            {
                printf("%s\n", buf);
            }

            if (st.type == T_DIR)
            {
                find_file(buf, file);
            }
        }
        break;
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(2, "Usage: find diectory_to_start file... \n");
        exit(1);
    }

    char *dir = argv[1];
    char *file = argv[2];
    find_file(dir, file);
    exit(0);
}
#define _POSIX_C_SOURCE 200809L
#include <limits.h> 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

// 39-bit VA, 3-level page table, 9-bit VPN fields
#define PAGE_SHIFT  12
#define PAGE_SIZE   (1UL << PAGE_SHIFT)
#define VPN_MASK    0x1FFul

#define PGD_IDX(va)  (((uint64_t)(va) >> 30) & VPN_MASK)
#define PMD_IDX(va)  (((uint64_t)(va) >> 21) & VPN_MASK)
#define PTE_IDX(va)  (((uint64_t)(va) >> 12) & VPN_MASK)

// PGD ~ 2^30 = 1 GiB; PMD ~ 2^21 = 2 MiB
#define PGD_COVER   (1UL << 30)
#define PMD_COVER   (1UL << 21)

// /proc/PID/pagemap layout
#define PM_PRESENT    (1ULL << 63)
#define PM_SWAPPED    (1ULL << 62)
#define PM_FILE       (1ULL << 61)
#define PM_EXCL_MAP   (1ULL << 56)
#define PM_SOFT_DIRTY (1ULL << 55)
#define PM_PFN_MASK   ((1ULL << 55) - 1)

struct page_rec {
    uint64_t va;
    uint64_t pm;        // raw pagemap entry
    char     perm[8];
    char     name[64];  // VMA label
};

static struct page_rec *pages;
static size_t n_pages, cap_pages;

static void page_add(uint64_t va, uint64_t pm,
                     const char *perm, const char *name)
{
    if (n_pages == cap_pages) {
        cap_pages = cap_pages ? cap_pages * 2 : 4096;
        pages = realloc(pages, cap_pages * sizeof *pages);
        if (!pages) { perror("realloc"); exit(1); }
    }

    struct page_rec *r = &pages[n_pages++];
    r->va = va;
    r->pm = pm;

    strncpy(r->perm, perm, sizeof r->perm - 1);
    r->perm[sizeof r->perm - 1] = '\0';

    strncpy(r->name, name, sizeof r->name - 1);
    r->name[sizeof r->name - 1] = '\0';
}

static int collect(int pid)
{
    char path[PATH_MAX];
    FILE *maps;
    int pmfd;
    char line[512];

    snprintf(path, sizeof path, "/proc/%d/maps", pid);
    maps = fopen(path, "r");
    if (!maps) { perror(path); return -1; }

    snprintf(path, sizeof path, "/proc/%d/pagemap", pid);
    pmfd = open(path, O_RDONLY);
    if (pmfd < 0) { perror(path); fclose(maps); return -1; }

    while (fgets(line, sizeof line, maps)) {
        uint64_t lo, hi, offset, ino;
        unsigned maj, min;
        char perm[8], rawname[256] = "";

        int r = sscanf(line, "%lx-%lx %7s %lx %x:%x %lu %255[^\n]",
                       &lo, &hi, perm, &offset, &maj, &min, &ino, rawname);
        if (r < 3) continue;

        const char *nm = rawname;
        while (*nm == ' ' || *nm == '\t') ++nm;

        for (uint64_t va = lo; va < hi; va += PAGE_SIZE) {
            uint64_t pm = 0;
            off_t off = (off_t)(va >> PAGE_SHIFT) * (off_t)sizeof pm;
            if (pread(pmfd, &pm, sizeof pm, off) != (ssize_t)sizeof pm)
                continue;

            if (pm & PM_PRESENT)
                page_add(va, pm, perm, nm);
        }
    }

    close(pmfd);
    fclose(maps);
    return 0;
}

// drawings
#define BR_CONT  "|-- "
#define BR_LAST  "`-- "
#define PFX_CONT "|   "
#define PFX_LAST "    "
#define SEPARATING_LINE "=============================================================================="

static void fmt_flags(char *buf, size_t n, uint64_t pm)
{
    size_t p = 0;
    p += (size_t)snprintf(buf + p, n - p, "pfn=0x%lx",
                          (unsigned long)(pm & PM_PFN_MASK));
    if ((pm & PM_SOFT_DIRTY) && p < n)
        p += (size_t)snprintf(buf + p, n - p, " soft-dirty");

    if ((pm & PM_FILE) && p < n)
        p += (size_t)snprintf(buf + p, n - p, " file");

    if ((pm & PM_SWAPPED) && p < n)
        p += (size_t)snprintf(buf + p, n - p, " swapped");
}

static void print_ptes(size_t start, size_t end, const char *pfx)
{
    size_t i = start;
    while (i < end) {
        size_t j = i + 1;
        while (j < end &&
               pages[j].va == pages[j-1].va + PAGE_SIZE &&
               strcmp(pages[j].perm, pages[i].perm) == 0 &&
               strcmp(pages[j].name, pages[i].name) == 0)
            ++j;

        int last = (j == end);
        const char *br = last ? BR_LAST : BR_CONT;
        size_t cnt = j - i;

        if (cnt == 1) {
            char flags[128];
            fmt_flags(flags, sizeof flags, pages[i].pm);
            printf("%s%sPTE[0x%03lx] 0x%016lx  [%s]  %s",
                   pfx, br,
                   (unsigned long)PTE_IDX(pages[i].va),
                   (unsigned long)pages[i].va,
                   pages[i].perm, flags);
        } else {
            printf("%s%sPTE[0x%03lx..0x%03lx] 0x%016lx-0x%016lx  [%s]  %zu pages",
                   pfx, br,
                   (unsigned long)PTE_IDX(pages[i].va),
                   (unsigned long)PTE_IDX(pages[j-1].va),
                   (unsigned long)pages[i].va,
                   (unsigned long)(pages[j-1].va + PAGE_SIZE - 1),
                   pages[i].perm, cnt);
        }
        if (pages[i].name[0])
            printf("  %s", pages[i].name);
        printf("\n");

        i = j;
    }
}

static void print_pmds(size_t start, size_t end)
{
    size_t i = start;
    while (i < end) {
        uint64_t pmd = PMD_IDX(pages[i].va);
        size_t j = i;
        while (j < end && PMD_IDX(pages[j].va) == pmd)
            ++j;

        int last = (j == end);
        const char *br  = last ? BR_LAST  : BR_CONT;
        const char *pfx = last ? PFX_LAST : PFX_CONT;

        printf("%sPMD[0x%03lx] 0x%016lx-0x%016lx  (%zu pages)\n",
               br, (unsigned long)pmd,
               (unsigned long)pages[i].va,
               (unsigned long)(pages[j-1].va + PAGE_SIZE - 1),
               j - i);

        print_ptes(i, j, pfx);
        i = j;
    }
}

static void print_tree(int pid)
{
    static const char sep[] = SEPARATING_LINE;

    printf("%s\n", sep);
    printf("  pagetree: PID %-6d  (RISC-V SV39, 3-level: PGD -> PMD -> PTE)\n",
           pid);
    printf("%s\n\n", sep);

    size_t i = 0;
    while (i < n_pages) {
        uint64_t pgd = PGD_IDX(pages[i].va);
        size_t j = i;
        while (j < n_pages && PGD_IDX(pages[j].va) == pgd)
            ++j;

        printf("PGD[0x%03lx]  slot 0x%lx-0x%lx  present: %zu pages  (%zu KiB)\n",
               (unsigned long)pgd,
               (unsigned long)(pgd * PGD_COVER),
               (unsigned long)(pgd * PGD_COVER + PGD_COVER - 1),
               j - i,
               (j - i) * (PAGE_SIZE / 1024));

        print_pmds(i, j);
        printf("\n");
        i = j;
    }

    printf("%s\n", sep);
    printf("  Total: %zu present pages  (%zu KiB)\n",
           n_pages, n_pages * (PAGE_SIZE / 1024));
    printf("%s\n", sep);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        fprintf(stderr, "       %s self\n",  argv[0]);
        return 1;
    }

    int pid;
    if (strcmp(argv[1], "self") == 0) {
        pid = (int)getpid();
    } else {
        errno = 0;
        long v = strtol(argv[1], NULL, 10);
        if (errno || v <= 0) {
            fprintf(stderr, "%s: invalid pid '%s'\n", argv[0], argv[1]);
            return 1;
        }

        pid = (int)v;
    }

    if (collect(pid) < 0)
        return 1;

    if (n_pages == 0) {
        fprintf(stderr,
                "No present pages found for PID %d.\n"
                "run as root, or check that the process exists.\n",
                pid);

        return 1;
    }

    print_tree(pid);
    free(pages);

    return 0;
}

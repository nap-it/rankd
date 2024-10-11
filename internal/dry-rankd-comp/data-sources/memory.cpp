#include "data-sources/memory.h"

static unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
static std::mt19937 range(seed);
static std::uniform_int_distribution<std::mt19937::result_type> full_randomizer(0, RAND_MAX);

Memory::Memory() {
    // Initialize all the variables for future snapping.
    _total = full_randomizer(range) % 64000 + 1024;                 // Total memory (1GB to 64GB in KB)
    _free = full_randomizer(range) % (_total.value() / 4);          // Free memory (up to 25% of total)
    _available = _free.value() + full_randomizer(range) % 10000;    // Available memory (free + some buffer)
    _buffers = full_randomizer(range) % 10000;                      // Buffers (up to 10MB in KB)
    _cached = full_randomizer(range) % 20000;                       // Cached memory (up to 20MB in KB)
    _swap_cached = full_randomizer(range) % 5000;                   // Swap cached (up to 5MB in KB)
    _active = full_randomizer(range) % (_total.value() / 2);        // Active memory (up to 50% of total)
    _inactive = full_randomizer(range) % (_total.value() / 4);      // Inactive memory (up to 25% of total)
    _active_anonymous = full_randomizer(range) % 5000;              // Active anonymous pages (up to 5MB)
    _inactive_anonymous = full_randomizer(range) % 5000;            // Inactive anonymous pages (up to 5MB)
    _active_file = full_randomizer(range) % 20000;                  // Active file (up to 20MB)
    _inactive_file = full_randomizer(range) % 10000;                // Inactive file (up to 10MB)
    _unevictable = full_randomizer(range) % 1000;                   // Unevictable memory (up to 1MB)
    _locked = full_randomizer(range) % 1000;                        // Locked memory (up to 1MB)
    _swap_total = full_randomizer(range) % 32000 + 1024;            // Swap total (1GB to 32GB in KB)
    _swap_free = full_randomizer(range) % _swap_total.value();      // Swap free (up to total swap)
    _dirty = full_randomizer(range) % 500;                          // Dirty pages (up to 500KB)
    _writeback = full_randomizer(range) % 100;                      // Writeback (up to 100KB)
    _anonymous_pages = full_randomizer(range) % 20000;              // Anonymous pages (up to 20MB)
    _mapped = full_randomizer(range) % 10000;                       // Mapped memory (up to 10MB)
    _shared_memory = full_randomizer(range) % 5000;                 // Shared memory (up to 5MB)
    _kernel_reclaimable = full_randomizer(range) % 5000;            // Kernel reclaimable memory (up to 5MB)
    _slab = full_randomizer(range) % 5000;                          // Slab memory (up to 5MB)
    _slab_reclaimable = full_randomizer(range) % 2000;              // Slab reclaimable (up to 2MB)
    _slab_unreclaim = full_randomizer(range) % 3000;                // Slab unreclaimable (up to 3MB)
    _kernel_stack = full_randomizer(range) % 500;                   // Kernel stack memory (up to 500KB)
    _page_tables = full_randomizer(range) % 1000;                   // Page tables (up to 1MB)
    _NFS_unstable = full_randomizer(range) % 1000;                  // NFS unstable pages (up to 1MB)
    _bounce = full_randomizer(range) % 100;                         // Bounce buffer memory (up to 100KB)
    _writeback_tmp = full_randomizer(range) % 100;                  // Writeback temporary (up to 100KB)
    _commit_limit = _total.value() + _swap_total.value();           // Commit limit (total + swap)
    _committed_AS = full_randomizer(range) % _commit_limit.value(); // Committed address space
    _vmalloc_total = full_randomizer(range) % 64000;                // Vmalloc total (up to 64GB)
    _vmalloc_used = full_randomizer(range) % 32000;                 // Vmalloc used (up to 32GB)
    _vmalloc_chunk = full_randomizer(range) % 16000;                // Vmalloc chunk (up to 16GB)
    _percpu = full_randomizer(range) % 10000;                       // Per-CPU memory (up to 10MB)
    _hardware_corrupted = full_randomizer(range) % 1000;            // Hardware corrupted memory (up to 1MB)
    _anon_huge_pages = full_randomizer(range) % 10000;              // Anon huge pages (up to 10MB)
    _shared_memory_huge_pages = full_randomizer(range) % 10000;     // Shared memory huge pages (up to 10MB)
    _shared_memory_pmd_mapped = full_randomizer(range) % 5000;      // Shared memory PMD mapped (up to 5MB)
    _file_huge_pages = full_randomizer(range) % 5000;               // File huge pages (up to 5MB)
    _file_pmd_mapped = full_randomizer(range) % 2000;               // File PMD mapped (up to 2MB)
    _huge_pages_total = full_randomizer(range) % 10000;             // Total huge pages (up to 10MB)
    _huge_pages_free = full_randomizer(range) % _huge_pages_total.value();  // Free huge pages (up to total)
    _huge_pages_rsvd = full_randomizer(range) % 1000;               // Reserved huge pages (up to 1MB)
    _huge_pages_surp = full_randomizer(range) % 500;                // Surplus huge pages (up to 500KB)
    _huge_page_size = 2048;                                         // Huge page size (2MB typical)
    _huge_tlb = full_randomizer(range) % 10000;                     // Huge TLB pages (up to 10MB)
    _oom_killer_invocations = full_randomizer(range) % 100;         // OOM killer invocations (up to 100)

    // Take a snapshot of the system.
    snap();
}

void Memory::snap() {
    // Update all the values for a snap.
    if (_free) _free = _free.value() + (full_randomizer(range) % 500) - 250;
    if (_available) _available = _available.value() + (full_randomizer(range) % 1000) - 500;
    if (_active) _active = _active.value() + (full_randomizer(range) % 1000) - 500;
    if (_inactive) _inactive = _inactive.value() + (full_randomizer(range) % 500) - 250;
    if (_swap_free) _swap_free = _swap_free.value() + (full_randomizer(range) % 500) - 250;
    if (_dirty) _dirty = _dirty.value() + (full_randomizer(range) % 100) - 50;
    if (_anon_huge_pages) _anon_huge_pages = _anon_huge_pages.value() + (full_randomizer(range) % 500) - 250;

    if (_free < 0) _free = 0;
    if (_available < 0) _available = 0;
    if (_active < 0) _active = 0;
    if (_inactive < 0) _inactive = 0;
    if (_swap_free < 0) _swap_free = 0;
    if (_dirty < 0) _dirty = 0;
    if (_anon_huge_pages < 0) _anon_huge_pages = 0;
}

size_t Memory::total() {
    return _total.value();
}

size_t Memory::free() {
    return _free.value();
}

size_t Memory::available() {
    return _available.value();
}

size_t Memory::buffers() {
    return _buffers.value();
}

size_t Memory::cached() {
    return _cached.value();
}

size_t Memory::swap_cached() {
    return _swap_cached.value();
}

size_t Memory::active() {
    return _active.value();
}

size_t Memory::inactive() {
    return _inactive.value();
}

size_t Memory::active_anonymous() {
    return _active_anonymous.value();
}

size_t Memory::inactive_anonymous() {
    return _inactive_anonymous.value();
}

size_t Memory::active_file() {
    return _active_file.value();
}

size_t Memory::inactive_file() {
    return _inactive_file.value();
}

size_t Memory::unevictable() {
    return _unevictable.value();
}

size_t Memory::locked() {
    return _locked.value();
}

size_t Memory::swap_total() {
    return _swap_total.value();
}

size_t Memory::swap_free() {
    return _swap_free.value();
}

size_t Memory::dirty() {
    return _dirty.value();
}

size_t Memory::writeback() {
    return _writeback.value();
}

size_t Memory::anonymous_pages() {
    return _anonymous_pages.value();
}

size_t Memory::mapped() {
    return _mapped.value();
}

size_t Memory::shared_memory() {
    return _shared_memory.value();
}

size_t Memory::kernel_reclaimable() {
    return _kernel_reclaimable.value();
}

size_t Memory::slab() {
    return _slab.value();
}

size_t Memory::slab_reclaimable() {
    return _slab_reclaimable.value();
}

size_t Memory::slab_unreclaim() {
    return _slab_unreclaim.value();
}

size_t Memory::kernel_stack() {
    return _kernel_stack.value();
}

size_t Memory::page_tables() {
    return _page_tables.value();
}

size_t Memory::NFS_unstable() {
    return _NFS_unstable.value();
}

size_t Memory::bounce() {
    return _bounce.value();
}

size_t Memory::writeback_tmp() {
    return _writeback_tmp.value();
}

size_t Memory::commit_limit() {
    return _commit_limit.value();
}

size_t Memory::committed_AS() {
    return _committed_AS.value();
}

size_t Memory::vmalloc_total() {
    return _vmalloc_total.value();
}

size_t Memory::vmalloc_used() {
    return _vmalloc_used.value();
}

size_t Memory::vmalloc_chunk() {
    return _vmalloc_chunk.value();
}

size_t Memory::percpu() {
    return _percpu.value();
}

size_t Memory::hardware_corrupted() {
    return _hardware_corrupted.value();
}

size_t Memory::anon_huge_pages() {
    return _anon_huge_pages.value();
}

size_t Memory::shared_memory_huge_pages() {
    return _shared_memory_huge_pages.value();
}

size_t Memory::shared_memory_pmd_mapped() {
    return _shared_memory_pmd_mapped.value();
}

size_t Memory::file_huge_pages() {
    return _file_huge_pages.value();
}

size_t Memory::file_pmd_mapped() {
    return _file_pmd_mapped.value();
}

size_t Memory::huge_pages_total() {
    return _huge_pages_total.value();
}

size_t Memory::huge_pages_free() {
    return _huge_pages_free.value();
}

size_t Memory::huge_pages_rsvd() {
    return _huge_pages_rsvd.value();
}

size_t Memory::huge_pages_surp() {
    return _huge_pages_surp.value();
}

size_t Memory::huge_page_size() {
    return _huge_page_size.value();
}

size_t Memory::huge_tlb() {
    return _huge_tlb.value();
}

int Memory::oom_killer_invocations() {
    return _oom_killer_invocations.value();
}

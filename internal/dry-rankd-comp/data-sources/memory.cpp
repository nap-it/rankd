#include "data-sources/memory.h"

Memory::Memory() {
    // Initialize all the variables for future snapping.
    // TODO

    // Take a snapshot of the system.
    snap();
}

void Memory::snap() {
    // Update all the values for a snap.
    // TODO
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

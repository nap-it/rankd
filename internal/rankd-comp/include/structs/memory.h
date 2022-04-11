#ifndef RANKD_MEMORY_H
#define RANKD_MEMORY_H

#include <optional>

#include "pfs/procfs.hpp"

class Memory {
public:
  void snap();
private:
  std::optional<size_t> _total;
  std::optional<size_t> _free;
  std::optional<size_t> _available;
  std::optional<size_t> _buffers;
  std::optional<size_t> _cached;
  std::optional<size_t> _swap_cached;
  std::optional<size_t> _active;
  std::optional<size_t> _inactive;
  std::optional<size_t> _active_anonymous;
  std::optional<size_t> _inactive_anonymous;
  std::optional<size_t> _active_file;
  std::optional<size_t> _inactive_file;
  std::optional<size_t> _unevictable;
  std::optional<size_t> _locked;
  std::optional<size_t> _swap_total;
  std::optional<size_t> _swap_free;
  std::optional<size_t> _dirty;
  std::optional<size_t> _writeback;
  std::optional<size_t> _anonymous_pages;
  std::optional<size_t> _mapped;
  std::optional<size_t> _shared_memory;
  std::optional<size_t> _kernel_reclaimable;
  std::optional<size_t> _slab;
  std::optional<size_t> _slab_reclaimable;
  std::optional<size_t> _slab_unreclaim;
  std::optional<size_t> _kernel_stack;
  std::optional<size_t> _page_tables;
  std::optional<size_t> _NFS_unstable;
  std::optional<size_t> _bounce;
  std::optional<size_t> _writeback_tmp;
  std::optional<size_t> _commit_limit;
  std::optional<size_t> _committed_AS;
  std::optional<size_t> _vmalloc_total;
  std::optional<size_t> _vmalloc_used;
  std::optional<size_t> _vmalloc_chunk;
  std::optional<size_t> _percpu;
  std::optional<size_t> _hardware_corrupted;
  std::optional<size_t> _anon_huge_pages;
  std::optional<size_t> _shared_memory_huge_pages;
  std::optional<size_t> _shared_memory_pmd_mapped;
  std::optional<size_t> _file_huge_pages;
  std::optional<size_t> _file_pmd_mapped;
  std::optional<size_t> _huge_pages_total;
  std::optional<size_t> _huge_pages_free;
  std::optional<size_t> _huge_pages_rsvd;
  std::optional<size_t> _huge_pages_surp;
  std::optional<size_t> _huge_page_size;
  std::optional<size_t> _huge_tlb;
};

#endif // RANKD_MEMORY_H

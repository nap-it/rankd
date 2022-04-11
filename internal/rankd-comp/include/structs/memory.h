#ifndef RANKD_MEMORY_H
#define RANKD_MEMORY_H

#include <optional>

#include "pfs/procfs.hpp"

class Memory {
public:
  void snap();
private:
  std::optional<unsigned long long int> total;
  std::optional<unsigned long long int> free;
  std::optional<unsigned long long int> available;
  std::optional<unsigned long long int> buffers;
  std::optional<unsigned long long int> cached;
  std::optional<unsigned long long int> swap_cached;
  std::optional<unsigned long long int> active;
  std::optional<unsigned long long int> inactive;
  std::optional<unsigned long long int> active_anonymous;
  std::optional<unsigned long long int> inactive_anonymous;
  std::optional<unsigned long long int> active_file;
  std::optional<unsigned long long int> inactive_file;
  std::optional<unsigned long long int> unevictable;
  std::optional<unsigned long long int> locked;
  std::optional<unsigned long long int> swap_total;
  std::optional<unsigned long long int> swap_free;
  std::optional<unsigned long long int> dirty;
  std::optional<unsigned long long int> writeback;
  std::optional<unsigned long long int> anonymous_pages;
  std::optional<unsigned long long int> mapped;
  std::optional<unsigned long long int> shared_memory;
  std::optional<unsigned long long int> kernel_reclaimable;
  std::optional<unsigned long long int> slab;
  std::optional<unsigned long long int> slab_reclaimable;
  std::optional<unsigned long long int> slab_unreclaim;
  std::optional<unsigned long long int> kernel_stack;
  std::optional<unsigned long long int> page_tables;
  std::optional<unsigned long long int> NFS_unstable;
  std::optional<unsigned long long int> bounce;
  std::optional<unsigned long long int> writeback_tmp;
  std::optional<unsigned long long int> commit_limit;
  std::optional<unsigned long long int> committed_AS;
  std::optional<unsigned long long int> vmalloc_total;
  std::optional<unsigned long long int> vmalloc_used;
  std::optional<unsigned long long int> vmalloc_chunk;
  std::optional<unsigned long long int> percpu;
  std::optional<unsigned long long int> hardware_corrupted;
  std::optional<unsigned long long int> anon_huge_pages;
  std::optional<unsigned long long int> shared_memory_huge_pages;
  std::optional<unsigned long long int> shared_memory_pmd_mapped;
  std::optional<unsigned long long int> file_huge_pages;
  std::optional<unsigned long long int> file_pmd_mapped;
  std::optional<unsigned long long int> huge_pages_total;
  std::optional<unsigned long long int> huge_pages_free;
  std::optional<unsigned long long int> huge_pages_rsvd;
  std::optional<unsigned long long int> huge_pages_surp;
  std::optional<unsigned long long int> huge_page_size;
  std::optional<unsigned long long int> huge_tlb;
};

#endif // RANKD_MEMORY_H

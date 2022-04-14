#ifndef RANKD_MEMORY_H
#define RANKD_MEMORY_H

#include <optional>

#include "pfs/procfs.hpp"

class Memory {
public:
  void snap();
  [[nodiscard]] const std::optional<size_t>& total() const;
  [[nodiscard]] const std::optional<size_t>& free() const;
  [[nodiscard]] const std::optional<size_t>& available() const;
  [[nodiscard]] const std::optional<size_t>& buffers() const;
  [[nodiscard]] const std::optional<size_t>& cached() const;
  [[nodiscard]] const std::optional<size_t>& swap_cached() const;
  [[nodiscard]] const std::optional<size_t>& active() const;
  [[nodiscard]] const std::optional<size_t>& inactive() const;
  [[nodiscard]] const std::optional<size_t>& active_anonymous() const;
  [[nodiscard]] const std::optional<size_t>& inactive_anonymous() const;
  [[nodiscard]] const std::optional<size_t>& active_file() const;
  [[nodiscard]] const std::optional<size_t>& inactive_file() const;
  [[nodiscard]] const std::optional<size_t>& unevictable() const;
  [[nodiscard]] const std::optional<size_t>& locked() const;
  [[nodiscard]] const std::optional<size_t>& swap_total() const;
  [[nodiscard]] const std::optional<size_t>& swap_free() const;
  [[nodiscard]] const std::optional<size_t>& dirty() const;
  [[nodiscard]] const std::optional<size_t>& writeback() const;
  [[nodiscard]] const std::optional<size_t>& anonymous_pages() const;
  [[nodiscard]] const std::optional<size_t>& mapped() const;
  [[nodiscard]] const std::optional<size_t>& shared_memory() const;
  [[nodiscard]] const std::optional<size_t>& kernel_reclaimable() const;
  [[nodiscard]] const std::optional<size_t>& slab() const;
  [[nodiscard]] const std::optional<size_t>& slab_reclaimable() const;
  [[nodiscard]] const std::optional<size_t>& slab_unreclaim() const;
  [[nodiscard]] const std::optional<size_t>& kernel_stack() const;
  [[nodiscard]] const std::optional<size_t>& page_tables() const;
  [[nodiscard]] const std::optional<size_t>& nfs_unstable() const;
  [[nodiscard]] const std::optional<size_t>& bounce() const;
  [[nodiscard]] const std::optional<size_t>& writeback_tmp() const;
  [[nodiscard]] const std::optional<size_t>& commit_limit() const;
  [[nodiscard]] const std::optional<size_t>& committed_as() const;
  [[nodiscard]] const std::optional<size_t>& vmalloc_total() const;
  [[nodiscard]] const std::optional<size_t>& vmalloc_used() const;
  [[nodiscard]] const std::optional<size_t>& vmalloc_chunk() const;
  [[nodiscard]] const std::optional<size_t>& percpu() const;
  [[nodiscard]] const std::optional<size_t>& hardware_corrupted() const;
  [[nodiscard]] const std::optional<size_t>& anon_huge_pages() const;
  [[nodiscard]] const std::optional<size_t>& shared_memory_huge_pages() const;
  [[nodiscard]] const std::optional<size_t>& shared_memory_pmd_mapped() const;
  [[nodiscard]] const std::optional<size_t>& file_huge_pages() const;
  [[nodiscard]] const std::optional<size_t>& file_pmd_mapped() const;
  [[nodiscard]] const std::optional<size_t>& huge_pages_total() const;
  [[nodiscard]] const std::optional<size_t>& huge_pages_free() const;
  [[nodiscard]] const std::optional<size_t>& huge_pages_rsvd() const;
  [[nodiscard]] const std::optional<size_t>& huge_pages_surp() const;
  [[nodiscard]] const std::optional<size_t>& huge_page_size() const;
  [[nodiscard]] const std::optional<size_t>& huge_tlb() const;

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

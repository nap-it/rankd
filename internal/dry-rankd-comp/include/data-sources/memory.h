#ifndef DRYRANKD_LINUX_MEMORY_H
#define DRYRANKD_LINUX_MEMORY_H

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <map>
#include <optional>
#include <random>

class Memory {
public:
  Memory();
  void snap();
  [[nodiscard]] size_t total();
  [[nodiscard]] size_t free();
  [[nodiscard]] size_t available();
  [[nodiscard]] size_t buffers();
  [[nodiscard]] size_t cached();
  [[nodiscard]] size_t swap_cached();
  [[nodiscard]] size_t active();
  [[nodiscard]] size_t inactive();
  [[nodiscard]] size_t active_anonymous();
  [[nodiscard]] size_t inactive_anonymous();
  [[nodiscard]] size_t active_file();
  [[nodiscard]] size_t inactive_file();
  [[nodiscard]] size_t unevictable();
  [[nodiscard]] size_t locked();
  [[nodiscard]] size_t swap_total();
  [[nodiscard]] size_t swap_free();
  [[nodiscard]] size_t dirty();
  [[nodiscard]] size_t writeback();
  [[nodiscard]] size_t anonymous_pages();
  [[nodiscard]] size_t mapped();
  [[nodiscard]] size_t shared_memory();
  [[nodiscard]] size_t kernel_reclaimable();
  [[nodiscard]] size_t slab();
  [[nodiscard]] size_t slab_reclaimable();
  [[nodiscard]] size_t slab_unreclaim();
  [[nodiscard]] size_t kernel_stack();
  [[nodiscard]] size_t page_tables();
  [[nodiscard]] size_t NFS_unstable();
  [[nodiscard]] size_t bounce();
  [[nodiscard]] size_t writeback_tmp();
  [[nodiscard]] size_t commit_limit();
  [[nodiscard]] size_t committed_AS();
  [[nodiscard]] size_t vmalloc_total();
  [[nodiscard]] size_t vmalloc_used();
  [[nodiscard]] size_t vmalloc_chunk();
  [[nodiscard]] size_t percpu();
  [[nodiscard]] size_t hardware_corrupted();
  [[nodiscard]] size_t anon_huge_pages();
  [[nodiscard]] size_t shared_memory_huge_pages();
  [[nodiscard]] size_t shared_memory_pmd_mapped();
  [[nodiscard]] size_t file_huge_pages();
  [[nodiscard]] size_t file_pmd_mapped();
  [[nodiscard]] size_t huge_pages_total();
  [[nodiscard]] size_t huge_pages_free();
  [[nodiscard]] size_t huge_pages_rsvd();
  [[nodiscard]] size_t huge_pages_surp();
  [[nodiscard]] size_t huge_page_size();
  [[nodiscard]] size_t huge_tlb();
  [[nodiscard]] int oom_killer_invocations();
  friend std::ostream& operator<<(std::ostream& os, const Memory& memory);

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
  std::optional<int> _oom_killer_invocations;
};

#endif // DRYRANKD_LINUX_MEMORY_H

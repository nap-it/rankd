#include "structs/memory.h"

void Memory::snap() {
  auto meminfo = pfs::procfs().get_meminfo();

  _total = meminfo.count("MemTotal") != 0 ? std::optional(meminfo.at("MemTotal")) : std::nullopt;
  _free = meminfo.count("MemFree") != 0 ? std::optional(meminfo.at("MemFree")) : std::nullopt;
  _available = meminfo.count("MemAvailable") != 0 ? std::optional(meminfo.at("MemAvailable")) : std::nullopt;
  _buffers = meminfo.count("Buffers") != 0 ? std::optional(meminfo.at("Buffers")) : std::nullopt;
  _cached = meminfo.count("Cached") != 0 ? std::optional(meminfo.at("Cached")) : std::nullopt;
  _swap_cached = meminfo.count("SwapCached") != 0 ? std::optional(meminfo.at("SwapCached")) : std::nullopt;
  _active = meminfo.count("Active") != 0 ? std::optional(meminfo.at("Active")) : std::nullopt;
  _inactive = meminfo.count("Inactive") != 0 ? std::optional(meminfo.at("Inactive")) : std::nullopt;
  _active_anonymous = meminfo.count("Active(anon)") != 0 ? std::optional(meminfo.at("Active(anon)")) : std::nullopt;
  _inactive_anonymous = meminfo.count("Inactive(anon)") != 0 ? std::optional(meminfo.at("Inactive(anon)")) : std::nullopt;
  _active_file = meminfo.count("Active(file)") != 0 ? std::optional(meminfo.at("Active(file)")) : std::nullopt;
  _inactive_file = meminfo.count("Inactive(file)") != 0 ? std::optional(meminfo.at("Inactive(file)")) : std::nullopt;
  _unevictable = meminfo.count("Unevictable") != 0 ? std::optional(meminfo.at("Unevictable")) : std::nullopt;
  _locked = meminfo.count("Mlocked") != 0 ? std::optional(meminfo.at("Mlocked")) : std::nullopt;
  _swap_total = meminfo.count("SwapTotal") != 0 ? std::optional(meminfo.at("SwapTotal")) : std::nullopt;
  _swap_free = meminfo.count("SwapFree") != 0 ? std::optional(meminfo.at("SwapFree")) : std::nullopt;
  _dirty = meminfo.count("Dirty") != 0 ? std::optional(meminfo.at("Dirty")) : std::nullopt;
  _writeback = meminfo.count("Writeback") != 0 ? std::optional(meminfo.at("Writeback")) : std::nullopt;
  _anonymous_pages = meminfo.count("AnonPages") != 0 ? std::optional(meminfo.at("AnonPages")) : std::nullopt;
  _mapped = meminfo.count("Mapped") != 0 ? std::optional(meminfo.at("Mapped")) : std::nullopt;
  _shared_memory = meminfo.count("Shmem") != 0 ? std::optional(meminfo.at("Shmem")) : std::nullopt;
  _kernel_reclaimable = meminfo.count("KReclaimable") != 0 ? std::optional(meminfo.at("KReclaimable")) : std::nullopt;
  _slab = meminfo.count("Slab") != 0 ? std::optional(meminfo.at("Slab")) : std::nullopt;
  _slab_reclaimable = meminfo.count("SReclaimable") != 0 ? std::optional(meminfo.at("SReclaimable")) : std::nullopt;
  _slab_unreclaim = meminfo.count("SUnreclaim") != 0 ? std::optional(meminfo.at("SUnreclaim")) : std::nullopt;
  _kernel_stack = meminfo.count("KernelStack") != 0 ? std::optional(meminfo.at("KernelStack")) : std::nullopt;
  _page_tables = meminfo.count("PageTables") != 0 ? std::optional(meminfo.at("PageTables")) : std::nullopt;
  _NFS_unstable = meminfo.count("NFS_Unstable") != 0 ? std::optional(meminfo.at("NFS_Unstable")) : std::nullopt;
  _bounce = meminfo.count("Bounce") != 0 ? std::optional(meminfo.at("Bounce")) : std::nullopt;
  _writeback_tmp = meminfo.count("WritebackTmp") != 0 ? std::optional(meminfo.at("WritebackTmp")) : std::nullopt;
  _commit_limit = meminfo.count("CommitLimit") != 0 ? std::optional(meminfo.at("CommitLimit")) : std::nullopt;
  _committed_AS = meminfo.count("Committed_AS") != 0 ? std::optional(meminfo.at("Committed_AS")) : std::nullopt;
  _vmalloc_total = meminfo.count("VmallocTotal") != 0 ? std::optional(meminfo.at("VmallocTotal")) : std::nullopt;
  _vmalloc_used = meminfo.count("VmallocUsed") != 0 ? std::optional(meminfo.at("VmallocUsed")) : std::nullopt;
  _vmalloc_chunk = meminfo.count("VmallocChunk") != 0 ? std::optional(meminfo.at("VmallocChunk")) : std::nullopt;
  _percpu = meminfo.count("Percpu") != 0 ? std::optional(meminfo.at("Percpu")) : std::nullopt;
  _hardware_corrupted = meminfo.count("HardwareCorrupted") != 0 ? std::optional(meminfo.at("HardwareCorrupted")) : std::nullopt;
  _anon_huge_pages = meminfo.count("AnonHugePages") != 0 ? std::optional(meminfo.at("AnonHugePages")) : std::nullopt;
  _shared_memory_huge_pages = meminfo.count("ShmemHugePages") != 0 ? std::optional(meminfo.at("ShmemHugePages")) : std::nullopt;
  _shared_memory_pmd_mapped = meminfo.count("ShmemPmdMapped") != 0 ? std::optional(meminfo.at("ShmemPmdMapped")) : std::nullopt;
  _file_huge_pages = meminfo.count("FileHugePages") != 0 ? std::optional(meminfo.at("FileHugePages")) : std::nullopt;
  _file_pmd_mapped = meminfo.count("FilePmdMapped") != 0 ? std::optional(meminfo.at("FilePmdMapped")) : std::nullopt;
  _huge_pages_total = meminfo.count("HugePages_Total") != 0 ? std::optional(meminfo.at("HugePages_Total")) : std::nullopt;
  _huge_pages_free = meminfo.count("HugePages_Free") != 0 ? std::optional(meminfo.at("HugePages_Free")) : std::nullopt;
  _huge_pages_rsvd = meminfo.count("HugePages_Rsvd") != 0 ? std::optional(meminfo.at("HugePages_Rsvd")) : std::nullopt;
  _huge_pages_surp = meminfo.count("HugePages_Surp") != 0 ? std::optional(meminfo.at("HugePages_Surp")) : std::nullopt;
  _huge_page_size = meminfo.count("Hugepagesize") != 0 ? std::optional(meminfo.at("Hugepagesize")) : std::nullopt;
  _huge_tlb = meminfo.count("Hugetlb") != 0 ? std::optional(meminfo.at("Hugetlb")) : std::nullopt;
}

const std::optional<size_t> &Memory::total() const { return _total; }

const std::optional<size_t> &Memory::free() const { return _free; }

const std::optional<size_t> &Memory::available() const {
  return _available;
}

const std::optional<size_t> &Memory::buffers() const { return _buffers; }

const std::optional<size_t> &Memory::cached() const { return _cached; }

const std::optional<size_t> &Memory::swap_cached() const {
  return _swap_cached;
}

const std::optional<size_t> &Memory::active() const { return _active; }

const std::optional<size_t> &Memory::inactive() const { return _inactive; }

const std::optional<size_t> &Memory::active_anonymous() const {
  return _active_anonymous;
}

const std::optional<size_t> &Memory::inactive_anonymous() const {
  return _inactive_anonymous;
}

const std::optional<size_t> &Memory::active_file() const {
  return _active_file;
}

const std::optional<size_t> &Memory::inactive_file() const {
  return _inactive_file;
}

const std::optional<size_t> &Memory::unevictable() const {
  return _unevictable;
}

const std::optional<size_t> &Memory::locked() const { return _locked; }

const std::optional<size_t> &Memory::swap_total() const {
  return _swap_total;
}

const std::optional<size_t> &Memory::swap_free() const {
  return _swap_free;
}

const std::optional<size_t> &Memory::dirty() const { return _dirty; }

const std::optional<size_t> &Memory::writeback() const {
  return _writeback;
}

const std::optional<size_t> &Memory::anonymous_pages() const {
  return _anonymous_pages;
}

const std::optional<size_t> &Memory::mapped() const { return _mapped; }

const std::optional<size_t> &Memory::shared_memory() const {
  return _shared_memory;
}

const std::optional<size_t> &Memory::kernel_reclaimable() const {
  return _kernel_reclaimable;
}

const std::optional<size_t> &Memory::slab() const { return _slab; }

const std::optional<size_t> &Memory::slab_reclaimable() const {
  return _slab_reclaimable;
}

const std::optional<size_t> &Memory::slab_unreclaim() const {
  return _slab_unreclaim;
}

const std::optional<size_t> &Memory::kernel_stack() const {
  return _kernel_stack;
}

const std::optional<size_t> &Memory::page_tables() const {
  return _page_tables;
}

const std::optional<size_t> &Memory::nfs_unstable() const {
  return _NFS_unstable;
}

const std::optional<size_t> &Memory::bounce() const { return _bounce; }

const std::optional<size_t> &Memory::writeback_tmp() const {
  return _writeback_tmp;
}

const std::optional<size_t> &Memory::commit_limit() const {
  return _commit_limit;
}

const std::optional<size_t> &Memory::committed_as() const {
  return _committed_AS;
}

const std::optional<size_t> &Memory::vmalloc_total() const {
  return _vmalloc_total;
}

const std::optional<size_t> &Memory::vmalloc_used() const {
  return _vmalloc_used;
}

const std::optional<size_t> &Memory::vmalloc_chunk() const {
  return _vmalloc_chunk;
}

const std::optional<size_t> &Memory::percpu() const { return _percpu; }

const std::optional<size_t> &Memory::hardware_corrupted() const {
  return _hardware_corrupted;
}

const std::optional<size_t> &Memory::anon_huge_pages() const {
  return _anon_huge_pages;
}

const std::optional<size_t> &Memory::shared_memory_huge_pages() const {
  return _shared_memory_huge_pages;
}

const std::optional<size_t> &Memory::shared_memory_pmd_mapped() const {
  return _shared_memory_pmd_mapped;
}

const std::optional<size_t> &Memory::file_huge_pages() const {
  return _file_huge_pages;
}

const std::optional<size_t> &Memory::file_pmd_mapped() const {
  return _file_pmd_mapped;
}

const std::optional<size_t> &Memory::huge_pages_total() const {
  return _huge_pages_total;
}

const std::optional<size_t> &Memory::huge_pages_free() const {
  return _huge_pages_free;
}

const std::optional<size_t> &Memory::huge_pages_rsvd() const {
  return _huge_pages_rsvd;
}

const std::optional<size_t> &Memory::huge_pages_surp() const {
  return _huge_pages_surp;
}

const std::optional<size_t> &Memory::huge_page_size() const {
  return _huge_page_size;
}

const std::optional<size_t> &Memory::huge_tlb() const { return _huge_tlb; }


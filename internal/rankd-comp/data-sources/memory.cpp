#include "data-sources/memory.h"

Memory::Memory() {
  snap();
}

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

  std::ifstream vmstat("/proc/vmstat");

  std::map<std::string, long> vmstat_map{};
  while (!vmstat.eof()) {
    std::string key;
    long value;
    vmstat >> key;
    vmstat >> value;

    vmstat_map[key] = value;
  }

  _oom_killer_invocations = vmstat_map.count("oom_kill") == 0 ? 0 : vmstat_map.at("oom_kill");

  vmstat.close();
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
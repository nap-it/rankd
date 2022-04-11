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
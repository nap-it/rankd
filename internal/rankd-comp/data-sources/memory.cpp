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
    _inactive_anonymous =
            meminfo.count("Inactive(anon)") != 0 ? std::optional(meminfo.at("Inactive(anon)")) : std::nullopt;
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
    _hardware_corrupted =
            meminfo.count("HardwareCorrupted") != 0 ? std::optional(meminfo.at("HardwareCorrupted")) : std::nullopt;
    _anon_huge_pages = meminfo.count("AnonHugePages") != 0 ? std::optional(meminfo.at("AnonHugePages")) : std::nullopt;
    _shared_memory_huge_pages =
            meminfo.count("ShmemHugePages") != 0 ? std::optional(meminfo.at("ShmemHugePages")) : std::nullopt;
    _shared_memory_pmd_mapped =
            meminfo.count("ShmemPmdMapped") != 0 ? std::optional(meminfo.at("ShmemPmdMapped")) : std::nullopt;
    _file_huge_pages = meminfo.count("FileHugePages") != 0 ? std::optional(meminfo.at("FileHugePages")) : std::nullopt;
    _file_pmd_mapped = meminfo.count("FilePmdMapped") != 0 ? std::optional(meminfo.at("FilePmdMapped")) : std::nullopt;
    _huge_pages_total =
            meminfo.count("HugePages_Total") != 0 ? std::optional(meminfo.at("HugePages_Total")) : std::nullopt;
    _huge_pages_free =
            meminfo.count("HugePages_Free") != 0 ? std::optional(meminfo.at("HugePages_Free")) : std::nullopt;
    _huge_pages_rsvd =
            meminfo.count("HugePages_Rsvd") != 0 ? std::optional(meminfo.at("HugePages_Rsvd")) : std::nullopt;
    _huge_pages_surp =
            meminfo.count("HugePages_Surp") != 0 ? std::optional(meminfo.at("HugePages_Surp")) : std::nullopt;
    _huge_page_size = meminfo.count("Hugepagesize") != 0 ? std::optional(meminfo.at("Hugepagesize")) : std::nullopt;
    _huge_tlb = meminfo.count("Hugetlb") != 0 ? std::optional(meminfo.at("Hugetlb")) : std::nullopt;

    std::ifstream vmstat("/proc/vmstat");

    std::map<std::string, long> vmstat_map {};
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

rapidjson::Document Memory::json() const {
    // Create a JSON document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    json_document["total"].SetInt64(_total.value_or(-1));
    json_document["free"].SetInt64(_free.value_or(-1));
    json_document["available"].SetInt64(_available.value_or(-1));
    json_document["buffers"].SetInt64(_buffers.value_or(-1));
    json_document["cached"].SetInt64(_cached.value_or(-1));
    json_document["swap-cached"].SetInt64(_swap_cached.value_or(-1));
    json_document["active"].SetInt64(_active.value_or(-1));
    json_document["inactive"].SetInt64(_inactive.value_or(-1));
    json_document["active-anonymous"].SetInt64(_active_anonymous.value_or(-1));
    json_document["inactive-anonymous"].SetInt64(_inactive_anonymous.value_or(-1));
    json_document["active-file"].SetInt64(_active_file.value_or(-1));
    json_document["inactive-file"].SetInt64(_inactive_file.value_or(-1));
    json_document["unevictable"].SetInt64(_unevictable.value_or(-1));
    json_document["locked"].SetInt64(_locked.value_or(-1));
    json_document["swap-total"].SetInt64(_swap_total.value_or(-1));
    json_document["swap-free"].SetInt64(_swap_free.value_or(-1));
    json_document["dirty"].SetInt64(_dirty.value_or(-1));
    json_document["writeback"].SetInt64(_writeback.value_or(-1));
    json_document["anonymous-pages"].SetInt64(_anonymous_pages.value_or(-1));
    json_document["mapped"].SetInt64(_mapped.value_or(-1));
    json_document["shared-memory"].SetInt64(_shared_memory.value_or(-1));
    json_document["kernel-reclaimable"].SetInt64(_kernel_reclaimable.value_or(-1));
    json_document["slab"].SetInt64(_slab.value_or(-1));
    json_document["slab-reclaimable"].SetInt64(_slab_reclaimable.value_or(-1));
    json_document["slab-unreclaim"].SetInt64(_slab_unreclaim.value_or(-1));
    json_document["kernel-stack"].SetInt64(_kernel_stack.value_or(-1));
    json_document["page-tables"].SetInt64(_page_tables.value_or(-1));
    json_document["NFS-unstable"].SetInt64(_NFS_unstable.value_or(-1));
    json_document["bounce"].SetInt64(_bounce.value_or(-1));
    json_document["writeback-tmp"].SetInt64(_writeback_tmp.value_or(-1));
    json_document["commit-limit"].SetInt64(_commit_limit.value_or(-1));
    json_document["committed-AS"].SetInt64(_committed_AS.value_or(-1));
    json_document["vmalloc-total"].SetInt64(_vmalloc_total.value_or(-1));
    json_document["vmalloc-used"].SetInt64(_vmalloc_used.value_or(-1));
    json_document["vmalloc-chunk"].SetInt64(_vmalloc_chunk.value_or(-1));
    json_document["percpu"].SetInt64(_percpu.value_or(-1));
    json_document["hardware-corrupted"].SetInt64(_hardware_corrupted.value_or(-1));
    json_document["anon-huge-pages"].SetInt64(_anon_huge_pages.value_or(-1));
    json_document["shared-memory-huge-pages"].SetInt64(_shared_memory_huge_pages.value_or(-1));
    json_document["shared-memory-pmd-mapped"].SetInt64(_shared_memory_pmd_mapped.value_or(-1));
    json_document["file-huge-pages"].SetInt64(_file_huge_pages.value_or(-1));
    json_document["file-pmd-mapped"].SetInt64(_file_pmd_mapped.value_or(-1));
    json_document["huge-pages-total"].SetInt64(_huge_pages_total.value_or(-1));
    json_document["huge-pages-free"].SetInt64(_huge_pages_free.value_or(-1));
    json_document["huge-pages-rsvd"].SetInt64(_huge_pages_rsvd.value_or(-1));
    json_document["huge-pages-surp"].SetInt64(_huge_pages_surp.value_or(-1));
    json_document["huge-page-size"].SetInt64(_huge_page_size.value_or(-1));
    json_document["huge-tlb"].SetInt64(_huge_tlb.value_or(-1));
    json_document["oom-killer-invocations"].SetInt(_oom_killer_invocations.value_or(-1));

    return json_document;
}

std::ostream& operator<<(std::ostream& os, const Memory& memory) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    auto json_document = memory.json();
    json_document.Accept(writer);

    os << buffer.GetString();

    return os;
}

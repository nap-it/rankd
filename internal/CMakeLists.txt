if(FROM_SIMUZILLA)
    add_subdirectory(rank-prelude)
    add_subdirectory(dry-rankd-comp)
    add_subdirectory(dry-rankd-net)
    add_subdirectory(dry-rankd-time)
else()
    add_subdirectory(rank-prelude)
    add_subdirectory(rankd-comp)
    add_subdirectory(rankd-net)
    add_subdirectory(rankd-time)
endif()
copy "data files\country_mappings.txt" "release\country_mappings.txt"
copy "data files\readme.txt" "release\readme.txt"
copy "data files\DW_province_mappings.txt" "release\DW_province_mappings.txt"
copy "data files\HttT_province_mappings.txt" "release\HttT_province_mappings.txt"
copy "data files\IN_province_mappings.txt" "release\IN_province_mappings.txt"
copy "data files\cultureMap.txt" "release\cultureMap.txt"
copy "data files\religionMap.txt" "release\religionMap.txt"
copy "data files\changeLog.txt" "release\changeLog.txt"
copy "data files\blocked_nations.txt" "release\blocked_nations.txt"
copy "data files\unions.txt" "release\unions.txt"
copy "data files\governmentMapping.txt" "release\governmentMapping.txt"
copy "data files\port_whitelist.txt" "release\port_whitelist.txt"
copy "data files\port_blacklist.txt" "release\port_blacklist.txt"
copy "data files\regiment_costs.txt" "release\regiment_costs.txt"
copy "data files\starting_factories.txt" "release\starting_factories.txt"
copy "data files\leader_traits.txt" "release\leader_traits.txt"
copy "data files\merge_nations.txt" "release\merge_nations.txt"
copy "data files\configuration.txt" "release\configuration.txt"
copy "data files\blocked_tech_schools.txt" "release\blocked_tech_schools.txt"

rem hg log > Release/log.txt
hg log --template "Change:\t\t{rev}: {node}\nAuthor:\t\t{author}\nDescription:\t{desc}\nDate:\t\t{date|isodate}\nBranch:\t\t{branch}\n***\n" > Release/log.txt
(for /f "delims=" %%i in (release/log.txt) do @echo %%i)>release/changelog.txt
del release\log.txt

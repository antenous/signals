# Add <target>-config.cmake.in next to this module with the following content:
#
# @PACKAGE_INIT@
# include(${CMAKE_CURRENT_LIST_DIR}/@targets_export_name@.cmake)
# check_required_components(@PROJECT_NAME@)

function(target_install target)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    set(cmake_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${target})
    set(version_config ${PROJECT_BINARY_DIR}/${target}-config-version.cmake)
    set(project_config_name ${target}-config.cmake)
    set(project_config ${PROJECT_BINARY_DIR}/${project_config_name})
    set(targets_export_name ${target}-targets)

    # Generate the version, config and target files in the build directory
    write_basic_package_version_file(
        ${version_config}
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion)

    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/${project_config_name}.in
        ${project_config}
        INSTALL_DESTINATION ${cmake_dir})

    export(TARGETS ${target} NAMESPACE ${target}::
        FILE ${PROJECT_BINARY_DIR}/${targets_export_name}.cmake)

    # Install version, config and target files
    install(
        FILES ${project_config} ${version_config}
        DESTINATION ${cmake_dir})
    install(EXPORT ${targets_export_name} DESTINATION ${cmake_dir}
        NAMESPACE ${target}::)

    # Install the library and headers
    install(TARGETS ${target} EXPORT ${targets_export_name}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${target}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endfunction()

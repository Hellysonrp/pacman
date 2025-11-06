# Check and install missing packages

function(check_package_installed PACKAGE_NAME PKG_CHECK_CMD RESULT_VAR)
    execute_process(
        COMMAND sh -c "${PKG_CHECK_CMD} ${PACKAGE_NAME}"
        OUTPUT_QUIET
        ERROR_QUIET
        RESULT_VARIABLE PKG_RESULT
    )
    set(${RESULT_VAR} ${PKG_RESULT} PARENT_SCOPE)
endfunction()

function(install_missing_packages)
    if(NOT PKG_MANAGER)
        message(WARNING "Package manager not detected. Please install dependencies manually.")
        return()
    endif()
    
    message(STATUS "Detected package manager: ${PKG_MANAGER}")
    message(STATUS "Required packages: ${REQUIRED_PACKAGES}")
    
    set(MISSING_PACKAGES)
    foreach(PKG ${REQUIRED_PACKAGES})
        check_package_installed(${PKG} "${PKG_CHECK}" PKG_RESULT)
        if(NOT PKG_RESULT EQUAL 0)
            list(APPEND MISSING_PACKAGES ${PKG})
            message(STATUS "Missing: ${PKG}")
        else()
            message(STATUS "Found: ${PKG}")
        endif()
    endforeach()
    
    if(MISSING_PACKAGES)
        message(STATUS "Installing missing packages: ${MISSING_PACKAGES}")
        execute_process(COMMAND sh -c "${PKG_UPDATE_CMD}" RESULT_VARIABLE UPDATE_RESULT)
        
        set(FAILED_PACKAGES)
        foreach(PKG ${MISSING_PACKAGES})
            message(STATUS "Installing ${PKG}...")
            execute_process(
                COMMAND sh -c "${PKG_INSTALL_CMD} ${PKG}"
                RESULT_VARIABLE INSTALL_RESULT
                OUTPUT_QUIET
                ERROR_QUIET
            )
            if(NOT INSTALL_RESULT EQUAL 0)
                list(APPEND FAILED_PACKAGES ${PKG})
                message(WARNING "Package ${PKG} not available in package manager. Will try to find installed version.")
            endif()
        endforeach()
        
        if(FAILED_PACKAGES)
            message(STATUS "Some packages may need manual installation: ${FAILED_PACKAGES}")
        endif()
    else()
        message(STATUS "All dependencies are installed")
    endif()
endfunction()


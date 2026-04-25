#!/bin/sh

# For debugging purposes
#set -x

does_env_var_contain_path() {
    _env_var="$1"
    _path="$2"
    echo "${_env_var}" | grep -qF "${_path}"
}

main() {
    GIT_INSTALL_DIR=$(powershell.exe -Command \
        "(Get-ItemProperty -Path 'HKLM:\\SOFTWARE\\GitForWindows').InstallPath" | tr -d '\r')
    GIT_CMD_PATH="${GIT_INSTALL_DIR}\\cmd"
    echo "Path to directory containing the 'git' binary: ${GIT_CMD_PATH}"

    SYS_PATH=$(powershell.exe -Command \
        "[System.Environment]::GetEnvironmentVariable('Path', 'Machine')" | tr -d '\r')
    if does_env_var_contain_path "${SYS_PATH}" "${GIT_CMD_PATH}"; then
        echo "Already present in System 'Path' variable — skipping."
        echo "  Verify: System Properties > Environment Variables > System variables > Path"
        exit 0
    fi

    USER_PATH=$(powershell.exe -Command \
        "[System.Environment]::GetEnvironmentVariable('Path', 'User')" | tr -d '\r')
    if does_env_var_contain_path "${USER_PATH}" "${GIT_CMD_PATH}"; then
        echo "Already present in User 'Path' variable — skipping."
        echo "  Verify: System Properties > Environment Variables > User variables > Path"
        exit 0
    fi

    powershell.exe -Command \
        "[System.Environment]::SetEnvironmentVariable('Path', '${USER_PATH};${GIT_CMD_PATH}', 'User')"
    echo "Added to User PATH."
    echo "  Verify: System Properties > Environment Variables > User variables > Path"
}

main

set +x

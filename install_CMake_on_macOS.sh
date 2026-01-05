#!/bin/sh

set +x

main() {
    # --- Download CMake installer

    CMAKE_URL_MAC="$(curl --silent https://api.github.com/repos/Kitware/CMake/releases/latest | grep "browser_download_url.*macos-universal.dmg" | cut -d '"' -f 4)"
    echo "${CMAKE_URL_MAC}"

    CMAKE_DOWNLOADED_FILE_MAC="$(echo "${CMAKE_URL_MAC}" | cut -d / -f 9)"
    echo "${CMAKE_DOWNLOADED_FILE_MAC}"

    echo "${TMPDIR}"
    curl --location --output "${TMPDIR}/${CMAKE_DOWNLOADED_FILE_MAC}" "${CMAKE_URL_MAC}"
    ls -l "${TMPDIR}/${CMAKE_DOWNLOADED_FILE_MAC}"

    # --- Download hashes file for integrity validation of the installer

    CMAKE_URL_HASHES="$(curl --silent https://api.github.com/repos/Kitware/CMake/releases/latest | grep "browser_download_url.*SHA-256\.txt\"" | cut -d \" -f 4)"
    echo "${CMAKE_URL_HASHES}"

    CMAKE_DOWNLOADED_HASHES="$(echo "${CMAKE_URL_HASHES}" | cut -d / -f 9)"
    echo "${CMAKE_DOWNLOADED_HASHES}"

    curl --location --output "${TMPDIR}/${CMAKE_DOWNLOADED_HASHES}" "${CMAKE_URL_HASHES}"
    ls -l "${TMPDIR}/${CMAKE_DOWNLOADED_HASHES}"

    # --- Verify integrity of the installer

    CMAKE_DOWNLOADED_FILE_MAC_HASH="$(shasum --algorithm 256 "${TMPDIR}/${CMAKE_DOWNLOADED_FILE_MAC}")"
    echo "${CMAKE_DOWNLOADED_FILE_MAC_HASH}"

    CMAKE_DOWNLOADED_FILE_MAC_HASH_ACTUAL="$(echo "${CMAKE_DOWNLOADED_FILE_MAC_HASH}" | cut -d ' ' -f 1)"
    echo "${CMAKE_DOWNLOADED_FILE_MAC_HASH_ACTUAL}"

    echo "${CMAKE_DOWNLOADED_FILE_MAC}"
    if grep "${CMAKE_DOWNLOADED_FILE_MAC_HASH_ACTUAL}" "${TMPDIR}/${CMAKE_DOWNLOADED_HASHES}" | grep "${CMAKE_DOWNLOADED_FILE_MAC}"; then
        echo "OK: Expected and actual checksums are matching: installer file approved and legitimate. Running installer..."
        open "${TMPDIR}/${CMAKE_DOWNLOADED_FILE_MAC}"
    else
        echo "WARNING: CMake installer file corrupted. Redownload the file."
    fi

    # --- Make CMake accessible from the Terminal, not only the CMake GUI app

    # shellcheck disable=SC2016
    PATH_SYSVAR_WITH_CMAKE_PATH_PREPENDED='export PATH="/Applications/CMake.app/Contents/bin:$PATH"'
    echo "${PATH_SYSVAR_WITH_CMAKE_PATH_PREPENDED}"
    if grep --quiet --line-regexp --fixed-strings "${PATH_SYSVAR_WITH_CMAKE_PATH_PREPENDED}" "${HOME}/.zshrc"; then
        echo "CMake path already present in '${HOME}/.zshrc'"
    else
        echo "${PATH_SYSVAR_WITH_CMAKE_PATH_PREPENDED}" >> "${HOME}/.zshrc"
        echo "CMake path added to '${HOME}/.zshrc'"
    fi
}

main

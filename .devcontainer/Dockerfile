FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.22.2"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt-get -y install --no-install-recommends libreadline-dev clang-format astyle \
     clang-tidy

# Copy .astylerc to vscode user's home so astyle auto-loads it
COPY ./.astylerc /home/vscode/.astylerc
# ensure ownership
RUN chown vscode:vscode /home/vscode/.astylerc

# install codecrafters crap
RUN curl -fsSL https://codecrafters.io/install.sh -o /tmp/install.sh \
    && bash /tmp/install.sh \
    && rm /tmp/install.sh
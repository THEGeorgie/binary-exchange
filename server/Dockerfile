# Use Alpine Linux as the base image
FROM alpine:latest

# Enable the 'community' and 'testing' repositories
RUN echo "http://dl-cdn.alpinelinux.org/alpine/edge/community" >> /etc/apk/repositories
RUN echo "http://dl-cdn.alpinelinux.org/alpine/edge/testing" >> /etc/apk/repositories

# Update the package list
RUN apk update

# Install essential build tools (gcc, make, musl-dev)
RUN apk add --no-cache build-base

# Install CMake (from community repo)
RUN apk add --no-cache cmake

# Install Git (for version control)
RUN apk add --no-cache git

# Install GDB (from testing repo)
RUN apk add --no-cache gdb

# Allow GDB to attach to processes
RUN echo "kernel.yama.ptrace_scope = 0" >> /etc/sysctl.conf

# Create a non-root user (recommended for security)
RUN adduser -D dev

# Allow the user to use sudo without a password
RUN echo "dev ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# Switch to the non-root user
USER dev
# Set the working directory
WORKDIR /home/dev/project
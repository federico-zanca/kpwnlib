# kpwn 
# Linux kernel exploit library
#
# Build:
#   make                  build with musl-gcc  (default)
#   make CC=gcc           build with glibc
#   make fuse             include FUSE module  (needs libfuse-dev)
#   make debug            debug build (-Og -g -DDEBUG)
#
# Install:
#   sudo make install     install to PREFIX    (default: /usr/local)
#   sudo make uninstall   remove installed files
#
# Compile your exploit:
#   musl-gcc -static exploit.c -o exploit -lkpwn


# Toolchain 
CC       ?= musl-gcc
AR       ?= ar
ARFLAGS   = rcs

# Flags
CFLAGS   ?= -O0 -Wall -Wextra -fPIC -masm=intel
CPPFLAGS += -Iinclude

# Paths
PREFIX   ?= /usr/local
BUILDDIR  = build
LIBNAME   = libkpwn.a

# Sources 
# Collect all .c files from src/, then filter out optional modules.
SRCS := $(wildcard src/*.c)

# fuse.c needs -DCONFIG_FUSE and libfuse-dev.
# net.c  needs linux kernel headers (linux/rtnetlink.h etc.).
# Both are excluded by default; opt in with: make fuse / make net
# Deprecated sources kept as shims but not built: state.c/ret2user.c/kpti.c
SRCS := $(filter-out src/fuse.c src/net.c src/state.c src/ret2user.c src/kpti.c, $(SRCS))

OBJS := $(patsubst src/%.c, $(BUILDDIR)/%.o, $(SRCS))
LIB   = $(BUILDDIR)/$(LIBNAME)

# Default target 
all: $(LIB)
	@echo ""
	@echo "  $(LIB) built successfully"
	@echo "  run   sudo make install   to install system-wide"
	@echo ""

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: src/%.c | $(BUILDDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

# Debug build 
debug:
	$(MAKE) CFLAGS="-Og -g -Wall -Wextra -fPIC -masm=intel -DDEBUG"

# Optional: FUSE support 
# Adds fuse.c back into the build.  Your exploit must also link:
#   -lfuse -lpthread
fuse:
	$(MAKE) CPPFLAGS="$(CPPFLAGS) -DCONFIG_FUSE -D_FILE_OFFSET_BITS=64" \
	        SRCS="$(wildcard src/*.c)"

# Optional: Netlink / TC module
# Needs linux kernel headers (linux/rtnetlink.h, etc.).
# With musl, install linux-headers or copy them manually.
net:
	$(MAKE) SRCS="$(filter-out src/fuse.c, $(wildcard src/*.c))"

# Install/Uninstall
install: $(LIB)
	@echo "Installing kpwn to $(DESTDIR)$(PREFIX) ..."
	install -d $(DESTDIR)$(PREFIX)/lib
	install -d $(DESTDIR)$(PREFIX)/include/kpwn
	install -m 644 $(LIB)                   $(DESTDIR)$(PREFIX)/lib/
	install -m 644 include/kpwn/*.h         $(DESTDIR)$(PREFIX)/include/kpwn/
	@echo "Done.  Use:  #include <kpwn/kpwn.h>  and link with  -lkpwn"

uninstall:
	rm -f  $(DESTDIR)$(PREFIX)/lib/$(LIBNAME)
	rm -rf $(DESTDIR)$(PREFIX)/include/kpwn
	@echo "kpwn uninstalled from $(DESTDIR)$(PREFIX)"

# Clean
clean:
	rm -rf $(BUILDDIR)

# Help
help:
	@echo ""
	@echo "kpwn — Linux kernel exploit library"
	@echo ""
	@echo "  make                Build with musl-gcc (default)"
	@echo "  make CC=gcc         Build with glibc"
	@echo "  make debug          Build with debug symbols (-Og -g)"
	@echo "  make fuse           Build with FUSE support"
	@echo "  make install        Install to PREFIX (default: /usr/local)"
	@echo "  make uninstall      Remove installed files"
	@echo "  make clean          Remove build artifacts"
	@echo ""

.PHONY: all debug fuse install uninstall clean help

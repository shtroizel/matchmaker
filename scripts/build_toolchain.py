#!/usr/bin/env python3

import fcntl
import getopt
import hashlib
import multiprocessing
import os
import shutil
import subprocess
import sys
import tarfile
import urllib.request

from shutil import which


LLVM_VERSION = '23.1.6'
LLVM_TARBALL = 'llvm-project-' + LLVM_VERSION + '.src.tar.xz'
LLVM_SHA256 = 'd3b86ab29cfb4445a6652e5c2642ccc75e04e38084dde3701d323320f35a31da'
LLVM_RELEASES = 'https://github.com/llvm/llvm-project/releases/download/'
LLVM_URL = LLVM_RELEASES + 'llvmorg-' + LLVM_VERSION + '/' + LLVM_TARBALL

# a change here names a new toolchain directory, so the toolchain is built again
LLVM_SETTINGS = [
    '-DCMAKE_BUILD_TYPE=Release',
    '-DLLVM_ENABLE_PROJECTS=clang;lld',
    '-DLLVM_ENABLE_RUNTIMES=libcxx;libcxxabi;libunwind',
    '-DLLVM_TARGETS_TO_BUILD=Native',
    '-DLLVM_ENABLE_ASSERTIONS=OFF',
    '-DLLVM_INCLUDE_TESTS=OFF',
    '-DLLVM_INCLUDE_BENCHMARKS=OFF',
    '-DLLVM_INCLUDE_EXAMPLES=OFF',
    '-DLLVM_INCLUDE_DOCS=OFF',
    '-DCLANG_INCLUDE_TESTS=OFF',
    '-DCLANG_INCLUDE_DOCS=OFF',
    '-DLLVM_ENABLE_ZSTD=OFF',
    '-DLLVM_ENABLE_LIBXML2=OFF',
    '-DLLVM_ENABLE_ZLIB=OFF',
]

COMPLETE = 'matchmaker_toolchain_complete'
LINK_GIB = 8



def usage():
    print('options for building the pinned toolchain (LLVM ' + LLVM_VERSION + '):\n')
    print('    -h, --help                    print this message\n')
    print('    -j  --jobs                    max jobs')
    print('                                    * default is cpu count [' +                                 \
            str(multiprocessing.cpu_count()) + ']\n')
    print('    -p, --print_dir               print the toolchain\'s directory and exit')
    print('                                    * nothing is built\n')
    print('    --tarball=<file>              the LLVM source release, when it is already here')
    print('                                    * ' + LLVM_TARBALL)
    print('                                    * downloaded from the LLVM release page by default\n')
    print('the toolchain is built once per user, into ' + toolchain_dir())
    print('    * MM_TOOLCHAIN_CACHE in the environment names another parent directory')
    print('    * clang, lld, the LLVM libraries, libc++, libc++abi and libunwind')
    print('    * about 7 CPU hours, 16 GB of memory at 12 jobs and 6 GB of disk while building\n')



def cache_dir():
    if os.environ.get('MM_TOOLCHAIN_CACHE'):
        return os.path.abspath(os.environ['MM_TOOLCHAIN_CACHE'])
    base = os.environ.get('XDG_CACHE_HOME') or os.path.join(os.path.expanduser('~'), '.cache')
    return os.path.join(base, 'matchmaker', 'toolchain')



def toolchain_dir():
    settings = hashlib.sha256('\n'.join(LLVM_SETTINGS).encode()).hexdigest()[:10]
    return os.path.join(cache_dir(), 'llvm-' + LLVM_VERSION + '-' + settings)



def is_complete(toolchain):
    return os.path.exists(os.path.join(toolchain, COMPLETE))



def sha256_of(path):
    digest = hashlib.sha256()
    with open(path, 'rb') as f:
        for block in iter(lambda: f.read(1 << 20), b''):
            digest.update(block)
    return digest.hexdigest()



def fetch(tarball, work):
    if tarball is None:
        tarball = os.path.join(work, LLVM_TARBALL)
        if not os.path.exists(tarball) or sha256_of(tarball) != LLVM_SHA256:
            print('downloading ' + LLVM_URL, flush=True)
            with urllib.request.urlopen(LLVM_URL) as response, open(tarball + '.part', 'wb') as f:
                shutil.copyfileobj(response, f)
            os.replace(tarball + '.part', tarball)
    found = sha256_of(tarball)
    if found != LLVM_SHA256:
        print(tarball + ' is not the pinned LLVM release')
        print('    sha256 ' + found)
        print('    pinned ' + LLVM_SHA256)
        return None
    return tarball



def unpack(tarball, work):
    src = os.path.join(work, 'src')
    shutil.rmtree(src, ignore_errors=True)
    print('unpacking ' + tarball, flush=True)
    with tarfile.open(tarball) as tar:
        if hasattr(tarfile, 'tar_filter'):
            tar.extractall(src, filter='tar')
        else:
            tar.extractall(src)
    return os.path.join(src, os.listdir(src)[0], 'llvm')



# a link of clang takes several GiB, so only as many run at once as fit in memory
def link_jobs(jobs):
    try:
        gib = os.sysconf('SC_PAGE_SIZE') * os.sysconf('SC_PHYS_PAGES') // (1 << 30)
    except (ValueError, OSError):
        return 1
    return max(1, min(int(jobs), gib // 2 // LINK_GIB))



def build(toolchain, jobs, tarball):
    for tool in ['cmake', 'ninja']:
        if which(tool) is None:
            print('building the toolchain needs ' + tool)
            return False
    work = toolchain + '.work'
    os.makedirs(work, exist_ok=True)
    tarball = fetch(tarball, work)
    if tarball is None:
        return False
    llvm_src = unpack(tarball, work)

    shutil.rmtree(toolchain, ignore_errors=True)
    build_dir = os.path.join(work, 'build')
    shutil.rmtree(build_dir, ignore_errors=True)
    cmake_cmd = ['cmake', '-S', llvm_src, '-B', build_dir, '-G', 'Ninja',
                 '-DCMAKE_INSTALL_PREFIX=' + toolchain,
                 '-DLLVM_PARALLEL_LINK_JOBS=' + str(link_jobs(jobs))]
    for linker in ['gold', 'lld']:
        if which('ld.' + linker) is not None:
            cmake_cmd.append('-DLLVM_USE_LINKER=' + linker)
            break
    env = {k: v for k, v in os.environ.items() if k != 'CMAKE_TOOLCHAIN_FILE'}
    for cmd in [cmake_cmd + LLVM_SETTINGS, ['ninja', '-C', build_dir, '-j', str(jobs), 'install']]:
        if subprocess.run(cmd, env=env).returncode != 0:
            print('building the toolchain failed; what was built is kept in ' + work)
            return False
    with open(os.path.join(toolchain, COMPLETE), 'w') as f:
        f.write('LLVM ' + LLVM_VERSION + '\n' + '\n'.join(LLVM_SETTINGS) + '\n')
    shutil.rmtree(work, ignore_errors=True)
    return True



# the toolchain's directory, built first when it isn't there yet; None when the build fails
def ensure(jobs, tarball=None):
    toolchain = toolchain_dir()
    if is_complete(toolchain):
        return toolchain
    os.makedirs(cache_dir(), exist_ok=True)
    with open(toolchain + '.lock', 'w') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        if is_complete(toolchain):
            return toolchain
        print('\n\nbuilding the pinned toolchain (LLVM ' + LLVM_VERSION + ') into ' + toolchain)
        print('this happens once and takes about 7 CPU hours\n\n', flush=True)
        if not build(toolchain, jobs, tarball):
            return None
    return toolchain



def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hj:p', ['help', 'jobs=', 'print_dir', 'tarball='])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    jobs = str(multiprocessing.cpu_count())
    print_dir = False
    tarball = None

    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-j', '--jobs'):
            jobs = a
        elif o in ('-p', '--print_dir'):
            print_dir = True
        elif o == '--tarball':
            tarball = os.path.abspath(a)
        else:
            assert False, "unhandled option"

    if not jobs.isdigit() or int(jobs) < 1:
        print('--jobs must be a whole number of at least 1')
        sys.exit(2)
    if print_dir:
        print(toolchain_dir())
        sys.exit(0)
    if tarball is not None and not os.path.exists(tarball):
        print('--tarball must name the LLVM source release: no ' + tarball)
        sys.exit(2)

    toolchain = ensure(jobs, tarball)
    if toolchain is None:
        sys.exit(1)
    print(toolchain)
    exit(0)



if __name__ == "__main__":
    main()

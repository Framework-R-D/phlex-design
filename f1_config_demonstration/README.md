# Demonstration Satisfying F1 "Configuration" Milestone

## Charge and discussion

https://github.com/orgs/Framework-R-D/discussions/1

## Setup

Either:

1. Obtain and install a recent version of Spack.

1. Obtain and configure Spack to use the `fnal_art` external recipe
   repository:
   ```
   git clone https://github.com/FNALssi/fnal_art.git
   git -C fnal_art switch no-jsonnet # Temporary, due to Spack bug.
   spack repo add --scope site fnal_art/
   ```

1. Install GCC 14.2.0 and register it as a compiler known to Spack, e.g.:
   ```
   curl -OJL --fail \
   https://scisoft.fnal.gov/scisoft/bundles/gcc/v14_2_0/buildcfg/gcc-14-2-0-gcc-11.yaml
   spack env create gcc-14-2-0 gcc-14-2-0-gcc-11.yaml
   spack -e gcc-14-2-0 install
   spack compiler add $(spack -e gcc-14-2-0 location -i gcc@14.2.0)
   ```

1. Obtain and install dependencies, e.g.:
   ```
   curl -OJL --fail \
   https://scisoft.fnal.gov/scisoft/bundles/f1_config_demonstrator/v1_0/buildcfg/f1_config_demonstrator-1-0-gcc-14-2-0-cxx20-prof-gcc-11.yaml
   spack env create f1_config_demonstrator f1_config_demonstrator-1-0-gcc-14-2-0-cxx20-prof-gcc-11.yaml
   spack -e f1_config_demonstrator install
   ```

1. Activate the installed environment:
   ```
   spack env activate f1_config_demonstrator
   ```

Or:

1. Obtain the `build-spack-env.sh` script:
   ```
   curl -OJL --fail \
   https://raw.githubusercontent.com/FNALssi/fermi-spack-tools/refs/heads/main/bin/build-spack-env.sh
   chmod a+x build-spack-env.sh
   ```

1. Invoke it with suitable options to build GCC and the dependency environments
   ```
   ./build-spack-env.sh \
   --spack-repo='https://github.com/FNALssi/fnal_art|no-jsonnet' \
   --cache-write-sources \
   --clear-mirrors \
   --cache-write-binaries=all \
   --no-emergency-buildcache \
   --no-fail-fast \
   --query-packages \
   --no-safe-concretize \
   --spack-version=fnal-develop \
   --upgrade-recipes \
   --upgrade-spack \
   --verbosity=PROGRESS \
   --with-cache=\
   'scisoft_mirror_sources|https://scisoft.fnal.gov/scisoft/spack-mirror/spack-packages/sources' \
   --with-cache=\
   'scisoft_mirror_compilers|https://scisoft.fnal.gov/scisoft/spack-mirror/spack-compiler-cache-plain' \
   --with-cache=\
   'scisoft_mirror_binaries|https://scisoft.fnal.gov/scisoft/spack-mirror/spack-binary-cache-plain' \
   --with-padding \
   https://scisoft.fnal.gov/scisoft/bundles/gcc/v14_2_0/buildcfg/gcc-14-2-0-gcc-11.yaml \
   https://scisoft.fnal.gov/scisoft/bundles/f1_config_demonstrator/v1_0/buildcfg/f1_config_demonstrator-1-0-gcc-14-2-0-cxx20-prof-gcc-11.yaml
   ```

1. Activate the installed environment:
   ```
   . spack_env/setup-env.sh
   spack env activate f1_config_demonstrator      
   ```

## Compilation and development

1. Make and enter a directory in which to build the code.

1. Configure:
   ```
   cmake -DCMAKE_CXX_COMPILER=$(spack location -i gcc@14.2.0) \
         -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING:BOOL=ON -GNinja \
         <path-to-f1_config_demonstrator-source>
   ```

1. Build:
   ```
   ninja
   ```

1. Test:
   ```
   ctest --output-on-failure
   ```

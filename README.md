# Races and Barriers

## Initialization

* An "auto-load safe-path" should be set in ~/.gdbinit, either / or a path to this directory:

```shell
echo "set auto-load safe-path /" >> ~/.gdbinit
```

* Install libraries required for Qemu and Buildroot:

```shell
# Fedora
sudo dnf install \
  ncurses-devel perl-ExtUtils-MakeMaker ninja-build \
  glib2-devel pixman-devel libslirp-devel \
  flex bison perl \
  gcc gcc-c++ make

# Ubuntu
sudo apt install \
  libncurses-dev libextutils-makemaker-cpanfile-perl ninja-build \
  libglib2.0-dev libpixman-1-dev libslirp-dev \
  flex bison perl \
  gcc g++ make
```

* Build everything:

```shell
./init_build.sh
```

---

## Running Optimization Demo

* Start Qemu

```shell
./8_run.sh
```

* Enter login `root` (without password)
* Run the compiler barriers demo

```shell
/opt/compiler_opts
```

* Run the CPU barriers demo

```shell
/opt/cpu_opts
```

* Read and edit the code

```shell
$EDITOR demo_races/compiler_opts.c
$EDITOR demo_races/cpu_opts.c
```

* Rebuild and re-run

```shell
./rebuild_rerun.sh
```

* Exit Qemu with key combination `Ctrl + A, X` or `poweroff`

* Run on a local machine:

```
% ./output/cpu_opts
without barriers: !A!B    942, !AB     24, A!B     33, AB      0
with barriers: !A!B      0, !AB      2, A!B      4, AB    993
%
```

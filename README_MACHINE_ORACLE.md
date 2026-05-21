# Machine Oracle: syscall -> SBI -> M-mode CSR

## Проделанная работа

Я реализовала небольшую учебную цепочку, которая проходит из userspace до OpenSBI в M-mode и возвращает значение обратно в приложение, добавила новый Linux syscall `machine_oracle`, новый SBI extension в OpenSBI и userspace-приложение `/opt/machine_oracle`. В OpenSBI читается machine-mode CSR `CSR_MVENDORID`, а приложение печатает значение, которое пришло обратно через SBI и syscall.

Главная идея простая: приложение само не читает machine CSR. Оно просит ядро, ядро делает SBI-вызов, а реальное чтение происходит в OpenSBI, где есть M-mode.

## Как работает цепочка

```text
/opt/machine_oracle
    -> syscall(__NR_machine_oracle)
    -> sys_machine_oracle()
    -> sbi_ecall(0x0900CAFE, 0)
    -> OpenSBI Machine Oracle extension
    -> csr_read(CSR_MVENDORID)
    -> return value
    -> printf()
```

## Что изменено в коде

### OpenSBI

- `opensbi/include/sbi/sbi_ecall_interface.h` - добавила ID расширения `SBI_EXT_MACHINE_ORACLE = 0x0900CAFE` и function IDs для чтения CSR.
- `opensbi/lib/sbi/sbi_ecall_machine_oracle.c` - реализовала обработчик SBI extension; function ID `0` читает `CSR_MVENDORID`.
- `opensbi/lib/sbi/objects.mk` - подключила новый обработчик к сборке OpenSBI и к списку ecall extensions.

### Linux kernel

- `linux/scripts/syscall.tbl` - зарегистрировала syscall `machine_oracle` с номером `471`.
- `linux/arch/riscv/include/asm/sbi.h` - добавила Linux-константы для `0x0900CAFE` и function ID `0`.
- `linux/arch/riscv/kernel/sys_riscv.c` - реализовала `SYSCALL_DEFINE0(machine_oracle)`, который вызывает `sbi_ecall()`.

### userspace/rootfs

- `demo_races/machine_oracle.c` - добавила userspace-приложение, которое вызывает `syscall(__NR_machine_oracle)` и печатает результат.
- `6_demos.sh` - добавила сборку бинарника в `rootfs_overlay/opt/machine_oracle`, чтобы после пересборки rootfs он оказался в `/opt/machine_oracle`.

### scripts/docs

- `5_opensbi.sh` - оставила новый файл OpenSBI handler вне удаления через `git clean`, чтобы учебный обработчик не исчезал перед сборкой.
- `README_MACHINE_ORACLE.md` - основной документ для демонстрации.
- `NOTES_MACHINE_ORACLE.md` - короткая ссылка на этот README, без дублирования.
- `demo-machine-oracle.log` - лог runtime-проверки в QEMU.

## Технические детали

- syscall name: `machine_oracle`
- syscall number: `471`
- SBI extension ID: `0x0900CAFE`
- function ID: `0`
- function name: `SBI_EXT_MACHINE_ORACLE_READ_MVENDORID`
- CSR: `CSR_MVENDORID`
- userspace path: `/opt/machine_oracle`

## Почему CSR читается именно в OpenSBI

Userspace-приложение работает в U-mode, Linux kernel работает в S-mode, а OpenSBI работает в M-mode. Machine-mode CSR нельзя честно читать напрямую из userspace, и идея задания как раз в том, чтобы дойти до M-mode. Поэтому приложение делает syscall, ядро делает SBI-вызов, а `csr_read(CSR_MVENDORID)` выполняется уже в OpenSBI.

Так я демонстрирую не просто новый syscall, а полный переход между уровнями привилегий: U-mode -> S-mode -> M-mode -> обратно.

## Как пересобрать

Для обычной сборки проект после изменений запускается из корня репозитория:

```sh
./5_opensbi.sh
./3_linux_prepare.sh
./4_linux.sh
./6_demos.sh
./7_rootfs.sh
```

Коротко по смыслу:

- `./5_opensbi.sh` пересобирает OpenSBI.
- `./3_linux_prepare.sh` подготавливает Linux tree.
- `./4_linux.sh` пересобирает ядро.
- `./6_demos.sh` собирает `/opt/machine_oracle` в rootfs overlay.
- `./7_rootfs.sh` пересобирает rootfs.

## Как запустить

```sh
./8_run.sh
```

Этот скрипт запускает QEMU с OpenSBI, ядром Linux и initramfs/rootfs из `output/`.

## Демонстрация работы

Внутри QEMU я показываю:

```sh
root
uname -a
ls -lh /opt/machine_oracle
/opt/machine_oracle
```

Ожидаемый смысл вывода: Linux загрузился, бинарник существует, приложение запускается и печатает `mvendorid`, полученный из M-mode.

## Проверка в runtime

Я проверила запуск внутри QEMU и сохранила лог в `demo-machine-oracle.log`. В локальном дереве не было полного buildroot SDK и локально собранного QEMU, поэтому runtime-проверка выполнялась с системным `/usr/bin/qemu-system-riscv64` и минимальным initramfs. Это не меняет суть проверки: цепочка `syscall -> SBI -> OpenSBI M-mode -> CSR -> userspace` реально прошла внутри QEMU.

Ключевой фрагмент лога:

```text
OpenSBI v1.8
Linux (none) 6.19.0-dirty #1 SMP PREEMPT Thu May 21 03:26:01 MSK 2026 riscv64
# ls -lh /opt/machine_oracle
-rwxr-xr-x 1 root root 556712 /opt/machine_oracle
# /opt/machine_oracle
Machine Oracle says:
mvendorid from M-mode = 0x0000000000000000
machine_oracle exit status: 0
RUNTIME CHECK PASSED
```

По этому логу видно, что:

- OpenSBI загрузился.
- Linux загрузился.
- `/opt/machine_oracle` есть внутри гостевой системы.
- Приложение запустилось.
- Syscall не вернул `ENOSYS`.
- SBI-вызов не упал.
- Значение `mvendorid` вернулось в userspace.
- Приложение завершилось с `exit status: 0`.

## Как показать, что это не просто printf

На хосте можно показать такие команды:

```sh
grep -R "0900CAFE" -n .
grep -R "machine_oracle" -n .
grep -R "CSR_MVENDORID" -n .
```

По ним видно, что:

- `0x0900CAFE` есть и в Linux, и в OpenSBI.
- `machine_oracle` зарегистрирован как syscall и вызывается из userspace.
- В OpenSBI есть отдельный handler `sbi_ecall_machine_oracle.c`.
- Реальное чтение CSR сделано через `csr_read(CSR_MVENDORID)`.


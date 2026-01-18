#### Environment:

```
clang llvm lld qemu-system-riscv32 curl
```

Ubuntu

```
sudo apt update && sudo apt install -y clang llvm lld qemu-system-riscv32 curl
```



#### Execute in os1k directory:

Prepare OpenSBI firmware

```
curl -LO https://github.com/qemu/qemu/raw/v8.0.4/pc-bios/opensbi-riscv32-generic-fw_dynamic.bin
```

Create disk

```
mkdir -p disk
echo "Hello from host" > disk/hello.txt
```

Build && Run

```
./run
```


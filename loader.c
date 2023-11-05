#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;
void *virtual_addr;

/*
 * release memory and other cleanups
 */
void loader_cleanup()
{
  close(fd);
  free(ehdr);
  free(phdr);
}

int mapped = 0;
size_t fragment_size = 0;
void sigsegv_handler(int signo, siginfo_t *info, void *context)
{
  printf("Received SIGSEGV - Segmentation Fault\n");
  printf("Address causing segmentation fault: %p\n", info->si_addr);

  for (int i = 0; i < ehdr->e_phnum; i++)
  {
    Elf32_Word segment_type = (phdr + i)->p_type;
    size_t segment_size = (phdr + i)->p_memsz;
    segment_size /= PAGESZ;
    segment_size++;
    segment_size *= PAGESZ;
    size_t start_addr = (phdr + i)->p_vaddr;
    start_addr /= PAGESZ;
    start_addr *= PAGESZ;
    size_t offset_corr = start_addr - (phdr + i)->p_vaddr;
    printf("segment addr %p %p size %ld %ld offset correction %ld\n", (phdr + i)->p_vaddr, start_addr, (phdr + i)->p_memsz, segment_size, offset_corr);

    if (segment_type == PT_LOAD && (phdr + i)->p_vaddr <= info->si_addr && ((phdr + i)->p_vaddr + (phdr + i)->p_memsz) >= info->si_addr)
    {
      void *virtual_addr = mmap((void *)start_addr, segment_size, PROT_READ | PROT_WRITE | PROT_EXEC | PROT_GROWSUP | PROT_GROWSDOWN, MAP_PRIVATE, fd, (phdr + i)->p_offset + offset_corr);
      if (virtual_addr == MAP_FAILED)
      {
        perror("mmap");
        exit(EXIT_FAILURE);
      }
      mapped++;
      fragment_size += segment_size - (phdr + i)->p_memsz;
      printf("mapped addr %p size %d\n", start_addr, segment_size);
      break;
    }
  }
  sleep(1);
  return;
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char **exe)
{
  struct sigaction sa;
  sa.sa_sigaction = sigsegv_handler;
  sa.sa_flags = SA_SIGINFO;

  if (sigaction(SIGSEGV, &sa, NULL) == -1)
  {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }
  // 1. Load entire binary content into the memory from the ELF file.
  if (fd = open(exe[1], O_RDONLY), fd == -1)
  {
    perror("open elf file");
    exit(1);
  }

  ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
  if (read(fd, ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr))
  {
    perror("read elf header");
    close(fd);
    exit(1);
  }

  phdr = (Elf32_Phdr *)malloc(ehdr->e_phnum * sizeof(Elf32_Phdr));
  if (lseek(fd, ehdr->e_phoff, SEEK_SET) == -1)
  {
    perror("lseek");
    close(fd);
    exit(1);
  }
  if (read(fd, phdr, sizeof(Elf32_Phdr) * ehdr->e_phnum) != sizeof(Elf32_Phdr) * ehdr->e_phnum)
  {
    perror("read program header");
    close(fd);
    exit(1);
  }

  // 2. Iterate through the PHDR table and find the section of PT_LOAD
  //    type that contains the address of the entrypoint method in fib.c

  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
  int (*_start)() = (int (*)())ehdr->e_entry;


  // 6. Call the "_start" method and print the value returned from the "_start"
  int result = _start();
  printf("User _start return value = %d\n", result);

  printf("the total page faults were %d\n", mapped);
  printf("the total fragmented memory were %d\n", fragment_size);
}

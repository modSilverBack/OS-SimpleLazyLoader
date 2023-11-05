# OS-SimpleLazyLoader
extention of the simple loader but it loads the segments only before they are needed
Contribution
Abhishek Benniwal(2022022)-Memory mapping ,load and run elf.
Shivam Pandey(2022477)- Memmory Cleanup, readme.
rest done together

This C code implements an ELF loader with a custom signal handler for segmentation faults. The load_and_run_elf function loads an ELF executable, maps memory segments for PT_LOAD sections on demand, and runs the entry point function "_start". The sigsegv_handler function is triggered on segmentation faults, dynamically mapping memory for the faulted address.The program dynamically maps memory for ELF executable sections during execution based on page faults. Signal Handling: Custom signal handler (sigsegv_handler) catches segmentation faults (SIGSEGV) and maps memory for the faulted address. Memory Cleanup: The loader_cleanup function releases resources, closes the file descriptor, and frees allocated memory. Page Fault Tracking: The global variable mapped tracks the total number of page faults during execution. Fragmented Memory: fragment_size records the total fragmented memory caused by mapping excess memory for page faults. Entry Point Execution: The program runs the ELF entry point function "_start" after loading and mapping required memory. Function Pointer Usage: The _start function pointer is typecast and used to call the entry point function. File Reading: The ELF header (ehdr) and program headers (phdr) are read from the ELF file to determine memory layout. File Descriptor Management: The loader opens the ELF file, reads headers, and closes the file descriptor. Output Summary: After execution, the program prints the return value from "_start" and statistics on page faults and fragmented memory.

private git link-https://github.com/modSilverBack/OS-SimpleLazyLoader

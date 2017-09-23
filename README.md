# umalloc
A tiny Unix dynamic memory allocator library. The allocated memory is divided into `4` parts:  

	 -------------------------------------------------
	|          |          |               |          |
	|chunk_size|head_guard|    chunk      |tail_guard|
	|          |          |               |          |
     ------------------------------------------------- 
Besides the allocated memory are initialized to `0`, when free the memory, it will check following cases:  

(1) Free `NULL` pointer;  
(2) Both up and down out-of-bound memory error;  
(3) Double-free pointer error.  

## Usage 
	$ git clone https://github.com/NanXiao/umalloc.git
	$ cd umalloc
	$ make && make install

## Example:  

    #include <stdint.h>
    #include <inttypes.h>
    #include <stdio.h>
    #include <string.h>
    #include <umalloc.h>

    int main(void)
    {
        uint32_t size = 1;
        for (size = 1; size < UINT32_MAX; size++)
        {
            printf("size=%" PRIu32 "\n", size);
            char *p = umalloc(size);
            memset(p, 0xFF, size);
            ufree(p);
        }
        return 0;
    }


Build and run it:  

	$ cc -I/usr/local/include -L/usr/local/lib -Wall -o test test.c -lumalloc
	$ ./test


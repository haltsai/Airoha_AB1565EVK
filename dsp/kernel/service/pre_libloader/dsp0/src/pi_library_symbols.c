/*
 * Copyright (c) 2012 by Tensilica Inc. ALL RIGHTS RESERVED.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>
#include "elf.h"
#include "xt_library_loader.h"
#include "loader_internal.h"

#ifdef __XTENSA__

static Elf32_Word
elf_hash(const unsigned char *name)
{
  Elf32_Word h = 0, g;
  while (*name)
    {
      h = (h << 4) + *name++;
      g = h & 0xf0000000;
      if (g)
	h ^= g >> 24;
      h &= ~g;
    }
  return h;
}



void * 
xtlib_lookup_pi_library_symbol(xtlib_pil_info * lib_info, const char * symname)
{
  Elf32_Word * entries = lib_info->hash;
  Elf32_Word nbuckets = *entries;
  
  Elf32_Word * buckets = entries + 2;
  Elf32_Word * chains = buckets + nbuckets;

  Elf32_Word bucket = elf_hash((const unsigned char * )symname) % nbuckets;
  Elf32_Sym * symbols = (Elf32_Sym *) lib_info->symtab;
  
  bucket = buckets[bucket];

  while (bucket != STN_UNDEF) {
    Elf32_Sym * candidate = &symbols[bucket];
    
    const char * candname = lib_info->strtab + candidate->st_name;
    if (strcmp (candname, symname) == 0) {
      if (candidate->st_value >= lib_info->src_data_offs)
	return candidate->st_value + lib_info->dst_data_addr - lib_info->src_data_offs;
      else
	return candidate->st_value + lib_info->dst_addr - lib_info->src_offs;
    }
    bucket = chains[bucket];
  }
  
  return 0;
}

#endif


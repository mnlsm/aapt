/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "dlmalloc-stubs"


#define UNUSED

/*
 * Stubs for functions defined in bionic/libc/bionic/dlmalloc.c. These
 * are used in host builds, as the host libc will not contain these
 * functions.
 */
void dlmalloc_inspect_all(void(*handler)(void*, void *, unsigned int a, void*) UNUSED,
                          void* arg UNUSED)
{
}

int dlmalloc_trim(unsigned int unused UNUSED)
{
  return 0;
}

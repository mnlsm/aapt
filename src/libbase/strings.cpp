/*
 * Copyright (C) 2015 The Android Open Source Project
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

#include "android-base/strings.h"

#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>


#include <string.h>

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)strcasecmp.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */

typedef unsigned char u_char;

/*
* This array is designed for mapping upper and lower case letter
* together for a case independent comparison.  The mappings are
* based upon ascii character sequences.
*/
static const u_char charmap[] = {
	'\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
	'\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
	'\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
	'\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
	'\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
	'\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
	'\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
	'\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
	'\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
	'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
	'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
	'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
	'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
	'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
	'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
	'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
	'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
	'\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
	'\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
	'\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
	'\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
	'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
	'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
	'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
	'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

int strcasecmp(const char *s1, const char *s2)
{
	register const u_char *cm = charmap,
		*us1 = (const u_char *)s1,
		*us2 = (const u_char *)s2;

	while (cm[*us1] == cm[*us2++])
		if (*us1++ == '\0')
			return (0);
	return (cm[*us1] - cm[*--us2]);
}

int strncasecmp(const char *s1, const char*s2, size_t n) {
	if (n != 0) {
		register const u_char *cm = charmap,
			*us1 = (const u_char *)s1,
			*us2 = (const u_char *)s2;

		do {
			if (cm[*us1] != cm[*us2++])
				return (cm[*us1] - cm[*--us2]);
			if (*us1++ == '\0')
				break;
		} while (--n != 0);
	}
	return (0);
}

namespace android {
namespace base {

#define CHECK_NE(a, b) \
  if ((a) == (b)) abort();

std::vector<std::string> Split(const std::string& s,
                               const std::string& delimiters) {
  CHECK_NE(delimiters.size(), 0U);

  std::vector<std::string> result;

  size_t base = 0;
  size_t found;
  while (true) {
    found = s.find_first_of(delimiters, base);
    result.push_back(s.substr(base, found - base));
    if (found == s.npos) break;
    base = found + 1;
  }

  return result;
}

std::string Trim(const std::string& s) {
  std::string result;

  if (s.size() == 0) {
    return result;
  }

  size_t start_index = 0;
  size_t end_index = s.size() - 1;

  // Skip initial whitespace.
  while (start_index < s.size()) {
    if (!isspace(s[start_index])) {
      break;
    }
    start_index++;
  }

  // Skip terminating whitespace.
  while (end_index >= start_index) {
    if (!isspace(s[end_index])) {
      break;
    }
    end_index--;
  }

  // All spaces, no beef.
  if (end_index < start_index) {
    return "";
  }
  // Start_index is the first non-space, end_index is the last one.
  return s.substr(start_index, end_index - start_index + 1);
}

// These cases are probably the norm, so we mark them extern in the header to
// aid compile time and binary size.
template std::string Join(const std::vector<std::string>&, char);
template std::string Join(const std::vector<const char*>&, char);
template std::string Join(const std::vector<std::string>&, const std::string&);
template std::string Join(const std::vector<const char*>&, const std::string&);

bool StartsWith(const std::string& s, const char* prefix) {
  return strncmp(s.c_str(), prefix, strlen(prefix)) == 0;
}

bool StartsWithIgnoreCase(const std::string& s, const char* prefix) {
  return strncasecmp(s.c_str(), prefix, strlen(prefix)) == 0;
}

static bool EndsWith(const std::string& s, const char* suffix, bool case_sensitive) {
  size_t suffix_length = strlen(suffix);
  size_t string_length = s.size();
  if (suffix_length > string_length) {
    return false;
  }
  size_t offset = string_length - suffix_length;
  return (case_sensitive ? strncmp : strncasecmp)(s.c_str() + offset, suffix, suffix_length) == 0;
}

bool EndsWith(const std::string& s, const char* suffix) {
  return EndsWith(s, suffix, true);
}

bool EndsWithIgnoreCase(const std::string& s, const char* suffix) {
  return EndsWith(s, suffix, false);
}

bool EqualsIgnoreCase(const std::string& lhs, const std::string& rhs) {
  return strcasecmp(lhs.c_str(), rhs.c_str()) == 0;
}

}  // namespace base
}  // namespace android

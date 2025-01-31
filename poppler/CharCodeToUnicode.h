//========================================================================
//
// CharCodeToUnicode.h
//
// Mapping from character codes to Unicode.
//
// Copyright 2001-2003 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2007 Julien Rebetez <julienr@svn.gnome.org>
// Copyright (C) 2007 Koji Otani <sho@bbr.jp>
// Copyright (C) 2008, 2011, 2012, 2018, 2019 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2017 Adrian Johnson <ajohnson@redneon.com>
// Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, <info@kdab.com>. Work sponsored by the LiMux project of the city of Munich
// Copyright (C) 2018 Adam Reichold <adam.reichold@t-online.de>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#ifndef CHARCODETOUNICODE_H
#define CHARCODETOUNICODE_H

#include <atomic>

#include "poppler-config.h"
#include "CharTypes.h"

struct CharCodeToUnicodeString;
class GooString;

//------------------------------------------------------------------------

class CharCodeToUnicode {
friend class UnicodeToCharCode;
public:

  // Create an identity mapping (Unicode = CharCode).
  static CharCodeToUnicode *makeIdentityMapping();

  // Read the CID-to-Unicode mapping for <collection> from the file
  // specified by <fileName>.  Sets the initial reference count to 1.
  // Returns NULL on failure.
  static CharCodeToUnicode *parseCIDToUnicode(const char *fileName,
					      GooString *collection);

  // Create a Unicode-to-Unicode mapping from the file specified by
  // <fileName>.  Sets the initial reference count to 1.  Returns NULL
  // on failure.
  static CharCodeToUnicode *parseUnicodeToUnicode(GooString *fileName);

  // Create the CharCode-to-Unicode mapping for an 8-bit font.
  // <toUnicode> is an array of 256 Unicode indexes.  Sets the initial
  // reference count to 1.
  static CharCodeToUnicode *make8BitToUnicode(Unicode *toUnicode);

  // Parse a ToUnicode CMap for an 8- or 16-bit font.
  static CharCodeToUnicode *parseCMap(GooString *buf, int nBits);
  static CharCodeToUnicode *parseCMapFromFile(GooString *fileName, int nBits);

  // Parse a ToUnicode CMap for an 8- or 16-bit font, merging it into
  // <this>.
  void mergeCMap(GooString *buf, int nBits);

  ~CharCodeToUnicode();

  CharCodeToUnicode(const CharCodeToUnicode &) = delete;
  CharCodeToUnicode& operator=(const CharCodeToUnicode &) = delete;

  void incRefCnt();
  void decRefCnt();

  // Return true if this mapping matches the specified <tagA>.
  bool match(GooString *tagA);

  // Set the mapping for <c>.
  void setMapping(CharCode c, Unicode *u, int len);

  // Map a CharCode to Unicode. Returns a pointer in u to internal storage
  // so never store the pointers it returns, just the data, otherwise
  // your pointed values might get changed by future calls
  int mapToUnicode(CharCode c, Unicode **u);

  // Map a Unicode to CharCode.
  int mapToCharCode(const Unicode* u, CharCode *c, int usize) const;

  // Return the mapping's length, i.e., one more than the max char
  // code supported by the mapping.
  CharCode getLength() { return mapLen; }

private:

  void parseCMap1(int (*getCharFunc)(void *), void *data, int nBits);
  void addMapping(CharCode code, char *uStr, int n, int offset);
  CharCodeToUnicode();
  CharCodeToUnicode(GooString *tagA);
  CharCodeToUnicode(GooString *tagA, Unicode *mapA,
		    CharCode mapLenA, bool copyMap,
		    CharCodeToUnicodeString *sMapA,
		    int sMapLenA, int sMapSizeA);

  GooString *tag;
  Unicode *map;
  CharCode mapLen;
  CharCodeToUnicodeString *sMap;
  int sMapLen, sMapSize;
  std::atomic_int refCnt;
  bool isIdentity;
};

//------------------------------------------------------------------------

class CharCodeToUnicodeCache {
public:

  CharCodeToUnicodeCache(int sizeA);
  ~CharCodeToUnicodeCache();

  CharCodeToUnicodeCache(const CharCodeToUnicodeCache &) = delete;
  CharCodeToUnicodeCache& operator=(const CharCodeToUnicodeCache &) = delete;

  // Get the CharCodeToUnicode object for <tag>.  Increments its
  // reference count; there will be one reference for the cache plus
  // one for the caller of this function.  Returns NULL on failure.
  CharCodeToUnicode *getCharCodeToUnicode(GooString *tag);

  // Insert <ctu> into the cache, in the most-recently-used position.
  void add(CharCodeToUnicode *ctu);

private:

  CharCodeToUnicode **cache;
  int size;
};

#endif

/*      Xenoblade Engine Format Library
        Copyright(C) 2017-2019 Lukas Cone

        This program is free software : you can redistribute it and / or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
        (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program.If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include "XenoLibAPI.h"
#include "datas/vectors.hpp"
#include <vector>

struct DRSMResourceStream {
  int size, uncompressedSize, offset;
};

struct DRSMTextureItem {
  int unk, cachedSize, cachedOffset, nameOffset;
};

struct DRSMResourceItem {
  int localOffset;
  int localSize;
  short id1;
  short type; // 0- model, 1- shader bundle, 2- cached textures, 3- uncached
              // textures
  int null[2];
};

struct DRSMTextureTable {
  int numTextureItems, texturesOffsets, null, stringBufferOffset;

  char *GetMe() { return reinterpret_cast<char *>(this); }

  DRSMTextureItem *Textures() {
    return reinterpret_cast<DRSMTextureItem *>(GetMe() + texturesOffsets);
  }

  const char *TextureName(DRSMTextureItem *item) {
    return reinterpret_cast<const char *>(GetMe() + item->nameOffset);
  }
};

struct DRSMResources {
  int tag, revision, numDataItems, dataItemsOffset, numFiles, filesTableOffset,
      unk1[7], IDTableCount, IDTableOffset, textureTableOffset, null[3];

  char *GetMe() { return reinterpret_cast<char *>(this); }

  DRSMResourceStream *Resources() {
    return reinterpret_cast<DRSMResourceStream *>(GetMe() + filesTableOffset);
  }

  short *IDs() { return reinterpret_cast<short *>(GetMe() + IDTableOffset); }

  DRSMTextureTable *TextureTable() {
    return reinterpret_cast<DRSMTextureTable *>(GetMe() + textureTableOffset);
  }

  DRSMResourceItem *ResourceItems() {
    return reinterpret_cast<DRSMResourceItem *>(GetMe() + dataItemsOffset);
  }
};

struct DRSMHeader {
  int magic, version, dataSize, dataOffset;
};

class DRSM {
  static const int ID = CompileFourCC("DRSM");

  union {
    char *masterBuffer;
    DRSMResources *header;
  } data;
  std::vector<char *> resources;

  template <class _Ty0>
  // typedef wchar_t _Ty0;
  int _Load(const _Ty0 *fileName, bool suppressErrors);

  template <class _Ty0>
  // typedef wchar_t _Ty0;
  int _ExtractTexture(const _Ty0 *outputFolder, int id,
                      TextureConversionParams params) const;

public:
  DRSM() : data() {}

  ~DRSM();

  int Load(const char *fileName, bool suppressErrors = false) {
    return _Load(fileName, suppressErrors);
  }

  int Load(const wchar_t *fileName, bool suppressErrors = false) {
    return _Load(fileName, suppressErrors);
  }

  DRSMResources *GetData() { return data.header; }

  char *GetResource(int id) { return resources[id]; }

  int GetNumTextures() const {
    return data.header->TextureTable()->numTextureItems;
  }

  const char *GetTextureName(int id) const;

  int ExtractTexture(const wchar_t *outputFolder, int id,
                     TextureConversionParams params) const {
    return _ExtractTexture(outputFolder, id, params);
  }

  int ExtractTexture(const char *outputFolder, int id,
                     TextureConversionParams params) const {
    return _ExtractTexture(outputFolder, id, params);
  }
};
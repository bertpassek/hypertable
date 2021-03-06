/* -*- c++ -*-
 * Copyright (C) 2007-2015 Hypertable, Inc.
 *
 * This file is part of Hypertable.
 *
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 3 of the
 * License.
 *
 * Hypertable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef HYPERTABLE_CELLSTOREV3_H
#define HYPERTABLE_CELLSTOREV3_H

#include <map>
#include <string>
#include <vector>

#include "CellStoreBlockIndexArray.h"

#include "AsyncComm/DispatchHandlerSynchronizer.h"
#include "Common/DynamicBuffer.h"
#include "Common/BloomFilter.h"
#include "Common/BlobHashSet.h"

#include "Hypertable/Lib/BlockCompressionCodec.h"
#include "Hypertable/Lib/SerializedKey.h"

#include "CellStore.h"
#include "CellStoreTrailerV3.h"
#include "KeyCompressor.h"


/**
 * Forward declarations
 */
namespace Hypertable {
  class BlockCompressionCodec;
  class Client;
  class Protocol;
}

namespace Hypertable {

  class CellStoreV3 : public CellStore {

    class IndexBuilder {
    public:
      IndexBuilder() : m_bigint(false) { }
      void add_entry(KeyCompressorPtr &key_compressor, int64_t offset);
      DynamicBuffer &fixed_buf() { return m_fixed; }
      DynamicBuffer &variable_buf() { return m_variable; }
      bool big_int() { return m_bigint; }
      void chop();
      void release_fixed_buf() { delete [] m_fixed.release(); }
    private:
      DynamicBuffer m_fixed;
      DynamicBuffer m_variable;
      bool m_bigint;
    };

  public:
    CellStoreV3(Filesystem *filesys, Schema *schema=0);
    virtual ~CellStoreV3();

    virtual void create(const char *fname, size_t max_entries,
                        PropertiesPtr &props,
                        const TableIdentifier *table_id=0);
    virtual void add(const Key &key, const ByteString value);
    virtual void finalize(TableIdentifier *table_identifier);
    virtual void open(const String &fname, const String &start_row,
                      const String &end_row, int32_t fd, int64_t file_length,
                      CellStoreTrailer *trailer);
    virtual int64_t get_blocksize() { return m_trailer.blocksize; }
    virtual bool may_contain(const void *ptr, size_t len);
    bool may_contain(const String &key) {
      return may_contain(key.data(), key.size());
    }
    virtual bool may_contain(ScanContextPtr &);

    virtual uint64_t disk_usage() {
      if (m_disk_usage < 0)
        HT_WARN_OUT << "[Issue 339] Disk usage for " << m_filename << "=" << m_disk_usage
                    << HT_END;
      return m_disk_usage;
    }
    virtual float compression_ratio() { return m_trailer.compression_ratio; }
    virtual int64_t get_total_entries() { return m_trailer.total_entries; }
    virtual std::string &get_filename() { return m_filename; }
    virtual int get_file_id() { return m_file_id; }
    virtual CellListScanner *create_scanner(ScanContextPtr &scan_ctx);
    virtual BlockCompressionCodec *create_block_compression_codec();
    virtual KeyDecompressor *create_key_decompressor();
    virtual void display_block_info();
    virtual int64_t end_of_last_block() { return m_trailer.fix_index_offset; }
    virtual size_t bloom_filter_size() { return m_bloom_filter ? m_bloom_filter->size() : 0; }
    virtual int64_t bloom_filter_memory_used() { return m_index_stats.bloom_filter_memory; }
    virtual int64_t block_index_memory_used() { return m_index_stats.block_index_memory; }
    virtual uint64_t purge_indexes();
    virtual bool restricted_range() { return m_restricted_range; }

    virtual int32_t get_fd() {
      ScopedLock lock(m_mutex);
      return m_fd;
    }

    virtual int32_t reopen_fd() {
      ScopedLock lock(m_mutex);
      if (m_fd != -1)
        m_filesys->close(m_fd);
      m_fd = m_filesys->open(m_filename, 0);
      return m_fd;
    }

    virtual CellStoreTrailer *get_trailer() { return &m_trailer; }

    virtual uint16_t block_header_format();

  protected:
    void create_bloom_filter(bool is_approx = false);
    void load_bloom_filter();
    void load_block_index();

    typedef BlobHashSet<> BloomFilterItems;

    Filesystem            *m_filesys;
    SchemaPtr              m_schema;
    int32_t                m_fd;
    std::string            m_filename;
    CellStoreBlockIndexArray<uint32_t> m_index_map32;
    CellStoreBlockIndexArray<int64_t> m_index_map64;
    bool                   m_64bit_index;
    CellStoreTrailerV3     m_trailer;
    BlockCompressionCodec *m_compressor;
    DynamicBuffer          m_buffer;
    IndexBuilder           m_index_builder;
    DispatchHandlerSynchronizer  m_sync_handler;
    uint32_t               m_outstanding_appends;
    int64_t                m_offset;
    ByteString             m_last_key;
    int64_t                m_file_length;
    int64_t                m_disk_usage;
    int                    m_file_id;
    float                  m_uncompressed_data;
    float                  m_compressed_data;
    int64_t                m_uncompressed_blocksize;
    BlockCompressionCodec::Args m_compressor_args;
    size_t                 m_max_entries;

    BloomFilterMode        m_bloom_filter_mode;
    BloomFilter           *m_bloom_filter;
    BloomFilterItems      *m_bloom_filter_items;
    int64_t                m_max_approx_items;
    float                  m_bloom_bits_per_item;
    float                  m_filter_false_positive_prob;
    KeyCompressorPtr       m_key_compressor;
    bool                   m_restricted_range;
    int64_t               *m_column_ttl;
  };

  typedef intrusive_ptr<CellStoreV3> CellStoreV3Ptr;

} // namespace Hypertable

#endif // HYPERTABLE_CELLSTOREV3_H

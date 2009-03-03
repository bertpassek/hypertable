/** -*- c++ -*-
 * Copyright (C) 2008 Doug Judd (Zvents, Inc.)
 *
 * This file is part of Hypertable.
 *
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 of the
 * License, or any later version.
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

#ifndef HYPERTABLE_CELLSTORESCANNERVERSION1_H
#define HYPERTABLE_CELLSTORESCANNERVERSION1_H

#include "Common/DynamicBuffer.h"

#include "CellStoreV0.h"
#include "CellListScanner.h"

namespace Hypertable {

  class BlockCompressionCodec;
  class CellStore;

  class CellStoreScannerV0 : public CellListScanner {
  public:
    CellStoreScannerV0(CellStorePtr &cellstore, ScanContextPtr &scan_ctx);
    virtual ~CellStoreScannerV0();
    virtual void forward();
    virtual bool get(Key &key, ByteString &value);

  private:
    
    void set_start_deletes(bool search_cf_delete);
    bool search_start_delete_keys(Key &start_key, bool block_loaded, bool &row_match);
    void set_start_deletes_readahead(bool search_cf_delete);
    bool search_start_delete_keys_readahead(Key &start_key, bool block_loaded, bool &row_match);
    void set_search_delete_keys(bool search_cf_delete);
    void start_buffered_read();

    struct BlockInfo {
      uint32_t offset;
      uint32_t zlength;
      const uint8_t *base;
      const uint8_t *ptr;
      const uint8_t *end;
    };

    bool fetch_next_block();
    bool fetch_next_block_readahead();
    bool initialize();

    CellStorePtr            m_cell_store_ptr;
    CellStoreV0            *m_cell_store_v0;
    CellStoreV0::IndexMap  &m_index;

    CellStoreV0::IndexMap::iterator m_iter;

    BlockInfo             m_block;
    Key                   m_key;
    SerializedKey         m_cur_key;
    ByteString            m_cur_value;
    SerializedKey         m_start_key;
    SerializedKey         m_end_key;
    DynamicBuffer         m_key_buf;
    BlockCompressionCodec *m_zcodec;
    bool                  m_check_for_range_end;
    int                   m_file_id;
    std::string           m_start_row;
    std::string           m_end_row;
    bool                  m_readahead;
    bool                  m_close_fd_on_exit;
    int32_t               m_fd;
    uint32_t              m_start_offset;
    uint32_t              m_end_offset;
    uint32_t              m_returned;
    bool                  m_has_start_deletes;
    bool                  m_has_start_row_delete;
    bool                  m_has_start_cf_delete;
    Key                   m_start_deletes[2];
    DynamicBuffer         m_start_delete_buf; 
    Key                   m_delete_search_keys[2];
    
    /**
     * Contents of m_start_delete_buf are:
     * 0 - search start delete cf key
     * 1 - start delete row key if found
     * 2 - start cf row key if found
     */
    size_t                m_start_delete_buf_offsets[3]; 
  };

}

#endif // HYPERTABLE_CELLSTORESCANNERVERSION1_H


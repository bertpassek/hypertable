/*
 * Copyright (C) 2007-2015 Hypertable, Inc.
 *
 * This file is part of Hypertable.
 *
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or any later version.
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

/// @file
/// Definitions for Append response callback.
/// This file contains definitions for Append, a response callback class used
/// to deliver results of the <i>append</i> function call back to the client.

#include <Common/Compat.h>

#include "Append.h"

#include <FsBroker/Lib/Response/Parameters/Append.h>

#include <AsyncComm/CommBuf.h>

#include <Common/Error.h>

using namespace Hypertable;
using namespace FsBroker::Lib::Response;

int Callback::Append::response(uint64_t offset, uint32_t amount) {
  CommHeader header;
  header.initialize_from_request_header(m_event->header);
  Parameters::Append params(offset, amount);
  CommBufPtr cbuf( new CommBuf(header, 4 + params.encoded_length()) );
  cbuf->append_i32(Error::OK);
  params.encode(cbuf->get_data_ptr_address());
  return m_comm->send_response(m_event->addr, cbuf);
}

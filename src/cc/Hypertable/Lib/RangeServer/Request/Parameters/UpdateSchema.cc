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
/// Definitions for UpdateSchema request parameters.
/// This file contains definitions for UpdateSchema, a class for encoding and
/// decoding paramters to the <i>update schema</i> %RangeServer function.

#include <Common/Compat.h>

#include "UpdateSchema.h"

#include <Common/Logger.h>
#include <Common/Serialization.h>

using namespace Hypertable;
using namespace Hypertable::Lib::RangeServer::Request::Parameters;

uint8_t UpdateSchema::encoding_version() const {
  return 1;
}

size_t UpdateSchema::encoded_length_internal() const {
  return m_table.encoded_length() +
    Serialization::encoded_length_vstr(m_schema);
}

/// @details
/// Encoding is as follows:
/// <table>
/// <tr>
/// <th>Encoding</th>
/// <th>Description</th>
/// </tr>
/// <tr>
/// <td>TableIdentifier</td>
/// <td>%Table identifier</td>
/// </tr>
/// <tr>
/// <td>vstr</td>
/// <td>%Table schema</td>
/// </tr>
/// </table>
void UpdateSchema::encode_internal(uint8_t **bufp) const {
  m_table.encode(bufp);
  Serialization::encode_vstr(bufp, m_schema);
}

void UpdateSchema::decode_internal(uint8_t version, const uint8_t **bufp,
                                   size_t *remainp) {
  m_table.decode(bufp, remainp);
  m_schema = Serialization::decode_vstr(bufp, remainp);
}




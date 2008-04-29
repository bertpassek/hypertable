/** -*- c++ -*-
 * Copyright (C) 2008 Doug Judd (Zvents, Inc.)
 * 
 * This file is part of Hypertable.
 * 
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 of the
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

#include <cstring>
#include <iostream>

#include "AsyncComm/CommBuf.h"
#include "AsyncComm/Serialization.h"

#include "Types.h"

using namespace Hypertable;
using namespace std;

const uint64_t Hypertable::END_OF_TIME = (uint64_t)-1;

size_t TableIdentifier::encoded_length() {
  return EncodedLengthTableIdentifier(*this);
}

void TableIdentifier::encode(uint8_t **bufPtr) {
  EncodeTableIdentifier(bufPtr, *this);
}

bool TableIdentifier::decode(uint8_t **bufPtr, size_t *remainingPtr) {
  return DecodeTableIdentifier(bufPtr, remainingPtr, this);
}

size_t RangeSpec::encoded_length() {
  return EncodedLengthRange(*this);
}

void RangeSpec::encode(uint8_t **bufPtr) {
  EncodeRange(bufPtr, *this);
}

bool RangeSpec::decode(uint8_t **bufPtr, size_t *remainingPtr) {
  return DecodeRange(bufPtr, remainingPtr, this);
}


ScanSpec::ScanSpec() : row_limit(0), max_versions(0), start_row(0), start_row_inclusive(true), end_row(0), end_row_inclusive(true), interval(0, END_OF_TIME), return_deletes(false) {
  return;
}

size_t ScanSpec::encoded_length() {
  return EncodedLengthScanSpecification(*this);
}

void ScanSpec::encode(uint8_t **bufPtr) {
  EncodeScanSpecification(bufPtr, *this);
}

bool ScanSpec::decode(uint8_t **bufPtr, size_t *remainingPtr) {
  return DecodeScanSpecification(bufPtr, remainingPtr, this);
}

void Hypertable::Copy(TableIdentifier &src, TableIdentifier &dst) {
  memcpy(&dst, &src, sizeof(TableIdentifier));
  dst.name = new char [ strlen(src.name) + 1 ];
  strcpy((char *)dst.name, src.name);
}

/** Returns encoded (serialized) length of the given TableIdentifier.
 */
size_t Hypertable::EncodedLengthTableIdentifier(TableIdentifier &table_identifier) {
  return 8 + Serialization::encoded_length_string(table_identifier.name);
}

/** Encodes a TableIdentifier into the given buffer. */
void Hypertable::EncodeTableIdentifier(uint8_t **bufPtr, TableIdentifier &table_identifier) {
  Serialization::encode_string(bufPtr, table_identifier.name);
  Serialization::encode_int(bufPtr, table_identifier.id);
  Serialization::encode_int(bufPtr, table_identifier.generation);
}

/** Decodes a TableIdentifier from the given buffer */
bool Hypertable::DecodeTableIdentifier(uint8_t **bufPtr, size_t *remainingPtr, TableIdentifier *table_identifier) {
  memset(table_identifier, 0, sizeof(TableIdentifier));
  if (!Serialization::decode_string(bufPtr, remainingPtr, &table_identifier->name))
    return false;
  if (!Serialization::decode_int(bufPtr, remainingPtr, &table_identifier->id))
    return false;
  if (!Serialization::decode_int(bufPtr, remainingPtr, &table_identifier->generation))
    return false;
  return true;
}

/** Returns encoded (serialized) length of a RangeSpec */
size_t Hypertable::EncodedLengthRange(RangeSpec &range) {
  return Serialization::encoded_length_string(range.start_row) + Serialization::encoded_length_string(range.end_row);
}

/** Encodes a RangeSpec into the given buffer. */
void Hypertable::EncodeRange(uint8_t **bufPtr, RangeSpec &range) {
  Serialization::encode_string(bufPtr, range.start_row);
  Serialization::encode_string(bufPtr, range.end_row);
}

/** Decodes a RangeSpec from the given buffer */
bool Hypertable::DecodeRange(uint8_t **bufPtr, size_t *remainingPtr, RangeSpec *range) {
  memset(range, 0, sizeof(RangeSpec));
  if (!Serialization::decode_string(bufPtr, remainingPtr, &range->start_row))
    return false;
  if (!Serialization::decode_string(bufPtr, remainingPtr, &range->end_row))
    return false;
  return true;
}


/**
 *
 */
size_t Hypertable::EncodedLengthScanSpecification(ScanSpec &scanSpec) {
  size_t len = 29;
  len += Serialization::encoded_length_string(scanSpec.start_row);
  len += Serialization::encoded_length_string(scanSpec.end_row);
  for (size_t i=0; i<scanSpec.columns.size(); i++)
    len += Serialization::encoded_length_string(scanSpec.columns[i]);
  return len;
}


void Hypertable::EncodeScanSpecification(uint8_t **bufPtr, ScanSpec &scanSpec) {
  Serialization::encode_int(bufPtr, scanSpec.row_limit);
  Serialization::encode_int(bufPtr, scanSpec.max_versions);
  Serialization::encode_string(bufPtr, scanSpec.start_row);
  *(*bufPtr)++ = (uint8_t)scanSpec.start_row_inclusive;
  Serialization::encode_string(bufPtr, scanSpec.end_row);
  *(*bufPtr)++ = (uint8_t)scanSpec.end_row_inclusive;
  Serialization::encode_short(bufPtr, (short)scanSpec.columns.size());
  for (size_t i=0; i<scanSpec.columns.size(); i++)
    Serialization::encode_string(bufPtr, scanSpec.columns[i]);
  Serialization::encode_long(bufPtr, scanSpec.interval.first);
  Serialization::encode_long(bufPtr, scanSpec.interval.second);
  Serialization::encode_bool(bufPtr, scanSpec.return_deletes);
}


bool Hypertable::DecodeScanSpecification(uint8_t **bufPtr, size_t *remainingPtr, ScanSpec *scanSpec) {
  uint16_t columnCount;
  const char *column;
  uint8_t inclusiveByte;

  if (!Serialization::decode_int(bufPtr, remainingPtr, &scanSpec->row_limit))
    return false;
  if (!Serialization::decode_int(bufPtr, remainingPtr, &scanSpec->max_versions))
    return false;
  if (!Serialization::decode_string(bufPtr, remainingPtr, &scanSpec->start_row))
    return false;
  if (!Serialization::decode_byte(bufPtr, remainingPtr, &inclusiveByte))
    return false;
  scanSpec->start_row_inclusive = inclusiveByte ? true : false;
  if (!Serialization::decode_string(bufPtr, remainingPtr, &scanSpec->end_row))
    return false;
  if (!Serialization::decode_byte(bufPtr, remainingPtr, &inclusiveByte))
    return false;
  scanSpec->end_row_inclusive = inclusiveByte ? true : false;
  if (!Serialization::decode_short(bufPtr, remainingPtr, &columnCount))
    return false;
  for (short i=0; i<columnCount; i++) {
    if (!Serialization::decode_string(bufPtr, remainingPtr, &column))
      return false;
    scanSpec->columns.push_back(column);
  }
  if (!Serialization::decode_long(bufPtr, remainingPtr, &scanSpec->interval.first))
    return false;
  if (!Serialization::decode_long(bufPtr, remainingPtr, &scanSpec->interval.second))
    return false;
  if (!Serialization::decode_bool(bufPtr, remainingPtr, &scanSpec->return_deletes))
    return false;
  
  return true;
}


std::ostream &Hypertable::operator<<(std::ostream &os, const TableIdentifier &table_identifier) {
  os << "Table Name = " << table_identifier.name << endl;
  os << "Table ID   = " << table_identifier.id << endl;
  os << "Generation = " << table_identifier.generation << endl;
  return os;
}

std::ostream &Hypertable::operator<<(std::ostream &os, const RangeSpec &range) {
  if (range.start_row == 0)
    os << "StartRow = [NULL]" << endl;
  else
    os << "StartRow = \"" << range.start_row << "\"" << endl;
  if (range.end_row == 0)
    os << "EndRow   = [NULL]" << endl;
  else
    os << "EndRow   = \"" << range.end_row << "\"" << endl;
  return os;
}

std::ostream &Hypertable::operator<<(std::ostream &os, const ScanSpec &scanSpec) {
  os << "RowLimit    = " << scanSpec.row_limit << endl;
  os << "MaxVersions = " << scanSpec.max_versions << endl;
  os << "Columns     = ";
  for (std::vector<const char *>::const_iterator iter = scanSpec.columns.begin(); iter != scanSpec.columns.end(); iter++)
    os << *iter << " ";
  os << endl;
  if (scanSpec.start_row)
    os << "StartRow  = " << scanSpec.start_row << endl;
  else
    os << "StartRow  = " << endl;
  os << "StartRowInclusive = " << scanSpec.start_row_inclusive << endl;
  if (scanSpec.end_row)
    os << "EndRow    = " << scanSpec.end_row << endl;
  else
    os << "EndRow    = " << endl;
  os << "EndRowInclusive = " << scanSpec.end_row_inclusive << endl;
  os << "MinTime     = " << scanSpec.interval.first << endl;
  os << "MaxTime     = " << scanSpec.interval.second << endl;
  return os;
}



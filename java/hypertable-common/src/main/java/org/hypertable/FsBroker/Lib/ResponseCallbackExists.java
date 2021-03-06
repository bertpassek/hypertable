/**
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

package org.hypertable.FsBroker.Lib;

import org.hypertable.AsyncComm.Comm;
import org.hypertable.AsyncComm.CommBuf;
import org.hypertable.AsyncComm.CommHeader;
import org.hypertable.AsyncComm.Event;
import org.hypertable.AsyncComm.ResponseCallback;
import org.hypertable.Common.Error;
import org.hypertable.Common.Serialization;

public class ResponseCallbackExists extends ResponseCallback {

  ResponseCallbackExists(Comm comm, Event event) {
    super(comm, event);
  }

  static final byte VERSION = 1;

  public int response(boolean exists) {
    CommHeader header = new CommHeader();
    header.initialize_from_request_header(mEvent.header);
    CommBuf cbuf = new CommBuf(header, 5 + Serialization.EncodedLengthVInt32(1) + 1);
    cbuf.AppendInt(Error.OK);
    cbuf.AppendByte(VERSION);
    Serialization.EncodeVInt32(cbuf.data, 1);
    cbuf.AppendBool(exists);
    return mComm.SendResponse(mEvent.addr, cbuf);
  }
}


/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.hypertable.hadoop.hive;

import org.hypertable.hadoop.hive.ColumnMappings.ColumnMapping;
import org.hypertable.hadoop.util.Row;

import org.apache.hadoop.hive.serde2.SerDeException;
import org.apache.hadoop.hive.serde2.lazy.ByteArrayRef;
import org.apache.hadoop.hive.serde2.lazy.LazyFactory;
import org.apache.hadoop.hive.serde2.lazy.LazyObjectBase;
import org.apache.hadoop.hive.serde2.lazy.LazyStruct;
import org.apache.hadoop.hive.serde2.lazy.objectinspector.LazyMapObjectInspector;
import org.apache.hadoop.hive.serde2.lazy.objectinspector.LazySimpleStructObjectInspector;
import org.apache.hadoop.hive.serde2.objectinspector.StructField;

import java.util.ArrayList;
import java.util.Arrays;

/**
 * LazyObject for storing an Hypertable row.  The field of an Hypertable row can be
 * primitive or non-primitive.
 */
public class LazyHypertableRow extends LazyStruct {

  /**
   * The Hypertable columns mapping of the row.
   */
  private Row row;
  private ColumnMapping[] columnsMapping;
  private ArrayList<Object> cachedList;

  private final int iKey;
  private final HypertableKeyFactory keyFactory;

  public LazyHypertableRow(LazySimpleStructObjectInspector oi) {
    this(oi, -1, null);
  }

  /**
   * Construct a LazyHypertableRow object with the ObjectInspector.
   */
  public LazyHypertableRow(LazySimpleStructObjectInspector oi, int iKey, HypertableKeyFactory keyFactory) {
    super(oi);
    this.iKey = iKey;
    this.keyFactory = keyFactory;
  }

  /**
   * Set the Hypertable row data(a Row writable) for this LazyStruct.
   * @see LazyHypertableRow#init(org.hypertable.hadoop.util.Row)
   */
  public void init(Row r, ColumnMappings columnsMappings) {
    this.row = r;
    this.columnsMapping = columnsMappings.getColumnsMapping();
    setParsed(false);
  }

  @Override
  public Object getObject() {
    return this;
  }
}

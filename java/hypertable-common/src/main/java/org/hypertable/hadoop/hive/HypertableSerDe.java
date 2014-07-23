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

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hive.ql.plan.TableDesc;
import org.apache.hadoop.hive.serde2.AbstractSerDe;
import org.apache.hadoop.hive.serde2.SerDeException;
import org.apache.hadoop.hive.serde2.SerDeStats;
import org.apache.hadoop.hive.serde2.lazy.LazySimpleSerDe;
import org.apache.hadoop.hive.serde2.lazy.objectinspector.LazySimpleStructObjectInspector;
import org.apache.hadoop.hive.serde2.objectinspector.ObjectInspector;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.mapred.JobConf;

import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

/**
 * HypertableSerDe can be used to serialize object into an Hypertable table and
 * deserialize objects from an Hypertable table.
 */
public class HypertableSerDe extends AbstractSerDe {
  public static final Log LOG = LogFactory.getLog(HypertableSerDe.class);

  private ObjectInspector cachedObjectInspector;
  private LazyHypertableRow cachedHypertableRow;

  private HypertableSerDeParameters serdeParams;
  private HypertableRowSerializer serializer;

  @Override
  public String toString() {
    return getClass() + "[" + serdeParams + "]";
  }

  public HypertableSerDe() throws SerDeException {
  }

  /**
   * Initialize the SerDe given parameters.
   * @see SerDe#initialize(Configuration, Properties)
   */
  @Override
  public void initialize(Configuration conf, Properties tbl)
      throws SerDeException {
    serdeParams = new HypertableSerDeParameters(conf, tbl, getClass().getName());

    cachedObjectInspector = HypertableLazyObjectFactory.createLazyHypertableStructInspector(
        serdeParams.getSerdeParams(), serdeParams.getKeyIndex(), serdeParams.getKeyFactory());

    cachedHypertableRow = new LazyHypertableRow(
        (LazySimpleStructObjectInspector) cachedObjectInspector,
        serdeParams.getKeyIndex(), serdeParams.getKeyFactory());

    serializer = new HypertableRowSerializer(serdeParams);

    if (LOG.isDebugEnabled()) {
      LOG.debug("HypertableSerDe initialized with : " + serdeParams);
    }
  }

  public LazySimpleSerDe.SerDeParameters getSerdeParams() {
    return serdeParams.getSerdeParams();
  }

  public HypertableSerDeParameters getHypertableSerdeParam() {
    return serdeParams;
  }

  /**
   * Deserialize a row from the Hypertable Row writable to a LazyObject
   * @param row the Hypertable Row Writable containing the row
   * @return the deserialized object
   * @see SerDe#deserialize(Writable)
   */
  @Override
  public Object deserialize(Writable row) throws SerDeException {
    if (!(row instanceof Row)) {
      throw new SerDeException(getClass().getName() + ": expects Row!");
    }

    cachedHypertableRow.init((Row) row, serdeParams.getColumnMappings());

    return cachedHypertableRow;
  }

  @Override
  public ObjectInspector getObjectInspector() throws SerDeException {
    return cachedObjectInspector;
  }

  @Override
  public Class<? extends Writable> getSerializedClass() {
    return Row.class;
  }

  @Override
  public Writable serialize(Object obj, ObjectInspector objInspector) throws SerDeException {
    try {
      return serializer.serialize(obj, objInspector);
    } catch (SerDeException e) {
      throw e;
    } catch (Exception e) {
      throw new SerDeException(e);
    }
  }

  @Override
  public SerDeStats getSerDeStats() {
    // no support for statistics
    return null;
  }

  public HypertableKeyFactory getKeyFactory() {
    return serdeParams.getKeyFactory();
  }

  public static void configureJobConf(TableDesc tableDesc, JobConf jobConf) throws Exception {
    HypertableSerDeParameters serdeParams =
        new HypertableSerDeParameters(jobConf, tableDesc.getProperties(), HypertableSerDe.class.getName());
    serdeParams.getKeyFactory().configureJobConf(tableDesc, jobConf);
  }
}

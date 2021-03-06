/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __Delta_Example__
#define __Delta_Example__

using namespace apache::geode::client;

class DeltaExample : public Cacheable, public Delta {
 private:
  // data members
  int32_t m_field1;
  int32_t m_field2;
  int32_t m_field3;

  // delta indicators
  mutable bool m_f1set;
  mutable bool m_f2set;
  mutable bool m_f3set;

 public:
  DeltaExample(int32_t field1, int32_t field2, int32_t field3)
      : m_field1(field1), m_field2(field2), m_field3(field3) {
    reset();
  }

  DeltaExample() { reset(); }

  DeltaExample(DeltaExample* copy) {
    m_field1 = copy->m_field1;
    m_field2 = copy->m_field2;
    m_field3 = copy->m_field3;
    reset();
  }

  void reset() const {
    m_f1set = false;
    m_f2set = false;
    m_f3set = false;
  }

  int getField1() { return m_field1; }

  int getField2() { return m_field2; }

  int getField3() { return m_field3; }

  void setField1(int val) {
    lock();
    m_field1 = val;
    m_f1set = true;
    unlock();
  }

  void setField2(int val) {
    lock();
    m_field2 = val;
    m_f2set = true;
    unlock();
  }

  void setField3(int val) {
    lock();
    m_field3 = val;
    m_f3set = true;
    unlock();
  }

  virtual bool hasDelta() { return m_f1set || m_f2set || m_f3set; }

  virtual void toDelta(DataOutput& out) const {
    lock();

    out.writeBoolean(m_f1set);
    if (m_f1set) {
      out.writeInt(m_field1);
    }
    out.writeBoolean(m_f2set);
    if (m_f2set) {
      out.writeInt(m_field2);
    }
    out.writeBoolean(m_f2set);
    if (m_f2set) {
      out.writeInt(m_field2);
    }

    reset();

    unlock();
  }

  virtual void fromDelta(DataInput& in) {
    lock();

    in.readBoolean(&m_f1set);
    if (m_f1set) {
      in.readInt(&m_field1);
    }
    in.readBoolean(&m_f2set);
    if (m_f2set) {
      in.readInt(&m_field2);
    }
    in.readBoolean(&m_f3set);
    if (m_f3set) {
      in.readInt(&m_field3);
    }

    reset();

    unlock();
  }

  virtual void toData(DataOutput& output) const {
    lock();
    output.writeInt(m_field1);
    output.writeInt(m_field2);
    output.writeInt(m_field3);
    unlock();
  }

  virtual Serializable* fromData(DataInput& input) {
    lock();
    input.readInt(&m_field1);
    input.readInt(&m_field2);
    input.readInt(&m_field3);
    unlock();
    return this;
  }

  virtual int32_t classId() const { return 2; }

  virtual size_t objectSize() const { return 0; }

  std::shared_ptr<Delta> clone() {
    return std::shared_ptr<Delta>(new DeltaExample(this));
  }

  virtual ~DeltaExample() {}

  static Serializable* create() { return new DeltaExample(); }

  void lock() const { /* add your platform dependent syncronization code here */
  }
  void unlock()
      const { /* add your platform dependent syncronization code here */
  }
};
#endif

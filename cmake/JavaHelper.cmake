# Copyright (C) 2007-2012 Hypertable, Inc.
#
# This file is part of Hypertable.
#
# Hypertable is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or any later version.
#
# Hypertable is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Hypertable. If not, see <http://www.gnu.org/licenses/>
#

set(CDH4_VERSION "2.0.0-cdh4.6.0")
set(HADOOP2_VERSION "2.4.1")

# Copy Maven source tree to build directory
file(COPY java DESTINATION ${HYPERTABLE_BINARY_DIR} PATTERN pom.xml.in EXCLUDE)

# Top-level pom.xml
configure_file(${HYPERTABLE_SOURCE_DIR}/java/pom.xml.in ${HYPERTABLE_BINARY_DIR}/java/pom.xml @ONLY)

# runtime-dependencies/pom.xml
configure_file(${HYPERTABLE_SOURCE_DIR}/java/runtime-dependencies/pom.xml.in
         ${HYPERTABLE_BINARY_DIR}/java/runtime-dependencies/pom.xml @ONLY)

# hypertable-common/pom.xml
configure_file(${HYPERTABLE_SOURCE_DIR}/java/hypertable-common/pom.xml.in
         ${HYPERTABLE_BINARY_DIR}/java/hypertable-common/pom.xml @ONLY)

# hypertable-hadoop2/pom.xml
configure_file(${HYPERTABLE_SOURCE_DIR}/java/hypertable-hadoop2/pom.xml.in
         ${HYPERTABLE_BINARY_DIR}/java/hypertable-hadoop2/pom.xml @ONLY)


if (Thrift_FOUND)
  HT_GLOB(ThriftGenJava_SRCS ${HYPERTABLE_BINARY_DIR}/java/hypertable-common/src/main/java/org/hypertable/thriftgen/*.java)

  add_custom_command(
    OUTPUT    ${ThriftGenJava_SRCS}
    COMMAND   thrift
    ARGS      -r -gen java -out ${HYPERTABLE_BINARY_DIR}/java/hypertable-common/src/main/java
              ${ThriftBroker_IDL_DIR}/Hql.thrift
    DEPENDS   ${ThriftBroker_IDL_DIR}/Client.thrift
              ${ThriftBroker_IDL_DIR}/Hql.thrift
    COMMENT   "thrift2java"
  )
endif ()


add_custom_target(HypertableHadoop2 ALL mvn -f java/pom.xml -Dmaven.test.skip=true package
                  DEPENDS ${ThriftGenJava_SRCS})

#set(HADOOP_DISTRO "cdh3")
#set(SOURCE_EXCLUDES "**/hadoop/2/*.java")
#configure_file(${HYPERTABLE_SOURCE_DIR}/build.xml.in build1.xml @ONLY)
#add_custom_target(Hadoop1JavaComponents ALL ant -f build1.xml
#                  -Dbuild.dir=${HYPERTABLE_BINARY_DIR}/java jar
#                  DEPENDS ${ThriftGenJava_SRCS})
#
#add_custom_target(Hadoop1JavaExamples ALL ant -f build1.xml
#                  -Dbuild.dir=${HYPERTABLE_BINARY_DIR}/java examples
#                  DEPENDS ${ThriftGenJava_SRCS})
#add_dependencies(Hadoop1JavaExamples Hadoop1JavaComponents)
#
#set(HADOOP_DISTRO "cdh4")
#set(SOURCE_EXCLUDES "**/hadoop/1/*.java")
#configure_file(${HYPERTABLE_SOURCE_DIR}/build.xml.in build2.xml @ONLY)
#add_custom_target(Hadoop2JavaComponents ALL ant -f build2.xml
#                  -Dbuild.dir=${HYPERTABLE_BINARY_DIR}/java jar
#                  DEPENDS ${ThriftGenJava_SRCS})
#add_dependencies(Hadoop2JavaComponents Hadoop1JavaExamples)
#
#add_custom_target(Hadoop2JavaExamples ALL ant -f build2.xml
#                  -Dbuild.dir=${HYPERTABLE_BINARY_DIR}/java examples
#                  DEPENDS ${ThriftGenJava_SRCS})
#add_dependencies(Hadoop2JavaExamples Hadoop2JavaComponents)

add_custom_target(java)
#add_dependencies(java Hadoop2JavaExamples)
add_dependencies(java HypertableHadoop2)

# Common jars
install(FILES ${MAVEN_REPOSITORY}/org/apache/thrift/libthrift/0.9.2/libthrift-0.9.2.jar
              DESTINATION lib/java)
install(FILES ${MAVEN_REPOSITORY}/commons-cli/commons-cli/1.2/commons-cli-1.2.jar
              DESTINATION lib/java)
install(FILES ${MAVEN_REPOSITORY}/commons-collections/commons-collections/3.2.1/commons-collections-3.2.1.jar
              DESTINATION lib/java)
install(FILES ${MAVEN_REPOSITORY}/commons-configuration/commons-configuration/1.6/commons-configuration-1.6.jar
              DESTINATION lib/java)
install(FILES ${MAVEN_REPOSITORY}/commons-lang/commons-lang/2.6/commons-lang-2.6.jar
              DESTINATION lib/java)
install(FILES ${MAVEN_REPOSITORY}/commons-logging/commons-logging/1.1.1/commons-logging-1.1.1.jar
              DESTINATION lib/java)
install(FILES ${MAVEN_REPOSITORY}/com/google/guava/guava/11.0.2/guava-11.0.2.jar
              DESTINATION lib/java)
install(FILES ${MAVEN_REPOSITORY}/org/slf4j/slf4j-api/1.7.5/slf4j-api-1.7.5.jar
              DESTINATION lib/java)
install(FILES ${MAVEN_REPOSITORY}/log4j/log4j/1.2.17/log4j-1.2.17.jar
              DESTINATION lib/java)
install(FILES ${MAVEN_REPOSITORY}/com/google/protobuf/protobuf-java/2.5.0/protobuf-java-2.5.0.jar
              DESTINATION lib/java)

# CDH4 jars
install(FILES ${MAVEN_REPOSITORY}/org/apache/hadoop/hadoop-annotations/${CDH4_VERSION}/hadoop-annotations-${CDH4_VERSION}.jar
              DESTINATION lib/java/cdh4)
install(FILES ${MAVEN_REPOSITORY}/org/apache/hadoop/hadoop-auth/${CDH4_VERSION}/hadoop-auth-${CDH4_VERSION}.jar
              DESTINATION lib/java/cdh4)
install(FILES ${MAVEN_REPOSITORY}/org/apache/hadoop/hadoop-common/${CDH4_VERSION}/hadoop-common-${CDH4_VERSION}.jar
              DESTINATION lib/java/cdh4)
install(FILES ${MAVEN_REPOSITORY}/org/apache/hadoop/hadoop-hdfs/${CDH4_VERSION}/hadoop-hdfs-${CDH4_VERSION}.jar
              DESTINATION lib/java/cdh4)
install(FILES ${MAVEN_REPOSITORY}/org/apache/hadoop/hadoop-mapreduce-client-core/${CDH4_VERSION}/hadoop-mapreduce-client-core-${CDH4_VERSION}.jar
              DESTINATION lib/java/cdh4)

# Hadoop 2 jars
install(FILES ${MAVEN_REPOSITORY}/org/apache/hadoop/hadoop-annotations/${HADOOP2_VERSION}/hadoop-annotations-${HADOOP2_VERSION}.jar
              DESTINATION lib/java/hadoop2)
install(FILES ${MAVEN_REPOSITORY}/org/apache/hadoop/hadoop-auth/${HADOOP2_VERSION}/hadoop-auth-${HADOOP2_VERSION}.jar
              DESTINATION lib/java/hadoop2)
install(FILES ${MAVEN_REPOSITORY}/org/apache/hadoop/hadoop-common/${HADOOP2_VERSION}/hadoop-common-${HADOOP2_VERSION}.jar
              DESTINATION lib/java/hadoop2)
install(FILES ${MAVEN_REPOSITORY}/org/apache/hadoop/hadoop-hdfs/${HADOOP2_VERSION}/hadoop-hdfs-${HADOOP2_VERSION}.jar
              DESTINATION lib/java/hadoop2)
install(FILES ${MAVEN_REPOSITORY}/org/apache/hadoop/hadoop-mapreduce-client-core/${HADOOP2_VERSION}/hadoop-mapreduce-client-core-${HADOOP2_VERSION}.jar
              DESTINATION lib/java/hadoop2)
install(FILES ${HYPERTABLE_BINARY_DIR}/java/hypertable-hadoop2/target/hypertable-hadoop2-${VERSION}-bundled.jar
              DESTINATION lib/java/hadoop2 RENAME hypertable-hadoop2-${VERSION}.jar)

#!/usr/bin/env bash
#
# Copyright (C) 2007-2015 Hypertable, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# The installation directory
export HYPERTABLE_HOME=$(cd `dirname "$0"`/.. && pwd)
. $HYPERTABLE_HOME/bin/ht-env.sh

usage() {
  echo ""
  echo "usage: ht-start-hyperspace.sh [OPTIONS] [<server-options>]"
  echo ""
  echo "OPTIONS:"
  echo "  --valgrind  Run hyperspace with valgrind"
  echo ""
}

while [ $# -gt 0 ]; do
  case $1 in
    --valgrind)
      VALGRIND="valgrind -v --log-file=vg.hyperspace.%p --leak-check=full --num-callers=20 "
      shift
      ;;
    -h|--help)
      usage;
      exit 0
      ;;
    *)
      break
      ;;
  esac
done

set_start_vars Hyperspace
check_pidfile $pidfile && exit 0

$HYPERTABLE_HOME/bin/ht-check-hyperspace.sh --silent "$@"
if [ $? != 0 ] ; then
  exec_server htHyperspace --verbose "$@"
  wait_for_ok hyperspace "Hyperspace" "$@"
else
  echo "WARNING: Hyperspace already running."
fi

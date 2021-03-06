#!/usr/bin/env bash
#
# Copyright (C) 2007-2015 Hypertable, Inc.
#
# This file is part of Hypertable.
#
# Hypertable is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 of the
# License, or any later version.
#
# Hypertable is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
#

# The installation directory
export HYPERTABLE_HOME=$(cd `dirname "$0"`/.. && pwd)
. $HYPERTABLE_HOME/bin/ht-env.sh

usage() {
  echo ""
  echo "usage: ht-stop-rangeserver.sh [<server-options>]"
  echo ""
}

while [ $# -gt 0 ]; do
  case $1 in
    -h|--help)
      usage;
      exit 0
      ;;
    *)
      break
      ;;
  esac
done

echo 'shutdown' | $HYPERTABLE_HOME/bin/ht rangeserver --batch --silent --no-hyperspace $@
wait_for_critical rangeserver "RangeServer" "$@"
if [ $? -ne 0 ]; then
  stop_server rangeserver
fi

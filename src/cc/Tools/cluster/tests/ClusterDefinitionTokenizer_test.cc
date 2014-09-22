/*
 * Copyright (C) 2007-2014 Hypertable, Inc.
 *
 * This file is part of Hypertable.
 *
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 3 of the
 * License, or any later version.
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
#include <Common/Compat.h>

#include <Tools/cluster/ClusterDefinitionTokenizer.h>

#include <Common/Error.h>
#include <Common/FileUtils.h>
#include <Common/Logger.h>
#include <Common/String.h>

#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

using namespace Hypertable;
using namespace std;

int main(int argc, char **argv) {
  string line;
  string output;

  if (argc != 3) {
    cout << "usage: ClusterDefinitionTokenizer_test <input-file> <golden-file>" << endl;
    exit(1);
  }

  ifstream input_file(argv[1]);

  if (!input_file.is_open()) {
    cout << "Unable to open input file '" << argv[1] << "'" << endl;
    exit(1);
  }

  ofstream output_file;
  output_file.open("ClusterDefinitionTokenizer_test.output");

  while (getline(input_file, line)) {

    if (line.find_first_of("#### test-definition:") == 0) {
      if (!output.empty())
        output_file << output;
      output.clear();
    }

    output.append(line);
    output.append("\n");
  }

  if (!output.empty())
    output_file << output;

  output_file.close();
  input_file.close();

  string cmd = format("diff ClusterDefinitionTokenizer_test.output %s", argv[2]);
  if (system(cmd.c_str()))
    exit(1);

  return 0;
}

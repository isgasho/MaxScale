/*
 * Copyright (c) 2018 MariaDB Corporation Ab
 *
 * Use of this software is governed by the Business Source License included
 * in the LICENSE.TXT file and at www.mariadb.com/bsl11.
 *
 * Change Date: 2022-01-01
 *
 * On the date above, in accordance with the Business Source License, use
 * of this software will be governed by version 2 or later of the General
 * Public License.
 */

#include "mariadbmon_common.hh"

/** Server id default value */
const int64_t SERVER_ID_UNKNOWN = -1;
/** Default gtid domain */
const int64_t GTID_DOMAIN_UNKNOWN = -1;
/** Default port */
const int PORT_UNKNOWN = 0;

using std::string;

DelimitedPrinter::DelimitedPrinter(const string& separator)
    : m_separator(separator)
{
}

void DelimitedPrinter::cat(string& target, const string& addition)
{
    target += m_current_separator + addition;
    m_current_separator = m_separator;
}

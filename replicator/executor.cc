/*
 * Copyright (c) 2019 MariaDB Corporation Ab
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

#define MXB_MODULE_NAME "SQLExecutor"

#include "executor.hh"

#include <maxbase/log.h>

SQLExecutor::SQLExecutor(const cdc::Server& servers)
    : m_server(servers)
{
}

bool SQLExecutor::connect()
{
    bool rval = true;

    if (!m_sql)
    {
        auto res = SQL::connect({m_server});
        m_sql = std::move(res.second);

        if (!m_sql)
        {
            MXB_ERROR("%s", res.first.c_str());
            rval = false;
        }
        else if (!m_sql->query("SET default_storage_engine=COLUMNSTORE")
                 || !m_sql->query("SET autocommit=0"))
        {
            MXB_ERROR("%s", m_sql->error().c_str());
            m_sql.reset();
            rval = false;
        }
    }

    return rval;
}

bool SQLExecutor::process_query(MARIADB_RPL_EVENT* event)
{
    bool rval = true;
    auto db = to_string(event->event.query.database);
    auto stmt = to_string(event->event.query.statement);
    MXB_INFO("[%s] %s", db.c_str(), stmt.c_str());

    // This is probably quite close to what the server actually does to execute query events
    if ((!db.empty() && !m_sql->query("USE " + db)) || !m_sql->query(stmt))
    {
        MXB_ERROR("%s", m_sql->error().c_str());
        m_sql.reset();
        rval = false;
    }

    return rval;
}

bool SQLExecutor::process_uservar(MARIADB_RPL_EVENT* event)
{
    bool rval = true;

    std::string name = to_string(event->event.uservar.name);
    std::string value = event->event.uservar.is_null ? "NULL" : to_string(event->event.uservar.value);

    if (!m_sql->query("SET @%s='%s'", name.c_str(), value.c_str()))
    {
        MXB_ERROR("%s", m_sql->error().c_str());
        m_sql.reset();
        rval = false;
    }

    return rval;
}

bool SQLExecutor::process(const std::vector<MARIADB_RPL_EVENT*>& queue)
{
    // Database connection was created in SQLExecutor::start_transaction
    for (MARIADB_RPL_EVENT* event : queue)
    {
        switch (event->event_type)
        {
        case QUERY_EVENT:
            // TODO: Filter out ENGINE=... parts and index definitions from CREATE and ALTER statements
            if (!process_query(event))
            {
                return false;
            }
            break;

        case USER_VAR_EVENT:
            if (process_uservar(event))
            {
                return false;
            }
            break;

        default:
            mxb_assert(!true);
            break;
        }
    }

    return true;
}

bool SQLExecutor::start_transaction()
{
    return connect();
}

bool SQLExecutor::commit_transaction()
{
    bool rval = m_sql->query("COMMIT");

    if (!rval)
    {
        MXB_ERROR("%s", m_sql->error().c_str());
    }

    return rval;
}


void SQLExecutor::rollback_transaction()
{
    if (m_sql)
    {
        m_sql->query("ROLLBACK");
    }
}
#pragma once
/*
 * Copyright (c) 2016 MariaDB Corporation Ab
 *
 * Use of this software is governed by the Business Source License included
 * in the LICENSE.TXT file and at www.mariadb.com/bsl.
 *
 * Change Date: 2019-07-01
 *
 * On the date above, in accordance with the Business Source License, use
 * of this software will be governed by version 2 or later of the General
 * Public License.
 */

/**
 * @file modules.h  Utilities for loading modules
 *
 * The module interface used within the gateway
 *
 * @verbatim
 * Revision History
 *
 * Date     Who                 Description
 * 13/06/13 Mark Riddoch        Initial implementation
 * 08/07/13 Mark Riddoch        Addition of monitor modules
 * 29/05/14 Mark Riddoch        Addition of filter modules
 * 01/10/14 Mark Riddoch        Addition of call to unload all modules on shutdown
 * 19/02/15 Mark Riddoch        Addition of moduleGetList
 * 26/02/15 Massimiliano Pinto  Addition of module_feedback_send
 *
 * @endverbatim
 */

#include <maxscale/cdefs.h>
#include <maxscale/dcb.h>
#include <maxscale/modinfo.h>
#include <maxscale/resultset.h>
#include <maxscale/debug.h>

MXS_BEGIN_DECLS

/**
 * Module types
 */
#define MODULE_PROTOCOL         "Protocol"        /**< A protocol module type */
#define MODULE_AUTHENTICATOR    "Authenticator"   /**< An authenticator module type */
#define MODULE_ROUTER           "Router"          /**< A router module type */
#define MODULE_MONITOR          "Monitor"         /**< A database monitor module type */
#define MODULE_FILTER           "Filter"          /**< A filter module type */
#define MODULE_QUERY_CLASSIFIER "QueryClassifier" /**< A query classifier module type */


/**
 *@brief Load a module
 *
 * @param module Name of the module to load
 * @param type   Type of module, used purely for registration
 * @return       The module specific entry point structure or NULL
 */
void *load_module(const char *module, const char *type);

/**
 * @brief Get a module
 *
 * @param name Name of the module
 * @return The loaded module or NULL if the module is not loaded
 */
const MXS_MODULE *get_module(const char *name);

/**
 * @brief Unload a module.
 *
 * No errors are returned since it is not clear that much can be done
 * to fix issues relating to unloading modules.
 *
 * @param module The name of the module
 */
void unload_module(const char *module);

/**
 * @brief Unload all modules
 *
 * Remove all the modules from the system, called during shutdown
 * to allow termination hooks to be called.
 */
void unload_all_modules();

/**
 * @brief Print Modules
 *
 * Diagnostic routine to display all the loaded modules
 */
void printModules();

/**
 * @brief Print Modules to a DCB
 *
 * Diagnostic routine to display all the loaded modules
 */
void dprintAllModules(DCB *);

/**
 * @brief Return a resultset that has the current set of modules in it
 *
 * @return A Result set
 */
RESULTSET *moduleGetList();

/**
 * @brief Send loaded modules info to notification service
 *
 *  @param data The configuration details of notification service
 */
void module_feedback_send(void*);

/**
 * @brief Show feedback report
 *
 * Prints the feedback report to a DCB
 */
void moduleShowFeedbackReport(DCB *dcb);

MXS_END_DECLS

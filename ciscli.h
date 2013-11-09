/****************************************************************************
 * CisCLI API declarations
 ****************************************************************************
 * CisCLI makes it easy to generate Cisco-style CLIs
 * Copyright (C) 2013 Nirenjan Krishnan <nirenjan@gmail.com>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 ***************************************************************************/
/** @file */

#ifndef CISCLI_H
#define CISCLI_H

#include <stdint.h>

__BEGIN_DECLS;

/** @defgroup CisCLI CisCLI Public API */

/** @addtogroup CisCLI
 * @{
 */

/** @brief Structure that holds all CLI data
 *
 * All the CisCLI APIs will be passed a pointer to this data type.
 */
typedef struct _ciscli  ciscli;

/** @brief Structure that manages CLI command nodes
 *
 * All the CisCLI Node APIs will be passed and/or return a pointer to
 * this data type.
 */
typedef struct _ciscli_node  ciscli_node;

/** @brief Enumeration that describes all CLI node types
 *
 * Each and every node type handled by CisCLI is enumerated here.
 */
/* The order declared here is also the priority of the selection, i.e.,
 * if more than one node matches, and they are of different types, the
 * node with the lower node type wins
 */
typedef enum {
    /** Nodes that handle constant alphanumeric strings */
    CISCLI_KEYWORD,
    /** Nodes that can handle a range of 64-bit integers */
    CISCLI_INTEGER,
    /** Nodes that can handle a range of double precision numbers */
    CISCLI_FLOAT,
    /** Nodes that can handle IPv4 and IPv6 addresses */
    CISCLI_IPADDR,
    /** Nodes that can handle Ethernet MAC addresses */
    CISCLI_MACADDR,
    /** Nodes that are used to create branches in the parse chain */
    CISCLI_CONDITIONAL,
    /** Nodes that are used to set parameters into the \ref ciscli structure */
    CISCLI_CONSTANT,
    /** Nodes that are used to get a specific range of keywords */
    CISCLI_KW_TRIE,
    /** Nodes that can handle an arbitrary alphanumeric string */
    CISCLI_STRING,        /* Must be the penultimate entry! */
    CISCLI_NODE_TYPE_MAX, /* Must be the last entry! */
} ciscli_node_type;

/** @brief Constant that indicates no parent tree when creating a parse tree
 *
 * Use this as the parent value when calling \ref ciscli_tree_alloc and you
 * want to create a top level parse tree
 */
const uint32_t CISCLI_NO_PARENT_TREE = 0;

/** @brief Allocate a CisCLI structure
 *
 * This will allocate a \ref ciscli structure and initialize it with
 * default values before returning the pointer to the user. This pointer
 * must be used in all calls to the API.
 *
 * @returns Pointer to an initialized \ref ciscli structure. If it fails
 *          for any reason, it returns NULL and sets errno accordingly.
 */
ciscli * ciscli_alloc(void);

/** @brief Free a CisCLI structure
 *
 * This will free the passed \ref ciscli structure and all associated data and
 * clear the pointer in order to prevent double freeing. Do NOT simply call free
 * on this pointer, it will result in leaving the associated data structures
 * hanging around and leaking memory.
 *
 * @param   cli     A pointer to a pointer to a \ref ciscli
 */
void ciscli_free(ciscli **cli);

/** @brief Create a new parse tree
 *
 * CisCLI commands are grouped into individual parse trees. This function
 * creates a parse tree and initializes it with defaults before returning an
 * index. This index must be passed to all command APIs.
 *
 * @param   cli     A pointer to a \ref ciscli structure.
 * @param   name    A pointer to a null-terminated string which describes the
 *                  parse tree.
 * @param   parent  Index of the parent parse tree. Pass \ref
 *                  CISCLI_NO_PARENT_TREE for no parent.
 *
 * @returns Index to the initialized parse tree. If it fails for any reason,
 *          it returns 0 and sets errno accordingly.
 */
uint32_t ciscli_tree_alloc(ciscli *cli, const char *name, uint32_t parent);

/** @brief Retrieve the root node for the given parse tree index
 *
 * Every parse tree has a root node which is used to save all the command
 * nodes. This node must be a parent to the top level commands.
 *
 * @param   cli     A pointer to a \ref ciscli structure
 * @param   tree    Index of the parse tree
 *
 * @returns Pointer to the root node for the given parse tree. If the pointer
 *          parameter is NULL or the tree index is not valid, it returns NULL
 *          and sets errno.
 */
ciscli_node * ciscli_get_root_for_tree(ciscli *cli, uint32_t tree);

/** @name Generic I/O API
 *
 * Use these API functions to manage input, output and error streams.
 */
/** @{ */

/** @brief Get a line of input from the user and process it
 *
 * This function works in concert with the registered commands and processes
 * user input.
 *
 * @params  cli     A pointer to a \ref ciscli structure
 *
 * @returns 0 on success, 1 on EOF.
 */
int ciscli_input(ciscli *cli);

/* GCC attribute to indicate that this is a printf style function */
#if __GNUC__
#define PRINTF_ATTR __attribute__ ((format (printf, 2, 3)))
#else
#define PRINTF_ATTR
#endif

/** @brief Write formatted text to the output stream
 *
 * This function takes a printf style format specifier and writes the formatted
 * string to the output file descriptor.
 *
 * @param   cli     A pointer to a \ref ciscli structure
 * @param   fmt     A printf-style format string
 * @param   ...     Arguments for the format string
 *
 * @returns Number of characters in the formatted output, negative if failure.
 */
int ciscli_print(ciscli *cli, const char *fmt, ...) PRINTF_ATTR;

/** @brief Write formatted text to the error stream
 *
 * This function takes a printf style format specifier and writes the formatted
 * string to the error file descriptor.
 *
 * @param   cli     A pointer to a \ref ciscli structure
 * @param   fmt     A printf-style format string
 * @param   ...     Arguments for the format string
 *
 * @returns Number of characters in the formatted error, negative if failure.
 */
int ciscli_error(ciscli *cli, const char *fmt, ...) PRINTF_ATTR;

/** @} */

/** @name Generic Node API
 *
 * Use these API functions to manage nodes. These are generic and should work
 * with all node types. There may be some exceptions, but these will return
 * with an error.
 */
/** @{ */

/** @brief Create a new command node
 *
 * Create a command node of the given type, based on \ref ciscli_node_type.
 * This will return a pointer to the allocated node and you will need to use
 * this pointer to initialize the node with your required values.
 *
 * @param   type    Type of the node to allocate.
 *
 * @returns Pointer to the allocated node. If it fails for any reason, it
 *          returns NULL and sets errno accordingly.
 */
ciscli_node * ciscli_node_alloc(ciscli_node_type type);

/** @brief Add help text to a command node
 *
 * This function adds help text that is displayed when the '?' key is pressed.
 *
 * @param   node    Pointer to the node to add help text to
 * @param   help    Pointer to a null-terminated string for the help text
 *
 * @returns 0 on success, -1 and sets errno on failure.
 */
int ciscli_node_add_help_text(ciscli_node *node, const char *help);

/** @brief Add child node to parent node
 *
 * Add a node as a child to another node.
 *
 * @param   parent  Pointer to the node which is the parent
 * @param   child   Pointer to the node to add as a child
 *
 * @returns 0 on success, -1 and sets errno on failure
 */
int ciscli_node_add_child(ciscli_node *parent, ciscli_node *child);

/** @} */

/** @name Keyword Node API
 *
 * Use these API functions to manage keyword nodes, which can handle constant
 * alphanumeric strings.
 */
/** @{ */

/** @brief Set the keyword for a keyword node
 *
 * In order to use a keyword node, you must set the keyword it matches before
 * adding it to the parse tree.
 *
 * @param   node    Pointer to the allocated \ref ciscli_node
 * @param   kw      Pointer to a null-terminated char string that contains the
 *                  keyword to match.
 * @returns 0 on success, -1 on failure and sets errno accordingly.
 */
int ciscli_keyword_node_set_keyword(ciscli_node *node, const char *kw);

/** @brief Set a minimum required match for a keyword node
 *
 * This function can be used to set a minimum prefix length to be input before
 * the node can be matched. For example, a keyword `ipv4` can be set to require
 * an input of at least `ip` before matching the node.
 *
 * @param   node    Pointer to the allocated \ref ciscli_node
 * @param   min     Number of characters to match. This function will silently
 *                  limit the value to the length of the keyword.
 *
 * @returns 0 on success, -1 on failure and sets errno accordingly.
 */
int ciscli_keyword_node_set_min_match(ciscli_node *node, int min);

/** @brief Set an integer on matching the keyword
 *
 * When the node matches a keyword, set an integer at the specified index.
 *
 * @param   node    Pointer to the allocated \ref ciscli node
 * @param   index   Index of the integer to set. If it exceeds the maximum
 *                  allowable index, this function will fail.
 * @param   value   Value to set. This is a 64-bit signed integer.
 *
 * @returns 0 on success, -1 on failure and sets errno accordingly.
 */
int ciscli_keyword_node_on_match_set_integer(ciscli_node *node, uint32_t index, int64_t value);

/** @brief Set a bit on matching the keyword
 *
 * When the node matches a keyword, set a bit at the specified index
 *
 * @param   node    Pointer to the allocated \ref ciscli node
 * @param   index   Index of the integer to set the bit. If it exceeds the
 *                  maximum allowable index, this function will fail.
 * @param   bit     Bit to set, this can be anywhere from 0 to 63.
 *
 * @returns 0 on success, -1 on failure and sets errno accordingly.
 */
int ciscli_keyword_node_on_match_set_bit(ciscli_node *node, uint32_t index, uint32_t bit);

/** @brief Set a string on matching the keyword
 *
 * When the node matches a keyword, set a string at the specified index
 *
 * @param   node    Pointer to the allocated \ref ciscli node
 * @param   index   Index of the string to set. If it exceeds the
 *                  maximum allowable index, this function will fail.
 * @param   str     Pointer to a null-terminated string. If the string length
 *                  exceeds the maximum allowable length, the function will
 *                  silently ignore the extra characters and null-terminate it
 *                  accordingly.
 *
 * @returns 0 on success, -1 on failure and sets errno accordingly.
 */
int ciscli_keyword_node_on_match_set_string(ciscli_node *node, uint32_t index, const char *str);

/** @} */

/** @name Integer Node API
 *
 * Use these API functions to manage integer nodes, which can handle a range
 * of integers in decimal, hexadecimal and octal formats.
 */
/** @{ */

/** Flags to indicate the formats accepted by the integer node. */
enum ciscli_integer_format {
    CISCLI_INTEGER_DEC = (1 << 0),  /**< Accepts decimal format */
    CISCLI_INTEGER_HEX = (1 << 1),  /**< Accepts hexadecimal format */
    CISCLI_INTEGER_OCT = (1 << 2),  /**< Accepts octal format */
};

/** @brief Set the index into which to store the parsed integer
 *
 * This is a required function which specifies the index in the \ref ciscli
 * structure into which to save the parsed integer. If you do not call this
 * function, the index will default to 0, potentially overwriting any previously
 * stored data.
 *
 * @param   node    Pointer to the allocated \ref ciscli_node
 * @param   index   Index into which to save the parsed integer.
 *
 * @returns 0 on success, -1 on failure and sets errno accordingly.
 */
int ciscli_integer_node_set_index(ciscli_node *node, uint32_t index);

/** @brief Set the formats accepted by the integer node
 *
 * This is an optional function which specifies the formats that are parsed
 * by the node. If this function is not called on a node, then it defaults to
 * accepting all 3 formats - decimal, hexadecimal and octal.
 *
 * @param   node    Pointer to the allocated \ref ciscli_node
 * @param   format  Bitmask of formats accepted. Can be any combination of
 *                  the fields of \ref ciscli_integer_format.
 *
 * @returns 0 on success, -1 on failure and sets errno accordingly.
 */
int ciscli_integer_node_set_format(ciscli_node *node, uint32_t format);

/** @brief Set the range of integers accepted by the node.
 *
 * This is an optional function call that restricts the range of integers that
 * are accepted by the node. If you do not call this function, the default range
 * is `INT64_MIN - INT64_MAX`. \p max must be greater than \p min. It is
 * possible for both \p min and \p max to be equal, however, it is recommended
 * that if this be the case, it is better to create a keyword node to
 * match the specified integer and use the corresponding set functions to
 * set it into the \ref ciscli structure.
 *
 * @param   node    Pointer to the allocated \ref ciscli node
 * @param   min     Minimum value accepted by this node.
 * @param   max     Maximum value accepted by this node.
 *
 * @returns 0 on success, -1 on failure and sets errno accordingly.
 */
int ciscli_integer_node_set_range(ciscli_node *node, int64_t min, int64_t max);

/** @} */

/** @} */

__END_DECLS;

#endif /* end of include guard: CISCLI_H */


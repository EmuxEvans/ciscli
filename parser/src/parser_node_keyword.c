/****************************************************************************
 * CLI parser keyword node functions
 ****************************************************************************
 * CisCLI makes it easy to generate Cisco router style CLIs
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "parser_node_keyword.h"
#include "parser_node_registration.h"
#include "parser_control.h"

#define UNUSED __attribute__((unused))

static int32_t match_keyword(PARSER_NODE *node, PARSER_CTRL *ctl)
{
    PARSER_NODE_KEYWORD *knode = (PARSER_NODE_KEYWORD *)node;
    int32_t match;
    int32_t i;
    uint32_t index;
    int64_t value;
    uint32_t retval;
    char *cmdptr;

    if (!knode || !ctl) {
        return -EINVAL;
    }

    cmdptr = &ctl->command_line[ctl->total_parsed];

    for (i = 0, match = 0; i < KEYWORD_LENGTH_MAX; i++) {
        if (knode->keyword[i] == cmdptr[i] && knode->keyword[i] != '\0') {
            /* Matched so far */
            match++;
        //} else if (knode->keyword[i] == '\0' && cmdptr[i] == ' ') {
            /*
             * We've reached the end of the keyword and the command pointer
             * points to whitespace or the end of line. Nothing more to match.
             */
            //break;
        } else if (cmdptr[i] == ' ' || cmdptr == '\0') {
            /* We've reached the end of the command, nothing more to match */
            break;
        } else {
            /* Mismatch */
            return 0;
        }
    }

    if (match < knode->minimum_match) {
        /* We have not satisfied the minimum match requirement */
        return 0;
    }

    /*
     * Knock off any trailing spaces so the next node can start without
     * having to strip it off.
     */
    while (cmdptr[i] == ' ') {
        i++;
    }
    match = i;

    if (knode->header.node_flags & PARSER_NODE_KW_FLAG_SET_VALUE) {
        /* Get the index to set from the node */
        index = knode->index;

        if (knode->header.node_flags & PARSER_NODE_KW_FLAG_SET_BIT) {
            /* Set bit in integer at specified index */
            retval = parser_control_get_integer(ctl, index, &value);
            if (!retval) {
                value |= ((int64_t)1) << (knode->value);
                parser_control_set_integer(ctl, index, &value);
            }
        } else if (knode->header.node_flags & PARSER_NODE_KW_FLAG_SET_STRING) {
            /* Set string in specified index */
            parser_control_set_string(ctl, index, &knode->string[0]);
        } else {
            /* Set integer value in the specified index */
            parser_control_set_integer(ctl, index, &knode->value);
        }
    }

    return match;
}

static char * disp_keyword(PARSER_NODE *node, PARSER_CTRL *ctl UNUSED)
{
    PARSER_NODE_KEYWORD *knode = (PARSER_NODE_KEYWORD *)node;
    char *disp;

    disp = malloc(KEYWORD_LENGTH_MAX);

    if (disp) {
        if (!knode) {
            strcpy(disp,"NULL");
        } else {
            strncpy(disp, knode->keyword, KEYWORD_LENGTH_MAX);
        }
    }

    return disp;
}

static const PARSER_NODE_REG registration = {
    .get_child = DEFAULT,
    .get_sibling = DEFAULT,
    .match = match_keyword,
    .alt_text = disp_keyword,
};

SETUP_FUNCTION
void init_keyword_node(void)
{
    parser_node_register_type(PARSER_NODE_TYPE_KEYWORD, &registration);
}

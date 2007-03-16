/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Jan Bretschneider  *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    lexer_owfn_wrap.h
 *
 * \brief   compatibility wrapper for the lexer
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "lexer_owfn.h"

// #defines YY_FLEX_HAS_YYLEX_DESTROY for newer versions of flex. If
// YY_FLEX_HAS_YYLEX_DESTROY is defined, you should call yylex_destroy() after
// parsing is complete.

#if (YY_FLEX_MAJOR_VERSION > 2) || \
    ((YY_FLEX_MAJOR_VERSION == 2) && (YY_FLEX_MINOR_VERSION > 5)) || \
    ((YY_FLEX_MAJOR_VERSION == 2) && (YY_FLEX_MINOR_VERSION == 5) && \
     (YY_FLEX_SUBMINOR_VERSION >= 31))
#define YY_FLEX_HAS_YYLEX_DESTROY
#endif

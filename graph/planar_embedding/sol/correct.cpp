// Model solution: planar embedding by the Boyer-Myrvold edge addition
// algorithm, using John M. Boyer's reference implementation from the
// Edge Addition Planarity Suite (https://github.com/graph-algorithms/edge-addition-planarity-suite,
// c/graphLib, version 4.x), bundled into a single file.
//
// Modifications for this bundle: only the core planarity embedding path is
// kept (no outerplanarity, K_{2,3}/K_{3,3}/K_4 search, drawing, Kuratowski
// subgraph isolation, graph I/O, random graphs, or integrity tests), the
// parallel-edge detector was removed (inputs are simple graphs), and unused
// functions were deleted.  The algorithm itself is unmodified.
//
// Copyright (c) 1997-2026, John M. Boyer
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of The Edge Addition Planarity Suite nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


// ===== lowLevelUtils/apiutils.h =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/
#ifndef APIUTILS_H
#define APIUTILS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <limits.h>

#define MAXLINE 1024

// The string representation for an integer must account for: an optional '-',
// then 10 digits (max signed 32-bit int), and a null-terminator
#define MAXCHARSFOR32BITINT 11

#if defined(_MSC_VER) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
#define APPLY_FORMAT_ATTRIBUTE 0
#elif defined(__has_attribute)
#define APPLY_FORMAT_ATTRIBUTE __has_attribute(format)
#elif defined(__GNUC__) || defined(__clang__)
#define APPLY_FORMAT_ATTRIBUTE 1
#else
#define APPLY_FORMAT_ATTRIBUTE 0
#endif

#if APPLY_FORMAT_ATTRIBUTE
#if defined(__GNUC__) && !defined(__clang__)
#define FORMAT_PRINTF(formatIndex, firstArg) __attribute__((format(gnu_printf, formatIndex, firstArg)))
#else
#define FORMAT_PRINTF(formatIndex, firstArg) __attribute__((format(printf, formatIndex, firstArg)))
#endif
#else
#define FORMAT_PRINTF(formatIndex, firstArg)
#endif

    // These methods control whether gp_ErrorMessage() and gp_Message() calls
    // emit output or skip producing output (the default)
    unsigned gp_GetQuietMode(void);

#define QUIETMODE_NONE 0
#define QUIETMODE_ERRORS 1
#define QUIETMODE_MESSAGES 2
#define QUIETMODE_ALL 0XFFFFFFFF

#define gp_ErrorMessage(...) (gp_LogErrorMessage(__LINE__, __FILE__, __VA_ARGS__))
    void gp_LogErrorMessage(int lineNum, const char *srcFileName, const char *message, ...) FORMAT_PRINTF(3, 4);

#ifdef __cplusplus
}
#endif

#endif

// ===== lowLevelUtils/appconst.h =====
#ifndef APPCONST_H
#define APPCONST_H

/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/


// NOTE: This is defined on 32- and 64-bit Windows systems; see
// https://sourceforge.net/p/predef/wiki/OperatingSystems
#if defined(WIN32) || defined(_WIN32)
#define WINDOWS
#endif

/* Defines fopen strings for reading and writing text files on PC and UNIX */

#ifdef WINDOWS
#define READTEXT "rt"
#define WRITETEXT "wt"
#define FILE_DELIMITER '\\'
#else
#define READTEXT "r"
#define WRITETEXT "w"
#define FILE_DELIMITER '/'
#endif

/* Define DEBUG to get additional debugging. The default is to define it when MSC does */

#ifdef _DEBUG
#define DEBUG
#endif

/* Some low-level functions are replaced by faster macros, except when debugging */

#define SPEED_MACROS
#ifdef DEBUG
#undef SPEED_MACROS
#endif

/* Return status values; OK/NOTOK behave like Boolean true/false,
   not like program exit codes. */

#define OK 1
#define NOTOK 0

#ifdef DEBUG
#undef NOTOK
extern int debugNOTOK(void);
#include <stdio.h>
#define NOTOK (gp_ErrorMessage("NOTOK occurred"), debugNOTOK())
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

// Define one of these to use 1-based arrays or the original 0-based arrays
// It used to be true that the 1-based arrays were faster, but compiler
// optimizations have come a long way in two decades.
//
// The main advantages of 1-based arrays are readability of the data,
// that NIL is still an index within array bounds, that 1-based supports
// 0-based files but not the reverse, and continuing the long-time default.
#define USE_1BASEDARRAYS
// #define USE_0BASEDARRAYS

#ifdef USE_0BASEDARRAYS
#undef USE_1BASEDARRAYS
#endif

/* Array indexes are used as pointers, and NIL means bad pointer */
#ifdef USE_1BASEDARRAYS
// This definition is used with 1-based array indexing
#define NIL 0
#define NIL_CHAR 0x00
#else
// This definition is used with 0-based array indexing
#define NIL -1
#define NIL_CHAR 0xFF
#endif

#endif

// ===== lowLevelUtils/apiutils.private.h =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/
#ifndef APIUTILS_PRIVATE_H
#define APIUTILS_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

    /* PRIVATE FUNCTIONS FOR ADDITIONAL INFORMATIONAL LOGGING.

       If LOGGING is defined by uncommenting it below, then log-related lines
       write to the log, and otherwise they no-op.

       By default, neither release nor DEBUG builds including LOGGING.
       Logging is used to see more details of how various algorithms
       handle a particular graph. */

// #define LOGGING
#ifdef LOGGING

#define _gp_LogLine _LogLine
#define _gp_Log _Log

    void _LogLine(const char *Line);
    void _Log(const char *Line);

#define _gp_MakeLogStr1 _MakeLogStr1
#define _gp_MakeLogStr2 _MakeLogStr2
#define _gp_MakeLogStr3 _MakeLogStr3
#define _gp_MakeLogStr4 _MakeLogStr4
#define _gp_MakeLogStr5 _MakeLogStr5

    char *_MakeLogStr1(const char *format, int one);
    char *_MakeLogStr2(const char *format, int one, int two);
    char *_MakeLogStr3(const char *format, int one, int two, int three);
    char *_MakeLogStr4(const char *format, int one, int two, int three, int four);
    char *_MakeLogStr5(const char *format, int one, int two, int three, int four, int five);

#else
#define _gp_LogLine(Line)
#define _gp_Log(Line)
#define _gp_MakeLogStr1(format, one)
#define _gp_MakeLogStr2(format, one, two)
#define _gp_MakeLogStr3(format, one, two, three)
#define _gp_MakeLogStr4(format, one, two, three, four)
#define _gp_MakeLogStr5(format, one, two, three, four, five)
#endif

#ifdef __cplusplus
}
#endif

#endif

// ===== lowLevelUtils/listcoll.h =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#ifndef _LISTCOLL_H
#define _LISTCOLL_H

#ifdef __cplusplus
extern "C"
{
#endif

/* This include is needed for memset and memcpy */
#include <string.h>

        typedef struct
        {
                int prev, next;
        } lcnode;

        struct listCollectionStruct
        {
                int N;
                lcnode *List;
        };

        typedef struct listCollectionStruct listCollectionStruct;
        typedef listCollectionStruct *listCollectionP;

        listCollectionP LCNew(int N);
        void LCFree(listCollectionP *pListColl);

#ifndef SPEED_MACROS

        void LCReset(listCollectionP listColl);
        void LCCopy(listCollectionP dst, listCollectionP src);

        int LCGetNext(listCollectionP listColl, int theList, int theNode);
        int LCGetPrev(listCollectionP listColl, int theList, int theNode);

        int LCPrepend(listCollectionP listColl, int theList, int theNode);
        int LCAppend(listCollectionP listColl, int theList, int theNode);
        int LCDelete(listCollectionP listColl, int theList, int theNode);

#else

/* void LCReset(listCollectionP listColl); */

#define LCReset(listColl) memset(listColl->List, NIL_CHAR, listColl->N * sizeof(lcnode))

/* void LCCopy(listCollectionP dst, listCollectionP src) */

#define LCCopy(dst, src) memcpy(dst->List, src->List, src->N * sizeof(lcnode))

/* int  LCGetNext(listCollectionP listColl, int theList, int theNode);
        Return theNode's successor, unless it is theList head pointer */

#define LCGetNext(listColl, theList, theNode) listColl->List[theNode].next == theList ? NIL : listColl->List[theNode].next

/* int  LCGetPrev(listCollectionP listColl, int theList, int theNode);
        Return theNode's predecessor unless theNode is theList head.
        To start going backwards, use NIL for theNode, which returns theList head's predecessor
        Usage: Obtain last node, loop while NIL not returned, process node then get predecessor.
                After theList head processed, get predecessor returns NIL because we started with
                theList head's predecessor. */

#define LCGetPrev(listColl, theList, theNode) \
        (theNode == NIL                       \
             ? listColl->List[theList].prev   \
         : theNode == theList ? NIL           \
                              : listColl->List[theNode].prev)

/* int  LCPrepend(listCollectionP listColl, int theList, int theNode);
        If theList is empty, then theNode becomes its only member and is returned.
        Otherwise, theNode is placed before theList head, and theNode is returned as the new head. */

#define LCPrepend(listColl, theList, theNode)                                          \
        (theList == NIL                                                                \
             ? (listColl->List[theNode].prev = listColl->List[theNode].next = theNode) \
             : (listColl->List[theNode].next = theList,                                \
                listColl->List[theNode].prev = listColl->List[theList].prev,           \
                listColl->List[listColl->List[theNode].prev].next = theNode,           \
                listColl->List[theList].prev = theNode,                                \
                listColl->List[theList].prev))

/* int  LCAppend(listCollectionP listColl, int theList, int theNode);
        If theList is empty, then theNode becomes its only member and is returned.
        Otherwise, theNode is placed before theList head, and then theList head is returned. */

#define LCAppend(listColl, theList, theNode)                                           \
        (theList == NIL                                                                \
             ? (listColl->List[theNode].prev = listColl->List[theNode].next = theNode) \
             : (listColl->List[theNode].next = theList,                                \
                listColl->List[theNode].prev = listColl->List[theList].prev,           \
                listColl->List[listColl->List[theNode].prev].next = theNode,           \
                listColl->List[theList].prev = theNode,                                \
                theList))

/* int  LCDelete(listCollectionP listColl, int theList, int theNode);
        If theList contains only one node, then NIL it out and return NIL meaning empty list
        Otherwise, join the predecessor and successor, then
        return either the list head or its successor if the deleted node is the list head
        (in that case, the caller makes the successor become the new list head).*/

#define LCDelete(listColl, theList, theNode)                                                     \
        listColl->List[theList].next == theList                                                  \
            ? (listColl->List[theList].prev = listColl->List[theList].next = NIL)                \
            : (listColl->List[listColl->List[theNode].prev].next = listColl->List[theNode].next, \
               listColl->List[listColl->List[theNode].next].prev = listColl->List[theNode].prev, \
               (theList == theNode ? listColl->List[theNode].next : theList))

#endif

#ifdef __cplusplus
}
#endif

#endif

// ===== lowLevelUtils/stack.h =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#ifndef STACK_H
#define STACK_H

#ifdef __cplusplus
extern "C"
{
#endif

// includes mem functions like memcpy
#include <string.h>

        struct stackStruct
        {
                int *S;
                int size, capacity;
        };

        typedef struct stackStruct stackStruct;
        typedef stackStruct *stackP;

        stackP sp_New(int capacity);
        void sp_Free(stackP *pStack);

        int sp_CopyContent(stackP stackDst, stackP stackSrc);

#define sp_GetCapacity(theStack) (theStack->capacity)

#ifndef SPEED_MACROS

        int sp_ClearStack(stackP theStack);
        int sp_GetCurrentSize(stackP theStack);
        int sp_SetCurrentSize(stackP theStack, int top);

        int sp_IsEmpty(stackP theStack);
        int sp_NonEmpty(stackP theStack);

#define sp_Push(theStack, a)                       \
        {                                          \
                if (sp__Push(theStack, (a)) != OK) \
                        return NOTOK;              \
        }
#define sp_Push2(theStack, a, b)                         \
        {                                                \
                if (sp__Push2(theStack, (a), (b)) != OK) \
                        return NOTOK;                    \
        }

        int sp__Push(stackP theStack, int a);
        int sp__Push2(stackP theStack, int a, int b);

#define sp_Pop(theStack, a)                        \
        {                                          \
                if (sp__Pop(theStack, &(a)) != OK) \
                        return NOTOK;              \
        }
#define sp_Pop_Discard(theStack)                     \
        {                                            \
                if (sp__Pop_Discard(theStack) != OK) \
                        return NOTOK;                \
        }

#define sp_Pop2(theStack, a, b)                           \
        {                                                 \
                if (sp__Pop2(theStack, &(a), &(b)) != OK) \
                        return NOTOK;                     \
        }
#define sp_Pop2_Discard1(theStack, a)                        \
        {                                                    \
                if (sp__Pop2_Discard1(theStack, &(a)) != OK) \
                        return NOTOK;                        \
        }
#define sp_Pop2_Discard(theStack)                     \
        {                                             \
                if (sp__Pop2_Discard(theStack) != OK) \
                        return NOTOK;                 \
        }

        int sp__Pop(stackP theStack, int *pA);
        int sp__Pop_Discard(stackP theStack);

        int sp__Pop2(stackP theStack, int *pA, int *pB);
        int sp__Pop2_Discard1(stackP theStack, int *pA);
        int sp__Pop2_Discard(stackP theStack);

        int sp_Top(stackP theStack);
        int sp_Get(stackP theStack, int pos);
        int sp_Set(stackP theStack, int pos, int val);

#else

#define sp_ClearStack(theStack) theStack->size = 0
#define sp_GetCurrentSize(theStack) (theStack->size)
#define sp_SetCurrentSize(theStack, Size) ((Size) > theStack->capacity ? NOTOK : (theStack->size = (Size), OK))

#define sp_IsEmpty(theStack) !theStack->size
#define sp_NonEmpty(theStack) theStack->size

#define sp_Push(theStack, a) theStack->S[theStack->size++] = a
#define sp_Push2(theStack, a, b)      \
        {                             \
                sp_Push(theStack, a); \
                sp_Push(theStack, b); \
        }

#define sp_Pop(theStack, a) a = theStack->S[--theStack->size]
#define sp_Pop_Discard(theStack) --theStack->size

#define sp_Pop2(theStack, a, b)      \
        {                            \
                sp_Pop(theStack, b); \
                sp_Pop(theStack, a); \
        }
#define sp_Pop2_Discard1(theStack, a)     \
        {                                 \
                sp_Pop_Discard(theStack); \
                sp_Pop(theStack, a);      \
        }
#define sp_Pop2_Discard(theStack)         \
        {                                 \
                sp_Pop_Discard(theStack); \
                sp_Pop_Discard(theStack); \
        }

#define sp_Top(theStack) (theStack->size ? theStack->S[theStack->size - 1] : NIL)
#define sp_Get(theStack, pos) (theStack->S[pos])
#define sp_Set(theStack, pos, val) (theStack->S[pos] = val)

#endif

#ifdef __cplusplus
}
#endif

#endif

// ===== graphEdgeDetector.h =====
#ifndef GRAPH_EDGE_DETECTOR_H
#define GRAPH_EDGE_DETECTOR_H
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        unsigned *edgeDetector;
        int edgeDetectorCapacity;
    } graphEdgeDetectorStruct;

    typedef graphEdgeDetectorStruct *graphEdgeDetectorP;

    graphEdgeDetectorP ged_New(int theCapacity);

    unsigned long long ged_Hash(graphEdgeDetectorP theDetector, int v, int w);
    int ged_Set(graphEdgeDetectorP theDetector, int v, int w);

    void ged_Free(graphEdgeDetectorP *pDetector);

#ifdef __cplusplus
}
#endif
#endif

// ===== extensionSystem/graphFunctionTable.h =====
#ifndef GRAPHFUNCTIONTABLE_H
#define GRAPHFUNCTIONTABLE_H

/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     NOTE: If you add any FUNCTION POINTERS to this function table, then you must
           also initialize them in _InitFunctionTable() in graph.c.
    */
    typedef struct graphStruct graphStruct;
    typedef graphStruct *graphP;

    struct graphFunctionTableStruct
    {
        // These function pointers allow extension modules to overload some of
        // the behaviors of protected functions.  Only advanced applications
        // will overload these functions
        int (*fpEmbeddingInitialize)(graphP theGraph);
        void (*fpEmbedBackEdgeToDescendant)(graphP theGraph, int RootSide, int RootVertex, int W, int WPrevLink);
        void (*fpWalkUp)(graphP theGraph, int v, int e);
        int (*fpWalkDown)(graphP theGraph, int v, int RootVertex);
        int (*fpMergeBicomps)(graphP theGraph, int v, int RootVertex, int W, int WPrevLink);
        void (*fpMergeVertex)(graphP theGraph, int W, int WPrevLink, int R);
        int (*fpHandleInactiveVertex)(graphP theGraph, int BicompRoot, int *pW, int *pWPrevLink);
        int (*fpHandleBlockedBicomp)(graphP theGraph, int v, int RootVertex, int R);
        int (*fpEmbedPostprocess)(graphP theGraph, int v, int edgeEmbeddingResult);
        int (*fpMarkDFSPath)(graphP theGraph, int ancestor, int descendant);

        int (*fpCheckEmbeddingIntegrity)(graphP theGraph, graphP origGraph);
        int (*fpCheckObstructionIntegrity)(graphP theGraph, graphP origGraph);

        // These function pointers allow extension modules to overload some
        // of the behaviors of gp_* function in the public API
        int (*fpEnsureVertexCapacity)(graphP theGraph, int N);
        void (*fpResetGraphStorage)(graphP theGraph);
        int (*fpEnsureEdgeCapacity)(graphP theGraph, int requiredEdgeCapacity);
        int (*fpSortVertices)(graphP theGraph);

        int (*fpReadPostprocess)(graphP theGraph, char *extraData);
        int (*fpWritePostprocess)(graphP theGraph, char **pExtraData);

        int (*fpDeleteEdge)(graphP theGraph, int e);
        void (*fpHideEdge)(graphP theGraph, int e);
        void (*fpRestoreEdge)(graphP theGraph, int e);
        int (*fpHideVertex)(graphP theGraph, int vertex);
        int (*fpRestoreVertex)(graphP theGraph);
        int (*fpContractEdge)(graphP theGraph, int e);
        int (*fpIdentifyVertices)(graphP theGraph, int u, int v, int eBefore);
    };

    typedef struct graphFunctionTableStruct graphFunctionTableStruct;
    typedef graphFunctionTableStruct *graphFunctionTableP;

#ifdef __cplusplus
}
#endif

#endif

// ===== graph.h =====
#ifndef GRAPH_H
#define GRAPH_H

/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#ifdef __cplusplus
extern "C"
{
#endif

// Basic declarations, such as for OK, NOTOK, and NIL

    ///////////////////////////////////////////////////////////////////////////////
    // The top-level operations at the graph, vertex, and edge levels
    ///////////////////////////////////////////////////////////////////////////////

    // Forward declaration of graph structure and graph pointer type definitions
    // (see the end of this header file).
    typedef struct graphStruct graphStruct;
    typedef graphStruct *graphP;

    // Methods related to graph allocation and destruction
    graphP gp_New(void);

    int gp_EnsureVertexCapacity(graphP theGraph, int N);

    void gp_Free(graphP *pGraph);

// N=# of vertices; NV=# of virtual vertices; M=# of edges
#define gp_GetN(theGraph) ((theGraph)->N)
#define gp_GetNV(theGraph) ((theGraph)->NV)
#define gp_GetM(theGraph) ((theGraph)->M)
#define gp_GetEdgeCapacity(theGraph) ((theGraph)->edgeCapacity)

    // Basic graph utility methods

    // Basic graph I/O methods: see graphIO.h
    // Intermediate graph I/O methods: see g6-read-iterator.h and g6-write-iterator.h

    // Basic vertex interrogators

    // Basic interrogators for directed graphs
    // The direction can be EDGEFLAG_DIRECTION_INONLY or EDGEFLAG_DIRECTION_OUTONLY

    // Basic graph structure manipulators
    int gp_AddEdge(graphP theGraph, int u, int ulink, int v, int vlink);
    int gp_InsertEdge(graphP theGraph, int u, int e_u, int e_ulink,
                      int v, int e_v, int e_vlink);

    // Intermediate graph structure manipulators
    void gp_HideEdge(graphP theGraph, int e);
    void gp_RestoreEdge(graphP theGraph, int e);

    // Advanced graph structure manipulators
    int gp_ContractEdge(graphP theGraph, int e);
    int gp_IdentifyVertices(graphP theGraph, int u, int v, int eBefore);

    // For methods and declarations related to depth-first search (DFS), see graphDFSUtils.h

/* Graph Flags (bit flags set by various public graphLib APIs):
        Bits 0-3 reserved for base graph class
        Bits 4-7 reserved for graph I/O
        Bits 8-15 reserved for DFS Utils
        Bits 16-23 reserved for Planarity-Related
        bits 24-31 reserved for future expansion
*/
#define GRAPHFLAGS_DIRECTEDEDGEDETECTED 1
#define GRAPHFLAGS_PARALLELEDGEDETECTED 4
#define gp_GetGraphFlags(theGraph) ((theGraph)->graphFlags)

    // For graph embedding methods and declarations, see graphPlanarity.h

    /********************************************************************
     Vertex Record Definition (For non-virtual and virtual vertices)

     This record definition provides the data members needed for the
     core structural information for both vertices and virtual vertices.
     Non-virtual vertices are also equipped with additional information
     provided by private vertexInfo records.

     The vertices of a graph are stored in the first N locations of array V.
     Virtual vertices are secondary vertices used to help represent the
     main vertices in substructural components of a graph (such as in
     biconnected components).

     link[2]: the first and last edge records in the adjacency list
              of the vertex.

     index: In vertices, stores either the depth first index of a vertex or
            the original array index of the vertex if the vertices of the
            graph are sorted by DFI.
            In virtual vertices, the index may be used to indicate the vertex
            that the virtual vertex represents, unless an algorithm has some
            other way of making the association (for example, the planarity
            algorithms rely on biconnected components and therefore place
            virtual vertices of a vertex at positions corresponding to the
            DFS children of the vertex).

     flags: Bits 0-15 reserved for library; bits 16 and higher for apps
            Bit 0: visited, for vertices and virtual vertices
            Bit 1: marked, 2nd visited flag, for while visiting all
                    Used in K4 homeomorph search algorithm
            Bit 2: Obstruction type VERTEX_TYPE_SET (versus not set, i.e. VERTEX_TYPE_UNKNOWN)
            Bit 3: Obstruction type qualifier RYW (set) versus RXW (clear)
            Bit 4: Obstruction type qualifier high (set) versus low (clear)
                    Bits 2-4 used in planarity-related algorithms
     ********************************************************************/

    struct vertexRec
    {
        int link[2];
        int index;
        unsigned flags;
    };

    typedef struct vertexRec vertexRec;
    typedef vertexRec *vertexRecP;

////////////////////////////////////////////
// Accessors for vertex adjacency list links
// such as for adjacency list iteration
// (see also gp_GetNextEdge and gp_IsEdge)
////////////////////////////////////////////
#define gp_GetFirstEdge(theGraph, v) (theGraph->V[v].link[0])
#define gp_GetLastEdge(theGraph, v) (theGraph->V[v].link[1])
#define gp_GetEdgeByLink(theGraph, v, theLink) (theGraph->V[v].link[theLink])

// Setters for adjacency list links
#define gp_SetFirstEdge(theGraph, v, newFirstEdge) (theGraph->V[v].link[0] = newFirstEdge)
#define gp_SetLastEdge(theGraph, v, newLastEdge) (theGraph->V[v].link[1] = newLastEdge)
#define gp_SetEdgeByLink(theGraph, v, theLink, newEdge) (theGraph->V[v].link[theLink] = newEdge)

///////////////////////////////////
// Vertex iteration-related methods
///////////////////////////////////

// The original N non-virtual vertices of a graph start at the lowest allowed storage location.
// The upper bound is one-past-the-end of the storage for the N non-virtual vertices.
#ifdef USE_1BASEDARRAYS
#define gp_LowerBoundVertices(theGraph) (1)
#else
#define gp_LowerBoundVertices(theGraph) (0)
#endif

#define gp_UpperBoundVertices(theGraph) (gp_LowerBoundVertexStorage(theGraph) + gp_GetN(theGraph))

// The virtual vertices start at the one-past-the-end upper bound of the non-virtual vertices.
// The upper bound is one-past-the-end of the virtual vertex storage locations.
#define gp_LowerBoundVirtualVertices(theGraph) gp_UpperBoundVertices(theGraph)
#define gp_UpperBoundVirtualVertices(theGraph) (gp_LowerBoundVertexStorage(theGraph) + gp_GetN(theGraph) + gp_GetNV(theGraph))

// These lower and upper bounds for all vertex storage are used for tasks like initializing
// that must iterate through all of non-virtual and virtual vertex records.
#define gp_LowerBoundVertexStorage(theGraph) gp_LowerBoundVertices(theGraph)
#define gp_UpperBoundVertexStorage(theGraph) gp_UpperBoundVirtualVertices(theGraph)

#ifdef USE_1BASEDARRAYS
// The use of *Vertex* alone consistently refers to the first N vertices.
// The use of *VirtualVertex* refers to vertex array locations after the first N.
#ifndef DEBUG
#define gp_IsVertex(theGraph, v) (v)
#define gp_IsVirtualVertex(theGraph, v) ((v) > gp_GetN(theGraph))
#else
// See below for definitions common to 1-based and 0-based
#endif

#else // Using 0-based Arrays
#ifndef DEBUG
#define gp_IsVertex(theGraph, v) ((v) != NIL)
#define gp_IsVirtualVertex(theGraph, v) ((v) >= gp_GetN(theGraph))
#else
// See below for definitions common to 1-based and 0-based
#endif
#endif // End of 0-based Arrays

// The same for 1-based and 0-based when debugging
#ifdef DEBUG
#define gp_IsVertex(theGraph, v) \
    ((v) == NIL ? 0 : ((v) < gp_LowerBoundVertices(theGraph) ? (NOTOK, 0) : ((v) >= gp_UpperBoundVertices(theGraph) ? (NOTOK, 0) : 1)))

// NOTE: gp_IsVirtualVertex() is sometimes called to distinguish between
// an existing non-virtual and a virtual
#define gp_IsVirtualVertex(theGraph, v)                                    \
    ((v) == NIL                                                            \
         ? 0                                                               \
         : ((v) < gp_LowerBoundVirtualVertices(theGraph)                   \
                ? ((v) < gp_LowerBoundVertices(theGraph) ? (NOTOK, 0) : 0) \
                : ((v) >= gp_UpperBoundVirtualVertices(theGraph) ? (NOTOK, 0) : 1)))

#endif

#define gp_IsNotVertex(theGraph, v) (!(gp_IsVertex(theGraph, v)))
#define gp_IsNotVirtualVertex(theGraph, v) (!(gp_IsVirtualVertex(theGraph, v)))

#define gp_VirtualVertexInUse(theGraph, virtualVertex) (gp_IsEdge(theGraph, gp_GetFirstEdge(theGraph, virtualVertex)))
#define gp_VirtualVertexNotInUse(theGraph, virtualVertex) (gp_IsNotEdge(theGraph, gp_GetFirstEdge(theGraph, virtualVertex)))
///////////////////////////////////////////
// End of Vertex iteration-related methods
//////////////////////////////////////////

// Accessors for non-virtual and virtual vertex index value
#define gp_GetIndex(theGraph, v) (theGraph->V[v].index)
#define gp_SetIndex(theGraph, v, theIndex) (theGraph->V[v].index = theIndex)

// Initializer for non-virtual and virtual vertex flags
#define gp_InitFlags(theGraph, v) (theGraph->V[v].flags = 0)

// Definition and accessors for the non-virtual and virtual vertex visited flag
#define VERTEX_VISITED_MASK 1
#define gp_GetVisited(theGraph, v) (theGraph->V[v].flags & VERTEX_VISITED_MASK)
#define gp_ClearVisited(theGraph, v) (theGraph->V[v].flags &= ~VERTEX_VISITED_MASK)
#define gp_SetVisited(theGraph, v) (theGraph->V[v].flags |= VERTEX_VISITED_MASK)

// Definition and accessors for the non-virtual and virtual vertex marked flag
// Essentially, this is a second visitation flag that can help applications that
// must visit all vertices to analyze and mark the ones important for some purpose.
#define VERTEX_MARKED_MASK 2
#define gp_GetMarked(theGraph, v) (theGraph->V[v].flags & VERTEX_MARKED_MASK)
#define gp_ClearMarked(theGraph, v) (theGraph->V[v].flags &= ~VERTEX_MARKED_MASK)
#define gp_SetMarked(theGraph, v) (theGraph->V[v].flags |= VERTEX_MARKED_MASK)

    /********************************************************************
     Edge Record Definition

     An edge is defined by a pair of edge records allocated in array E
     of a graph. A pair of edge records represents the edge in the
     adjacency lists of each vertex to which the edge is incident.

     link[2]: the next and previous edge records in the adjacency
              list that contains this edge record.

     v: The vertex neighbor of the vertex whose adjacency list contains
        this edge record (an index into array V).

     flags: Bits 0-15 reserved for library; bits 16 and higher for apps
            Bit 0: Visited
            Bit 1: Marked (2nd visited flag, for while visiting all)
            Bit 2: DFS type has been set, versus not set
            Bit 3: DFS tree edge, versus cycle edge (co-tree edge, etc.)
            Bit 4: DFS edge pointing to descendant, versus to ancestor
            Bit 5: Inverted (same as marking an edge with a "sign" of -1)
            Bit 6: Edge record is directed into the containing vertex only
            Bit 7: Edge record is directed from the containing vertex only
     ********************************************************************/

    struct edgeRec
    {
        int link[2];
        int neighbor;
        unsigned flags;
    };

    typedef struct edgeRec edgeRec;
    typedef edgeRec *edgeRecP;

// An edge is represented by two consecutive edge records in the edge array E.
// If an even number, xor 1 will add one; if an odd number, xor 1 will subtract 1
#define gp_GetTwin(theGraph, e) ((e) ^ 1)

////////////////////////////////////////////
// Access to adjacency list pointers
// such as for adjacency list iteration
////////////////////////////////////////////
#define gp_GetNextEdge(theGraph, e) (theGraph->E[e].link[0])
#define gp_GetPrevEdge(theGraph, e) (theGraph->E[e].link[1])
#define gp_GetAdjacentEdge(theGraph, e, theLink) (theGraph->E[e].link[theLink])

#define gp_SetNextEdge(theGraph, e, newNextEdge) (theGraph->E[e].link[0] = newNextEdge)
#define gp_SetPrevEdge(theGraph, e, newPrevEdge) (theGraph->E[e].link[1] = newPrevEdge)
#define gp_SetAdjacentEdge(theGraph, e, theLink, newEdge) (theGraph->E[e].link[theLink] = newEdge)

////////////////////////////////////////////
// gp_IsEdge() helps detect the end of an
// adjacency list iteration loop
////////////////////////////////////////////
#ifdef USE_1BASEDARRAYS
#define gp_IsEdge(theGraph, e) (e)
#define gp_IsNotEdge(theGraph, e) (!(e))
#else
#define gp_IsEdge(theGraph, e) ((e) != NIL)
#define gp_IsNotEdge(theGraph, e) ((e) == NIL)
#endif

// Get/set 'neighbor' member indicated by edge record e
#define gp_GetNeighbor(theGraph, e) (theGraph->E[e].neighbor)
#define gp_SetNeighbor(theGraph, e, v) (theGraph->E[e].neighbor = v)

// Initializer for edge flags
#define gp_InitEdgeFlags(theGraph, e) (theGraph->E[e].flags = 0)

// Definitions of and access to edge flags
#define EDGE_VISITED_MASK 1
#define gp_GetEdgeVisited(theGraph, e) (theGraph->E[e].flags & EDGE_VISITED_MASK)
#define gp_ClearEdgeVisited(theGraph, e) (theGraph->E[e].flags &= ~EDGE_VISITED_MASK)
#define gp_SetEdgeVisited(theGraph, e) (theGraph->E[e].flags |= EDGE_VISITED_MASK)

// Definition and accessors for the edge marked flag
// Essentially, this is a second visitation flag that can help applications that
// must visit all edges to analyze and mark the ones important for some purpose.
#define EDGE_MARKED_MASK 2
#define gp_GetEdgeMarked(theGraph, e) (theGraph->E[e].flags & EDGE_MARKED_MASK)
#define gp_ClearEdgeMarked(theGraph, e) (theGraph->E[e].flags &= ~EDGE_MARKED_MASK)
#define gp_SetEdgeMarked(theGraph, e) (theGraph->E[e].flags |= EDGE_MARKED_MASK)

// The edge type is defined by bits 2-4 and 8, 4+8+16+256=284
// Bit 2 set means the edge record neighbor field indicates a parent (in a
//       tree edge) or ancestor (in a "back" edge, aka a cotree edge)
// Bit 3 set means edge record is part of a "tree" edge whose endpoints share
//       the direct DFS parent/child relationship
// Bit 4 set means the edge record's neighbor field indicates either a child
//       (in a tree edge) or a descendant (in a "back" edge, aka "cotree" edge)
// Bit 5 set means the edge record is in a "cross" edge that is neither a
//       tree edge nor a back edge (so, bits 2, 3, and 4 are clear).
#define EDGE_TYPE_MASK 284

// EDGE_TYPE_NOTDEFINED - the edge record type has not been defined
#define EDGE_TYPE_NOTDEFINED 0

// EDGE_TYPE_TREE - gives a name to the bit indicating a tree edge
// EDGE_TYPE_PARENT - edge record neighbor field indicates DFS parent
// EDGE_TYPE_CHILD - edge record neighbor field indicates a DFS child
#define EDGE_TYPE_TREE 8
#define EDGE_TYPE_PARENT 12
#define EDGE_TYPE_CHILD 28

// EDGE_TYPE_BACK - edge record points to a DFS ancestor, not the DFS parent
// EDGE_TYPE_FORWARD - edge record points to a DFS descendant, not a DFS child
#define EDGE_TYPE_BACK 4
#define EDGE_TYPE_FORWARD 20

// EDGE_TYPE_CROSS - edge record and its twin represent a cross edge
//    Bit3 is not set because the edge is not a tree edge
//    Bits 2 and 4 are not set because the edge's endpoings are not
//        in a parent-or-ancestor/child-or-descendant relationship
#define EDGE_TYPE_CROSS 256

#define gp_GetEdgeType(theGraph, e) (theGraph->E[e].flags & EDGE_TYPE_MASK)
#define gp_ClearEdgeType(theGraph, e) (theGraph->E[e].flags &= ~EDGE_TYPE_MASK)
#define gp_SetEdgeType(theGraph, e, type) (theGraph->E[e].flags |= type)
#define gp_ResetEdgeType(theGraph, e, type) \
    (theGraph->E[e].flags = (theGraph->E[e].flags & ~EDGE_TYPE_MASK) | type)

#define EDGEFLAG_INVERTED_MASK 32
#define gp_GetEdgeFlagInverted(theGraph, e) (theGraph->E[e].flags & EDGEFLAG_INVERTED_MASK)
#define gp_SetEdgeFlagInverted(theGraph, e) (theGraph->E[e].flags |= EDGEFLAG_INVERTED_MASK)
#define gp_ClearEdgeFlagInverted(theGraph, e) (theGraph->E[e].flags &= (~EDGEFLAG_INVERTED_MASK))
#define gp_XorEdgeFlagInverted(theGraph, e) (theGraph->E[e].flags ^= EDGEFLAG_INVERTED_MASK)

#define EDGEFLAG_DIRECTION_INONLY 64
#define EDGEFLAG_DIRECTION_OUTONLY 128
#define EDGEFLAG_DIRECTION_MASK 192

// NOTE: Edge 'flags' bit 8 used by EDGE_TYPE_CROSS above,
//       so next available bit is bit 9 = 512

// Returns the direction, if any, of the edge record
#define gp_GetDirection(theGraph, e) (theGraph->E[e].flags & EDGEFLAG_DIRECTION_MASK)

// A direction of 0 clears directedness. Otherwise, edge record e is set
// to direction and e's twin edge record is set to the opposing setting.
#define gp_SetDirection(theGraph, e, direction)                                                   \
    {                                                                                             \
        if (direction == EDGEFLAG_DIRECTION_INONLY)                                               \
        {                                                                                         \
            theGraph->E[e].flags |= EDGEFLAG_DIRECTION_INONLY;                                    \
            theGraph->E[gp_GetTwin(theGraph, e)].flags |= EDGEFLAG_DIRECTION_OUTONLY;             \
            if (gp_GetNeighbor(theGraph, e) != gp_GetNeighbor(theGraph, gp_GetTwin(theGraph, e))) \
            {                                                                                     \
                theGraph->graphFlags |= GRAPHFLAGS_DIRECTEDEDGEDETECTED;                          \
            }                                                                                     \
        }                                                                                         \
        else if (direction == EDGEFLAG_DIRECTION_OUTONLY)                                         \
        {                                                                                         \
            theGraph->E[e].flags |= EDGEFLAG_DIRECTION_OUTONLY;                                   \
            theGraph->E[gp_GetTwin(theGraph, e)].flags |= EDGEFLAG_DIRECTION_INONLY;              \
            if (gp_GetNeighbor(theGraph, e) != gp_GetNeighbor(theGraph, gp_GetTwin(theGraph, e))) \
            {                                                                                     \
                theGraph->graphFlags |= GRAPHFLAGS_DIRECTEDEDGEDETECTED;                          \
            }                                                                                     \
        }                                                                                         \
        else                                                                                      \
        {                                                                                         \
            theGraph->E[e].flags &= ~EDGEFLAG_DIRECTION_MASK;                                     \
            theGraph->E[gp_GetTwin(theGraph, e)].flags &= ~EDGEFLAG_DIRECTION_MASK;               \
        }                                                                                         \
    }

// Iterate through all edges with gp_LowerBoundEdges, gp_UpperBoundEdges, and gp_EdgeInUse
#ifdef USE_1BASEDARRAYS
#define gp_LowerBoundEdges(theGraph) (2)
#define gp_UpperBoundEdges(theGraph) (gp_LowerBoundEdges(theGraph) + ((gp_GetM(theGraph) + (theGraph)->numEdgeHoles) << 1))

#define gp_EdgeInUse(theGraph, e) (gp_GetNeighbor(theGraph, e))
#define gp_EdgeNotInUse(theGraph, e) (!gp_GetNeighbor(theGraph, e))

#else
#define gp_LowerBoundEdges(theGraph) (0)
#define gp_UpperBoundEdges(theGraph) (gp_LowerBoundEdges(theGraph) + ((gp_GetM(theGraph) + (theGraph)->numEdgeHoles) << 1))

#define gp_EdgeInUse(theGraph, e) (gp_GetNeighbor(theGraph, e) != NIL)
#define gp_EdgeNotInUse(theGraph, e) (gp_GetNeighbor(theGraph, e) == NIL)
#endif

// These lower and upper bounds for edge storage are used for tasks like initializing that
// must iterate through all of the edge storage, including space not yet containing edges.
#define gp_LowerBoundEdgeStorage(theGraph) (gp_LowerBoundEdges(theGraph))
#define gp_UpperBoundEdgeStorage(theGraph) (gp_LowerBoundEdgeStorage(theGraph) + ((theGraph)->edgeCapacity << 1))

// The initial setting for the edge storage capacity expressed as a constant factor of N,
// which is the number of vertices in the graph. By default, array E is allocated enough
// space to contain 3N edges, which is 6N edge records, but this initial setting can be
// overridden using gp_EnsureEdgeCapacity(). It is especially efficient to change to ensure
// a higher edge capacity if done before calling gp_EnsureVertexCapacity() or gp_Read().
#define DEFAULT_EDGE_CAPACITY_FACTOR 3

// This value is returned by gp_AddEdge() and gp_InsertEdge() if adding or inserting
// the edge would exceed the edge capacity limit. The limit can be increased by
// calling gp_EnsureEdgeCapacity(), gp_DynamicAddEdge(), or gp_DynamicInsertEdge().
#define AT_EDGE_CAPACITY_LIMIT -1

// A bounds-checking version of gp_IsEdge() for DEBUG mode compilation
#ifdef DEBUG
#undef gp_IsEdge
#define gp_IsEdge(theGraph, e)                                                                    \
    ((e) == NIL                                                                                   \
         ? 0                                                                                      \
         : ((e) < gp_LowerBoundEdgeStorage(theGraph) || (e) >= gp_UpperBoundEdgeStorage(theGraph) \
                ? (NOTOK, 0)                                                                      \
                : 1))
#endif

    // Declaration of package-private data type for managing a
    // stack of integers
    typedef struct stackStruct stackStruct;
    typedef stackStruct *stackP;

    // Declaration of package private data types for extending the base Graph class
    // with subclasses having data members and function overloads
    typedef struct graphExtensionStruct graphExtensionStruct;
    typedef graphExtensionStruct *graphExtensionP;

    typedef struct graphFunctionTableStruct graphFunctionTableStruct;
    typedef graphFunctionTableStruct *graphFunctionTableP;

    /********************************************************************
         Graph structure definition
                V : Array of vertex records (allocated size N + NV)
                N : Number of non-virtual vertices (the "order" of the graph)
                NV: Number of virtual vertices (currently always equal to N)

                E : Array of edge records (edge records come in pairs and represent
                    an edge in each of the two vertex endpoints of the edge)
                M: Number of edges (the "size" of the graph)
                edgeCapacity: the maximum number of edges allowed in E
                edgeHoles: free locations in E where edges have been deleted
                numEdgeHoles: package private member indicating the number of edge holes.

                graphFlags: Additional state information about the graph
                embedFlags: records the type of embedding requested (uses EMBEDFLAGS)

                theStack: Used by methods of the base Graph class and its subclasses

                extensions: an object-oriented hierarchy of graph classes is implemented
                            manually as a list of extensions for data of any subclasses
                            with which a graph has been extended.
                extensionLookupTable: if not NULL, keeps an array of pointers to the
                                      extensions indexed by ID for constant-time lookup.
                functions: object-oriented class hierarchies includes virtual function
                           overloading, which is provided by this function pointer table.

                privateData: pointer to a package private data structure that can be
                             backwards compatibly changed in releases of graphLib.
        */

    struct graphStruct
    {
        vertexRecP V;
        int N, NV;

        edgeRecP E;
        int M, edgeCapacity;
        stackP edgeHoles;
        int numEdgeHoles;

        unsigned graphFlags, embedFlags;

        // Used by base Graph class and its subclasses
        stackP theStack;

        // Provides ability to subclass the base Graph,
        // including virtual function overloads by subclasses
        graphExtensionP extensions;
        graphExtensionP *extensionLookupTable;
        graphFunctionTableP functions;

        void *privateData;
    };

    typedef struct graphStruct graphStruct;
    typedef graphStruct *graphP;

#ifdef __cplusplus
}
#endif

#endif

// ===== extensionSystem/graphExtensions.h =====
#ifndef GRAPH_EXTENSIONS_H
#define GRAPH_EXTENSIONS_H

/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#ifdef __cplusplus
extern "C"
{
#endif
    void gp_FreeExtensions(graphP theGraph);

#ifdef __cplusplus
}
#endif

#endif

// ===== extensionSystem/graphExtensions.private.h =====
#ifndef GRAPH_EXTENSIONS_PRIVATE_H
#define GRAPH_EXTENSIONS_PRIVATE_H

/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/


#ifdef __cplusplus
extern "C"
{
#endif

    struct graphExtensionStruct
    {
        int moduleID;
        void *context;
        void *(*dupContext)(void *pContext, void *theGraph);
        int  (*copyData)(void *dstContext, void *srcContext);
        void (*freeContext)(void *pContext);

        graphFunctionTableP functions;

        struct graphExtensionStruct *next;
    };

    typedef struct graphExtensionStruct graphExtensionStruct;
    typedef graphExtensionStruct *graphExtensionP;

#ifdef __cplusplus
}
#endif

#endif

// ===== graph.private.h =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#ifndef GRAPH_PRIVATE_H
#define GRAPH_PRIVATE_H


#ifdef __cplusplus
extern "C"
{
#endif

    // DFS-RELATED and PLANARITY-RELATED ONLY

    // Declaration of package-private data type for managing a
    // collection of lists of integers
    typedef struct listCollectionStruct listCollectionStruct;
    typedef listCollectionStruct *listCollectionP;

    // Declaration of package-private data type for managing additional 
    // DFS--related information associated with each non-virtual vertex
    typedef struct DFSUtils_VertexInfo DFSUtils_VertexInfo;
    typedef DFSUtils_VertexInfo *DFSUtils_VertexInfoP;

    // PLANARITY-RELATED ONLY

    // Declaration of package-private data type for managing additional 
    // planarity-related information associated with each non-virtual vertex
    typedef struct Planarity_VertexInfo Planarity_VertexInfo;
    typedef Planarity_VertexInfo *Planarity_VertexInfoP;

    // Declaration of package private data type for optimizing management of
    // the external face of a planar embedding as it is being built
    typedef struct extFaceLinkRec extFaceLinkRec;
    typedef extFaceLinkRec *extFaceLinkRecP;

    // Declaration of package private data type for isolating
    // minimal subgraphs obstructing planarity-related embedding
    typedef struct isolatorContextStruct isolatorContextStruct;
    typedef isolatorContextStruct *isolatorContextP;


    /********************************************************************
     A structure for package private data associated with a graph.

        BicompRootLists: storage for bicomp root lists (DFSUtils) or, for
                            Planarity, pertinent child bicomp lists that develop
                        during embedding
        DVI: package private pointer; if the graph is extended to DFSUtils,
                then N instances of DFSUtils vertex info records are allocated

        PVI: package private pointer; if the graph is extended to Planarity,
                then N instances of Planarity vertex info records are allocated
        IC: contains additional useful variables for Kuratowski subgraph isolation.
        sortedDFSChildLists: for Planarity graphs, storage for the sorted DFS child
                lists of each vertex
        extFace: For Planarity graphs, an array of (N + NV) external face
                short circuit records
     ********************************************************************/
    struct graphPrivateDataStruct
    {
        // Private Data members specific to a DFSUtilsGraph subclass
        listCollectionP BicompRootLists;
        DFSUtils_VertexInfoP DVI;

        // Private data members specific to a PlanarityGraph subclass
        Planarity_VertexInfoP PVI;
        listCollectionP sortedDFSChildLists;
        extFaceLinkRecP extFace;
        isolatorContextP IC;
        graphEdgeDetectorP edgeDetector;

        // Counts the modifications of the graph structure made through the
        // functions of the graph library, so that a package such as the
        // sparse6 writer can tell whether the graph is still the one it
        // saw last. Only functions increment it; the low-level setter
        // macros do not, so as not to slow the algorithms down.
        unsigned long long modificationCount;
    };

    typedef struct graphPrivateDataStruct graphPrivateDataStruct;
    typedef graphPrivateDataStruct *graphPrivateDataP;

// These macros describe how to currently get access to additional data structures
// used in DFSUtils Graphs and Planarity Graphs. Once full extensions are
// developed for those, then these definitions can change to access the data 
// from the extensions, and the data can be removed from graphPrivateDataStruct. 
#define theGraphBicompRootLists(theGraph) (((graphPrivateDataP)((theGraph)->privateData))->BicompRootLists)
#define theGraphDVI(theGraph) (((graphPrivateDataP)((theGraph)->privateData))->DVI)

#define theGraphPVI(theGraph) (((graphPrivateDataP)((theGraph)->privateData))->PVI)
#define theGraphSortedDFSChildLists(theGraph) (((graphPrivateDataP)((theGraph)->privateData))->sortedDFSChildLists)
#define theGraphExtFace(theGraph) (((graphPrivateDataP)((theGraph)->privateData))->extFace)
#define theGraphIC(theGraph) (((graphPrivateDataP)((theGraph)->privateData))->IC)
#define theGraphEdgeDetector(theGraph) (((graphPrivateDataP)((theGraph)->privateData))->edgeDetector)

// Package private access to the modification counter: the getter reads it,
// and gp_NoteModification() is what a modifying function calls
#define theGraphModificationCount(theGraph) (((graphPrivateDataP)((theGraph)->privateData))->modificationCount)
#define gp_NoteModification(theGraph) (theGraphModificationCount(theGraph)++)

/********************************************************************
 Additional edge link accessors and manipulators
 ********************************************************************/

// Methods that enable getting the next or previous edge as if
// the adjacency list of the containing vertex were circular,
// i.e. that the first and last edge records were linked
#define gp_GetNextEdgeCircular(theGraph, e)           \
    (gp_IsEdge(theGraph, gp_GetNextEdge(theGraph, e)) \
         ? gp_GetNextEdge(theGraph, e)                \
         : gp_GetFirstEdge(theGraph, theGraph->E[gp_GetTwin(theGraph, e)].neighbor))

#define gp_GetPrevEdgeCircular(theGraph, e)           \
    (gp_IsEdge(theGraph, gp_GetPrevEdge(theGraph, e)) \
         ? gp_GetPrevEdge(theGraph, e)                \
         : gp_GetLastEdge(theGraph, theGraph->E[gp_GetTwin(theGraph, e)].neighbor))

// Methods that make the cross-link binding between a vertex and an
// edge record. The old first or last edge record should be bound to
// this new first or last edge record, e, by separate calls,
// e.g. see gp_AttachFirstEdge() and gp_AttachLastEdge()
#define gp_BindFirstEdge(theGraph, v, e)  \
    {                                     \
        gp_SetPrevEdge(theGraph, e, NIL); \
        gp_SetFirstEdge(theGraph, v, e);  \
    }

#define gp_BindLastEdge(theGraph, v, e)   \
    {                                     \
        gp_SetNextEdge(theGraph, e, NIL); \
        gp_SetLastEdge(theGraph, v, e);   \
    }

// Attaches edge e between the current binding between v and its first edge
#define gp_AttachFirstEdge(theGraph, v, e)                             \
    {                                                                  \
        if (gp_IsEdge(theGraph, gp_GetFirstEdge(theGraph, v)))         \
        {                                                              \
            gp_SetNextEdge(theGraph, e, gp_GetFirstEdge(theGraph, v)); \
            gp_SetPrevEdge(theGraph, gp_GetFirstEdge(theGraph, v), e); \
        }                                                              \
        else                                                           \
            gp_BindLastEdge(theGraph, v, e);                           \
        gp_BindFirstEdge(theGraph, v, e);                              \
    }

// Attaches edge e between the current binding between v and its last edge
#define gp_AttachLastEdge(theGraph, v, e)                             \
    {                                                                 \
        if (gp_IsEdge(theGraph, gp_GetLastEdge(theGraph, v)))         \
        {                                                             \
            gp_SetPrevEdge(theGraph, e, gp_GetLastEdge(theGraph, v)); \
            gp_SetNextEdge(theGraph, gp_GetLastEdge(theGraph, v), e); \
        }                                                             \
        else                                                          \
            gp_BindFirstEdge(theGraph, v, e);                         \
        gp_BindLastEdge(theGraph, v, e);                              \
    }

// Moves an edge e that is in the adjacency list of v to the start of the adjacency list
#define gp_MoveEdgeToFirst(theGraph, v, e)                                                      \
    if (e != gp_GetFirstEdge(theGraph, v))                                                      \
    {                                                                                           \
        /* If e is last in the adjacency list of v, then we                                     \
           detach it by adjacency list end management */                                        \
        if (e == gp_GetLastEdge(theGraph, v))                                                   \
        {                                                                                       \
            gp_SetNextEdge(theGraph, gp_GetPrevEdge(theGraph, e), NIL);                         \
            gp_SetLastEdge(theGraph, v, gp_GetPrevEdge(theGraph, e));                           \
        }                                                                                       \
        /* Otherwise, we detach e from the middle of the list */                                \
        else                                                                                    \
        {                                                                                       \
            gp_SetNextEdge(theGraph, gp_GetPrevEdge(theGraph, e), gp_GetNextEdge(theGraph, e)); \
            gp_SetPrevEdge(theGraph, gp_GetNextEdge(theGraph, e), gp_GetPrevEdge(theGraph, e)); \
        }                                                                                       \
                                                                                                \
        /* Now add e as the new first edge of v.                                                \
           Note that the adjacency list is non-empty at this time */                            \
        gp_SetNextEdge(theGraph, e, gp_GetFirstEdge(theGraph, v));                              \
        gp_SetPrevEdge(theGraph, gp_GetFirstEdge(theGraph, v), e);                              \
        gp_BindFirstEdge(theGraph, v, e);                                                       \
    }

// Moves an edge e that is in the adjacency list of v to the end of the adjacency list
#define gp_MoveEdgeToLast(theGraph, v, e)                                                       \
    if (e != gp_GetLastEdge(theGraph, v))                                                       \
    {                                                                                           \
        /* If e is first in the adjacency list of vertex v, then we                             \
           detach it by adjacency list beginning management */                                  \
        if (e == gp_GetFirstEdge(theGraph, v))                                                  \
        {                                                                                       \
            gp_SetPrevEdge(theGraph, gp_GetNextEdge(theGraph, e), NIL);                         \
            gp_SetFirstEdge(theGraph, v, gp_GetNextEdge(theGraph, e));                          \
        }                                                                                       \
        /* Otherwise, we detach e from the middle of the list */                                \
        else                                                                                    \
        {                                                                                       \
            gp_SetNextEdge(theGraph, gp_GetPrevEdge(theGraph, e), gp_GetNextEdge(theGraph, e)); \
            gp_SetPrevEdge(theGraph, gp_GetNextEdge(theGraph, e), gp_GetPrevEdge(theGraph, e)); \
        }                                                                                       \
                                                                                                \
        /* Now we add e as the new last edge of v.                                              \
           Note that the adjacency list is non-empty at this time */                            \
        gp_SetPrevEdge(theGraph, e, gp_GetLastEdge(theGraph, v));                               \
        gp_SetNextEdge(theGraph, gp_GetLastEdge(theGraph, v), e);                               \
        gp_BindLastEdge(theGraph, v, e);                                                        \
    }

#ifdef __cplusplus
}
#endif

// This number can just be made bigger if ever needed
#define MAXNUMSUPPORTEDEXTENSIONS 32

#endif /* GRAPH_PRIVATE_H */

// ===== graphDFSUtils.h =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#ifndef GRAPHDFSUTILS_H
#define GRAPHDFSUTILS_H


#ifdef __cplusplus
extern "C"
{
#endif

// Create a DFSUtils Graph, i.e., subclass a Graph by extending it with the
// ability to perform the depth-first search (DFS) utility methods below.
#define DFSUTILS_NAME "DFSUtils"

        int gp_ExtendWith_DFSUtils(graphP theGraph);
        int gp_Detach_DFSUtils(graphP theGraph);

/* Graph Flags: see gp_GetGraphFlags()
        GRAPHFLAGS_EXTENDEDWITH_DFSUTILS is set by calling gp_ExtendWith_DFSUtils()
                This is automatically called by the utility methods below that create
                a DFS tree, sort vertices, and compute least ancestors and lowpoints
        GRAPHFLAGS_DFSNUMBERED is set if DFS numbering has been performed on the graph,
                such as by calling the gp_DepthFirstSearch() utility method below
        GRAPHFLAGS_SORTEDBYDFI records whether the graph is in original vertex order
                or sorted by depth first index. Successive calls to the
                gp_SortVertices() utility method below toggle this bit.
        GRAPHFLAGS_LOWPOINTSCOMPUTED records whether lowpoint calculations have
                been performed on the graph.
        GRAPHFLAGS_DFSNUMBERED_DIRECTED is set when a directed depth-first search
                performs the DFS numbering.
*/
#define GRAPHFLAGS_EXTENDEDWITH_DFSUTILS 256
#define GRAPHFLAGS_DFSNUMBERED 512
#define GRAPHFLAGS_SORTEDBYDFI 1024
#define GRAPHFLAGS_LOWPOINTSCOMPUTED 2048
#define GRAPHFLAGS_DFSNUMBERED_DIRECTED 4096

        // DFS-related utility methods that create a DFS tree, sort vertices and
        // compute least ancestor and lowpoint values
        int gp_DepthFirstSearch(graphP theGraph);

#define DFSMODE_UNDIRECTED 1
#define DFSMODE_DIRECTED 2

        int gp_SortVertices(graphP theGraph);
        int gp_ComputeLowpoints(graphP theGraph);

        // Additional DFS-related uitility methods (functions and macros) that assume
        // one or more of the above methods have been called to create a DFS tree,
        // sort vertices and/or compute least ancestor and lowpoint values

// A DFS tree root is one that has no DFS parent. There is one DFS tree root
// per connected component of a graph (connected, not biconnected; component, not bicomp)
#define gp_IsDFSTreeRoot(theGraph, v) gp_IsNotVertex(theGraph, gp_GetParent(theGraph, v))
#define gp_IsNotDFSTreeRoot(theGraph, v) gp_IsVertex(theGraph, gp_GetParent(theGraph, v))

// Mapping between bicomp roots and virtual vertex locations used to store them.
// A cut vertex v separates one or more of its DFS children, say c1 and c2, from
// the DFS parent and ancesstors of v. Because a DFS tree contains only tree edges
// and back edges, there are no cross edges connecting vertices in the DFS subtree
// rooted by c1, T(c1), with vertices in the DFS subtree rooted by c2, T(c2).
// We say that v is a cut vertex because the only paths that go from vertices in
// T(c1) to vertices in T(c2) are paths that contain v.
//
// Therefore, bicomp root copies of v, say R1 and R2, can be created at locations
// c1 and c2 in virtual vertex space, in other words at locations N+c1 and N+c2.
// The bicomps rooted by R1 and R2 are called child bicomps of v, and they contain,
// respectively, c1 and c2 as well as possibly more vertices from, respectively,
// T(c1) and T(c2), depending on what back edges may exist in the graph between
// pairs of vertices in, respectively, T(c1) and T(c2).
#define gp_GetBicompRootFromDFSChild(theGraph, c) ((c) + gp_GetN(theGraph))
#define gp_GetDFSChildFromBicompRoot(theGraph, R) ((R) - gp_GetN(theGraph))
#define gp_GetVertexFromBicompRoot(theGraph, R) gp_GetParent(theGraph, gp_GetDFSChildFromBicompRoot(theGraph, R))
#define gp_IsBicompRoot(theGraph, v) ((v) >= gp_LowerBoundVirtualVertices(theGraph))

// If a vertex v is a cut vertex that separates one of its DFS children, say c,
// from the DFS ancestors and other children of v, then when the graph has been
// separated into bicomps, there will be a root copy of v in virtual vertex space
// at location c+N that will have at least one edge connecting it to c.
// These macros detect whether or not that is the case for a given DFS child.
#define gp_IsSeparatedDFSChild(theGraph, theChild) (gp_VirtualVertexInUse(theGraph, gp_GetBicompRootFromDFSChild(theGraph, theChild)))
#define gp_IsNotSeparatedDFSChild(theGraph, theChild) (gp_VirtualVertexNotInUse(theGraph, gp_GetBicompRootFromDFSChild(theGraph, theChild)))

#ifdef __cplusplus
}
#endif

#endif /* GRAPHDFSUTILS_H */

// ===== graphDFSUtils.private.h =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#ifndef GRAPHDFSUTILS_PRIVATE_H
#define GRAPHDFSUTILS_PRIVATE_H


#ifdef __cplusplus
extern "C"
{
#endif

    /********************************************************************
     Vertex Info Structure Definition.

     This structure equips the non-virtual vertices with additional
     information needed for DFS-related and planarity-related algorithms.

        parent: The DFI of the DFS tree parent of this vertex
        leastAncestor: min(DFI of neighbors connected by backedge)
        lowpoint: min(leastAncestor, min(lowpoint of DFS Children))
        visitedIndex: enables algorithms to manage vertex visitation with more than
                    just a flag. In a directed depth-first search, the vertex index
                    indicates the discovery time, so visitedIndex is needed for the
                    finish time. The planarity test uses this member to flag
                    visitation as a step number so that it implicitly resets on each
                    vertex step of embedding. The planar graph drawing method
                    signifies a first visitation by storing the index of the first
                    _edge_ used to reach a vertex.
    */

    struct DFSUtils_VertexInfo
    {
        int parent, leastAncestor, lowpoint, visitedIndex;
    };

    typedef struct DFSUtils_VertexInfo DFSUtils_VertexInfo;
    typedef DFSUtils_VertexInfo *DFSUtils_VertexInfoP;

#define gp_GetVertexParent(theGraph, v) (theGraphDVI(theGraph)[v].parent)
#define gp_SetVertexParent(theGraph, v, theParent) (theGraphDVI(theGraph)[v].parent = theParent)

#define _gp_IsDFSTreeRoot(theGraph, v) gp_IsNotVertex(theGraph, gp_GetVertexParent(theGraph, v))
#define _gp_IsNotDFSTreeRoot(theGraph, v) gp_IsVertex(theGraph, gp_GetVertexParent(theGraph, v))
#define _gp_GetVertexFromBicompRoot(theGraph, R) gp_GetVertexParent(theGraph, gp_GetDFSChildFromBicompRoot(theGraph, R))

#define gp_GetVertexLeastAncestor(theGraph, v) (theGraphDVI(theGraph)[v].leastAncestor)
#define gp_SetVertexLeastAncestor(theGraph, v, theLeastAncestor) (theGraphDVI(theGraph)[v].leastAncestor = theLeastAncestor)

#define gp_GetVertexLowpoint(theGraph, v) (theGraphDVI(theGraph)[v].lowpoint)
#define gp_SetVertexLowpoint(theGraph, v, theLowpoint) (theGraphDVI(theGraph)[v].lowpoint = theLowpoint)

#define gp_GetVertexVisitedIndex(theGraph, v) (theGraphDVI(theGraph)[v].visitedIndex)
#define gp_SetVertexVisitedIndex(theGraph, v, newVisitedIndex) (theGraphDVI(theGraph)[v].visitedIndex = newVisitedIndex)

#ifdef __cplusplus
}
#endif

#endif /* GRAPHPDFSUTILS_PRIVATE_H */

// ===== planarityRelated/graphPlanarity.h =====
#ifndef GRAPHPLANARITY_H
#define GRAPHPLANARITY_H

/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/


#ifdef __cplusplus
extern "C"
{
#endif

// Create a Planarity Graph, i.e., subclass a DFSUtils Graph by extending it with
// the ability to perform planar graph embedding and obstruction isolation.
#define PLANARITY_NAME "Planarity"

    int gp_ExtendWith_Planarity(graphP theGraph);
    int gp_Detach_Planarity(graphP theGraph);

/* Graph Flags: see gp_GetGraphFlags()
        GRAPHFLAGS_EXTENDEDWITH_PLANARITY is set by calling gp_ExtendWith_Planarity()
                This is automatically by gp_Embed() if not already done.
*/
#define GRAPHFLAGS_EXTENDEDWITH_PLANARITY 65536

    // Graph embedding and result validation methods
    // The embedResult output by gp_Embed() and input to gp_TestEmbedResultIntegrity()
    // can be OK if the graph is embedded or embeddable, NONEMBEDDABLE if a minimal
    // subgraph obstructing embedding has been isolated, or NOTOK on error
    int gp_Embed(graphP theGraph, unsigned embedFlags);
    int gp_TestEmbedResultIntegrity(graphP theGraph, graphP origGraph, int embedResult);

    // Graph embedding face enumeration and listing methods
    int gp_CountEmbeddingFaces(graphP theGraph);
    int gp_CreateEmbeddingFaceList(graphP theGraph, char **pFaceList);

// A return result value for gp_Embed() to indicate success prior to embedding completion,
// due to finding an obstruction to embedding.
#define NONEMBEDDABLE -1

// Below are the possible graph embedFlags to pass to gp_Embed() and which are
// then set into the graph by gp_Embed() and returned by this method.
#define gp_GetEmbedFlags(theGraph) ((theGraph)->embedFlags)

#define EMBEDFLAGS_PLANAR 1
#define EMBEDFLAGS_OUTERPLANAR 2

#define EMBEDFLAGS_DRAWPLANAR (4 | EMBEDFLAGS_PLANAR)

#define EMBEDFLAGS_SEARCHFORK23 (8 | EMBEDFLAGS_OUTERPLANAR)
#define EMBEDFLAGS_SEARCHFORK33 (16 | EMBEDFLAGS_PLANAR)
#define EMBEDFLAGS_SEARCHFORK4 (32 | EMBEDFLAGS_OUTERPLANAR)

// Reserve flag bits for possible future embedding-related extension modules
#define EMBEDFLAGS_SEARCHFORK5 (64 | EMBEDFLAGS_PLANAR)
#define EMBEDFLAGS_SEARCHFORK5MINOR (128 | EMBEDFLAGS_PLANAR)
#define EMBEDFLAGS_MAXIMALPLANARSUBGRAPH (256 | EMBEDFLAGS_PLANAR)
#define EMBEDFLAGS_PROJECTIVEPLANAR 512
#define EMBEDFLAGS_TOROIDAL 1024

    // After gp_Embed(), if the result is NONEMBEDDABLE, then this method
    // returns the obstructing minor type from the list below.
    // It is best to compare using a bitwise-and operation.
    unsigned gp_GetObstructionMinorType(graphP theGraph);

#define MINORTYPE_NONE 0
#define MINORTYPE_A 1
#define MINORTYPE_B 2
#define MINORTYPE_C 4
#define MINORTYPE_D 8
#define MINORTYPE_E 16
#define MINORTYPE_E1 32
#define MINORTYPE_E2 64
#define MINORTYPE_E3 128
#define MINORTYPE_E4 256

#define MINORTYPE_E5 512
#define MINORTYPE_E6 1024
#define MINORTYPE_E7 2048

#ifdef __cplusplus
}
#endif

#endif

// ===== planarityRelated/graphPlanarity.private.h =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#ifndef GRAPHPLANARITY_PRIVATE_H
#define GRAPHPLANARITY_PRIVATE_H


#ifdef __cplusplus
extern "C"
{
#endif

// PLANARITY-RELATED ONLY VERTEX FLAGS
//
// The ANYVERTEX_OBSTRUCTIONMARK_MASK bits are bits 2-4, 4+8+16=28
// They are used by planarity-related algorithms to identify the four
// regions of the external face cycle of a bicomp, relative to an
// XY-path in the bicomp.
// Bit 2 - 4 if the OBSTRUCTIONMARK is set, 0 if not
// Bit 3 - 8 if the OBSTRUCTIONMARK indicates Y side, 0 if X side
// Bit 4 - 16 if the OBSTRUCTIONMARK indicates high, 0 if low
#define ANYVERTEX_OBSTRUCTIONMARK_MASK 28

// Call gp_GetObstructionMark, then compare to one of these four possibilities
// ANYVERTEX_OBSTRUCTIONMARK_HIGH_RXW - On the external face path between vertices R and X
// ANYVERTEX_OBSTRUCTIONMARK_LOW_RXW  - X or on the external face path between vertices X and W
// ANYVERTEX_OBSTRUCTIONMARK_HIGH_RYW - On the external face path between vertices R and Y
// ANYVERTEX_OBSTRUCTIONMARK_LOW_RYW  - Y or on the external face path between vertices Y and W
// ANYVERTEX_OBSTRUCTIONMARK_UNMARKED  - corresponds to all three bits off
#define ANYVERTEX_OBSTRUCTIONMARK_HIGH_RXW 20
#define ANYVERTEX_OBSTRUCTIONMARK_LOW_RXW 4
#define ANYVERTEX_OBSTRUCTIONMARK_HIGH_RYW 28
#define ANYVERTEX_OBSTRUCTIONMARK_LOW_RYW 12
#define ANYVERTEX_OBSTRUCTIONMARK_UNMARKED 0

#define gp_GetObstructionMark(theGraph, v) (theGraph->V[v].flags & ANYVERTEX_OBSTRUCTIONMARK_MASK)
#define gp_ClearObstructionMark(theGraph, v) (theGraph->V[v].flags &= ~ANYVERTEX_OBSTRUCTIONMARK_MASK)
#define gp_SetObstructionMark(theGraph, v, type) (theGraph->V[v].flags |= type)
#define gp_ResetObstructionMark(theGraph, v, type) \
    (theGraph->V[v].flags = (theGraph->V[v].flags & ~ANYVERTEX_OBSTRUCTIONMARK_MASK) | type)

    /********************************************************************
    // PLANARITY-RELATED ONLY
    //
     This structure defines a pair of links used by each vertex and virtual vertex
        to create "short circuit" paths that eliminate unimportant vertices from
        the external face, enabling more efficient traversal of the external face.

        It is also possible to embed the "short circuit" edges, but this approach
        creates a better separation of concerns, imparts greater clarity, and
        removes exceptionalities for handling additional fake "short circuit" edges.

        vertex[2]: The two adjacent vertices along the external face, possibly
                short-circuiting paths of inactive vertices.
    */

    struct extFaceLinkRec
    {
        int vertex[2];
    };

    typedef struct extFaceLinkRec extFaceLinkRec;
    typedef extFaceLinkRec *extFaceLinkRecP;

#define gp_GetExtFaceVertex(theGraph, v, link) (theGraphExtFace(theGraph)[v].vertex[link])
#define gp_SetExtFaceVertex(theGraph, v, link, theVertex) (theGraphExtFace(theGraph)[v].vertex[link] = theVertex)

    /********************************************************************
    // PLANARITY-RELATED ONLY
    //
    Planarity-specific additional vertex information.

        pertinentEdge: Used by the planarity method; during Walkup, each vertex
                    that is directly adjacent via a back edge to the vertex v
                    currently being embedded will have the forward edge's index
                    stored in this field.  During Walkdown, each vertex for which
                    this field is set will cause a back edge to be embedded.
                    Implicitly resets at each vertex step of the planarity method
        pertinentRootsList: used by Walkup to store a list of child bicomp roots of
                    a vertex descendant of the current vertex that are pertinent
                    and must be merged by the Walkdown in order to embed the cycle
                    edges of the current vertex.  Future pertinent child bicomp roots
                    are placed at the end of the list to ensure bicomps that are
                    only pertinent are processed first.
        futurePertinentChild: indicates a DFS child with a lowpoint less than the
                    current vertex v.  This member is initialized to the start of
                    the sortedDFSChildList and is advanced in a relaxed manner as
                    needed until one with a lowpoint less than v is found or until
                    there are no more children.
        sortedDFSChildList: at the start of embedding, the list of DFS children of
                    this vertex is calculated in ascending order by DFI (sorted in
                    linear time). The list is used during Walkdown processing of
                    a vertex to process all of its children.  It is also used in
                    future pertinence management when processing the ancestors of
                    the vertex. When a child C is merged into the same bicomp as
                    the vertex, it is removed from the list.
        fwdEdgeList: at the start of embedding, the "back" edges from a vertex to
                    its DFS *descendants* (i.e. the forward edge records) are
                    separated from the main adjacency list and placed in a
                    circular list until they are embedded. The list is sorted in
                    ascending DFI order of the descendants (in linear time).
                    This member indicates (by index) a node in that list.
    */

    struct Planarity_VertexInfo
    {
        int pertinentEdge,
            pertinentRoots,
            futurePertinentChild,
            sortedDFSChildList,
            fwdEdgeList;
    };

    typedef struct Planarity_VertexInfo Planarity_VertexInfo;
    typedef Planarity_VertexInfo *Planarity_VertexInfoP;

#define gp_GetVertexPertinentEdge(theGraph, v) (theGraphPVI(theGraph)[v].pertinentEdge)
#define gp_SetVertexPertinentEdge(theGraph, v, e) (theGraphPVI(theGraph)[v].pertinentEdge = e)

#define gp_GetVertexPertinentRootsList(theGraph, v) (theGraphPVI(theGraph)[v].pertinentRoots)
#define gp_SetVertexPertinentRootsList(theGraph, v, pertinentRootsHead) (theGraphPVI(theGraph)[v].pertinentRoots = pertinentRootsHead)

#define gp_GetVertexFirstPertinentRoot(theGraph, v) gp_GetBicompRootFromDFSChild(theGraph, theGraphPVI(theGraph)[v].pertinentRoots)
#define gp_GetVertexFirstPertinentRootChild(theGraph, v) (theGraphPVI(theGraph)[v].pertinentRoots)
#define gp_GetVertexLastPertinentRoot(theGraph, v) gp_GetBicompRootFromDFSChild(theGraph, LCGetPrev(theGraphBicompRootLists(theGraph), theGraphPVI(theGraph)[v].pertinentRoots, NIL))
#define gp_GetVertexLastPertinentRootChild(theGraph, v) LCGetPrev(theGraphBicompRootLists(theGraph), theGraphPVI(theGraph)[v].pertinentRoots, NIL)

#define gp_DeleteVertexPertinentRoot(theGraph, v, R)                                     \
    gp_SetVertexPertinentRootsList(theGraph, v,                                          \
                                   LCDelete(theGraphBicompRootLists(theGraph),           \
                                            gp_GetVertexPertinentRootsList(theGraph, v), \
                                            gp_GetDFSChildFromBicompRoot(theGraph, R)))

#define gp_PrependVertexPertinentRoot(theGraph, v, R)                                     \
    gp_SetVertexPertinentRootsList(theGraph, v,                                           \
                                   LCPrepend(theGraphBicompRootLists(theGraph),           \
                                             gp_GetVertexPertinentRootsList(theGraph, v), \
                                             gp_GetDFSChildFromBicompRoot(theGraph, R)))

#define gp_AppendVertexPertinentRoot(theGraph, v, R)                                     \
    gp_SetVertexPertinentRootsList(theGraph, v,                                          \
                                   LCAppend(theGraphBicompRootLists(theGraph),           \
                                            gp_GetVertexPertinentRootsList(theGraph, v), \
                                            gp_GetDFSChildFromBicompRoot(theGraph, R)))

#define gp_GetVertexFuturePertinentChild(theGraph, v) (theGraphPVI(theGraph)[v].futurePertinentChild)
#define gp_SetVertexFuturePertinentChild(theGraph, v, theFuturePertinentChild) (theGraphPVI(theGraph)[v].futurePertinentChild = theFuturePertinentChild)

// Used to advance futurePertinentChild of w to the next separated DFS child with a lowpoint less than v
// Once futurePertinentChild advances past a child, no future planarity operation could make that child
// relevant to future pertinence.
#define gp_UpdateVertexFuturePertinentChild(theGraph, w, v)                                             \
    while (gp_IsVertex(theGraph, theGraphPVI(theGraph)[w].futurePertinentChild))                        \
    {                                                                                                   \
        /* Skip children that 1) aren't future pertinent, 2) have been merged into the bicomp with w */ \
        if (gp_GetVertexLowpoint(theGraph, theGraphPVI(theGraph)[w].futurePertinentChild) >= v ||       \
            gp_IsNotSeparatedDFSChild(theGraph, theGraphPVI(theGraph)[w].futurePertinentChild))         \
        {                                                                                               \
            theGraphPVI(theGraph)[w].futurePertinentChild =                                             \
                gp_GetVertexNextDFSChild(theGraph, w, gp_GetVertexFuturePertinentChild(theGraph, w));   \
        }                                                                                               \
        else                                                                                            \
            break;                                                                                      \
    }

#define gp_GetVertexSortedDFSChildList(theGraph, v) (theGraphPVI(theGraph)[v].sortedDFSChildList)
#define gp_SetVertexSortedDFSChildList(theGraph, v, theSortedDFSChildList) (theGraphPVI(theGraph)[v].sortedDFSChildList = theSortedDFSChildList)

#define gp_GetVertexNextDFSChild(theGraph, v, c) LCGetNext(theGraphSortedDFSChildLists(theGraph), gp_GetVertexSortedDFSChildList(theGraph, v), c)

#define gp_AppendDFSChild(theGraph, v, c) \
    LCAppend(theGraphSortedDFSChildLists(theGraph), gp_GetVertexSortedDFSChildList(theGraph, v), c)

#define gp_GetVertexFwdEdgeList(theGraph, v) (theGraphPVI(theGraph)[v].fwdEdgeList)
#define gp_SetVertexFwdEdgeList(theGraph, v, theFwdEdgeList) (theGraphPVI(theGraph)[v].fwdEdgeList = theFwdEdgeList)

    /********************************************************************
    // PLANARITY-RELATED ONLY
    //
     Variables needed in embedding by Kuratowski subgraph isolator:
            minorType: the type of planarity obstruction found.
            v: the current vertex being processed
            r: the root of the bicomp on which the Walkdown failed
            x,y: stopping vertices on bicomp rooted by r
            w: pertinent vertex on ext. face path below x and y
            px, py: attachment points of x-y path,
            z: Unused except in minors D and E (not needed in A, B, C).

            ux,dx: endpoints of unembedded edge that helps connext x with
                    ancestor of v
            uy,dy: endpoints of unembedded edge that helps connext y with
                    ancestor of v
            dw: descendant endpoint in unembedded edge to v
            uz,dz: endpoints of unembedded edge that helps connext z with
                    ancestor of v (for minors B and E, not A, C, D).
    */

    struct isolatorContextStruct
    {
        unsigned minorType;
        int v, r, x, y, w, px, py, z;
        int ux, dx, uy, dy, dw, uz, dz;
    };

    typedef struct isolatorContextStruct isolatorContextStruct;
    typedef isolatorContextStruct *isolatorContextP;

//********************************************************************
// A few simple integer selection macros for obstruction isolation
//********************************************************************
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define MIN3(x, y, z) MIN(MIN((x), (y)), MIN((y), (z)))
#define MAX3(x, y, z) MAX(MAX((x), (y)), MAX((y), (z)))

/********************************************************************
 PERTINENT()
    A vertex is pertinent in a partially processed graph if there is an
    unprocessed back edge between the vertex v whose edges are currently
    being processed and either the vertex or a DFS descendant D of the
    vertex not in the same bicomp as the vertex.

    The vertex is either directly adjacent to v by an unembedded back edge
    or there is an unembedded back edge (v, D) and the vertex is a cut
    vertex in the partially processed graph along the DFS tree path from
    D to v.

    Pertinence is a dynamic property that can change for a vertex after
    each edge addition.  In other words, a vertex can become non-pertinent
    during step v as more back edges to v are embedded.

    NOTE: Pertinent roots are stored using the DFS children with which
    they are associated, so we test 'is vertex' (rather than virtual).
    ********************************************************************/
#define PERTINENT(theGraph, theVertex)                                      \
    (gp_IsEdge(theGraph, gp_GetVertexPertinentEdge(theGraph, theVertex)) || \
     gp_IsVertex(theGraph, gp_GetVertexPertinentRootsList(theGraph, theVertex)))

#define NOTPERTINENT(theGraph, theVertex)                                      \
    (gp_IsNotEdge(theGraph, gp_GetVertexPertinentEdge(theGraph, theVertex)) && \
     gp_IsNotVertex(theGraph, gp_GetVertexPertinentRootsList(theGraph, theVertex)))

/********************************************************************
 FUTUREPERTINENT()
    A vertex is future-pertinent in a partially processed graph if
    there is an unprocessed back edge between a DFS ancestor A of the
    vertex v whose edges are currently being processed and either
    theVertex or a DFS descendant D of theVertex not in the same bicomp
    as theVertex.

    Either theVertex is directly adjacent to A by an unembedded back edge
    or there is an unembedded back edge (A, D) and theVertex is a cut
    vertex in the partially processed graph along the DFS tree path from
    D to A.

    If no more edges are added to the partially processed graph prior to
    processing the edges of A, then the vertex would be pertinent.
    The addition of edges to the partially processed graph can alter
    both the pertinence and future pertinence of a vertex.  For example,
    if the vertex is pertinent due to an unprocessed back edge (v, D1) and
    future pertinent due to an unprocessed back edge (A, D2), then the
    vertex may lose both its pertinence and future pertinence when edge
    (v, D1) is added if D2 is in the same subtree as D1.

    Generally, pertinence and future pertinence are dynamic properties
    that can change for a vertex after each edge addition.

    Note that gp_UpdateVertexFuturePertinentChild() must be called before
    this macro. Since it is a statement and not a void expression, the
    desired commented out version does not compile (except with special
    compiler extensions not assumed by this code).
    ********************************************************************/
#define FUTUREPERTINENT(theGraph, theVertex, v)                                       \
    (theGraphDVI(theGraph)[theVertex].leastAncestor < v ||                            \
     (gp_IsVertex(theGraph, theGraphPVI(theGraph)[theVertex].futurePertinentChild) && \
      theGraphDVI(theGraph)[theGraphPVI(theGraph)[theVertex].futurePertinentChild].lowpoint < v))

#define NOTFUTUREPERTINENT(theGraph, theVertex, v)                                       \
    (theGraphDVI(theGraph)[theVertex].leastAncestor >= v &&                              \
     (gp_IsNotVertex(theGraph, theGraphPVI(theGraph)[theVertex].futurePertinentChild) || \
      theGraphDVI(theGraph)[theGraphPVI(theGraph)[theVertex].futurePertinentChild].lowpoint >= v))

/********************************************************************
 INACTIVE()
    For planarity algorithms, a vertex is inactive if it is neither pertinent
    nor future pertinent.
    ********************************************************************/
#define INACTIVE(theGraph, theVertex, v)  \
    (NOTPERTINENT(theGraph, theVertex) && \
     NOTFUTUREPERTINENT(theGraph, theVertex, v))

#ifdef __cplusplus
}
#endif

#endif /* GRAPHPLANARITY_PRIVATE_H */

// ===== lowLevelUtils/apiutils.c =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>



// The graphLib gp_ErrorMessage() and gp_Message() calls are suppressed by
// default, but an application can turn them on if desired.
unsigned quietMode = QUIETMODE_ALL;

unsigned gp_GetQuietMode(void)
{
    return quietMode;
}

void gp_LogErrorMessage(int lineNum, const char *srcFileName, const char *message, ...)
{
    if (!(gp_GetQuietMode() & QUIETMODE_ERRORS))
    {
        va_list args;

        fprintf(stderr, "[ERROR] ");

        va_start(args, message);
        vfprintf(stderr, message, args);
        va_end(args);

        if (lineNum > 0 && srcFileName != NULL)
            fprintf(stderr, "\n\ton line %d of '%s'", lineNum, srcFileName);

        fprintf(stderr, "\n");

        fflush(stderr);
    }
}

/********************************************************************
 debugNOTOK()

 This function returns the literal value of NOTOK. In debug mode,
 NOTOK is redefined to first use printf() to emit information about
 where in the code a NOTOK has occurred. Then, this method is invoked
 so that the debug version of NOTOK still returns the NOTOK value.

 Rather than just returning 0 in the debug-mode NOTOK macro, we
 invoke this method because it gives the option (with recompilation)
 of having the program exit on the first NOTOK occurrence. That
 option is off by default, so we normally get a stack trace of the
 NOTOK occcurences, but on an exhaustive, long-run test, it can be
 handy to stop on the first error since otherwise the error message
 might not be seen.
 ********************************************************************/

#ifdef DEBUG
int debugNOTOK(void)
{
    // exit(-1);
    return 0; // NOTOK is normally defined to be zero
}
#endif

// LOGGING is not defined in the standard compile configuration.
// A graphLib developer can uncomment LOGGING in apiutils.private.h
#ifdef LOGGING

/********************************************************************
 _Log()

 When the project is compiled with LOGGING enabled, this method writes
 a string to the file Edge_Addition_Planarity_Suite.LOG in the current
 working directory.

 On first write, the file is created or cleared.
 Call this method with NULL to close the log file.
 ********************************************************************/

void closeLogFileAtExit(void);

void _Log(char const *Str)
{
    static FILE *logfile = NULL;
    static int triedlogfile = FALSE;

    if (logfile == NULL && !triedlogfile)
    {
        triedlogfile = TRUE;
        if (atexit(closeLogFileAtExit) != 0)
            gp_ErrorMessage("Unable to set up atexit() to close Edge_Addition_Planarity_Suite log file on exit");
        else
        {
            if ((logfile = fopen("Edge_Addition_Planarity_Suite.LOG", WRITETEXT)) == NULL)
                gp_ErrorMessage("Unable to open the Edge_Addition_Planarity_Suite log file");
        }
    }

    if (logfile != NULL)
    {
        if (Str != NULL)
        {
            fprintf(logfile, "%s", Str);
            fflush(logfile);
        }
        else
        {
            fclose(logfile);
            logfile = NULL;
        }
    }
}

void _LogLine(char const *Str)
{
    _Log(Str);
    _Log("\n");
}

void closeLogFileAtExit(void)
{
    _gp_Log(NULL);
}

static char LogStr[MAXLINE + 1];

char *_MakeLogStr1(const char *format, int one)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    sprintf(LogStr, format, one);
#pragma GCC diagnostic pop
    return LogStr;
}

char *_MakeLogStr2(const char *format, int one, int two)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    sprintf(LogStr, format, one, two);
#pragma GCC diagnostic pop
    return LogStr;
}

char *_MakeLogStr3(const char *format, int one, int two, int three)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    sprintf(LogStr, format, one, two, three);
#pragma GCC diagnostic pop
    return LogStr;
}

char *_MakeLogStr4(const char *format, int one, int two, int three, int four)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    sprintf(LogStr, format, one, two, three, four);
#pragma GCC diagnostic pop
    return LogStr;
}

char *_MakeLogStr5(const char *format, int one, int two, int three, int four, int five)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    sprintf(LogStr, format, one, two, three, four, five);
#pragma GCC diagnostic pop
    return LogStr;
}

#endif // LOGGING

// ===== lowLevelUtils/listcoll.c =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#define _LISTCOLL_C

#include <stdlib.h>

/*****************************************************************************
 The data structure defined by this module manages a set of N objects
 arranged as a collection of circular lists, each containing distinct
 elements from the set.

 On construction, LCNew() creates an array of N nodes, each containing a
 prev and next pointer.  The identity of the node is given by its array index.
 Each node's prev and next pointers are set to NIL, indicating that the node
 is not currently part of a list.  LCReset() can be called to reset all
 pointers to NIL.

 The function LCFree() deallocates the collection of lists and clears the
 pointer variable used to pass the collection.

 An empty list is indicated by NIL.  To begin a list with node I, call
 LCPrepend() or LCAppend() with the NIL list and with I as the node.  The prev
 and next pointers in node I are set to I and I is returned as the head of
 the list.

 Future calls to LCPrepend() add a node J as the new first element of the list,
 so the list given as input is pointed to by J's next, and J is returned as
 the head of the list.

 Future calls to LCAppend() add a node J as the new last element, so the prev
 pointer of the list given as input will indicate node J, and the input list
 is returned as the head of the list.

 LCInsertAfter() adds a node immediately after a given anchor node.

 LCInsertBefore() adds a node immediately before a given anchor node and has
    the same effect on a list as LCPrepend().

 The function LCDelete() removes a node I from a list L.  If node I is in the
 list alone, then its pointers are set to NIL, and NIL is returned as the list.
 If node I is not alone in the list, but it is the head of the list (in other
 words, I is equal to L), then L's successor is returned as the new head of the
 list. Whether or not I equals L, node I is deleted by joining its predecessor
 and successor nodes.

 LCCopy() copies the contents of one collection to another if both are of
 equal size.

 LCGetNext() is used for forward iteration through a list in the collection.
 The expected iteration pattern is first to process the node one has, then call
 LCGetNext() to get the next node, so if the result of LCGetNext() would be the
 head of the list, then NIL is returned instead.  This simplifies most
 coding operations involving LCGetNext().

 LCGetPrev() is used for backward iteration through a list in the collection.
 The expected iteration pattern is that the last list element will be obtained
 by an initial call to LCGetPrev() with theNode equal to NIL.  This call
 should appear outside of the iteration loop.  The iteration loop then
 proceeds while the current node is not NIL.  The loop body processes the
 current node, then LCGetPrev() is called with theNode equal to the current
 node.  LCGetPrev() returns NIL if theNode is equal to theList.  Otherwise,
 the predecessor of theNode is returned.

 *****************************************************************************/

/*****************************************************************************
 LCNew()
 *****************************************************************************/

listCollectionP LCNew(int N)
{
     listCollectionP theListColl = NULL;

     if (N <= 0)
          return theListColl;

     theListColl = (listCollectionP)malloc(sizeof(listCollectionStruct));
     if (theListColl != NULL)
     {
          theListColl->List = (lcnode *)malloc(N * sizeof(lcnode));
          if (theListColl->List == NULL)
          {
               free(theListColl);
               theListColl = NULL;
          }
          else
          {
               theListColl->N = N;
               LCReset(theListColl);
          }
     }
     return theListColl;
}

/*****************************************************************************
 LCFree()
 *****************************************************************************/

void LCFree(listCollectionP *pListColl)
{
     if (pListColl == NULL || *pListColl == NULL)
          return;

     if ((*pListColl)->List != NULL)
          free((*pListColl)->List);

     free(*pListColl);
     *pListColl = NULL;
}

#ifndef SPEED_MACROS

/*****************************************************************************
 LCReset()
 *****************************************************************************/

void LCReset(listCollectionP listColl)
{
     int K;

     for (K = 0; K < listColl->N; K++)
          listColl->List[K].prev = listColl->List[K].next = NIL;
}

/*****************************************************************************
 LCCopy()
 *****************************************************************************/

void LCCopy(listCollectionP dst, listCollectionP src)
{
     int K;

     if (dst == NULL || src == NULL || dst->N != src->N)
          return;

     for (K = 0; K < dst->N; K++)
          dst->List[K] = src->List[K];
}

/*****************************************************************************
 LCGetNext()
 *****************************************************************************/

int LCGetNext(listCollectionP listColl, int theList, int theNode)
{
     int next;

     if (listColl == NULL || theList == NIL || theNode == NIL)
          return NIL;
     next = listColl->List[theNode].next;
     return next == theList ? NIL : next;
}

/*****************************************************************************
 LCGetPrev()
 *****************************************************************************/

int LCGetPrev(listCollectionP listColl, int theList, int theNode)
{
     if (listColl == NULL || theList == NIL)
          return NIL;
     if (theNode == NIL)
          return listColl->List[theList].prev;
     if (theNode == theList)
          return NIL;
     return listColl->List[theNode].prev;
}

/*****************************************************************************
 LCPrepend()
 *****************************************************************************/

int LCPrepend(listCollectionP listColl, int theList, int theNode)
{
     /* If the append worked, then theNode is last, which in a circular
        list is the direct predecessor of the list head node, so we
        just back up one. For singletons, the result is unchanged. */

     return listColl->List[LCAppend(listColl, theList, theNode)].prev;
}

/*****************************************************************************
 LCAppend()
 *****************************************************************************/

int LCAppend(listCollectionP listColl, int theList, int theNode)
{
     /* If the given list is empty, then the given node becomes the
        singleton list output */

     if (theList == NIL)
     {
          listColl->List[theNode].prev = listColl->List[theNode].next = theNode;
          theList = theNode;
     }

     /* Otherwise, make theNode the predecessor of head node of theList,
        which is where the last node goes in a circular list. */

     else
     {
          int pred = listColl->List[theList].prev;

          listColl->List[theList].prev = theNode;
          listColl->List[theNode].next = theList;
          listColl->List[theNode].prev = pred;
          listColl->List[pred].next = theNode;
     }

     /* Return the list (only really important if it was NIL) */

     return theList;
}

/*****************************************************************************
 LCDelete()
 *****************************************************************************/

int LCDelete(listCollectionP listColl, int theList, int theNode)
{
     /* If the list is a singleton, then NIL its pointers and
        return NIL for theList*/

     if (listColl->List[theList].next == theList)
     {
          listColl->List[theList].prev = listColl->List[theList].next = NIL;
          theList = NIL;
     }

     /* Join predecessor and successor, dropping theNode from the list.
        If theNode is the head of the list, then return the successor as
        the new head node. */

     else
     {
          int pred = listColl->List[theNode].prev,
              succ = listColl->List[theNode].next;

          listColl->List[pred].next = succ;
          listColl->List[succ].prev = pred;

          listColl->List[theNode].prev = listColl->List[theNode].next = NIL;

          if (theList == theNode)
               theList = succ;
     }

     return theList;
}

#endif // SPEED_MACROS

// ===== lowLevelUtils/stack.c =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#include <stdlib.h>

stackP sp_New(int capacity)
{
    stackP theStack;

    theStack = (stackP)malloc(sizeof(stackStruct));

    if (theStack != NULL)
    {
        theStack->S = (int *)malloc(capacity * sizeof(int));
        if (theStack->S == NULL)
        {
            free(theStack);
            theStack = NULL;
        }
    }

    if (theStack != NULL)
    {
        theStack->capacity = capacity;
        sp_ClearStack(theStack);
    }

    return theStack;
}

void sp_Free(stackP *pStack)
{
    if (pStack == NULL || *pStack == NULL)
        return;

    (*pStack)->capacity = (*pStack)->size = 0;

    if ((*pStack)->S != NULL)
        free((*pStack)->S);
    (*pStack)->S = NULL;
    free(*pStack);

    *pStack = NULL;
}

int sp_CopyContent(stackP stackDst, stackP stackSrc)
{
    if (stackDst->capacity < stackSrc->size)
        return NOTOK;

    if (stackSrc->size > 0)
        memcpy(stackDst->S, stackSrc->S, stackSrc->size * sizeof(int));

    stackDst->size = stackSrc->size;
    return OK;
}

#ifndef SPEED_MACROS

int sp_ClearStack(stackP theStack)
{
    theStack->size = 0;
    return OK;
}

int sp_GetCurrentSize(stackP theStack)
{
    return theStack->size;
}

int sp_SetCurrentSize(stackP theStack, int size)
{
    return size > theStack->capacity ? NOTOK : (theStack->size = size, OK);
}

int sp_IsEmpty(stackP theStack)
{
    return !theStack->size;
}

int sp_NonEmpty(stackP theStack)
{
    return theStack->size;
}

int sp__Push(stackP theStack, int a)
{
    if (theStack->size >= theStack->capacity)
        return NOTOK;

    theStack->S[theStack->size++] = a;
    return OK;
}

int sp__Push2(stackP theStack, int a, int b)
{
    if (theStack->size + 1 >= theStack->capacity)
        return NOTOK;

    theStack->S[theStack->size++] = a;
    theStack->S[theStack->size++] = b;
    return OK;
}

int sp__Pop(stackP theStack, int *pA)
{
    if (theStack->size <= 0)
        return NOTOK;

    *pA = theStack->S[--theStack->size];
    return OK;
}

int sp__Pop_Discard(stackP theStack)
{
    if (theStack->size <= 0)
        return NOTOK;

    --theStack->size;
    return OK;
}

int sp__Pop2(stackP theStack, int *pA, int *pB)
{
    if (theStack->size <= 1)
        return NOTOK;

    *pB = theStack->S[--theStack->size];
    *pA = theStack->S[--theStack->size];

    return OK;
}

int sp__Pop2_Discard1(stackP theStack, int *pA)
{
    if (theStack->size <= 1)
        return NOTOK;

    // When a pair of the form (main, secondary) are pushed in order,
    // it is sometimes necessary to pop the secondary and discard,
    // then pop and store the main datum.
    --theStack->size;
    *pA = theStack->S[--theStack->size];

    return OK;
}

int sp__Pop2_Discard(stackP theStack)
{
    if (theStack->size <= 1)
        return NOTOK;

    --theStack->size;
    --theStack->size;

    return OK;
}

int sp_Top(stackP theStack)
{
    return theStack->size ? theStack->S[theStack->size - 1] : NIL;
}

int sp_Get(stackP theStack, int pos)
{
    if (theStack == NULL || pos < 0 || pos >= theStack->size)
        return NOTOK;

    return (theStack->S[pos]);
}

int sp_Set(stackP theStack, int pos, int val)
{
    if (theStack == NULL || pos < 0 || pos >= theStack->size)
        return NOTOK;

    return (theStack->S[pos] = val);
}

#endif // not defined SPEED_MACROS

// ===== graphEdgeDetector.c =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/
#include <stdlib.h>
#include <stdio.h>

graphEdgeDetectorP ged_New(int theCapacity)
{
    graphEdgeDetectorP theDetector = NULL;

    if (theCapacity <= 0)
    {
        return NULL;
    }

    theDetector = (graphEdgeDetectorP)calloc(1, sizeof(graphEdgeDetectorStruct));
    if (theDetector == NULL)
    {
        return NULL;
    }

    theDetector->edgeDetector = (unsigned *)calloc(theCapacity, sizeof(unsigned));
    if (theDetector->edgeDetector == NULL)
    {
        free(theDetector);
        return NULL;
    }

    theDetector->edgeDetectorCapacity = theCapacity;

    return theDetector;
}

unsigned long long ged_Hash(graphEdgeDetectorP theDetector, int v, int w)
{
    unsigned uv = (unsigned)v;
    unsigned uw = (unsigned)w;
    unsigned FNV_PRIME = 16777619u;
    unsigned FNV_OFFSET_BASIS = 2166136261u;
    unsigned long long hash = FNV_OFFSET_BASIS;
    unsigned long long totalBits;

    if (uv > uw)
    {
        unsigned temp = uv;
        uv = uw;
        uw = temp;
    }
    hash ^= uv;
    hash *= FNV_PRIME;

    hash ^= uw;
    hash *= FNV_PRIME;

    totalBits = ((unsigned long long)(theDetector->edgeDetectorCapacity)) << 5;
    hash = hash % totalBits;

    return hash;
}

int ged_Set(graphEdgeDetectorP theDetector, int v, int w)
{
    unsigned long long H;
    unsigned arrayidx;
    unsigned bitmask;

    if (theDetector == NULL || theDetector->edgeDetector == NULL)
    {
        return NOTOK;
    }

    H = ged_Hash(theDetector, v, w);
    if ((H >> 5) > INT_MAX)
    {
        return NOTOK;
    }

    arrayidx = (unsigned)(H >> 5);
    bitmask = 1u << ((unsigned)(H & 31));
    theDetector->edgeDetector[arrayidx] |= bitmask;

    return OK;
}

void ged_Free(graphEdgeDetectorP *pDetector)
{
    if (pDetector == NULL || *pDetector == NULL)
    {
        return;
    }

    if ((*pDetector)->edgeDetector != NULL)
    {
        free((*pDetector)->edgeDetector);
        (*pDetector)->edgeDetector = NULL;
    }

    free(*pDetector);
    *pDetector = NULL;
}

// ===== extensionSystem/graphExtensions.c =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#include <stdlib.h>
#include <string.h>



/* Imported functions */

extern void _InitFunctionTable(graphP theGraph);

/* Private function */

void _FreeExtension(graphExtensionP extension);

/********************************************************************
 * The moduleIDGenerator is used to help ensure that all extensions
 * added during a run-time have a different integer identifier.
 * An ID identifies an extension, which may be added to multiple
 * graphs.  It is used in lieu of identifying extensions by a string
 * name, which is noticeably expensive when a frequently called
 * overload function seeks the extension context for a graph.
 ********************************************************************/


/********************************************************************
 The extension mechanism allows new modules to equip a graph with the
 data structures and functions needed to implement new algorithms
 without impeding the performance of the core graph planar embedding
 algorithms on graphs that have not been so equipped.

 The following steps must be used to create a graph extension:

  1) Create a moduleID variable initialized to zero that will be
     assigned a positive integer the first time the extension is

     a) The _Feature_ClearStructures() should simply null out pointers
        to extra structures on its first invocation, but thereafter it
        should free them and then null them.  Since the null-only step
        is done only once in gp_ExtendWith_Feature(), it seems reasonable
        to not bother with a more complicated _Feature_ClearStructures().
        But, as an extension is developed, the data structures change,
        so it is best to keep all this logic in one place.

     b) The _Feature_CreateStructures() should just allocate memory for
        but not initialize any vertex level and edge level data structures.
        Data structures maintained at the graph level, such as a stack or a
        list collection, should be created _and_ initialized.

     c) The _Feature_InitStructures() should invoke just the functions
        needed to initialize the custom VertexRec, VertexInfo and
        EdgeRec data members, if any.

     d) The _Feature_CopyData() should invoke just the functions needed
        to copy the custom VertexRec, VertexInfo and EdgeRec data members, 
        if any, from a source extension context to a destination extension 
        context. For custom EdgeRec arrays, if the destination has more 
        capacitiy than the source, then the implementation should also 
        ensure the extra EdgeRecs are initialized in the destination.

  8) Define a function gp_Detach_Feature() that invokes gp_RemoveExtension()
     This should be done for consistency, so that users of a feature
     do not attach it with gp_ExtendWith_Feature() and remove it with
     gp_RemoveExtension().  However, it may sometimes be necessary to
     run more code than just gp_RemoveExtension() when detaching a feature,
     e.g., some final result values of a feature may be saved to data
     available in the core graph or in other features.
 ********************************************************************/

/********************************************************************
 gp_FreeExtensions()

 @param pFirst - pointer to head pointer of graph extension list

 Each graph extension is freed, including invoking the freeContext
 function provided when the extension was added.
 ********************************************************************/

void gp_FreeExtensions(graphP theGraph)
{
    if (theGraph != NULL)
    {
        graphExtensionP curr = theGraph->extensions;
        graphExtensionP next = NULL;

        while (curr != NULL)
        {
            next = (graphExtensionP)curr->next;
            _FreeExtension(curr);
            curr = next;
        }

        theGraph->extensions = NULL;
        if (theGraph->extensionLookupTable != NULL)
        {
            memset(theGraph->extensionLookupTable, 0, (MAXNUMSUPPORTEDEXTENSIONS+1)*sizeof(graphExtensionP));
        }
        _InitFunctionTable(theGraph);
    }
}

/********************************************************************
 _FreeExtension()
 ********************************************************************/
void _FreeExtension(graphExtensionP extension)
{
    if (extension->context != NULL && extension->freeContext != NULL)
    {
        extension->freeContext(extension->context);
    }
    free(extension);
}

// ===== graph.c =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/


// To enable performing of certain initialization calls for the
// DFSUtils, Planarity, and Outerplanarity pseudo-extensions.

#include <stdlib.h>

/* Imported functions for FUNCTION POINTERS */

extern int _EmbeddingInitialize(graphP theGraph);
extern int _SortVertices(graphP theGraph);
extern void _EmbedBackEdgeToDescendant(graphP theGraph, int RootSide, int RootVertex, int W, int WPrevLink);
extern void _WalkUp(graphP theGraph, int v, int e);
extern int _WalkDown(graphP theGraph, int v, int RootVertex);
extern int _MergeBicomps(graphP theGraph, int v, int RootVertex, int W, int WPrevLink);
extern void _MergeVertex(graphP theGraph, int W, int WPrevLink, int R);
extern int _HandleBlockedBicomp(graphP theGraph, int v, int RootVertex, int R);
extern int _HandleInactiveVertex(graphP theGraph, int BicompRoot, int *pW, int *pWPrevLink);
extern int _EmbedPostprocess(graphP theGraph, int v, int edgeEmbeddingResult);

/* Internal util functions for FUNCTION POINTERS */

int _HideVertex(graphP theGraph, int vertex);
void _HideEdge(graphP theGraph, int e);
void _RestoreEdge(graphP theGraph, int e);
int _ContractEdge(graphP theGraph, int e);
int _IdentifyVertices(graphP theGraph, int u, int v, int eBefore);
int _RestoreVertex(graphP theGraph);

/********************************************************************
 Private functions, except exported within library
 ********************************************************************/

void _InitIsolatorContext(graphP theGraph);
void _ClearVertexVisitedFlags(graphP theGraph, int includeVirtualVertices);
int _FillVertexVisitedIndexes(graphP theGraph, int FillValue);

int _gp_FindEdge(graphP theGraph, int u, int v);

int _EquipGraphWithParallelEdgeDetector(graphP theGraph, int requiredEdgeCapacity);
int _RestoreHiddenEdges(graphP theGraph, int stackBottom);

void _InitFunctionTable(graphP theGraph);

/********************************************************************
 Private functions.
 ********************************************************************/

void _InitVertices(graphP theGraph);
void _InitEdges(graphP theGraph);

void _ClearGraph(graphP theGraph);

typedef struct
{
    int u;
    int v;
} randomGraphEdgeRec;

typedef struct
{
    int a;
    int b;
    int c;
} randomGraphFaceRec;

void _AttachEdgeRecord(graphP theGraph, int v, int e, int link, int newEdge);
void _DetachEdgeRecord(graphP theGraph, int e);
void _RestoreEdgeRecord(graphP theGraph, int e);
int _DeleteEdge(graphP theGraph, int e);

/* Private functions for which there are FUNCTION POINTERS */

void _InitVertexRec(graphP theGraph, int v);
void _InitEdgeRec(graphP theGraph, int e);

int _EnsureVertexCapacity(graphP theGraph, int N);
void _ResetGraphStorage(graphP theGraph);
int _EnsureEdgeCapacity(graphP theGraph, int requiredEdgeCapacity);

/********************************************************************
 gp_New()
 Constructor for graph object.
 Can create two graphs if restricted to no dynamic memory.
 ********************************************************************/

graphP gp_New(void)
{
    graphP theGraph = (graphP)calloc(1, sizeof(graphStruct));
    graphFunctionTableP functionTable = (graphFunctionTableP)calloc(1, sizeof(graphFunctionTableStruct));
    graphPrivateDataP theGraphPrivateData = (graphPrivateDataP)calloc(1, sizeof(graphPrivateDataStruct));
    graphExtensionP *extensionLookupTable = (graphExtensionP *)calloc(MAXNUMSUPPORTEDEXTENSIONS + 1, sizeof(graphExtensionP));

    if (theGraph != NULL && functionTable != NULL &&
        theGraphPrivateData != NULL && extensionLookupTable != NULL)
    {
        theGraph->privateData = (void *)theGraphPrivateData;
        theGraph->extensionLookupTable = extensionLookupTable;

        theGraph->functions = functionTable;
        _InitFunctionTable(theGraph);

        _ClearGraph(theGraph);
    }
    else
    {
        if (theGraph != NULL)
        {
            free(theGraph);
            theGraph = NULL;
        }
        if (functionTable != NULL)
        {
            free(functionTable);
            functionTable = NULL;
        }
        if (theGraphPrivateData != NULL)
        {
            free(theGraphPrivateData);
            theGraphPrivateData = NULL;
        }
        if (extensionLookupTable != NULL)
        {
            free(extensionLookupTable);
            extensionLookupTable = NULL;
        }
    }

    return theGraph;
}

/********************************************************************
 _InitFunctionTable()

 If you add functions to the function table, then they must be
 initialized here, but you must also add the new function pointer
 to the definition of the graphFunctionTableStruct in graphFunctionTable.h

 Function headers for the functions used to initialize the table are
 classified at the top of this file as either imported from other
 compilation units (extern) or private to this compilation unit.
 Search for FUNCTION POINTERS in this file to see where to add the
 function header.
 ********************************************************************/

void _InitFunctionTable(graphP theGraph)
{
    if (theGraph != NULL && theGraph->functions != NULL)
    {
        theGraph->functions->fpEmbeddingInitialize = _EmbeddingInitialize;
        theGraph->functions->fpEmbedBackEdgeToDescendant = _EmbedBackEdgeToDescendant;
        theGraph->functions->fpWalkUp = _WalkUp;
        theGraph->functions->fpWalkDown = _WalkDown;
        theGraph->functions->fpMergeBicomps = _MergeBicomps;
        theGraph->functions->fpMergeVertex = _MergeVertex;
        theGraph->functions->fpHandleBlockedBicomp = _HandleBlockedBicomp;
        theGraph->functions->fpHandleInactiveVertex = _HandleInactiveVertex;
        theGraph->functions->fpEmbedPostprocess = _EmbedPostprocess;

        theGraph->functions->fpEnsureVertexCapacity = _EnsureVertexCapacity;
        theGraph->functions->fpResetGraphStorage = _ResetGraphStorage;
        theGraph->functions->fpEnsureEdgeCapacity = _EnsureEdgeCapacity;
        theGraph->functions->fpSortVertices = _SortVertices;

        theGraph->functions->fpDeleteEdge = _DeleteEdge;
        theGraph->functions->fpHideEdge = _HideEdge;
        theGraph->functions->fpRestoreEdge = _RestoreEdge;
        theGraph->functions->fpHideVertex = _HideVertex;
        theGraph->functions->fpRestoreVertex = _RestoreVertex;
        theGraph->functions->fpContractEdge = _ContractEdge;
        theGraph->functions->fpIdentifyVertices = _IdentifyVertices;
    }
}

/********************************************************************
 gp_EnsureVertexCapacity()

 Allocates memory for N vertices and N virtual vertices. Once N > 0
 vertices have been allocated, this method currently does not support
 being called a second time to add more vertices.

 This method will also ensure that the edge capacity is allocated or
 reallocated to be at least (DEFAULT_EDGE_CAPACITY_FACTOR * N), i.e.,
 a capacity for twice that many edge records, two per edge (plus 2
 for the default of using one-based arrays). The edge capacity can
 be set before this function using gp_EnsureEdgeCapacity().

 The edgeHoles stack, initially empty, is set to the edgeCapacity,
     which is big enough to push every edge (to indicate an edge,
     only one of its two edge records need be pushed).

 The numEdgeHoles is set to 0; it tracks the edgeHoles stack size,
    so the number of edge records in use can be efficiently computed.

 The stack, initially empty, is made big enough for a pair of integers
     per edge (2 * edgeCapacity), or 6N integers if the edgeCapacity
     was set below the default. Space for 2 extra integers is added so
     depth-first search can push (NIL, NIL) to start at a DFS tree root.

 The BicompRootLists and sortedDFSChildLists are set to a size of N,
     and they start out empty.

 DVI and PVI are set to store N of their respective vertex info records.

 An instance of the isolator context is created.

 Returns OK on success, NOTOK on any failure.
          On NOTOK, graph extensions are freed so that the graph is
          returned to the post-condition of gp_New().
 ********************************************************************/

int gp_EnsureVertexCapacity(graphP theGraph, int N)
{
    long long effectiveEdgeCapacity = 0;

    // valid params check
    if (theGraph == NULL || N <= 0)
        return NOTOK;

    // Should not call init a second time; use reinit
    if (gp_GetN(theGraph) > 0)
        return NOTOK;

    // Reject a vertex count whose capacity arithmetic cannot be represented
    // in int (issue #325). The effective edge capacity is the greater of a
    // pre-set edgeCapacity and DEFAULT_EDGE_CAPACITY_FACTOR * N, because
    // gp_EnsureEdgeCapacity() may legitimately have stored a lower value
    // before this call. The stack holds (edgeCapacity << 2) + 2 entries,
    // which strictly exceeds every other derived quantity (vertex storage,
    // edge storage and the 2 * 2 * DEFAULT_EDGE_CAPACITY_FACTOR * N + 2
    // fallback), so one test on it shields them all, including the
    // gp_UpperBoundEdgeStorage() uses in the algorithm extensions.
    effectiveEdgeCapacity = (long long)DEFAULT_EDGE_CAPACITY_FACTOR * N;
    if (effectiveEdgeCapacity < theGraph->edgeCapacity)
        effectiveEdgeCapacity = theGraph->edgeCapacity;
    if ((effectiveEdgeCapacity << 2) + 2 > INT_MAX)
        return NOTOK;

    return theGraph->functions->fpEnsureVertexCapacity(theGraph, N);
}

int _EnsureVertexCapacity(graphP theGraph, int N)
{
    int Vsize, VIsize, Esize, stackSize;

    // Compute the vertex and edge capacities of the graph
    theGraph->N = N;
    theGraph->NV = N;
    theGraph->edgeCapacity = theGraph->edgeCapacity > 0 ? theGraph->edgeCapacity : DEFAULT_EDGE_CAPACITY_FACTOR * N;
    theGraph->numEdgeHoles = 0;

    VIsize = gp_UpperBoundVertices(theGraph);
    Vsize = gp_UpperBoundVertexStorage(theGraph);
    Esize = gp_UpperBoundEdgeStorage(theGraph);

    // Stack size is 2 integers per edge record plus 2 to start depth-first search at a tree root
    stackSize = (theGraph->edgeCapacity << 2) + 2;
    // In case of small edgeCapacity, ensure minimum based on number of vertices
    stackSize = stackSize <= 2 * 2 * DEFAULT_EDGE_CAPACITY_FACTOR * N ? 2 * 2 * DEFAULT_EDGE_CAPACITY_FACTOR * N + 2 : stackSize;

    // Allocate memory as described above
    if ((theGraph->V = (vertexRecP)calloc(Vsize, sizeof(vertexRec))) == NULL ||
        (theGraph->E = (edgeRecP)calloc(Esize, sizeof(edgeRec))) == NULL ||
        (theGraph->edgeHoles = sp_New(theGraph->edgeCapacity)) == NULL ||

        (theGraph->theStack = sp_New(stackSize)) == NULL ||
        (theGraphBicompRootLists(theGraph) = LCNew(VIsize)) == NULL ||
        (theGraphDVI(theGraph) = (DFSUtils_VertexInfoP)calloc(VIsize, sizeof(DFSUtils_VertexInfo))) == NULL ||

        (theGraphPVI(theGraph) = (Planarity_VertexInfoP)calloc(VIsize, sizeof(Planarity_VertexInfo))) == NULL ||
        (theGraphSortedDFSChildLists(theGraph) = LCNew(VIsize)) == NULL ||
        (theGraphExtFace(theGraph) = (extFaceLinkRecP)calloc(Vsize, sizeof(extFaceLinkRec))) == NULL ||
        (theGraphIC(theGraph) = (isolatorContextP)calloc(1, sizeof(isolatorContextStruct))) == NULL ||
        0)
    {
        _ClearGraph(theGraph);
        return NOTOK;
    }

    // Initialize memory
    _InitVertices(theGraph);
    _InitEdges(theGraph);
    _InitIsolatorContext(theGraph);

    return OK;
}

/********************************************************************
 _InitVertices()
 ********************************************************************/
void _InitVertices(graphP theGraph)
{
    memset(theGraph->V, NIL_CHAR, gp_UpperBoundVertexStorage(theGraph) * sizeof(vertexRec));

    memset(theGraphDVI(theGraph), NIL_CHAR, gp_UpperBoundVertices(theGraph) * sizeof(DFSUtils_VertexInfo));

    memset(theGraphPVI(theGraph), NIL_CHAR, gp_UpperBoundVertices(theGraph) * sizeof(Planarity_VertexInfo));
    memset(theGraphExtFace(theGraph), NIL_CHAR, gp_UpperBoundVertexStorage(theGraph) * sizeof(extFaceLinkRec));

#ifdef USE_1BASEDARRAYS
// For 1-based arrays, the memset() initializes the flags correctly
#else
    for (int v = gp_LowerBoundVertexStorage(theGraph); v < gp_UpperBoundVertexStorage(theGraph); ++v)
        gp_InitFlags(theGraph, v);
#endif
}

/********************************************************************
 _InitEdges()
 ********************************************************************/
void _InitEdges(graphP theGraph)
{
    memset(theGraph->E, NIL_CHAR, gp_UpperBoundEdgeStorage(theGraph) * sizeof(edgeRec));

#ifdef USE_1BASEDARRAYS
#else
    for (int e = gp_LowerBoundEdgeStorage(theGraph); e < gp_UpperBoundEdgeStorage(theGraph); ++e)
        gp_InitEdgeFlags(theGraph, e);
#endif
}

void _ResetGraphStorage(graphP theGraph)
{
    theGraph->M = 0;
    theGraph->embedFlags = 0;

    theGraph->graphFlags &= ~GRAPHFLAGS_DFSNUMBERED;
    theGraph->graphFlags &= ~GRAPHFLAGS_DFSNUMBERED_DIRECTED;
    theGraph->graphFlags &= ~GRAPHFLAGS_SORTEDBYDFI;
    theGraph->graphFlags &= ~GRAPHFLAGS_LOWPOINTSCOMPUTED;
    theGraph->graphFlags &= ~GRAPHFLAGS_DIRECTEDEDGEDETECTED;
    theGraph->graphFlags &= ~GRAPHFLAGS_PARALLELEDGEDETECTED;
    _InitVertices(theGraph);
    _InitEdges(theGraph);
    _InitIsolatorContext(theGraph);

    LCReset(theGraphBicompRootLists(theGraph));
    LCReset(theGraphSortedDFSChildLists(theGraph));
    sp_ClearStack(theGraph->theStack);
    sp_ClearStack(theGraph->edgeHoles);
    theGraph->numEdgeHoles = 0;
}

int _EnsureEdgeCapacity(graphP theGraph, int requiredEdgeCapacity)
{
    stackP newStack = NULL;
    int newEsize = gp_LowerBoundEdgeStorage(theGraph) + (requiredEdgeCapacity << 1);

    // If the new size is less than or equal to the current edge storage size,
    // then the graph already has the required edge capacity
    if (newEsize <= gp_UpperBoundEdgeStorage(theGraph))
        return OK;

    // Expand theStack. Depth-first search needs 2 integers per edge record
    // (2 edge records per edge), plus 2 to start the DFS on a tree root
    //
    if (sp_GetCapacity(theGraph->theStack) < 2 * (2 * requiredEdgeCapacity) + 2)
    {
        int newStackSize = 2 * (2 * requiredEdgeCapacity) + 2;

        if (newStackSize < 2 * (2 * DEFAULT_EDGE_CAPACITY_FACTOR * gp_GetN(theGraph)) + 2)
        {
            // NOTE: We enforce a minimum stack based on number of vertices
            //       if edgeCapacity is small. Currently, this will not
            //       happen because we only 'ensure' edge capacity, i.e.,
            //       the capacity can only ever get bigger. However, this
            //       rule is enforced in case future methods are added
            //       that reduce edge capacity
            newStackSize = 2 * (2 * DEFAULT_EDGE_CAPACITY_FACTOR * gp_GetN(theGraph)) + 2;
        }

        if ((newStack = sp_New(newStackSize)) == NULL)
            return NOTOK;

        sp_CopyContent(newStack, theGraph->theStack);
        sp_Free(&theGraph->theStack);
        theGraph->theStack = newStack;
    }

    // Expand edgeHoles (at most, every edge is a hole if all edges deleted)
    if ((newStack = sp_New(requiredEdgeCapacity)) == NULL)
    {
        return NOTOK;
    }

    sp_CopyContent(newStack, theGraph->edgeHoles);
    sp_Free(&theGraph->edgeHoles);
    theGraph->edgeHoles = newStack;
    theGraph->numEdgeHoles = sp_GetCurrentSize(theGraph->edgeHoles);

    // Reallocate the edgeRec array to the new size,
    theGraph->E = (edgeRecP)realloc(theGraph->E, newEsize * sizeof(edgeRec));
    if (theGraph->E == NULL)
        return NOTOK;

    // Initialize the new edge records
    for (int e = gp_UpperBoundEdgeStorage(theGraph); e < newEsize; ++e)
        _InitEdgeRec(theGraph, e);

    theGraph->edgeCapacity = requiredEdgeCapacity;

    if (_EquipGraphWithParallelEdgeDetector(theGraph, requiredEdgeCapacity) != OK)
        return NOTOK;

    return OK;
}

/********************************************************************
 _EquipGraphWithParallelEdgeDetector()
 ********************************************************************/

int _EquipGraphWithParallelEdgeDetector(graphP theGraph, int requiredEdgeCapacity)
{
    graphEdgeDetectorP newDetector = NULL;
    int v, e, w, twin_e;

    if (theGraph == NULL)
        return NOTOK;

    newDetector = ged_New(requiredEdgeCapacity);
    if (newDetector == NULL)
        return NOTOK;

    for (e = gp_LowerBoundEdges(theGraph); e < gp_UpperBoundEdges(theGraph); e += 2)
    {
        if (gp_EdgeNotInUse(theGraph, e))
            continue;

        twin_e = gp_GetTwin(theGraph, e);

        v = gp_GetNeighbor(theGraph, e);
        w = gp_GetNeighbor(theGraph, twin_e);

        ged_Set(newDetector, v, w);
    }

    if (theGraphEdgeDetector(theGraph) != NULL)
    {
        ged_Free(&theGraphEdgeDetector(theGraph));
    }
    theGraphEdgeDetector(theGraph) = newDetector;

    return OK;
}

/********************************************************************
 _InitVertexRec()
 Sets the fields in a single vertex record to initial values
 ********************************************************************/

void _InitVertexRec(graphP theGraph, int v)
{
    gp_SetFirstEdge(theGraph, v, NIL);
    gp_SetLastEdge(theGraph, v, NIL);
    gp_SetIndex(theGraph, v, NIL);
    gp_InitFlags(theGraph, v);
}

/********************************************************************
 _InitEdgeRec()
 Sets the fields in a single edge record structure to initial values
 ********************************************************************/

void _InitEdgeRec(graphP theGraph, int e)
{
    gp_SetNeighbor(theGraph, e, NIL);
    gp_SetPrevEdge(theGraph, e, NIL);
    gp_SetNextEdge(theGraph, e, NIL);
    gp_InitEdgeFlags(theGraph, e);
}

/********************************************************************
 _InitIsolatorContext()
 ********************************************************************/

void _InitIsolatorContext(graphP theGraph)
{
    isolatorContextP IC = theGraphIC(theGraph);

    if (IC != NULL)
    {
        IC->minorType = MINORTYPE_NONE;
        IC->v = IC->r = IC->x = IC->y = IC->w = IC->px = IC->py = IC->z =
            IC->ux = IC->dx = IC->uy = IC->dy = IC->dw = IC->uz = IC->dz = NIL;
    }
}

/********************************************************************
 _ClearVertexVisitedFlags()
 Clears the visited flags of vertices, and if the second parameter
 is truthy, also clears the visited flags of virtual vertices.
 ********************************************************************/

void _ClearVertexVisitedFlags(graphP theGraph, int includeVirtualVertices)
{
    for (int v = gp_LowerBoundVertices(theGraph); v < gp_UpperBoundVertices(theGraph); ++v)
        gp_ClearVisited(theGraph, v);

    if (includeVirtualVertices)
        for (int vv = gp_LowerBoundVirtualVertices(theGraph); vv < gp_UpperBoundVirtualVertices(theGraph); ++vv)
            gp_ClearVisited(theGraph, vv);
}

/********************************************************************
 _FillVertexVisitedIndexes()

 Places the FillValue into the visitedIndex of all non-virtual vertices
 in the graph.

 Returns OK on success, NOTOK on failure.
 ********************************************************************/

int _FillVertexVisitedIndexes(graphP theGraph, int FillValue)
{
    if (theGraph == NULL)
        return NOTOK;

    for (int v = gp_LowerBoundVertices(theGraph); v < gp_UpperBoundVertices(theGraph); ++v)
        gp_SetVertexVisitedIndex(theGraph, v, FillValue);

    return OK;
}

/********************************************************************
 _ClearGraph()
 Clears all memory used by the graph, restoring it to the state it
 was in immediately after gp_New() created it.
 ********************************************************************/

void _ClearGraph(graphP theGraph)
{
    if (theGraph->V != NULL)
    {
        free(theGraph->V);
        theGraph->V = NULL;
    }
    if (theGraph->E != NULL)
    {
        free(theGraph->E);
        theGraph->E = NULL;
    }

    theGraph->N = 0;
    theGraph->NV = 0;
    theGraph->M = 0;
    theGraph->edgeCapacity = 0;
    theGraph->embedFlags = 0;

    sp_Free(&theGraph->edgeHoles);
    theGraph->numEdgeHoles = 0;

    sp_Free(&theGraph->theStack);
    LCFree(&theGraphBicompRootLists(theGraph));
    if (theGraphDVI(theGraph) != NULL)
    {
        free(theGraphDVI(theGraph));
        theGraphDVI(theGraph) = NULL;
    }

    if (theGraphPVI(theGraph) != NULL)
    {
        free(theGraphPVI(theGraph));
        theGraphPVI(theGraph) = NULL;
    }
    LCFree(&theGraphSortedDFSChildLists(theGraph));
    if (theGraphExtFace(theGraph) != NULL)
    {
        free(theGraphExtFace(theGraph));
        theGraphExtFace(theGraph) = NULL;
    }
    if (theGraphIC(theGraph) != NULL)
    {
        free(theGraphIC(theGraph));
        theGraphIC(theGraph) = NULL;
    }
    ged_Free(&theGraphEdgeDetector(theGraph));

    gp_FreeExtensions(theGraph);

    // Free the pseudo-extensions
    if (gp_GetGraphFlags(theGraph) & GRAPHFLAGS_EXTENDEDWITH_PLANARITY)
        gp_Detach_Planarity(theGraph);

    if (gp_GetGraphFlags(theGraph) & GRAPHFLAGS_EXTENDEDWITH_DFSUTILS)
        gp_Detach_DFSUtils(theGraph);

    theGraph->graphFlags = 0;
}

/********************************************************************
 gp_Free()
 Frees G and V, then the graph record. Then sets the caller's graph
 pointer to NULL (caller must pass the address of a graphP variable).
 ********************************************************************/

void gp_Free(graphP *pGraph)
{
    if (pGraph == NULL)
        return;
    if (*pGraph == NULL)
        return;

    _ClearGraph(*pGraph);

    if ((*pGraph)->functions != NULL)
    {
        free((*pGraph)->functions);
        (*pGraph)->functions = NULL;
    }

    if ((*pGraph)->privateData != NULL)
    {
        free((*pGraph)->privateData);
        (*pGraph)->privateData = NULL;
    }
    if ((*pGraph)->extensionLookupTable != NULL)
    {
        free((*pGraph)->extensionLookupTable);
        (*pGraph)->extensionLookupTable = NULL;
    }

    free(*pGraph);
    *pGraph = NULL;
}

/********************************************************************
 gp_CopyGraph()

 Copies the content of the srcGraph into the dstGraph.

 The dstGraph must have been previously initialized with the same
 number of vertices as the srcGraph.

 NOTE: If the dstGraph has a higher edge capacity than the srcGraph,
 then this call will fail unless the caller first ensures that the
 edge capacity of the srcGraph is increased to match the dstGraph.

 Returns OK for success, NOTOK for failure.
 ********************************************************************/

// Give macro names to three copy operations
#define _gp_CopyVertexRec(dstGraph, vdst, srcGraph, vsrc) (dstGraph->V[vdst] = srcGraph->V[vsrc])
#define _gp_CopyDFSUtilsVertexInfo(dstGraph, dstI, srcGraph, srcI) (theGraphDVI(dstGraph)[dstI] = theGraphDVI(srcGraph)[srcI])
#define _gp_CopyPlanarityVertexInfo(dstGraph, dstI, srcGraph, srcI) (theGraphPVI(dstGraph)[dstI] = theGraphPVI(srcGraph)[srcI])
#define _gp_CopyEdgeRec(dstGraph, edst, srcGraph, esrc) (dstGraph->E[edst] = srcGraph->E[esrc])

/*****************************************************************
 * _gp_FindEdge()
 *
 * Private version of the public method that performs the search
 * without preceding parameter validation checks. This is called
 * from other private methods of the graph library, to avoid
 * duplication of the effort of the checks performed by invoking
 * public methods.
 */
int _gp_FindEdge(graphP theGraph, int u, int v)
{
    int e = gp_GetFirstEdge(theGraph, u);
    while (gp_IsEdge(theGraph, e))
    {
        if (gp_GetNeighbor(theGraph, e) == v)
            return e;

        e = gp_GetNextEdge(theGraph, e);
    }
    return NIL;
}

/********************************************************************
 _AttachEdgeRecord()

 This routine adds newEdge into v's adjacency list at a position
 adjacent to the edge record for e, either before or after e,
 depending on link.  If e is not an edge (e.g. if e is NIL),
 then link is assumed to indicate whether the newEdge is to be
 placed at the beginning or end of v's adjacency list.

 NOTE: The caller can pass NIL for v if e is not NIL, since the
       vertex is implied (gp_GetNeighbor(theGraph, eTwin))

 The newEdge is assumed to already exist in the data structure (i.e.
 the storage of edges), as only a whole edge (both edge records) can
 be inserted into or deleted from the data structure.

 See also _RestoreEdgeRecord()
 ********************************************************************/

void _AttachEdgeRecord(graphP theGraph, int v, int e, int link, int newEdge)
{
    if (gp_IsEdge(theGraph, e))
    {
        int e2 = gp_GetAdjacentEdge(theGraph, e, link);

        // e's link is newEdge, and newEdge's 1^link is e
        gp_SetAdjacentEdge(theGraph, e, link, newEdge);
        gp_SetAdjacentEdge(theGraph, newEdge, 1 ^ link, e);

        // newEdge's link is e2
        gp_SetAdjacentEdge(theGraph, newEdge, link, e2);

        // if e2 is an edge, then e2's 1^link is newEdge,
        // else v's 1^link is newEdge
        if (gp_IsEdge(theGraph, e2))
            gp_SetAdjacentEdge(theGraph, e2, 1 ^ link, newEdge);
        else
            gp_SetEdgeByLink(theGraph, v, 1 ^ link, newEdge);
    }
    else
    {
        int e2 = gp_GetEdgeByLink(theGraph, v, link);

        // v's link is newEdge, and newEdge's 1^link is NIL
        gp_SetEdgeByLink(theGraph, v, link, newEdge);
        gp_SetAdjacentEdge(theGraph, newEdge, 1 ^ link, NIL);

        // newEdge's elink is e2
        gp_SetAdjacentEdge(theGraph, newEdge, link, e2);

        // if e2 is an edge, then e2's 1^link is newEdge,
        // else v's 1^link is newEdge
        if (gp_IsEdge(theGraph, e2))
            gp_SetAdjacentEdge(theGraph, e2, 1 ^ link, newEdge);
        else
            gp_SetEdgeByLink(theGraph, v, 1 ^ link, newEdge);
    }
}

/****************************************************************************
 _DetachEdge()

 This routine detaches edge record e from its adjacency list, but it does not
 delete it from the data structure (only a whole edge can be deleted).

 Some algorithms must temporarily detach an edge, perform some calculation,
 and eventually put the edge back. This routine supports that operation.
 The neighboring adjacency list nodes are cross-linked, but the two link
 members of edge record e are retained, so edge record e can be reattached
 later by invoking _RestoreEdgeRecord().

 A sequence of detached edge records can only be restored in the exact opposite
 order of their detachment. Thus, algorithms do not directly use this method to
 implement the temporary detach/restore method. Instead, gp_HideEdge() and
 gp_RestoreEdge() are used, and algorithms push and pop hidden edges onto and
 from a stack. A example of this is shown by detaching edges with
 gp_ContractEdge() or gp_IdentifyVertices(), and then reattaching them with
 gp_RestoreVertices(), which unwinds the stack with gp_RestoreVertex().
 ****************************************************************************/

void _DetachEdgeRecord(graphP theGraph, int e)
{
    int nextEdge = gp_GetNextEdge(theGraph, e),
        prevEdge = gp_GetPrevEdge(theGraph, e);

    if (gp_IsEdge(theGraph, nextEdge))
        gp_SetPrevEdge(theGraph, nextEdge, prevEdge);
    else
        gp_SetLastEdge(theGraph, gp_GetNeighbor(theGraph, gp_GetTwin(theGraph, e)), prevEdge);

    if (gp_IsEdge(theGraph, prevEdge))
        gp_SetNextEdge(theGraph, prevEdge, nextEdge);
    else
        gp_SetFirstEdge(theGraph, gp_GetNeighbor(theGraph, gp_GetTwin(theGraph, e)), nextEdge);
}

/********************************************************************
 gp_AddEdge()

 Adds the undirected edge (u,v) to the graph by placing edge records
 representing u into v's circular edge record list and v into u's
 circular edge record list.

 upos receives the location in G where the u record in v's list will be
 placed, and vpos is the location in G of the v record we placed in
 u's list.  These are used to initialize the short circuit links.

 ulink (0|1) indicates whether the edge record to v in u's list should
        become adjacent to u by its 0 or 1 link, i.e. u[ulink] == vpos.
 vlink (0|1) indicates whether the edge record to u in v's list should
        become adjacent to v by its 0 or 1 link, i.e. v[vlink] == upos.

 NOTE: Only the neighbor and link pointer data members are modified in
       the edge records. The edge records are otherwise assumed to be in
       initial state, either from graph initialization/reinitialization,
       or from edge record reinitialization during gp_DeleteEdge(), if
       the new edge is filling an edge hole in the edge array.This
       expectation of being in initial state includes data stored in
       parallel edge record extension arrays.

 NOTE: This method does not forbid the addition of duplicate and loop
       edges. Use with care because other API endpoints do not all
       support nor check for and eliminate duplicates and loops. The
       caller can guard against these conditions by pre-testing that
       u != v and that gp_FindEdge() returns NIL.

 Returns OK on success, NOTOK on failure (including when adding the
         edge would exceed the graph's edge capacity; the caller can
         use gp_DynamicAddEdge()).
 ********************************************************************/

int gp_AddEdge(graphP theGraph, int u, int ulink, int v, int vlink)
{
    if (theGraph == NULL ||
        u < gp_LowerBoundVertexStorage(theGraph) || v < gp_LowerBoundVertexStorage(theGraph) ||
        u >= gp_UpperBoundVertexStorage(theGraph) || v >= gp_UpperBoundVertexStorage(theGraph))
        return NOTOK;

    if ((ulink != 0 && ulink != 1) || (vlink != 0 && vlink != 1))
    {
        return NOTOK;
    }

    if (gp_InsertEdge(theGraph, u, NIL, ulink, v, NIL, vlink) != OK)
        return NOTOK;

    return OK;
}

/********************************************************************
 gp_InsertEdge()

 This function adds the edge (u, v) such that the edge record added
 to the adjacency list of u is adjacent to e_u and the edge record
 added to the adjacency list of v is adjacent to e_v.
 The direction of adjacency is given by e_ulink for e_u and e_vlink
 for e_v. Specifically, the new edge will be comprised of two edge
 records, n_u and n_v.  In u's (v's) adjacency list, n_u (n_v) will
 be added so that it is indicated by e_u's (e_v's) e_ulink (e_vlink).

 If e_u (or e_v) is not an edge, then e_ulink (e_vlink) indicates
 whether to prepend or append to the adjacency list for u (v).

 NOTE: See notes on gp_AddEdge().

 Returns OK on success, NOTOK on failure, or AT_EDGE_CAPACITY_LIMIT if
         adding the edge would exceed the graph's edge capacity (the
         caller can invoke gp_EnsureEdgeCapacity() beforehand to avoid
         an AT_EDGE_CAPACITY_LIMIT result).
 ********************************************************************/

int gp_InsertEdge(graphP theGraph, int u, int e_u, int e_ulink,
                  int v, int e_v, int e_vlink)
{
    int upos, vpos;

    if (theGraph == NULL)
        return NOTOK;

    if (u < gp_LowerBoundVertexStorage(theGraph) ||
        u >= gp_UpperBoundVertexStorage(theGraph) ||
        v < gp_LowerBoundVertexStorage(theGraph) ||
        v >= gp_UpperBoundVertexStorage(theGraph) ||
        (e_u < gp_LowerBoundEdges(theGraph) && gp_IsEdge(theGraph, e_u)) ||
        e_u >= gp_UpperBoundEdges(theGraph) ||
        (gp_IsEdge(theGraph, e_u) && gp_EdgeNotInUse(theGraph, e_u)) ||
        (e_v < gp_LowerBoundEdges(theGraph) && gp_IsEdge(theGraph, e_v)) ||
        e_v >= gp_UpperBoundEdges(theGraph) ||
        (gp_IsEdge(theGraph, e_v) && gp_EdgeNotInUse(theGraph, e_v)) ||
        e_ulink < 0 || e_ulink > 1 || e_vlink < 0 || e_vlink > 1)
        return NOTOK;


    if (sp_NonEmpty(theGraph->edgeHoles))
    {
        sp_Pop(theGraph->edgeHoles, vpos);
        theGraph->numEdgeHoles = sp_GetCurrentSize(theGraph->edgeHoles);
    }
    else if (gp_GetM(theGraph) >= theGraph->edgeCapacity)
        return AT_EDGE_CAPACITY_LIMIT;
    else
        vpos = gp_UpperBoundEdges(theGraph);

    // NOTE: We do not _InitEdgeRec() nor gp_InitEdgeFlags() here because
    // the vpos edge location is expected to be in initialized state,
    // either from graph initialization/reinitialization, or from
    // edge record reinitialization during gp_DeleteEdge, if vpos was
    // an edge hole. This expectation includes edge record extensions
    // in graph extensions.

    upos = gp_GetTwin(theGraph, vpos);

    gp_SetNeighbor(theGraph, upos, v);
    _AttachEdgeRecord(theGraph, u, e_u, e_ulink, upos);

    gp_SetNeighbor(theGraph, vpos, u);
    _AttachEdgeRecord(theGraph, v, e_v, e_vlink, vpos);

    theGraph->M++;

    gp_NoteModification(theGraph);

    return OK;
}

int _DeleteEdge(graphP theGraph, int e)
{
    // Delete the edge records e and eTwin from their adjacency lists.
    _DetachEdgeRecord(theGraph, e);
    _DetachEdgeRecord(theGraph, gp_GetTwin(theGraph, e));

    // Clear the two edge records
    // (the bit twiddle (e & ~1) chooses the lesser of e and its twin)
#ifdef USE_1BASEDARRAYS
    memset(theGraph->E + (e & ~1), NIL_CHAR, sizeof(edgeRec) << 1);
#else
    _InitEdgeRec(theGraph, e);
    _InitEdgeRec(theGraph, gp_GetTwin(theGraph, e));
#endif

    // Now we reduce the number of edges in the data structure
    theGraph->M--;

    // If records e and eTwin were not the last in the edge record array,
    // then record a new hole in the edge array.
    if (e < gp_UpperBoundEdges(theGraph))
    {
        if (theGraph->edgeHoles->size + 1 >= theGraph->edgeHoles->capacity)
            return NOTOK;

        sp_Push(theGraph->edgeHoles, e);
        theGraph->numEdgeHoles = sp_GetCurrentSize(theGraph->edgeHoles);
    }

    // Return the previously calculated successor of e.
    return OK;
}

/********************************************************************
 _RestoreEdgeRecord()

 This routine reinserts an edge record e into the adjacency list from
 which it was previously removed by _DetachEdgeRecord().

 The assumed processing model is that edge records will be restored in
 reverse of the order in which they were hidden, i.e. it is assumed
 that the hidden edges will be pushed on a stack from which they will
 be popped during restoration.
 ********************************************************************/
void _RestoreEdgeRecord(graphP theGraph, int e)
{
    int nextEdge = gp_GetNextEdge(theGraph, e),
        prevEdge = gp_GetPrevEdge(theGraph, e);

    if (gp_IsEdge(theGraph, nextEdge))
        gp_SetPrevEdge(theGraph, nextEdge, e);
    else
        gp_SetLastEdge(theGraph, gp_GetNeighbor(theGraph, gp_GetTwin(theGraph, e)), e);

    if (gp_IsEdge(theGraph, prevEdge))
        gp_SetNextEdge(theGraph, prevEdge, e);
    else
        gp_SetFirstEdge(theGraph, gp_GetNeighbor(theGraph, gp_GetTwin(theGraph, e)), e);
}

/********************************************************************
 gp_HideEdge()
 This routine removes the two edge records of an edge from the
 adjacency lists of its endpoint vertices, but it does not delete them
 from the storage data structure.

 Many algorithms must temporarily remove an edge, perform some
 calculation, and eventually put the edge back. This routine supports
 that operation.

 For each edge record of e, the neighboring adjacency list nodes are
 cross-linked, but the links in the edge record are retained because
 they indicate the neighbor edge records to which the edge record can
 be reattached by gp_RestoreEdge().
 ********************************************************************/

void gp_HideEdge(graphP theGraph, int e)
{
    if (theGraph == NULL ||
        e < gp_LowerBoundEdges(theGraph) || e >= gp_UpperBoundEdges(theGraph) ||
        gp_EdgeNotInUse(theGraph, e))
    {
#ifdef DEBUG
        NOTOK;
#endif
        return;
    }

    theGraph->functions->fpHideEdge(theGraph, e);

    gp_NoteModification(theGraph);
}

void _HideEdge(graphP theGraph, int e)
{
    _DetachEdgeRecord(theGraph, e);
    _DetachEdgeRecord(theGraph, gp_GetTwin(theGraph, e));
}

/********************************************************************
 gp_RestoreEdge()
 This routine reinserts two edge records of an edge into the adjacency
 lists of the edge's endpoints, the edge records having been previously
 removed by gp_HideEdge().

 The assumed processing model is that edges will be restored in
 reverse of the order in which they were hidden, i.e. it is assumed
 that the hidden edges will be pushed on a stack and the edges will
 be popped from the stack for restoration.

 NOTE: Since both edge records of an edge are restored, only one
       edge record needs to be  pushed on the stack for restoration.
       This routine restores the two edge records in the opposite order
       from the order in which they were hidden by gp_HideEdge().
 ********************************************************************/

void gp_RestoreEdge(graphP theGraph, int e)
{
    if (theGraph == NULL ||
        e < gp_LowerBoundEdges(theGraph) || e >= gp_UpperBoundEdges(theGraph) ||
        gp_EdgeNotInUse(theGraph, e))
    {
#ifdef DEBUG
        NOTOK;
#endif
        return;
    }

    theGraph->functions->fpRestoreEdge(theGraph, e);

    gp_NoteModification(theGraph);
}

void _RestoreEdge(graphP theGraph, int e)
{
    _RestoreEdgeRecord(theGraph, gp_GetTwin(theGraph, e));
    _RestoreEdgeRecord(theGraph, e);
}

/********************************************************************
 _RestoreHiddenEdges()

 Each entry on the stack, down to stackBottom, is assumed to be an
 edge record pushed in concert with invoking gp_HideEdge().
 Each edge is restored using gp_RestoreEdge() in exact reverse of the
 hiding order. The stack is reduced in content size to stackBottom.

 Returns OK on success, NOTOK on internal failure.
 ********************************************************************/

int _RestoreHiddenEdges(graphP theGraph, int stackBottom)
{
    int e;

    while (sp_GetCurrentSize(theGraph->theStack) > stackBottom)
    {
        sp_Pop(theGraph->theStack, e);
        if (gp_IsNotEdge(theGraph, e))
            return NOTOK;
        gp_RestoreEdge(theGraph, e);
    }

    return OK;
}

int _HideVertex(graphP theGraph, int vertex)
{
    int hiddenEdgeStackBottom = sp_GetCurrentSize(theGraph->theStack);
    int e = gp_GetFirstEdge(theGraph, vertex);

    // Cycle through all the edges, pushing and hiding each
    while (gp_IsEdge(theGraph, e))
    {
        if (sp_GetCurrentSize(theGraph->theStack) >= sp_GetCapacity(theGraph->theStack))
        {
            gp_ErrorMessage("_HideVertex() is attempting to push to a full stack.");
            return NOTOK;
        }

        sp_Push(theGraph->theStack, e);
        gp_HideEdge(theGraph, e);
        e = gp_GetNextEdge(theGraph, e);
    }

    // Push the additional integers needed by gp_RestoreVertex()
    if (sp_GetCurrentSize(theGraph->theStack) + 7 > sp_GetCapacity(theGraph->theStack))
    {
        gp_ErrorMessage("_HideVertex() is attempting to push to a full stack.");
        return NOTOK;
    }

    sp_Push(theGraph->theStack, hiddenEdgeStackBottom);
    sp_Push(theGraph->theStack, NIL);
    sp_Push(theGraph->theStack, NIL);
    sp_Push(theGraph->theStack, NIL);
    sp_Push(theGraph->theStack, NIL);
    sp_Push(theGraph->theStack, NIL);
    sp_Push(theGraph->theStack, vertex);

    return OK;
}

/********************************************************************
 gp_ContractEdge()

 Contracts the edge e=(u,v).  This hides the edge (both e and its
 twin edge record), and it also identifies vertex v with u.
 See gp_IdentifyVertices() for further details.

 Returns OK for success, NOTOK for internal failure.
 ********************************************************************/

int gp_ContractEdge(graphP theGraph, int e)
{
    if (theGraph == NULL ||
        e < gp_LowerBoundEdges(theGraph) || e >= gp_UpperBoundEdges(theGraph) ||
        gp_EdgeNotInUse(theGraph, e))
    {
        return NOTOK;
    }

    return theGraph->functions->fpContractEdge(theGraph, e);
}

int _ContractEdge(graphP theGraph, int e)
{
    int eBefore, u, v;

    if (gp_IsNotEdge(theGraph, e))
        return NOTOK;

    u = gp_GetNeighbor(theGraph, gp_GetTwin(theGraph, e));
    v = gp_GetNeighbor(theGraph, e);

    eBefore = gp_GetNextEdge(theGraph, e);

    if (sp_GetCurrentSize(theGraph->theStack) >= sp_GetCapacity(theGraph->theStack))
    {
        gp_ErrorMessage("_ContractEdge() is attempting to push to a full stack.");
        return NOTOK;
    }

    sp_Push(theGraph->theStack, e);
    gp_HideEdge(theGraph, e);

    return gp_IdentifyVertices(theGraph, u, v, eBefore);
}

/********************************************************************
 gp_IdentifyVertices()

 Identifies vertex v with vertex u by transferring all adjacencies
 of v to u.  Any duplicate edges are removed as described below.
 The non-duplicate edges of v are added to the adjacency list of u
 without disturbing their relative order, and they are added before
 the edge record eBefore in u's list. If eBefore is NIL, then the
 edges are simply appended to u's list.

 If u and v are adjacent, then gp_HideEdge() is invoked to remove
 the edge e=(u,v). Then, the edges of v that indicate neighbors of
 u are also hidden.  This is done by setting the visited flags of
 u's neighbors, then traversing the adjacency list of v.  For each
 visited neighbor of v, the edge is hidden because it would duplicate
 an adjacency already expressed in u's list. Finally, the remaining
 edges of v are moved to u's list, and each twin edge record is
 adjusted to indicate u as a neighbor rather than v.

 This routine assumes that the visited flags are clear beforehand,
 and visited flag settings made herein are cleared before returning.

 The following are pushed, in order, onto the graph's built-in stack:
 1) an integer for each hidden edge
 2) the stack size before any hidden edges were pushed
 3) six integers that indicate u, v and the edges moved from v to u

 An algorithm that identifies a series of vertices, either through
 directly calling this method or via gp_ContractEdge(), can unwind
 the identifications using gp_RestoreVertices(), which
 invokes gp_RestoreVertex() repeatedly.

 Returns OK on success, NOTOK on internal failure
 ********************************************************************/

int gp_IdentifyVertices(graphP theGraph, int u, int v, int eBefore)
{
    if (theGraph == NULL ||
        u < gp_LowerBoundVertexStorage(theGraph) || u >= gp_UpperBoundVertexStorage(theGraph) ||
        v < gp_LowerBoundVertexStorage(theGraph) || v >= gp_UpperBoundVertexStorage(theGraph) ||
        (eBefore != NIL && eBefore < gp_LowerBoundEdges(theGraph)) ||
        eBefore >= gp_UpperBoundEdges(theGraph) ||
        (eBefore != NIL && gp_EdgeNotInUse(theGraph, eBefore)))
    {
        return NOTOK;
    }

    // Noted before the call, since a failure part way through has already
    // changed the graph
    gp_NoteModification(theGraph);

    return theGraph->functions->fpIdentifyVertices(theGraph, u, v, eBefore);
}

int _IdentifyVertices(graphP theGraph, int u, int v, int eBefore)
{
    int e = _gp_FindEdge(theGraph, u, v);
    int hiddenEdgeStackBottom, eBeforePred;

    // If the vertices are adjacent, then the identification is
    // essentially an edge contraction with a bit of fixup.
    if (gp_IsEdge(theGraph, e))
    {
        int result = gp_ContractEdge(theGraph, e);
        int hiddenEdgesStackBottomIndex;
        int hiddenEdgesStackBottomValue;

        if (result != OK)
            return result;

        // The edge contraction operation pushes one hidden edge then
        // recursively calls this method. This method then pushes K
        // hidden edges then an integer indicating where the top of
        // stack was before the edges were hidden. That integer
        // indicator must be decremented, thereby incrementing the
        // number of hidden edges to K+1.
        // After pushing the K hidden edges and the stackBottom of
        // the hidden edges, the recursive call to this method pushes
        // six more integers to indicate edges that were moved from
        // v to u, so the "hidden edges stackBottom" is in the next
        // position down.
        hiddenEdgesStackBottomIndex = sp_GetCurrentSize(theGraph->theStack) - 7;
        hiddenEdgesStackBottomValue = sp_Get(theGraph->theStack, hiddenEdgesStackBottomIndex);

        sp_Set(theGraph->theStack, hiddenEdgesStackBottomIndex, hiddenEdgesStackBottomValue - 1);

        return result;
    }

    // Now, u and v are not adjacent. Before we do any edge hiding or
    // moving, we record the current stack size, as this is the
    // stackBottom for the edges that will be hidden next.
    hiddenEdgeStackBottom = sp_GetCurrentSize(theGraph->theStack);

    // Mark as visited all neighbors of u
    e = gp_GetFirstEdge(theGraph, u);
    while (gp_IsEdge(theGraph, e))
    {
        if (gp_GetVisited(theGraph, gp_GetNeighbor(theGraph, e)))
            return NOTOK;

        gp_SetVisited(theGraph, gp_GetNeighbor(theGraph, e));
        e = gp_GetNextEdge(theGraph, e);
    }

    // For each edge record of v, if the neighbor is visited, then
    // push and hide the edge.
    e = gp_GetFirstEdge(theGraph, v);
    while (gp_IsEdge(theGraph, e))
    {
        if (gp_GetVisited(theGraph, gp_GetNeighbor(theGraph, e)))
        {
            if (sp_GetCurrentSize(theGraph->theStack) >= sp_GetCapacity(theGraph->theStack))
            {
                gp_ErrorMessage("_IdentifyVertices() is attempting to push to a full stack.");
                e = gp_GetFirstEdge(theGraph, u);
                while (gp_IsEdge(theGraph, e))
                {
                    gp_ClearVisited(theGraph, gp_GetNeighbor(theGraph, e));
                    e = gp_GetNextEdge(theGraph, e);
                }
                return NOTOK;
            }

            sp_Push(theGraph->theStack, e);
            gp_HideEdge(theGraph, e);
        }
        e = gp_GetNextEdge(theGraph, e);
    }

    // Mark as unvisited all neighbors of u
    e = gp_GetFirstEdge(theGraph, u);
    while (gp_IsEdge(theGraph, e))
    {
        gp_ClearVisited(theGraph, gp_GetNeighbor(theGraph, e));
        e = gp_GetNextEdge(theGraph, e);
    }

    // Push the hiddenEdgeStackBottom as a record of how many hidden
    // edges were pushed (also, see above for Contract Edge adjustment)
    if (sp_GetCurrentSize(theGraph->theStack) + 7 > sp_GetCapacity(theGraph->theStack))
    {
        gp_ErrorMessage("_IdentifyVertices() is attempting to push to a full stack.");
        return NOTOK;
    }

    sp_Push(theGraph->theStack, hiddenEdgeStackBottom);

    // Moving v's adjacency list to u is aided by knowing the predecessor
    // of u's eBefore (the edge record in u's list before which the
    // edge records of v will be added).
    eBeforePred = gp_IsEdge(theGraph, eBefore)
                      ? gp_GetPrevEdge(theGraph, eBefore)
                      : gp_GetLastEdge(theGraph, u);

    // Turns out we only need to record six integers related to the edges
    // being moved in order to easily restore them later.
    sp_Push(theGraph->theStack, eBefore);
    sp_Push(theGraph->theStack, gp_GetLastEdge(theGraph, v));
    sp_Push(theGraph->theStack, gp_GetFirstEdge(theGraph, v));
    sp_Push(theGraph->theStack, eBeforePred);
    sp_Push(theGraph->theStack, u);
    sp_Push(theGraph->theStack, v);

    // For the remaining edge records of v, reassign the 'v' member
    //    of each twin edge record to indicate u rather than v.
    e = gp_GetFirstEdge(theGraph, v);
    while (gp_IsEdge(theGraph, e))
    {
        gp_SetNeighbor(theGraph, gp_GetTwin(theGraph, e), u);
        e = gp_GetNextEdge(theGraph, e);
    }

    // If v has any edges left after hiding edges, indicating common neighbors with u, ...
    if (gp_IsEdge(theGraph, gp_GetFirstEdge(theGraph, v)))
    {
        // Then perform the list union of v into u between eBeforePred and eBefore
        if (gp_IsEdge(theGraph, eBeforePred))
        {
            if (gp_IsEdge(theGraph, gp_GetFirstEdge(theGraph, v)))
            {
                gp_SetNextEdge(theGraph, eBeforePred, gp_GetFirstEdge(theGraph, v));
                gp_SetPrevEdge(theGraph, gp_GetFirstEdge(theGraph, v), eBeforePred);
            }
        }
        else
        {
            gp_SetFirstEdge(theGraph, u, gp_GetFirstEdge(theGraph, v));
        }

        if (gp_IsEdge(theGraph, eBefore))
        {
            if (gp_IsEdge(theGraph, gp_GetLastEdge(theGraph, v)))
            {
                gp_SetNextEdge(theGraph, gp_GetLastEdge(theGraph, v), eBefore);
                gp_SetPrevEdge(theGraph, eBefore, gp_GetLastEdge(theGraph, v));
            }
        }
        else
        {
            gp_SetLastEdge(theGraph, u, gp_GetLastEdge(theGraph, v));
        }

        gp_SetFirstEdge(theGraph, v, NIL);
        gp_SetLastEdge(theGraph, v, NIL);
    }

    return OK;
}

int _RestoreVertex(graphP theGraph)
{
    int u, v, e_u_succ, e_u_pred, e_v_first, e_v_last, HESB, e;

    if (sp_GetCurrentSize(theGraph->theStack) < 7)
    {
        gp_ErrorMessage("_RestoreVertex() is attempting to pop from an empty stack.");
        return NOTOK;
    }

    sp_Pop(theGraph->theStack, v);
    sp_Pop(theGraph->theStack, u);
    sp_Pop(theGraph->theStack, e_u_pred);
    sp_Pop(theGraph->theStack, e_v_first);
    sp_Pop(theGraph->theStack, e_v_last);
    sp_Pop(theGraph->theStack, e_u_succ);

    // If u is not NIL, then vertex v was identified with u.  Otherwise, v was
    // simply hidden, so we skip to restoring the hidden edges.
    if (gp_IsVertex(theGraph, u))
    {
        // Remove v's adjacency list from u, including accounting for degree 0 case
        if (gp_IsEdge(theGraph, e_u_pred))
        {
            gp_SetNextEdge(theGraph, e_u_pred, e_u_succ);
            // If the successor edge exists, link it to the predecessor,
            // otherwise the predecessor is the new last edge
            if (gp_IsEdge(theGraph, e_u_succ))
                gp_SetPrevEdge(theGraph, e_u_succ, e_u_pred);
            else
                gp_SetLastEdge(theGraph, u, e_u_pred);
        }
        else if (gp_IsEdge(theGraph, e_u_succ))
        {
            // The successor edge exists, but not the predecessor,
            // so the successor is the new first edge
            gp_SetPrevEdge(theGraph, e_u_succ, NIL);
            gp_SetFirstEdge(theGraph, u, e_u_succ);
        }
        else
        {
            // Just in case u was degree zero
            gp_SetFirstEdge(theGraph, u, NIL);
            gp_SetLastEdge(theGraph, u, NIL);
        }

        // Place v's adjacency list into v, including accounting for degree 0 case
        gp_SetFirstEdge(theGraph, v, e_v_first);
        gp_SetLastEdge(theGraph, v, e_v_last);
        if (gp_IsEdge(theGraph, e_v_first))
            gp_SetPrevEdge(theGraph, e_v_first, NIL);
        if (gp_IsEdge(theGraph, e_v_last))
            gp_SetNextEdge(theGraph, e_v_last, NIL);

        // For each edge record restored to v's adjacency list, reassign the 'v' member
        //    of each twin edge record to indicate v rather than u.
        e = e_v_first;
        while (gp_IsEdge(theGraph, e))
        {
            gp_SetNeighbor(theGraph, gp_GetTwin(theGraph, e), v);
            e = (e == e_v_last ? NIL : gp_GetNextEdge(theGraph, e));
        }
    }

    // Restore the hidden edges of v, if any
    if (sp_IsEmpty(theGraph->theStack))
    {
        gp_ErrorMessage("_RestoreVertex() is attempting to pop from an empty stack.");
        return NOTOK;
    }

    sp_Pop(theGraph->theStack, HESB);
    return _RestoreHiddenEdges(theGraph, HESB);
}

// ===== graphDFSUtils.c =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#define GRAPHDFSUTILS_C


// For LOGGING-related declarations

// Allows the default _SortVertices() to swap planarity vertex info, if present

// Private methods, except exported within library
int _SortVertices(graphP theGraph);

// Imported methods
extern void _ClearVertexVisitedFlags(graphP theGraph, int includeVirtualVertices);
extern int _FillVertexVisitedIndexes(graphP theGraph, int FillValue);

/********************************************************************
 gp_ExtendWith_DFSUtils()

 Makes any necessary preparations for supporting DFS utility methods
 that create a DFS tree, sort vertices, and compute least ancestor.
 and lowpoint values. Those four utility methods automatically call
 this method to extend the graph, though this method can also be
 called beforehand.

 This method should be called after gp_EnsureVertexCapacity() or
 gp_Read() because the number of vertices must be known.

 On success, sets GRAPHFLAGS_EXTENDEDWITH_DFSUTILS.

 Returns OK on success, NOTOK on failure.
 ********************************************************************/

int gp_ExtendWith_DFSUtils(graphP theGraph)
{
    if (theGraph == NULL)
        return NOTOK;

    // if the Graph has already been extended with DFS Utils,
    // then just return successfully
    if (gp_GetGraphFlags(theGraph) & GRAPHFLAGS_EXTENDEDWITH_DFSUTILS)
        return OK;

    // Allocate supporting data structures as needed

    // Perform "on success" operations
    theGraph->graphFlags |= GRAPHFLAGS_EXTENDEDWITH_DFSUTILS;
    return OK;
}

/********************************************************************
 gp_Detach_DFSUtils()

 This function is intended to disinherit the DFS Utils feature by
 removing the extension from the graph, which also frees any
 DFS-specific data structures.

 Clears GRAPHFLAGS_EXTENDEDWITH_DFSUTILS after detaching support for
 the DFS utility methods.

 Returns OK for success, NOTOK for failure
 ********************************************************************/

int gp_Detach_DFSUtils(graphP theGraph)
{
    // Free any data structures allocated by the ExtendWith function

    // Indicate successful detachment of DFSUtils
    theGraph->graphFlags &= ~GRAPHFLAGS_EXTENDEDWITH_DFSUTILS;
    return OK;
}

/********************************************************************
 gp_DepthFirstSearch()

 This depth-first search (DFS) assigns a Depth First Index (DFI) to
 each vertex and records the DFS parent of each vertex in each DFS tree
 that forms during the depth-first search. Also, the type of each
 edge record of each edge is set to indicate whether the edge record's
 neighbor value points to a DFS child or parent (a DFS tree edge) or
 a farther DFS ancestor or descendant (the backward and forward
 edge records of a "back" edge/"cycle" edge/"co-tree" edge).

 NOTE: This is a utility function provided for general use. The core
        planarity algorithm uses its own DFS so it can build related
        data structures at the same time.
 ********************************************************************/

int gp_DepthFirstSearch(graphP theGraph)
{
    stackP theStack;
    int DFI, v, uparent, u, e;

    if (theGraph == NULL)
        return NOTOK;
        
    if (theGraph->graphFlags & GRAPHFLAGS_PARALLELEDGEDETECTED)
    {
        gp_ErrorMessage("Parallel edges were previously added to the graph. See gp_DeleteParallelEdges().");
        return NOTOK;
    }

    if (gp_GetGraphFlags(theGraph) & GRAPHFLAGS_DFSNUMBERED)
        return OK;

    if (gp_ExtendWith_DFSUtils(theGraph) != OK)
        return NOTOK;

    _gp_LogLine("\ngraphDFSUtils.c/gp_DepthFirstSearch() start");

    theStack = theGraph->theStack;

    /* There are 2M edge records and for each we can push 2 integers,
        plus one extra (NIL, NIL) at the beginning to represent
        arriving at a DFS tree root. So, a stack of 2 * 2 * (1+M)
        integers suffices.
        This stack is already in theGraph structure, so we make sure
        it has the capacity and, if so, that it's empty. */

    if (sp_GetCapacity(theStack) < 2 * 2 * gp_GetM(theGraph) + 2)
        return NOTOK;

    sp_ClearStack(theStack);

    /* Clear the visited flags because they are used to detect what has
        been visited as the DFS traverses the graph. */
    _ClearVertexVisitedFlags(theGraph, FALSE);

    /* This outer loop causes the connected subgraphs of a disconnected
            graph to be numbered */

    for (DFI = v = gp_LowerBoundVertices(theGraph); v < gp_UpperBoundVertices(theGraph); ++v)
    {
        if (_gp_IsNotDFSTreeRoot(theGraph, v))
            continue;

        sp_Push2(theStack, NIL, NIL);
        while (sp_NonEmpty(theStack))
        {
            sp_Pop2(theStack, uparent, e);
            u = gp_IsNotVertex(theGraph, uparent) ? v : gp_GetNeighbor(theGraph, e);

            if (!gp_GetVisited(theGraph, u))
            {
                _gp_LogLine(_gp_MakeLogStr3("V=%d, DFI=%d, Parent=%d", u, DFI, uparent));

                gp_SetVisited(theGraph, u);
                gp_SetIndex(theGraph, u, DFI++);
                gp_SetVertexParent(theGraph, u, uparent);
                if (gp_IsEdge(theGraph, e))
                {
                    gp_SetEdgeType(theGraph, e, EDGE_TYPE_CHILD);
                    gp_SetEdgeType(theGraph, gp_GetTwin(theGraph, e), EDGE_TYPE_PARENT);
                }

                /* Push edges to all unvisited neighbors. These will be either
                      tree edges to children or forward edge records of back edges */

                e = gp_GetFirstEdge(theGraph, u);
                while (gp_IsEdge(theGraph, e))
                {
                    if (!gp_GetVisited(theGraph, gp_GetNeighbor(theGraph, e)))
                        sp_Push2(theStack, u, e);
                    e = gp_GetNextEdge(theGraph, e);
                }
            }
            else
            {
                // If the edge leads to a visited vertex, then it is
                // the forward component of a back edge.
                gp_SetEdgeType(theGraph, e, EDGE_TYPE_FORWARD);
                gp_SetEdgeType(theGraph, gp_GetTwin(theGraph, e), EDGE_TYPE_BACK);
            }
        }
    }

    _gp_LogLine("graphDFSUtils.c/gp_DepthFirstSearch() end\n");

    theGraph->graphFlags |= GRAPHFLAGS_DFSNUMBERED;

    if (gp_GetGraphFlags(theGraph) & GRAPHFLAGS_DFSNUMBERED_DIRECTED)
    {
        theGraph->graphFlags &= ~GRAPHFLAGS_DFSNUMBERED_DIRECTED;
        if (_FillVertexVisitedIndexes(theGraph, 0) != OK)
            return NOTOK;
    }

    return OK;
}

/********************************************************************
 gp_SortVertices()

 Once depth first numbering has been applied to the graph, the index
 member of each vertex contains the DFI.  This routine can reorder the
 vertices in linear time so that they appear in ascending order by DFI.
 Note that the index field is then used to store the original number
 of the vertex. Therefore, a second call to this method will put the
 vertices back to the original order and put the DFIs back into the
 index fields of the vertices.

 NOTE: This function is used by the core planarity algorithm, once its
 custom DFS has assigned DFIs to the vertices.  Once gp_Embed() has
 finished creating an embedding or obstructing subgraph, this function
 can be called to restore the original vertex numbering, if needed.
 ********************************************************************/

int gp_SortVertices(graphP theGraph)
{
    if (theGraph == NULL)
        return NOTOK;

    if (gp_ExtendWith_DFSUtils(theGraph) != OK)
        return NOTOK;

    // Noted before the call, since a failure part way through has already
    // changed the graph
    gp_NoteModification(theGraph);

    return theGraph->functions->fpSortVertices(theGraph);
}

// Give macro names to swap operations used when sorting vertices
// These are macros and hence not overloadable. If an extension
// needs to reorder parallel vertex data, then this must be done
// by a post-processing step in an overload of gp_SortVertices().
// The index values of the first N vertices are changed to hold
// the prior locations of vertices when they are rearranged to
// or from DFI order.
#define _gp_SwapVertexRec(dstGraph, vdst, srcGraph, vsrc) \
    {                                                     \
        vertexRec tempV = dstGraph->V[vdst];              \
        dstGraph->V[vdst] = srcGraph->V[vsrc];            \
        srcGraph->V[vsrc] = tempV;                        \
    }
#define _gp_SwapDFSUtilsVertexInfo(dstGraph, dstPos, srcGraph, srcPos) \
    {                                                                  \
        DFSUtils_VertexInfo tempDVI = theGraphDVI(dstGraph)[dstPos];   \
        theGraphDVI(dstGraph)[dstPos] = theGraphDVI(srcGraph)[srcPos]; \
        theGraphDVI(srcGraph)[srcPos] = tempDVI;                       \
    }
#define _gp_SwapPlanarityVertexInfo(dstGraph, dstPos, srcGraph, srcPos) \
    if (theGraphPVI(dstGraph) != NULL && theGraphPVI(srcGraph) != NULL) \
    {                                                                   \
        Planarity_VertexInfo tempPVI = theGraphPVI(dstGraph)[dstPos];   \
        theGraphPVI(dstGraph)[dstPos] = theGraphPVI(srcGraph)[srcPos];  \
        theGraphPVI(srcGraph)[srcPos] = tempPVI;                        \
    }

// This is the default method for sorting vertices into and back
// out of DFI order.
int _SortVertices(graphP theGraph)
{
    int v, srcPos, dstPos;

    if (theGraph == NULL)
        return NOTOK;

    if (!(gp_GetGraphFlags(theGraph) & GRAPHFLAGS_DFSNUMBERED))
        if (gp_DepthFirstSearch(theGraph) != OK)
            return NOTOK;

    _gp_LogLine("\ngraphDFSUtils.c/_SortVertices() start");

    /* Change labels of edges from v to DFI(v)-- or vice versa
       Also, if any links go back to locations 0 to n-1, then they
       need to be changed because we are reordering the vertices */

    if (theGraph->numEdgeHoles == 0)
    {
        // Slightly optimized loop body, for when edge deletion has not been used
        // (Optimization level O1 or higher hoists the upperBoundEdges calculation,
        //  so this is mainly just a little less work in the loop body).
        int upperBoundEdges = gp_LowerBoundEdges(theGraph) + (gp_GetM(theGraph) << 1);
        for (int e = gp_LowerBoundEdges(theGraph); e < upperBoundEdges; ++e)
            gp_SetNeighbor(theGraph, e, gp_GetIndex(theGraph, gp_GetNeighbor(theGraph, e)));
    }
    else
    {
        for (int e = gp_LowerBoundEdges(theGraph); e < gp_UpperBoundEdges(theGraph); e += 2)
        {
            if (gp_EdgeInUse(theGraph, e))
            {
                gp_SetNeighbor(theGraph, e, gp_GetIndex(theGraph, gp_GetNeighbor(theGraph, e)));
                gp_SetNeighbor(theGraph, e + 1, gp_GetIndex(theGraph, gp_GetNeighbor(theGraph, e + 1)));
            }
        }
    }

    /* Convert DFSParent from v to DFI(v) or vice versa */

    for (v = gp_LowerBoundVertices(theGraph); v < gp_UpperBoundVertices(theGraph); ++v)
        if (_gp_IsNotDFSTreeRoot(theGraph, v))
            gp_SetVertexParent(theGraph, v, gp_GetIndex(theGraph, gp_GetVertexParent(theGraph, v)));

    /* Sort by 'v using constant time random access. Move each vertex to its
       destination 'v', and store its source location in 'v'. */

    /* First we clear the visitation flags.  We need these to help mark
       visited vertices because we change the 'v' field to be the source
       location, so we cannot use index==v as a test for whether the
       correct vertex is in location 'index'. */

    _ClearVertexVisitedFlags(theGraph, FALSE);

    /* We visit each vertex location, skipping those marked as visited since
       we've already moved the correct vertex into that location. The
       inner loop swaps the vertex at location v into the correct position,
       given by the index of the vertex at location v.  Then it marks that
       location as visited, then sets its index to be the location from
       whence we obtained the vertex record. */

    for (v = gp_LowerBoundVertices(theGraph); v < gp_UpperBoundVertices(theGraph); ++v)
    {
        srcPos = v;
        while (!gp_GetVisited(theGraph, v))
        {
            dstPos = gp_GetIndex(theGraph, v);

            _gp_SwapVertexRec(theGraph, dstPos, theGraph, v);
            _gp_SwapDFSUtilsVertexInfo(theGraph, dstPos, theGraph, v);
            _gp_SwapPlanarityVertexInfo(theGraph, dstPos, theGraph, v);

            gp_SetVisited(theGraph, dstPos);
            gp_SetIndex(theGraph, dstPos, srcPos);

            srcPos = dstPos;
        }
    }

    /* Invert the bit that records the sort order of the graph */

    theGraph->graphFlags &= ~GRAPHFLAGS_LOWPOINTSCOMPUTED;
    theGraph->graphFlags ^= GRAPHFLAGS_SORTEDBYDFI;

    _gp_LogLine("graphDFSUtils.c/_SortVertices() end\n");

    return OK;
}

/********************************************************************
 gp_ComputeLowpoints()

        leastAncestor(v): min(v, ancestor neighbors of v, excluding parent)
        Lowpoint(v): min(leastAncestor(v), Lowpoint of DFS children of v)

 The Lowpoint of each vertex is computed via a post-order traversal of the
 DFS tree. Lowpoint calculations require leastAncestor calculations, so
 both are computed by this method.

 We push the root of the DFS tree, then we loop while the stack is not empty.
 We pop a vertex; if it is not marked, then we are on our way down the DFS
 tree, so we mark it and push it back on, followed by pushing its
 DFS children.  The next time we pop the node, all of its children
 will have been popped, marked+children pushed, and popped again.  On
 the second pop of the vertex, we can therefore compute the lowpoint
 values based on the childrens' lowpoints and the least ancestor from
 among the edges in the vertex's adjacency list.

 If they have not already been performed, gp_DepthFirstSearch() and
 gp_SortVertices() are invoked on the graph, and it is left in the
 sorted state on completion of this method.

 NOTE: This is a utility function provided for general use of the graph
       library. The core planarity algorithm computes leastAncestor during
       its initial DFS, and it computes the lowpoint of each a vertex as
       it embeds the tree edges to its children.
 ********************************************************************/

int gp_ComputeLowpoints(graphP theGraph)
{
    stackP theStack = NULL;
    int v, u, uneighbor, e, L, leastAncestor;

    if (theGraph == NULL)
        return NOTOK;

    if (theGraph->graphFlags & GRAPHFLAGS_PARALLELEDGEDETECTED)
    {
        gp_ErrorMessage("Parallel edges were previously added to the graph. See gp_DeleteParallelEdges().");
        return NOTOK;
    }

    if (gp_GetGraphFlags(theGraph) & GRAPHFLAGS_DIRECTEDEDGEDETECTED)
    {
        gp_ErrorMessage("gp_ComputeLowpoints() does not support directed graphs.");
        return NOTOK;
    }

    if (gp_ExtendWith_DFSUtils(theGraph) != OK)
        return NOTOK;

    theStack = theGraph->theStack;

    if (!(gp_GetGraphFlags(theGraph) & GRAPHFLAGS_DFSNUMBERED))
        if (gp_DepthFirstSearch(theGraph) != OK)
            return NOTOK;

    if (!(gp_GetGraphFlags(theGraph) & GRAPHFLAGS_SORTEDBYDFI))
        if (gp_SortVertices(theGraph) != OK)
            return NOTOK;

    _gp_LogLine("\ngraphDFSUtils.c/gp_ComputeLowpoints() start");

    // A stack of size N suffices because at maximum every vertex is pushed only once
    // However, since a larger stack is needed for the main DFS, this is really
    // just 'documentation' of the requirement
    if (sp_GetCapacity(theStack) < gp_GetN(theGraph))
        return NOTOK;

    sp_ClearStack(theStack);

    _ClearVertexVisitedFlags(theGraph, FALSE);

    // This outer loop causes the connected subgraphs of a disconnected graph to be processed
    for (v = gp_LowerBoundVertices(theGraph); v < gp_UpperBoundVertices(theGraph);)
    {
        if (gp_GetVisited(theGraph, v))
        {
            ++v;
            continue;
        }

        sp_Push(theStack, v);
        while (sp_NonEmpty(theStack))
        {
            sp_Pop(theStack, u);

            // If not visited, then we're on the pre-order visitation, so push u and its DFS children
            if (!gp_GetVisited(theGraph, u))
            {
                // Mark u as visited, then push it back on the stack
                gp_SetVisited(theGraph, u);
                ++v;
                sp_Push(theStack, u);

                // Push the DFS children of u
                e = gp_GetFirstEdge(theGraph, u);
                while (gp_IsEdge(theGraph, e))
                {
                    if (gp_GetEdgeType(theGraph, e) == EDGE_TYPE_CHILD)
                    {
                        sp_Push(theStack, gp_GetNeighbor(theGraph, e));
                    }

                    e = gp_GetNextEdge(theGraph, e);
                }
            }

            // If u has been visited before, then this is the post-order visitation
            else
            {
                // Start with high values because we are doing a min function
                leastAncestor = L = u;

                // Compute leastAncestor and L, the least lowpoint from the DFS children
                e = gp_GetFirstEdge(theGraph, u);
                while (gp_IsEdge(theGraph, e))
                {
                    uneighbor = gp_GetNeighbor(theGraph, e);
                    if (gp_GetEdgeType(theGraph, e) == EDGE_TYPE_CHILD)
                    {
                        if (L > gp_GetVertexLowpoint(theGraph, uneighbor))
                            L = gp_GetVertexLowpoint(theGraph, uneighbor);
                    }
                    else if (gp_GetEdgeType(theGraph, e) == EDGE_TYPE_BACK)
                    {
                        if (leastAncestor > uneighbor)
                            leastAncestor = uneighbor;
                    }

                    e = gp_GetNextEdge(theGraph, e);
                }

                /* Assign leastAncestor and Lowpoint to the vertex */
                gp_SetVertexLeastAncestor(theGraph, u, leastAncestor);
                gp_SetVertexLowpoint(theGraph, u, leastAncestor < L ? leastAncestor : L);
            }
        }
    }

    _gp_LogLine("graphDFSUtils.c/gp_ComputeLowpoints() end\n");

    theGraph->graphFlags |= GRAPHFLAGS_LOWPOINTSCOMPUTED;

    return OK;
}

// ===== planarityRelated/graphPlanarity_Extensions.c =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/


#include <stdlib.h>

/****************************************************************************
 gp_ExtendWith_Planarity()

 This function is intended to subclass a DFSUtils Graph by extending it with
 the planar graph embedding and obstruction isolation capabilities and any
 additional required data structures. If the given graph has not already
 been extended with DFSUtils, then gp_ExtendWith_DFSUtils() is called.

 To use Planarity during gp_Embed(), use EMBEDFLAGS_PLANAR.

 Returns OK for success, NOTOK for failure.
 ****************************************************************************/

int gp_ExtendWith_Planarity(graphP theGraph)
{
    if (theGraph == NULL)
        return NOTOK;

    // If the Graph has already been extended with Planarity,
    // then just return successfully
    if (gp_GetGraphFlags(theGraph) & GRAPHFLAGS_EXTENDEDWITH_PLANARITY)
        return OK;

    // Ensure theGraph is a DFSUtils Graph
    if (gp_ExtendWith_DFSUtils(theGraph) != OK)
        return NOTOK;

    // Allocate supporting data structures as needed

    // Perform "on success" operations
    theGraph->graphFlags |= GRAPHFLAGS_EXTENDEDWITH_PLANARITY;
    return OK;
}

/********************************************************************
 gp_Detach_Planarity()

 This function is intended to disinherit the planar graph embedding and
 obstruction isolation feature by remove the extension from the graph,
 which also frees any planarity-specific data structures.

 Clears GRAPHFLAGS_EXTENDEDWITH_PLANARITY after detaching support
 for Planarity.

 Returns OK on success, NOTOK on failure
 ********************************************************************/

int gp_Detach_Planarity(graphP theGraph)
{
    // Free any data structures allocated by the ExtendWith function

    // Indicate successful detachment of Planarity
    theGraph->graphFlags &= ~GRAPHFLAGS_EXTENDEDWITH_PLANARITY;
    return OK;
}

// ===== planarityRelated/graphEmbed.c =====
/*
Copyright (c) 1997-2026, John M. Boyer
All rights reserved.
See the LICENSE.TXT file for licensing information.
*/

#include <stdlib.h>

// This source file implements the main graph planarity/outerplanarity method, gp_Embed()

// Includes needed by _gp_EmbedFlagsValid()

// For LOGGING-related declarations

/* Imported functions */

extern void _ClearVertexVisitedFlags(graphP theGraph, int includeVirtualVertices);
extern int _FillVertexVisitedIndexes(graphP theGraph, int FillValue);


extern void _InitVertexRec(graphP theGraph, int v);

extern int _gp_FindEdge(graphP theGraph, int u, int v);

/* Private functions (some are exported to system only) */

int _gp_EmbedFlagsValid(graphP theGraph, int embedFlags);
int _EmbeddingInitialize(graphP theGraph);
int _EmbeddingInitialize_Optimized(graphP theGraph);
int _EmbeddingInitialize_Incremental(graphP theGraph);

void _EmbedBackEdgeToDescendant(graphP theGraph, int RootSide, int RootVertex, int W, int WPrevLink);

void _InvertVertex(graphP theGraph, int V);
void _MergeVertex(graphP theGraph, int W, int WPrevLink, int R);
int _MergeBicomps(graphP theGraph, int v, int RootVertex, int W, int WPrevLink);

void _WalkUp(graphP theGraph, int v, int e);
int _WalkDown(graphP theGraph, int v, int RootVertex);

int _HandleInactiveVertex(graphP theGraph, int BicompRoot, int *pW, int *pWPrevLink);

int _HandleBlockedBicomp(graphP theGraph, int v, int RootVertex, int R);
void _AdvanceFwdEdgeList(graphP theGraph, int v, int child, int nextChild);

int _EmbedPostprocess(graphP theGraph, int v, int edgeEmbeddingResult);
int _OrientVerticesInEmbedding(graphP theGraph);
int _OrientVerticesInBicomp(graphP theGraph, int BicompRoot, int PreserveSigns);
int _JoinBicomps(graphP theGraph);

/********************************************************************
 gp_Embed()

  Either a planar embedding is created in theGraph, or a Kuratowski
  subgraph is isolated.  Either way, theGraph remains sorted by DFI
  since that is the most common desired result.  The original vertex
  numbers are available in the 'index' members of the vertex records.
  Moreover, gp_SortVertices() can be invoked to put the vertices in
  the order of the input graph, at which point the 'index' members of
  the vertex records will contain the vertex DFIs.

 return OK if the embedding was successfully created or no subgraph
            homeomorphic to a topological obstruction was found.

        NOTOK on failure (e.g., NULL graph, gp_Embed already called,
                          failure to attach algorithm extension)

        NONEMBEDDABLE if the embedding couldn't be created due to
                the existence of a subgraph homeomorphic to a
                topological obstruction.

  For core planarity, OK is returned when theGraph contains a planar
  embedding of the input graph, and NONEMBEDDABLE is returned when a
  subgraph homeomorphic to K5 or K3,3 has been isolated in theGraph.

  Extension modules can overload functions used by gp_Embed to achieve
  alternate algorithms.  In those cases, the return results are
  similar.  For example, a K3,3 search algorithm would return
  NONEMBEDDABLE if it finds the K3,3 obstruction, and OK if the graph
  is planar or only contains K5 homeomorphs.  Similarly, an
  outerplanarity module can return OK for an outerplanar embedding or
  NONEMBEDDABLE when a subgraph homeomorphic to K2,3 or K4 has been
  isolated.

  The algorithm extension for gp_Embed() is encoded in the embedFlags,
  and the details of the return value can be found in the extension
  module that defines the embedding flag.
 ********************************************************************/

int gp_Embed(graphP theGraph, unsigned embedFlags)
{
    int v, e, c;
    int RetVal = OK;

    // Basic safety checks
    if (theGraph == NULL || embedFlags == 0 || gp_GetEmbedFlags(theGraph) != 0)
        return NOTOK;

    if (theGraph->graphFlags & GRAPHFLAGS_PARALLELEDGEDETECTED)
    {
        gp_ErrorMessage("Parallel edges were previously added to the graph. See gp_DeleteParallelEdges().");
        return NOTOK;
    }

    // Preprocessing
    if (!_gp_EmbedFlagsValid(theGraph, embedFlags))
    {
        // For historical reasons, the graph will be automatically extended with
        // Planarity or Outerplanarity if not already done.
        if (embedFlags == EMBEDFLAGS_PLANAR)
        {
            if (gp_ExtendWith_Planarity(theGraph) != OK)
                return NOTOK;
        }
        else
            return NOTOK;
    }

    theGraph->embedFlags = embedFlags;

    // Initialize embedding data structures and allow extension algorithms
    // that overload the function to postprocess the DFS
    if (theGraph->functions->fpEmbeddingInitialize(theGraph) != OK)
        return NOTOK;

    // In reverse DFI order, embed the back edges from each vertex to its DFS descendants.
    for (v = gp_UpperBoundVertices(theGraph) - 1; v >= gp_LowerBoundVertices(theGraph); --v)
    {
        RetVal = OK;

        // Walkup calls establish Pertinence in Step v
        // Do the Walkup for each cycle edge from v to a DFS descendant W.
        e = gp_GetVertexFwdEdgeList(theGraph, v);
        while (gp_IsEdge(theGraph, e))
        {
            theGraph->functions->fpWalkUp(theGraph, v, e);

            e = gp_GetNextEdge(theGraph, e);
            if (e == gp_GetVertexFwdEdgeList(theGraph, v))
                e = NIL;
        }
        gp_SetVertexPertinentRootsList(theGraph, v, NIL);

        // Work systematically through the DFS children of vertex v, using Walkdown
        // to add the back edges from v to its descendants in each of the DFS subtrees
        c = gp_GetVertexSortedDFSChildList(theGraph, v);
        while (gp_IsVertex(theGraph, c))
        {
            if (gp_IsVertex(theGraph, gp_GetVertexPertinentRootsList(theGraph, c)))
            {
                RetVal = theGraph->functions->fpWalkDown(theGraph, v, gp_GetBicompRootFromDFSChild(theGraph, c));
                // If Walkdown returns OK, then it is OK to proceed with edge addition.
                // Otherwise, if Walkdown returns NONEMBEDDABLE then we stop edge addition.
                if (RetVal != OK)
                    break;
            }
            c = gp_GetVertexNextDFSChild(theGraph, v, c);
        }

        // If the Walkdown determined that the graph is NONEMBEDDABLE,
        // then the guiding embedder loop can be stopped now.
        if (RetVal != OK)
            break;
    }

    // Postprocessing to orient the embedding and merge any remaining separated bicomps.
    // Some extension algorithms may overload this function, e.g. to do nothing if they
    // have no need of an embedding.
    return theGraph->functions->fpEmbedPostprocess(theGraph, v, RetVal);
}

/********************************************************************
 _gp_EmbedFlagsValid()

 Returns TRUE the theGraph has been extended to a subclass that
 supports the value in embedFlags and if embedFlags has a value
 that is supportable by extensions available in the graphLib.
 Returns FALSE otherwise.

 NOTE: Returns TRUE/FALSE rather than OK/NOTOK so the caller can
       decide if it is an error or if they want to try to take
       corrective actions on theGraph.
 ********************************************************************/

int _gp_EmbedFlagsValid(graphP theGraph, int embedFlags)
{
    if (embedFlags == EMBEDFLAGS_PLANAR)
    {
        if (gp_GetGraphFlags(theGraph) & GRAPHFLAGS_EXTENDEDWITH_PLANARITY)
            return TRUE;
    }
    return FALSE;
}

/********************************************************************
 _EmbeddingInitialize()

 Routes to the full initialization path when no DFS state is present,
 otherwise uses the incremental path to honor DFS utility work already
 performed by the caller.
 ********************************************************************/
int _EmbeddingInitialize(graphP theGraph)
{
    unsigned graphFlags;

    if (theGraph == NULL)
        return NOTOK;

    graphFlags = gp_GetGraphFlags(theGraph);

    if (!(graphFlags & (GRAPHFLAGS_DFSNUMBERED |
                        GRAPHFLAGS_SORTEDBYDFI |
                        GRAPHFLAGS_LOWPOINTSCOMPUTED)))
    {
        return _EmbeddingInitialize_Optimized(theGraph);
    }

    return _EmbeddingInitialize_Incremental(theGraph);
}

/********************************************************************
 _EmbeddingInitialize_Incremental()

 Uses previously computed DFS, sort, and/or lowpoint data to perform
 only the embedding initialization steps still required by gp_Embed().
 ********************************************************************/
int _EmbeddingInitialize_Incremental(graphP theGraph)
{
    stackP theStack;
    unsigned graphFlags;
    int v, R, uparent, u, uneighbor, e, f, eTwin, ePrev, eNext;

    _gp_LogLine("graphEmbed.c/_EmbeddingInitialize_Incremental() start\n");

    graphFlags = gp_GetGraphFlags(theGraph);

    if ((graphFlags & GRAPHFLAGS_SORTEDBYDFI) &&
        !(graphFlags & GRAPHFLAGS_DFSNUMBERED))
    {
        gp_ErrorMessage("Invalid graph flags: SORTEDBYDFI requires DFSNUMBERED.");
        return NOTOK;
    }

    if ((graphFlags & GRAPHFLAGS_LOWPOINTSCOMPUTED) &&
        ((graphFlags & (GRAPHFLAGS_DFSNUMBERED | GRAPHFLAGS_SORTEDBYDFI)) !=
         (GRAPHFLAGS_DFSNUMBERED | GRAPHFLAGS_SORTEDBYDFI)))
    {
        gp_ErrorMessage("Invalid graph flags: LOWPOINTSCOMPUTED requires DFSNUMBERED and SORTEDBYDFI.");
        return NOTOK;
    }

    // Start with the standard initializations to perform depth-first search,
    // sorting vertices (in linear time) by their depth-first indexes, and
    // computing least ancestor and lowpoint values for the vertices, if they
    // have not already been done before calling gp_Embed()
    if (!(graphFlags & GRAPHFLAGS_DFSNUMBERED))
    {
        if (gp_DepthFirstSearch(theGraph) != OK)
            return NOTOK;
        graphFlags = gp_GetGraphFlags(theGraph);
    }

    if (!(graphFlags & GRAPHFLAGS_SORTEDBYDFI))
    {
        if (gp_SortVertices(theGraph) != OK)
            return NOTOK;
        graphFlags = gp_GetGraphFlags(theGraph);
    }

    if (!(graphFlags & GRAPHFLAGS_LOWPOINTSCOMPUTED))
    {
        if (gp_ComputeLowpoints(theGraph) != OK)
            return NOTOK;
        graphFlags = gp_GetGraphFlags(theGraph);
    }

    // The planarity embedder uses the visitedIndex like a flag, except that equality
    // means 'set' and greater than means 'clear'. So, the 'flag' is implicitly
    // cleared when the main embedding loop decrements v to process the next lower
    // numbered vertex (because then all the visitedIndex values are greater than v).
    // This call starts all 'flags' as clear since gp_UpperBoundVertices() returns
    // a value one greater than the highest numbered vertex.
    if (_FillVertexVisitedIndexes(theGraph, gp_UpperBoundVertices(theGraph)) != OK)
        return NOTOK;

    // Use the stack and visited flags to traverse the previously created DFS tree to
    // do initializations that build up the sorted DFS child lists of each vertex, to
    // associate each tree edge with the bicomp root associated with the child endpoint,
    // and to remove the back edges from being embedded in the graph and instead put
    // their forward edge records in the forward edge lists of the ancestor endpoints
    // (so back edges from a vertex to its descendants can be easily processed).
    theStack = theGraph->theStack;

    if (sp_GetCapacity(theStack) < 2 * 2 * gp_GetM(theGraph) + 2)
        return NOTOK;

    sp_ClearStack(theStack);
    _ClearVertexVisitedFlags(theGraph, FALSE);

    for (v = gp_LowerBoundVertices(theGraph); v < gp_UpperBoundVertices(theGraph); ++v)
    {
        if (gp_IsVertex(theGraph, gp_GetVertexParent(theGraph, v)))
            continue;

        sp_Push2(theStack, NIL, NIL);
        while (sp_NonEmpty(theStack))
        {
            sp_Pop2(theStack, uparent, e);
            u = gp_IsNotVertex(theGraph, uparent) ? v : gp_GetNeighbor(theGraph, e);

            if (!gp_GetVisited(theGraph, u))
            {
                gp_SetVisited(theGraph, u);

                if (gp_IsEdge(theGraph, e))
                {
                    // If we are visiting a previously unvisited vertex via an existing edge,
                    // then of course we are arriving via a DFS tree edge from its parent.
                    if (gp_GetEdgeType(theGraph, e) != EDGE_TYPE_CHILD)
                        return NOTOK;

                    // Build up the sorted DFS child lists of each vertex (by appending
                    // the vertex being visited to the sorted DFS child list of its parent)
                    gp_SetVertexSortedDFSChildList(theGraph, uparent,
                                                   gp_AppendDFSChild(theGraph, uparent, u));

                    // Associate each tree edge with the bicomp root associated with the child endpoint
                    R = gp_GetBicompRootFromDFSChild(theGraph, u);
                    gp_SetFirstEdge(theGraph, R, e);
                    gp_SetLastEdge(theGraph, R, e);
                }

                // Iterate the adjacency list of vertex u
                e = gp_GetFirstEdge(theGraph, u);
                while (gp_IsEdge(theGraph, e))
                {
                    if (gp_GetEdgeType(theGraph, e) == EDGE_TYPE_CHILD)
                    {
                        // If the edge record points to an unvisited DFS child, then
                        // that is a new vertex to add to the navigation stack
                        if (!gp_GetVisited(theGraph, gp_GetNeighbor(theGraph, e)))
                            sp_Push2(theStack, u, e);
                    }
                    else if (gp_GetEdgeType(theGraph, e) == EDGE_TYPE_BACK)
                    {
                        // For each back edge record, we get the associated forward
                        // edge record (into eTwin) and info about it.
                        eTwin = gp_GetTwin(theGraph, e);
                        uneighbor = gp_GetNeighbor(theGraph, e);
                        ePrev = gp_GetPrevEdge(theGraph, eTwin);
                        eNext = gp_GetNextEdge(theGraph, eTwin);

                        // The forward edge record is then removed from the adjacency list
                        // of the ancestor (uneighbor), and...
                        if (gp_IsEdge(theGraph, ePrev))
                            gp_SetNextEdge(theGraph, ePrev, eNext);
                        else
                            gp_SetFirstEdge(theGraph, uneighbor, eNext);
                        if (gp_IsEdge(theGraph, eNext))
                            gp_SetPrevEdge(theGraph, eNext, ePrev);
                        else
                            gp_SetLastEdge(theGraph, uneighbor, ePrev);

                        // ... placed into the forward edge list of the ancestor (uneighbor)
                        if (gp_IsEdge(theGraph, f = gp_GetVertexFwdEdgeList(theGraph, uneighbor)))
                        {
                            ePrev = gp_GetPrevEdge(theGraph, f);
                            gp_SetPrevEdge(theGraph, eTwin, ePrev);
                            gp_SetNextEdge(theGraph, eTwin, f);
                            gp_SetPrevEdge(theGraph, f, eTwin);
                            gp_SetNextEdge(theGraph, ePrev, eTwin);
                        }
                        else
                        {
                            gp_SetVertexFwdEdgeList(theGraph, uneighbor, eTwin);
                            gp_SetPrevEdge(theGraph, eTwin, eTwin);
                            gp_SetNextEdge(theGraph, eTwin, eTwin);
                        }
                    }

                    e = gp_GetNextEdge(theGraph, e);
                }
            }
        }
    }

    // Initialize the future pertinent child of each vertex to just be the first
    // element of the sorted DFS child list. Initially, the embedding comprises
    // only DFS tree edges embedded as singleton bicomps, so every DFS child is
    // separated from its DFS parent(in a separate bicomp from its parent). In the
    // first articulation of the edge addition planarity algorithm, the knowledge
    // of future pertinent children was managed by a "separated DFS child list" that
    // was sorted by lowpoint. In the current implementation, this has been
    // relaxed. We start out with any separated DFS child, but then just before
    // the future pertinent child must be used in a vertex step v, it is updated to
    // be the first child that has any lowpoint less than v (it doesn't have to be
    // the lowest, so we don't need a separated DFS child list strictly sorted by
    // lowpoint). See the invocations of gp_UpdateVertexFuturePertinentChild().
    for (v = gp_LowerBoundVertices(theGraph); v < gp_UpperBoundVertices(theGraph); ++v)
        gp_SetVertexFuturePertinentChild(theGraph, v, gp_GetVertexSortedDFSChildList(theGraph, v));

    // A second pass through the vertices to set up the embedding of all
    // DFS tree edges as singleton comps
    for (v = gp_LowerBoundVertices(theGraph); v < gp_UpperBoundVertices(theGraph); ++v)
    {
        if (_gp_IsDFSTreeRoot(theGraph, v))
        {
            // Each DFS tree root vertex is not going to be in a singleton bicomp
            // with its DFS parent because it has no parent, so it will initially
            // have no adjacency list entries (rather than having an adjacency list
            // entry for a tree edge to its parent, as in the else clause).
            gp_SetFirstEdge(theGraph, v, NIL);
            gp_SetLastEdge(theGraph, v, NIL);
        }
        else
        {
            // For each vertex v that is not a DFS tree root, v will be joining
            // a virtual vertex R representing its DFS parent in an embedding of
            // a singleton biconnected component containing only the DFS tree edge
            // between v and the virtual vertex R representing v's DFS parent.
            // In the preceding initialization for-loop, the DFS tree edge was
            // already linked to R, so we obtain R now...
            R = gp_GetBicompRootFromDFSChild(theGraph, v);

            // ... and then ensure that the DFS tree edge record is alone in the
            // adjacency list of R...
            e = gp_GetFirstEdge(theGraph, R);
            gp_SetPrevEdge(theGraph, e, NIL);
            gp_SetNextEdge(theGraph, e, NIL);

            // ... then we get the twin edge record that will be going into
            // v's adjacency list, and we reset its neighbor from indicating
            // v's DFS parent to indicating R (because R is the virtual vertex
            // representing v's parent in the bicomp that will contain v).
            eTwin = gp_GetTwin(theGraph, e);
            gp_SetNeighbor(theGraph, eTwin, R);

            // Now we place that twin edge record into v's adjacency list as
            // its only adjacency.
            gp_SetFirstEdge(theGraph, v, eTwin);
            gp_SetLastEdge(theGraph, v, eTwin);
            gp_SetPrevEdge(theGraph, eTwin, NIL);
            gp_SetNextEdge(theGraph, eTwin, NIL);

            // And finally we initialize the auxiliary data structure that
            // optimizes knowledge and management of the vertices on the
            // external face of the bicomp rooted by R. At this initial stage,
            // the external face contains only R and v. The links are arranged
            // so that one can navigate around the external face from R to v a
            // and back to R in either a clockwise or counterclockwise direction
            // by consistently following either 0 links or 1 links.
            gp_SetExtFaceVertex(theGraph, R, 0, v);
            gp_SetExtFaceVertex(theGraph, R, 1, v);
            gp_SetExtFaceVertex(theGraph, v, 0, R);
            gp_SetExtFaceVertex(theGraph, v, 1, R);
        }
    }

    _gp_LogLine("graphEmbed.c/_EmbeddingInitialize_Incremental() end\n");

    return OK;
}

/********************************************************************
 _EmbeddingInitialize_Optimized()

 This method performs the following tasks:
 (1) Assign depth first index (DFI) and DFS parentvalues to vertices
 (2) Assign DFS edge types
 (3) Create a sortedDFSChildList for each vertex, sorted by child DFI
 (4) Create a sorted fwdEdgeList for each vertex, sorted by descendant DFI
 (5) Assign leastAncestor values to vertices
 (6) Sort the vertices by their DFIs
 (7) Initialize for pertinence and future pertinence management
 (8) Embed each tree edge as a singleton biconnected component

 The first five of these are performed in a single-pass DFS of theGraph.
 Afterward, the vertices are sorted by their DFIs, the lowpoint values
 are assigned and then the DFS tree edges stored in virtual vertices
 during the DFS are used to create the DFS tree embedding.

 This performs the same function as _EmbeddingInitialize_Incremental()
 but has optimized total gp_Embed() execution by about 12%.
 ********************************************************************/
int _EmbeddingInitialize_Optimized(graphP theGraph)
{
    stackP theStack;
    int DFI, v, R, uparent, u, uneighbor, e, f, eTwin, ePrev, eNext;
    int leastValue, child;

    _gp_LogLine("graphEmbed.c/_EmbeddingInitialize_Optimized() start\n");

    theStack = theGraph->theStack;

    // At most we push 2 integers per edge from a vertex to each *unvisited* neighbor
    // plus one extra (NIL, NIL) at the beginning to represent arriving at a DFS tree
    // root. We ensure that theGraph's stack has this capacity and, if so, we clear
    // the stack for use in the depth-first search (DFS).

    if (sp_GetCapacity(theStack) < 2 * 2 * gp_GetM(theGraph) + 2)
        return NOTOK;

    sp_ClearStack(theStack);

    // We clear the visited flags of vertices because they are used to determine
    // which vertices have already been visited as the DFS traverses theGraph.
    _ClearVertexVisitedFlags(theGraph, FALSE);

    // This outer loop processes each connected component of a disconnected graph
    // No need to compare v < N since DFI will reach N when inner loop processes the
    // last connected component in the graph
    for (DFI = v = gp_LowerBoundVertices(theGraph); v < gp_UpperBoundVertices(theGraph); ++v)
    {
        // Skip numbered vertices to cause the outerloop to find the
        // next DFS tree root in a disconnected graph
        if (gp_IsVertex(theGraph, gp_GetVertexParent(theGraph, v)))
            continue;

        // DFS a connected component
        sp_Push2(theStack, NIL, NIL);
        while (sp_NonEmpty(theStack))
        {
            sp_Pop2(theStack, uparent, e);

            // For vertex uparent and edge e, obtain the opposing endpoint u of e
            // If uparent is NIL, then e is also NIL and we have encountered the
            // false edge to the DFS tree root as pushed above.
            u = gp_IsNotVertex(theGraph, uparent) ? v : gp_GetNeighbor(theGraph, e);

            // We popped an edge to an unvisited vertex, so it is either a DFS tree edge
            // or a false edge to the DFS tree root (u).
            if (!gp_GetVisited(theGraph, u))
            {
                _gp_LogLine(_gp_MakeLogStr3("v=%d, DFI=%d, parent=%d", u, DFI, uparent));

                // (1) Set the DFI and DFS parent
                gp_SetVisited(theGraph, u);
                gp_SetIndex(theGraph, u, DFI++);
                gp_SetVertexParent(theGraph, u, uparent);

                if (gp_IsEdge(theGraph, e))
                {
                    // (2) Set the edge type values for tree edges
                    gp_SetEdgeType(theGraph, e, EDGE_TYPE_CHILD);
                    gp_SetEdgeType(theGraph, gp_GetTwin(theGraph, e), EDGE_TYPE_PARENT);

                    // (3) Record u in the sortedDFSChildList of uparent
                    gp_SetVertexSortedDFSChildList(theGraph, uparent,
                                                   gp_AppendDFSChild(theGraph, uparent, gp_GetIndex(theGraph, u)));

                    // (8) Record e as the first and last edges of the virtual vertex R,
                    //     a root copy of uparent uniquely associated with child u
                    R = gp_GetBicompRootFromDFSChild(theGraph, gp_GetIndex(theGraph, u));
                    gp_SetFirstEdge(theGraph, R, e);
                    gp_SetLastEdge(theGraph, R, e);
                }

                // (5) Initialize the least ancestor value
                gp_SetVertexLeastAncestor(theGraph, u, gp_GetIndex(theGraph, u));

                // Push edges to all unvisited neighbors. These will be either
                // tree edges to children or forward edge records to descendants
                // Edges that are not pushed are either marked as back edges or as
                // a tree edge if it leads back to the immediate DFS parent.
                e = gp_GetFirstEdge(theGraph, u);
                while (gp_IsEdge(theGraph, e))
                {
                    if (!gp_GetVisited(theGraph, gp_GetNeighbor(theGraph, e)))
                    {
                        sp_Push2(theStack, u, e);
                    }
                    else if (gp_GetEdgeType(theGraph, e) != EDGE_TYPE_PARENT)
                    {
                        // (2) Set the edge type values for back edges
                        gp_SetEdgeType(theGraph, e, EDGE_TYPE_BACK);
                        eTwin = gp_GetTwin(theGraph, e);
                        gp_SetEdgeType(theGraph, eTwin, EDGE_TYPE_FORWARD);

                        // (4) Move the twin of back edge record e to the sorted FwdEdgeList of the ancestor
                        uneighbor = gp_GetNeighbor(theGraph, e);
                        ePrev = gp_GetPrevEdge(theGraph, eTwin);
                        eNext = gp_GetNextEdge(theGraph, eTwin);

                        if (gp_IsEdge(theGraph, ePrev))
                            gp_SetNextEdge(theGraph, ePrev, eNext);
                        else
                            gp_SetFirstEdge(theGraph, uneighbor, eNext);
                        if (gp_IsEdge(theGraph, eNext))
                            gp_SetPrevEdge(theGraph, eNext, ePrev);
                        else
                            gp_SetLastEdge(theGraph, uneighbor, ePrev);

                        if (gp_IsEdge(theGraph, f = gp_GetVertexFwdEdgeList(theGraph, uneighbor)))
                        {
                            ePrev = gp_GetPrevEdge(theGraph, f);
                            gp_SetPrevEdge(theGraph, eTwin, ePrev);
                            gp_SetNextEdge(theGraph, eTwin, f);
                            gp_SetPrevEdge(theGraph, f, eTwin);
                            gp_SetNextEdge(theGraph, ePrev, eTwin);
                        }
                        else
                        {
                            gp_SetVertexFwdEdgeList(theGraph, uneighbor, eTwin);
                            gp_SetPrevEdge(theGraph, eTwin, eTwin);
                            gp_SetNextEdge(theGraph, eTwin, eTwin);
                        }

                        // (5) Update the leastAncestor value for the vertex u
                        uneighbor = gp_GetIndex(theGraph, uneighbor);
                        if (uneighbor < gp_GetVertexLeastAncestor(theGraph, u))
                            gp_SetVertexLeastAncestor(theGraph, u, uneighbor);
                    }

                    e = gp_GetNextEdge(theGraph, e);
                }
            }
        }
    }

    // The graph is now DFS numbered
    theGraph->graphFlags |= GRAPHFLAGS_DFSNUMBERED;

    if (gp_GetGraphFlags(theGraph) & GRAPHFLAGS_DFSNUMBERED_DIRECTED)
    {
        theGraph->graphFlags &= ~GRAPHFLAGS_DFSNUMBERED_DIRECTED;
        if (_FillVertexVisitedIndexes(theGraph, 0) != OK)
            return NOTOK;
    }

    // (6) Now that all vertices have a DFI in the index member, we can sort vertices
    if (gp_SortVertices(theGraph) != OK)
        return NOTOK;

    // Loop through the vertices to...
    for (v = gp_UpperBoundVertices(theGraph) - 1; v >= gp_LowerBoundVertices(theGraph); --v)
    {
        // (7) Initialize for pertinence management
        gp_SetVertexVisitedIndex(theGraph, v, gp_UpperBoundVertices(theGraph));

        // (7) Initialize for future pertinence management
        child = gp_GetVertexSortedDFSChildList(theGraph, v);
        gp_SetVertexFuturePertinentChild(theGraph, v, child);
        leastValue = gp_GetVertexLeastAncestor(theGraph, v);
        while (gp_IsVertex(theGraph, child))
        {
            if (leastValue > gp_GetVertexLowpoint(theGraph, child))
                leastValue = gp_GetVertexLowpoint(theGraph, child);

            child = gp_GetVertexNextDFSChild(theGraph, v, child);
        }
        gp_SetVertexLowpoint(theGraph, v, leastValue);

        // (8) Create the DFS tree embedding using the child edge records stored in the virtual vertices
        //     For each vertex v that is a DFS child, the virtual vertex R that will represent v's parent
        //     in the singleton bicomp with v is at location v + N in the vertex array.
        if (_gp_IsDFSTreeRoot(theGraph, v))
        {
            gp_SetFirstEdge(theGraph, v, NIL);
            gp_SetLastEdge(theGraph, v, NIL);
        }
        else
        {
            R = gp_GetBicompRootFromDFSChild(theGraph, v);

            // Make the child edge the only edge in the virtual vertex adjacency list
            e = gp_GetFirstEdge(theGraph, R);
            gp_SetPrevEdge(theGraph, e, NIL);
            gp_SetNextEdge(theGraph, e, NIL);

            // Reset the twin's neighbor value to point to the virtual vertex
            eTwin = gp_GetTwin(theGraph, e);
            gp_SetNeighbor(theGraph, eTwin, R);

            // Make its twin the only edge in the child's adjacency list
            gp_SetFirstEdge(theGraph, v, eTwin);
            gp_SetLastEdge(theGraph, v, eTwin);
            gp_SetPrevEdge(theGraph, eTwin, NIL);
            gp_SetNextEdge(theGraph, eTwin, NIL);

            // Set up the external face management data structure to match
            gp_SetExtFaceVertex(theGraph, R, 0, v);
            gp_SetExtFaceVertex(theGraph, R, 1, v);
            gp_SetExtFaceVertex(theGraph, v, 0, R);
            gp_SetExtFaceVertex(theGraph, v, 1, R);
        }
    }

    theGraph->graphFlags |= GRAPHFLAGS_LOWPOINTSCOMPUTED;

    _gp_LogLine("graphEmbed.c/_EmbeddingInitialize_Optimized() end\n");

    return OK;
}

/********************************************************************
 _EmbedBackEdgeToDescendant()
 The Walkdown has found a descendant vertex W to which it can
 attach a back edge up to the root of the bicomp it is processing.
 The RootSide and WPrevLink indicate the parts of the external face
 that will be replaced at each endpoint of the back edge.
 ********************************************************************/

void _EmbedBackEdgeToDescendant(graphP theGraph, int RootSide, int RootVertex, int W, int WPrevLink)
{
    int fwdEdgeRec, backEdgeRec, parentCopy;

    /* We get the two edge records of the back edge (v, W) to embed.
        The Walkup recorded in W's adjacentTo the index of the forward edge record
        that goes from the root's parent copy, v, to the descendant W. */

    fwdEdgeRec = gp_GetVertexPertinentEdge(theGraph, W);
    backEdgeRec = gp_GetTwin(theGraph, fwdEdgeRec);

    /* The forward edge record is removed from the fwdEdgeList of the root's parent copy. */

    parentCopy = _gp_GetVertexFromBicompRoot(theGraph, RootVertex);

    _gp_LogLine(_gp_MakeLogStr5("graphEmbed.c/_EmbedBackEdgeToDescendant() V=%d, R=%d, R_out=%d, W=%d, W_in=%d",
                                parentCopy, RootVertex, RootSide, W, WPrevLink));

    if (gp_GetVertexFwdEdgeList(theGraph, parentCopy) == fwdEdgeRec)
    {
        gp_SetVertexFwdEdgeList(theGraph, parentCopy, gp_GetNextEdge(theGraph, fwdEdgeRec));
        if (gp_GetVertexFwdEdgeList(theGraph, parentCopy) == fwdEdgeRec)
            gp_SetVertexFwdEdgeList(theGraph, parentCopy, NIL);
    }

    gp_SetNextEdge(theGraph, gp_GetPrevEdge(theGraph, fwdEdgeRec), gp_GetNextEdge(theGraph, fwdEdgeRec));
    gp_SetPrevEdge(theGraph, gp_GetNextEdge(theGraph, fwdEdgeRec), gp_GetPrevEdge(theGraph, fwdEdgeRec));

    // The forward edge record is added to the adjacency list of the RootVertex.
    // Note that we're guaranteed that the RootVertex adjacency list is non-empty,
    // so tests for NIL are not needed
    gp_SetAdjacentEdge(theGraph, fwdEdgeRec, 1 ^ RootSide, NIL);
    gp_SetAdjacentEdge(theGraph, fwdEdgeRec, RootSide, gp_GetEdgeByLink(theGraph, RootVertex, RootSide));
    gp_SetAdjacentEdge(theGraph, gp_GetEdgeByLink(theGraph, RootVertex, RootSide), 1 ^ RootSide, fwdEdgeRec);
    gp_SetEdgeByLink(theGraph, RootVertex, RootSide, fwdEdgeRec);

    // The back edge record is added to the adjacency list of W.
    // The adjacency list of W is also guaranteed non-empty
    gp_SetAdjacentEdge(theGraph, backEdgeRec, 1 ^ WPrevLink, NIL);
    gp_SetAdjacentEdge(theGraph, backEdgeRec, WPrevLink, gp_GetEdgeByLink(theGraph, W, WPrevLink));
    gp_SetAdjacentEdge(theGraph, gp_GetEdgeByLink(theGraph, W, WPrevLink), 1 ^ WPrevLink, backEdgeRec);
    gp_SetEdgeByLink(theGraph, W, WPrevLink, backEdgeRec);

    gp_SetNeighbor(theGraph, backEdgeRec, RootVertex);

    /* Link the two endpoint vertices together on the external face */

    gp_SetExtFaceVertex(theGraph, RootVertex, RootSide, W);
    gp_SetExtFaceVertex(theGraph, W, WPrevLink, RootVertex);
}

/********************************************************************
 _InvertVertex()
 This function flips the orientation of a single vertex such that
 instead of using link successors to go clockwise (or counterclockwise)
 around a vertex's adjacency list, link predecessors would be used.
 ********************************************************************/

void _InvertVertex(graphP theGraph, int W)
{
    int e, temp;

    _gp_LogLine(_gp_MakeLogStr1("graphEmbed.c/_InvertVertex() W=%d", W));

    // Swap the links in all of the edge records of the adjacency list
    e = gp_GetFirstEdge(theGraph, W);
    while (gp_IsEdge(theGraph, e))
    {
        temp = gp_GetNextEdge(theGraph, e);
        gp_SetNextEdge(theGraph, e, gp_GetPrevEdge(theGraph, e));
        gp_SetPrevEdge(theGraph, e, temp);

        e = temp;
    }

    // Swap the first/last edge record indicators in the vertex
    temp = gp_GetFirstEdge(theGraph, W);
    gp_SetFirstEdge(theGraph, W, gp_GetLastEdge(theGraph, W));
    gp_SetLastEdge(theGraph, W, temp);

    // Swap the first/last external face indicators in the vertex
    temp = gp_GetExtFaceVertex(theGraph, W, 0);
    gp_SetExtFaceVertex(theGraph, W, 0, gp_GetExtFaceVertex(theGraph, W, 1));
    gp_SetExtFaceVertex(theGraph, W, 1, temp);
}

/********************************************************************
 _MergeVertex()
 The merge step joins the vertex W to the root R of a child bicompRoot,
 which is a root copy of W appearing in the region N to 2N-1.

 Actually, the first step of this is to redirect all of the edges leading
 into R so that they indicate W as the neighbor instead of R.
 For each edge node pointing to R, we set the 'v' field to W.  Once an
 edge is redirected from a root copy R to a parent copy W, the edge is
 never redirected again, so we associate the cost of the redirection
 as constant per edge, which maintains linear time performance.

 After this is done, a regular circular list union occurs. The only
 consideration is that WPrevLink is used to indicate the two edge
 records e_w and e_r that will become consecutive in the resulting
 adjacency list of W.  We set e_w to W's link [WPrevLink] and e_r to
 R's link [1^WPrevLink] so that e_w and e_r indicate W and R with
 opposing links, which become free to be cross-linked.  Finally,
 the edge record e_ext, set equal to R's link [WPrevLink], is the edge
 that, with e_r, held R to the external face.  Now, e_ext will be the
 new link [WPrevLink] edge record for W.  If e_w and e_r become part
 of a proper face, then e_ext and W's link [1^WPrevLink] are the two
 edges that attach W to the external face cycle of the containing bicomp.
 ********************************************************************/

void _MergeVertex(graphP theGraph, int W, int WPrevLink, int R)
{
    int e, eTwin, e_w, e_r, e_ext;

    _gp_LogLine(_gp_MakeLogStr4("graphEmbed.c/_MergeVertex() W=%d, W_in=%d, R=%d, R_out=%d",
                                W, WPrevLink, R, 1 ^ WPrevLink));

    // All edge records leading _into_ R _from_ its neighbors must be changed
    // to say that they are leading into W.
    e = gp_GetFirstEdge(theGraph, R);
    while (gp_IsEdge(theGraph, e))
    {
        eTwin = gp_GetTwin(theGraph, e);
        gp_GetNeighbor(theGraph, eTwin) = W;

        e = gp_GetNextEdge(theGraph, e);
    }

    // Obtain the edge records that will be involved in the adjacency list union
    e_w = gp_GetEdgeByLink(theGraph, W, WPrevLink);
    e_r = gp_GetEdgeByLink(theGraph, R, 1 ^ WPrevLink);
    e_ext = gp_GetEdgeByLink(theGraph, R, WPrevLink);

    // If W has any edges, then join its adjacency list with that of R
    if (gp_IsEdge(theGraph, e_w))
    {
        // The WPrevLink edge of W is e_w, so the 1^WPrevLink edge in e_w leads back to W.
        // Now it must lead to e_r.  Likewise, e_r needs to lead back to e_w with the
        // opposing link, which is WPrevLink
        // Note that the adjacency lists of W and R are guaranteed non-empty, which is
        // why these linkages can be made without NIL tests.
        gp_SetAdjacentEdge(theGraph, e_w, 1 ^ WPrevLink, e_r);
        gp_SetAdjacentEdge(theGraph, e_r, WPrevLink, e_w);

        // Cross-link W's WPrevLink edge record and the 1^WPrevLink edge record in e_ext
        gp_SetEdgeByLink(theGraph, W, WPrevLink, e_ext);
        gp_SetAdjacentEdge(theGraph, e_ext, 1 ^ WPrevLink, NIL);
    }
    // Otherwise, W just receives R's adjacency list.  This can happen, for example, on
    // a DFS tree root vertex during JoinBicomps()
    else
    {
        // Cross-link W's 1^WPrevLink edge record and the WPrevLink edge record in e_r
        gp_SetEdgeByLink(theGraph, W, 1 ^ WPrevLink, e_r);
        gp_SetAdjacentEdge(theGraph, e_r, WPrevLink, NIL);

        // Cross-link W's WPrevLink edge record and the 1^WPrevLink edge record in e_ext
        gp_SetEdgeByLink(theGraph, W, WPrevLink, e_ext);
        gp_SetAdjacentEdge(theGraph, e_ext, 1 ^ WPrevLink, NIL);
    }

    // Erase the entries in R, which is a root copy that is no longer needed
    _InitVertexRec(theGraph, R);
}

/********************************************************************
 _MergeBicomps()

 Merges all biconnected components at the cut vertices indicated by
 entries on the stack.

 theGraph contains the stack of bicomp roots and cut vertices to merge

 v, RootVertex, W and WPrevLink are not used in this routine, but are
          used by overload extensions

 Returns OK, but an extension function may return a value other than
         OK in order to cause Walkdown to terminate immediately.
********************************************************************/

int _MergeBicomps(graphP theGraph, int v, int RootVertex, int W, int WPrevLink)
{
    int R, Rout, Z, ZPrevLink, e, extFaceVertex;

    // Suppresses an unused-parameter warning for a parameter we intend to keep
    (void)v;
    // Suppresses an unused-parameter warning for a parameter we intend to keep
    (void)RootVertex;
    // Suppresses an unused-parameter warning for a parameter we intend to keep
    (void)W;
    // Suppresses an unused-parameter warning for a parameter we intend to keep
    (void)WPrevLink;

    while (sp_NonEmpty(theGraph->theStack))
    {
        sp_Pop2(theGraph->theStack, R, Rout);
        sp_Pop2(theGraph->theStack, Z, ZPrevLink);

        /* The external faces of the bicomps containing R and Z will
           form two corners at Z.  One corner will become part of the
           internal face formed by adding the new back edge. The other
           corner will be the new external face corner at Z.
           We first want to update the links at Z to reflect this. */

        extFaceVertex = gp_GetExtFaceVertex(theGraph, R, 1 ^ Rout);
        gp_SetExtFaceVertex(theGraph, Z, ZPrevLink, extFaceVertex);

        if (gp_GetExtFaceVertex(theGraph, extFaceVertex, 0) == gp_GetExtFaceVertex(theGraph, extFaceVertex, 1))
            // When (R, extFaceVertex) form a singleton bicomp, they have the same orientation, so the Rout link in extFaceVertex
            // is the one that has to now point back to Z
            gp_SetExtFaceVertex(theGraph, extFaceVertex, Rout, Z);
        else
            // When R and extFaceVertex are not alone in the bicomp, then they may not have the same orientation, so the
            // ext face link that should point to Z is whichever one pointed to R, since R is a root copy of Z.
            gp_SetExtFaceVertex(theGraph, extFaceVertex, gp_GetExtFaceVertex(theGraph, extFaceVertex, 0) == R ? 0 : 1, Z);

        /* If the path used to enter Z is opposed to the path
           used to exit R, then we have to flip the bicomp
           rooted at R, which we signify by inverting R
           then setting the sign on its DFS child edge to
           indicate that its descendants must be flipped later */

        if (ZPrevLink == Rout)
        {
            Rout = 1 ^ ZPrevLink;

            if (gp_GetFirstEdge(theGraph, R) != gp_GetLastEdge(theGraph, R))
                _InvertVertex(theGraph, R);

            e = gp_GetFirstEdge(theGraph, R);
            while (gp_IsEdge(theGraph, e))
            {
                if (gp_GetEdgeType(theGraph, e) == EDGE_TYPE_CHILD)
                {
                    // The core planarity algorithm could simply "set" the inverted flag
                    // because a bicomp root edge cannot be already inverted in the core
                    // planarity algorithm at the time of this merge.
                    // However, extensions may perform edge reductions on tree edges, resulting
                    // in an inversion sign being promoted to the root edge of a bicomp before
                    // it gets merged.  So, xor is used to reverse the inversion flag on the
                    // root edge if the bicomp root must be inverted before it is merged.
                    gp_XorEdgeFlagInverted(theGraph, e);
                    break;
                }

                e = gp_GetNextEdge(theGraph, e);
            }
        }

        // R is no longer pertinent to Z since we are about to merge R into Z, so we delete R
        // from Z's pertinent bicomp list (Walkdown gets R from the head of the list).
        gp_DeleteVertexPertinentRoot(theGraph, Z, R);

        // If the merge will place the current future pertinence child into the same bicomp as Z,
        // then we advance to the next child (or NIL) because future pertinence is based on
        // back edge connections from vertices in other bicomps (i.e., DFS subtrees rooted by
        // other DFS children of Z that have not yet been merged into the bicomp with Z).
        if (gp_GetDFSChildFromBicompRoot(theGraph, R) == gp_GetVertexFuturePertinentChild(theGraph, Z))
        {
            gp_SetVertexFuturePertinentChild(theGraph, Z,
                                             gp_GetVertexNextDFSChild(theGraph, Z, gp_GetVertexFuturePertinentChild(theGraph, Z)));
        }

        // Now we push R into Z, eliminating R
        theGraph->functions->fpMergeVertex(theGraph, Z, ZPrevLink, R);
    }

    return OK;
}

/********************************************************************
 _WalkUp()
 v is the vertex currently being embedded
 e is the forward edge record of the "back" edge between v and
   a descendant W of v

 The Walkup establishes pertinence for step v.  It marks W with e
 as a way of indicating it is pertinent because it should be made
 'adjacent to' v by adding a back edge (v', W), which will occur when
 the Walkdown encounters W.

 The Walkup also determines the pertinent child bicomps that should be
 set up as a result of the need to embed edge (v, W). It does this by
 recording the pertinent child biconnected components of all cut
 vertices between W and the child of v that is an ancestor of W.
 Note that it stops the traversal if it finds a visited info value set
 to v, which indicates that a prior walkup call in step v has already
 done the work. This ensures work is not duplicated.

 A second technique used to maintain a total linear time bound for the
 whole planarity method is that of parallel external face traversal.
 This ensures that the cost of determining pertinence in step v is
 linearly commensurate with the length of the path that ultimately
 is removed from the external face.

 Zig and Zag are so named because one goes around one side of a bicomp
 and the other goes around the other side, yet we have as yet no notion
 of orientation for the bicomp. The edge record e from vertex v gestures
 to a descendant vertex W in some other bicomp.  Zig and Zag start out
 at W. They go around alternate sides of the bicomp until its root is
 found.  We then hop from the root copy to the parent copy of the vertex
 in order to record which bicomp we just came from and also to continue
 the walk-up at the parent copy as if it were the new W.  We reiterate
 this process until the parent copy actually is v, at which point the
 Walkup is done.
 ********************************************************************/

void _WalkUp(graphP theGraph, int v, int e)
{
    int W = gp_GetNeighbor(theGraph, e);
    int Zig = W, Zag = W, ZigPrevLink = 1, ZagPrevLink = 0;
    int nextZig, nextZag, R;

    // Start by marking W as being directly pertinent
    gp_SetVertexPertinentEdge(theGraph, W, e);

    // Zig and Zag are initialized at W, and we continue looping around
    // the external faces of bicomps up from W until we reach vertex v
    // (or until the visited info optimization breaks the loop)
    while (Zig != v)
    {
        // Obtain the next vertex in a first direction and determine if it is a bicomp root
        if (gp_IsVirtualVertex(theGraph, (nextZig = gp_GetExtFaceVertex(theGraph, Zig, 1 ^ ZigPrevLink))))
        {
            // If the current vertex along the external face was visited in this step v,
            // then the bicomp root and its ancestor roots have already been added.
            if (gp_GetVertexVisitedIndex(theGraph, Zig) == v)
                break;

            // Store the bicomp root that was found
            R = nextZig;

            // Since the bicomp root was the next vertex on the path from Zig, determine the
            // vertex on the opposing path that enters the bicomp root.
            nextZag = gp_GetExtFaceVertex(theGraph, R,
                                          gp_GetExtFaceVertex(theGraph, R, 0) == Zig ? 1 : 0);

            // If the opposing vertex was already marked visited in this step, then a prior
            // Walkup already recorded as pertinent the bicomp root and its ancestor roots.
            if (gp_GetVertexVisitedIndex(theGraph, nextZag) == v)
                break;
        }

        // Obtain the next vertex in the parallel direction and perform the analogous logic
        else if (gp_IsVirtualVertex(theGraph, (nextZag = gp_GetExtFaceVertex(theGraph, Zag, 1 ^ ZagPrevLink))))
        {
            if (gp_GetVertexVisitedIndex(theGraph, Zag) == v)
                break;
            R = nextZag;
            nextZig = gp_GetExtFaceVertex(theGraph, R,
                                          gp_GetExtFaceVertex(theGraph, R, 0) == Zag ? 1 : 0);
            if (gp_GetVertexVisitedIndex(theGraph, nextZig) == v)
                break;
        }

        // The bicomp root was not found in either direction.
        else
        {
            if (gp_GetVertexVisitedIndex(theGraph, Zig) == v)
                break;
            if (gp_GetVertexVisitedIndex(theGraph, Zag) == v)
                break;
            R = NIL;
        }

        // This Walkup has now finished with another vertex along each of the parallel
        // paths, so they are marked visited in step v so that future Walkups in this
        // step v can break if these vertices are encountered again.
        gp_SetVertexVisitedIndex(theGraph, Zig, v);
        gp_SetVertexVisitedIndex(theGraph, Zag, v);

        // If both directions found new non-root vertices, then proceed with parallel external face traversal
        if (gp_IsNotVirtualVertex(theGraph, R))
        {
            ZigPrevLink = gp_GetExtFaceVertex(theGraph, nextZig, 0) == Zig ? 0 : 1;
            Zig = nextZig;

            ZagPrevLink = gp_GetExtFaceVertex(theGraph, nextZag, 0) == Zag ? 0 : 1;
            Zag = nextZag;
        }

        // The bicomp root was found and not previously recorded as pertinent,
        // so walk up to the parent bicomp and continue
        else
        {
            // Step up from the bicomp root vertex (virtual) to the parent copy of
            // the vertex (non-virtual; called parent copy because it is the one that
            // is in a bicomp with a virtual or non-virtual copy of its DFS parent)
            Zig = Zag = _gp_GetVertexFromBicompRoot(theGraph, R);
            ZigPrevLink = 1;
            ZagPrevLink = 0;

            // Add the new bicomp root o the list of pertinent bicomp roots of the parent copy vertex.
            // The new root vertex is appended if future pertinent and prepended if only pertinent
            // so that, by virtue of storage, the Walkdown will process all pertinent bicomps that
            // are not future pertinent before any future pertinent bicomps.

            // NOTE: Unlike vertices, the activity status of a bicomp is computed solely using
            //       the lowpoint of the DFS child in the bicomp's root edge, which indicates
            //       whether the DFS child or any of its descendants connect by a back edge to
            //       ancestors of v. If so, then the bicomp rooted at RootVertex must contain a
            //       future pertinent vertex that must be kept on the external face.
            if (gp_GetVertexLowpoint(theGraph, gp_GetDFSChildFromBicompRoot(theGraph, R)) < v)
                gp_AppendVertexPertinentRoot(theGraph, Zig, R);
            else
                gp_PrependVertexPertinentRoot(theGraph, Zag, R);
        }
    }
}

/********************************************************************
 _WalkDown()
 Consider a circular shape with small circles and squares along its perimeter.
 The small circle at the top is the root vertex of the bicomp.  The other small
 circles represent active vertices, and the squares represent future pertinent
 vertices.  The root vertex is a root copy of v, the vertex currently being processed.

 The Walkup previously marked all vertices adjacent to v by setting their
 pertinentEdge members with the forward edge records of the back edges to embed.
 Two Walkdown traversals are performed to visit all reachable vertices
 along each of the external face paths emanating from RootVertex (a root
 copy of vertex v) to embed back edges to descendants of vertex v that
 have their pertinentEdge members marked.

 During each Walkdown traversal, it is sometimes necessary to hop from a
 vertex to one of its child biconnected components in order to reach the
 desired vertices.  In such cases, the biconnected components are merged
 such that adding the back edge forms a new proper face in the biconnected
 component rooted at RootVertex (which, again, is a root copy of v).

 The outer loop performs both walks, unless the first walk got all the way
 around to RootVertex (only happens when bicomp contains no external activity,
 such as when processing the last vertex), or when non-planarity is
 discovered (in a pertinent child bicomp such that the stack is non-empty).

 For the inner loop, each iteration visits a vertex W.  If W is marked as
 requiring a back edge, then MergeBicomps is called to merge the biconnected
 components whose cut vertices have been collecting in merge stack.  Then,
 the back edge (RootVertex, W) is added, and the pertinentEdge of W is cleared.

 Next, we check whether W has a pertinent child bicomp.  If so, then we figure
 out which path down from the root of the child bicomp leads to the next vertex
 to be visited, and we push onto the stack information on the cut vertex and
 the paths used to enter into it and exit from it.  Alternately, if W
 had no pertinent child bicomps, then we check to see if it is inactive.
 If so, we find the next vertex along the external face, then short-circuit
 its inactive predecessor (under certain conditions).  Finally, if W is not
 inactive, but it has no pertinent child bicomps, then we already know its
 adjacentTo flag is clear so both criteria for internal activity also fail.
 Therefore, W must be a stopping vertex.

 A stopping vertex X is a future pertinent vertex that has no pertinent
 child bicomps and no unembedded back edge to the current vertex v.
 The inner loop of Walkdown stops walking when it reaches a stopping vertex X
 because if it were to proceed beyond X and embed a back edge, then X would be
 surrounded by the bounding cycle of the bicomp.  This would clearly be
 incorrect because X has a path leading from it to an ancestor of v, which
 would have to cross the bounding cycle.

 Either Walkdown traversal can halt the Walkdown and return if a pertinent
 child biconnected component to which the traversal has descended is blocked,
 i.e. has stopping vertices on both paths emanating from the root.  This
 indicates an obstruction to embedding. In core planarity it is evidence of
 a K_{3,3}, but some extension algorithms are able to clear the blockage and
 proceed with embedding.

 If both Walkdown traversals successfully completed, then the outer loop
 ends.  Post-processing code tests whether the Walkdown embedded all the
 back edges from v to its descendants in the subtree rooted by c, a DFS
 child of v uniquely associated with the RootVertex.  If not, then embedding
 was obstructed.  In core planarity it is evidence of a K_{3,3} or K_5, but some
 extension algorithms are able to clear the blockage and proceed with embedding.

  Returns OK if all possible edges were embedded,
          NONEMBEDDABLE if less than all possible edges were embedded,
          NOTOK for an internal code failure
 ********************************************************************/

int _WalkDown(graphP theGraph, int v, int RootVertex)
{
    int RetVal, W, WPrevLink, R, X, XPrevLink, Y, YPrevLink, RootSide, e;
    int RootEdgeChild = gp_GetDFSChildFromBicompRoot(theGraph, RootVertex);

    sp_ClearStack(theGraph->theStack);

    for (RootSide = 0; RootSide < 2; RootSide++)
    {
        W = gp_GetExtFaceVertex(theGraph, RootVertex, RootSide);

        // Determine the link used to enter W based on which side points back to RootVertex
        // Implicitly handled special case: In core planarity, the first Walkdown traversal
        // Will be on a singleton edge.  In this case, RootVertex and W are *consistently*
        // oriented, and the RootSide is 0, so WPrevLink should be 1. This calculation is
        // written to implicitly produce that result.
        WPrevLink = gp_GetExtFaceVertex(theGraph, W, 1) == RootVertex ? 1 : 0;

        while (W != RootVertex)
        {
            // Detect unembedded back edge descendant endpoint W
            if (gp_IsEdge(theGraph, gp_GetVertexPertinentEdge(theGraph, W)))
            {
                // Merge any bicomps whose cut vertices were traversed to reach W, then add the
                // edge to W to form a new proper face in the embedding.
                if (sp_NonEmpty(theGraph->theStack))
                {
                    if ((RetVal = theGraph->functions->fpMergeBicomps(theGraph, v, RootVertex, W, WPrevLink)) != OK)
                        return RetVal;
                }
                theGraph->functions->fpEmbedBackEdgeToDescendant(theGraph, RootSide, RootVertex, W, WPrevLink);

                // Clear W's pertinentEdge since the forward edge record it contained has been embedded
                gp_SetVertexPertinentEdge(theGraph, W, NIL);
            }

            // If W has a pertinent child bicomp, then we descend to the first one...
            // NOTE: Each pertinent root is stored as the DFS child with which it is
            //       associated, so we test gp_IsVertex, not gp_IsVirtualVertex here.
            if (gp_IsVertex(theGraph, gp_GetVertexPertinentRootsList(theGraph, W)))
            {
                // Push the vertex W and the direction of entry, then descend to a root copy R of W
                sp_Push2(theGraph->theStack, W, WPrevLink);
                R = gp_GetVertexFirstPertinentRoot(theGraph, W);

                // Get the next active vertices X and Y on the external face paths emanating from R
                X = gp_GetExtFaceVertex(theGraph, R, 0);
                XPrevLink = gp_GetExtFaceVertex(theGraph, X, 1) == R ? 1 : 0;
                Y = gp_GetExtFaceVertex(theGraph, R, 1);
                YPrevLink = gp_GetExtFaceVertex(theGraph, Y, 0) == R ? 0 : 1;

                // Now we implement the Walkdown's simple path selection rules!
                // Select a direction from the root to a pertinent vertex,
                // preferentially toward a vertex that is not future pertinent
                gp_UpdateVertexFuturePertinentChild(theGraph, X, v);
                gp_UpdateVertexFuturePertinentChild(theGraph, Y, v);
                if (PERTINENT(theGraph, X) && NOTFUTUREPERTINENT(theGraph, X, v))
                {
                    W = X;
                    WPrevLink = XPrevLink;
                    sp_Push2(theGraph->theStack, R, 0);
                }
                else if (PERTINENT(theGraph, Y) && NOTFUTUREPERTINENT(theGraph, Y, v))
                {
                    W = Y;
                    WPrevLink = YPrevLink;
                    sp_Push2(theGraph->theStack, R, 1);
                }
                else if (PERTINENT(theGraph, X))
                {
                    W = X;
                    WPrevLink = XPrevLink;
                    sp_Push2(theGraph->theStack, R, 0);
                }
                else if (PERTINENT(theGraph, Y))
                {
                    W = Y;
                    WPrevLink = YPrevLink;
                    sp_Push2(theGraph->theStack, R, 1);
                }
                else
                {
                    // Both the X and Y sides of the descendant bicomp are blocked.
                    // Let the application decide whether it can unblock the bicomp.
                    // The core planarity/outerplanarity embedder simply isolates a
                    // planarity/outerplanary obstruction and returns NONEMBEDDABLE
                    if ((RetVal = theGraph->functions->fpHandleBlockedBicomp(theGraph, v, RootVertex, R)) != OK)
                        return RetVal;

                    // If an extension algorithm cleared the blockage, then we pop W and WPrevLink
                    // back off the stack and let the Walkdown traversal try descending again
                    sp_Pop2(theGraph->theStack, W, WPrevLink);
                }
            }
            else
            {
                // The vertex W is known to be non-pertinent, so if it is future pertinent
                // (or if the algorithm is based on outerplanarity), then the vertex is
                // a stopping vertex for the Walkdown traversal.
                gp_UpdateVertexFuturePertinentChild(theGraph, W, v);
                if (FUTUREPERTINENT(theGraph, W, v) || (gp_GetEmbedFlags(theGraph) & EMBEDFLAGS_OUTERPLANAR))
                {
                    // Create an external face short-circuit between RootVertex and the stopping vertex W
                    // so that future steps do not walk down a long path of inactive vertices between them.
                    // As a special case, we ensure that the external face is not reduced to just two
                    // vertices, W and RootVertex, because it would then become a challenge to determine
                    // whether W has the same orientation as RootVertex.
                    // So, if the other side of RootVertex is already attached to W, then we simply push
                    // W back one vertex so that the external face will have at least three vertices.
                    if (gp_GetExtFaceVertex(theGraph, RootVertex, 1 ^ RootSide) == W)
                    {
                        X = W;
                        W = gp_GetExtFaceVertex(theGraph, W, WPrevLink);
                        WPrevLink = gp_GetExtFaceVertex(theGraph, W, 0) == X ? 1 : 0;
                    }
                    gp_SetExtFaceVertex(theGraph, RootVertex, RootSide, W);
                    gp_SetExtFaceVertex(theGraph, W, WPrevLink, RootVertex);

                    // Terminate the Walkdown traversal since it encountered the stopping vertex
                    break;
                }

                // If the vertex is neither pertinent nor future pertinent, then it is inactive.
                // The default handler planarity handler simply skips inactive vertices by traversing
                // to the next vertex on the external face.
                // Once upon a time, false edges called short-circuit edges were added to eliminate
                // inactive vertices, but the extFace links above achieve the same result with less work.
                else
                {
                    if (theGraph->functions->fpHandleInactiveVertex(theGraph, RootVertex, &W, &WPrevLink) != OK)
                        return NOTOK;
                }
            }
        }
    }

    // Detect and handle the case in which Walkdown was blocked from embedding all the back edges from v
    // to descendants in the subtree of the child of v associated with the bicomp RootVertex.
    if (gp_IsEdge(theGraph, e = gp_GetVertexFwdEdgeList(theGraph, v)) && RootEdgeChild < gp_GetNeighbor(theGraph, e))
    {
        int nextChild = gp_GetVertexNextDFSChild(theGraph, v, RootEdgeChild);

        // We finish detecting  that the Walkdown was blocked from embedding all forward edge records into
        // the RootEdgeChild subtree if there the next child's DFI is greater than the descendant endpoint
        // of the next forward edge record, or if there is no next child.
        if (gp_IsNotVertex(theGraph, nextChild) || nextChild > gp_GetNeighbor(theGraph, e))
        {
            // If an extension to core planarity indicates it is OK to proceed despite having detected
            // unembedded forward edges, then advance to the forward edges for the next child, if any
            if ((RetVal = theGraph->functions->fpHandleBlockedBicomp(theGraph, v, RootVertex, RootVertex)) == OK)
                _AdvanceFwdEdgeList(theGraph, v, RootEdgeChild, nextChild);

            return RetVal;
        }
    }

    return OK;
}

/********************************************************************
 _HandleBlockedBicomp()

 A biconnected component has blocked the Walkdown from embedding
 back edges.  Each external face path emanating from the root is
 blocked by a stopping vertex.

 The core planarity/outerplanarity algorithm handles the blockage
 by isolating an embedding obstruction (a subgraph homeomorphic to
 K_{3,3} or K_5 for planarity, or a subgraph homeomorphic to K_{2,3}
 or K_4 for outerplanarity). Then NONEMBEDDABLE is returned so that
 the WalkDown can terminate.

 Extension algorithms are able to clear some of the blockages, in
 which case OK is returned to indicate that the WalkDown can proceed.

 Returns OK to proceed with WalkDown at W,
         NONEMBEDDABLE to terminate WalkDown of Root Vertex
         NOTOK for internal error
 ********************************************************************/

int _HandleBlockedBicomp(graphP theGraph, int v, int RootVertex, int R)
{
    (void)theGraph;
    (void)v;
    (void)RootVertex;
    (void)R;
    return NONEMBEDDABLE;
}

/********************************************************************
 _AdvanceFwdEdgeList()

 If an extension determines that it is OK to leave some forward edges
 unembedded, then we advance the forward edge list head pointer past
 the unembedded edges for the current child so that it points to the
 first forward edge for the next child, if any.

 There are two meanings of the phrase "if any".  First, there may be
 no next child, in which case nextChild is NIL, and the forward edge
 list need not be advanced.

 If there is a next child, then the forward edge list head needs to
 be advanced to the first edge whose descendant endpoint is greater
 than the nextChild, if any. However, the tail end of the forward edge
 list may include unembedded forward edge records to a preceding sibling
 of the child vertex.  So, we advance an edge pointer e until one of
 the following happens:

 1) e gets all the way around to the head of the forward edge list
 2) e finds an edge whose descendant endpoint is less than the child
 3) e finds an edge whose descendant endpoint is greater than the next child

 In case 1, all the forward edges belong in the subtree of the child, so
 there is no need to change the forward edge list head.

 In case 2, there are no more forward edges to any following siblings of
 the child, only left-behind unembedded forward edges that we advanced
 past in previous calls to this method from Walkdowns of the preceding
 children of v.  So the forward edge list head should be set to e so that
 it is set to the forward edge with the least numbered descendant endpoint.

 In case 3, the desired forward edge into the subtree of a following sibling
 of the child has been found, so again the forward edge list head should be
 set to e to indicate that edge.

 After all Walkdowns of the children of a vertex, the forward edge list will
 be NIL if all edges were embedded, or it will indicate the unembedded
 forward edge whose descendant endpoint has the least number.  Cases 1 and 2
 directly implement this in cases where a Walkdown for the given child
 fails to embed an edge, and case 3 indirectly finishes the job by making
 sure the forward edge list head has the right value at the beginning of
 a Walkdown for a particular child.  If the Walkdown of that child succeeds
 at embedding all the forward edges into that child's subtree, then each
 embedding advances the forward edge list head.  So, even if the Walkdown
 of the last pertinent child embeds all forward edges, then the Walkdown
 itself advances the head of the forward edge list to the first unembedded
 forward edge, or to NIL.
 ********************************************************************/

void _AdvanceFwdEdgeList(graphP theGraph, int v, int child, int nextChild)
{
    int e = gp_GetVertexFwdEdgeList(theGraph, v);

    while (gp_IsEdge(theGraph, e))
    {
        // 2) e finds an edge whose descendant endpoint is less than the child
        if (gp_GetNeighbor(theGraph, e) < child)
        {
            gp_SetVertexFwdEdgeList(theGraph, v, e);
            break;
        }

        // 3) e finds an edge whose descendant endpoint is greater than the next child
        else if (gp_IsVertex(theGraph, nextChild) && nextChild < gp_GetNeighbor(theGraph, e))
        {
            gp_SetVertexFwdEdgeList(theGraph, v, e);
            break;
        }

        e = gp_GetNextEdge(theGraph, e);
        // 1) e gets all the way around to the head of the forward edge list
        if (e == gp_GetVertexFwdEdgeList(theGraph, v))
            e = NIL;
    }
}

/********************************************************************
 _HandleInactiveVertex()

 Although it is possible to short-circuit every inactive vertex from
 the external face, for efficiency the Walkdown traversal now just does
 a single short-circuit between the bicomp root and a stopping vertex.
 This is because the main thing that short-circuiting needs to optimize
 is the Walkdown's choice of direction after descending to the root
 of a pertinent biconnected component.  So, the Walkdown just optimizes
 the external face of a biconnected component as it finishes processing
 it so it will be ready in future steps when it becomes pertinent.
 Hence, when traversing the face of a bicomp during the current step,
 we only need to skip an inactive vertex and traverse to the next vertex
 on the external face.
 ********************************************************************/

int _HandleInactiveVertex(graphP theGraph, int BicompRoot, int *pW, int *pWPrevLink)
{
    int X;

    // Suppresses an unused-parameter warning for a parameter we intend to keep
    (void)BicompRoot;

    X = gp_GetExtFaceVertex(theGraph, *pW, 1 ^ *pWPrevLink);
    *pWPrevLink = gp_GetExtFaceVertex(theGraph, X, 0) == *pW ? 0 : 1;
    *pW = X;

    return OK;
}

/********************************************************************
 _EmbedPostprocess()

 After the loop that embeds the cycle edges from each vertex to its
 DFS descendants, this method is invoked to postprocess the graph.
 If the graph is planar or outerplanar, then a consistent orientation
 is imposed on the vertices of the embedding, and any remaining
 separated biconnected components are joined together.
 If the graph is non-planar or non-outerplanar, then an obstruction
 to planarity or outerplanarity has already been isolated.
 Extensions may override this function to provide alternate behavior.

  @param theGraph - the graph ready for postprocessing
  @param v - the last vertex processed by the edge embedding loop
  @param edgeEmbeddingResult -
         OK if all edge embedding iterations returned OK
         NONEMBEDDABLE if an embedding iteration failed to embed
             all edges for a vertex

  @return NOTOK on internal failure
          NONEMBEDDABLE if a subgraph homeomorphic to a topological
              obstruction is isolated in the graph
          OK otherwise (e.g. if the graph contains an embedding)
 *****************************************************************/

int _EmbedPostprocess(graphP theGraph, int v, int edgeEmbeddingResult)
{
    int RetVal = edgeEmbeddingResult;

    // Suppresses an unused-parameter warning for a parameter we intend to keep
    (void)v;

    // If an embedding was found, then post-process the embedding structure give
    // a consistent orientation to all vertices then eliminate virtual vertices
    if (edgeEmbeddingResult == OK)
    {
        if (_OrientVerticesInEmbedding(theGraph) != OK ||
            _JoinBicomps(theGraph) != OK)
            RetVal = NOTOK;
    }

    // If the graph is embedded (OK) or NONEMBEDDABLE, we pass the result back
    return RetVal;
}

/********************************************************************
 _OrientVerticesInEmbedding()

 Each vertex will then have an orientation, either clockwise or
 counterclockwise.  All vertices in each bicomp need to have the
 same orientation.
 This method clears the stack, and the stack is clear when it
 is finished.
 Returns OK on success, NOTOK on implementation failure.
 ********************************************************************/

int _OrientVerticesInEmbedding(graphP theGraph)
{
    sp_ClearStack(theGraph->theStack);

    // For each vertex, obtain the associated bicomp root location and,
    // if it is still in use as a bicomp root, orient the vertices in the bicomp
    for (int R = gp_LowerBoundVirtualVertices(theGraph); R < gp_UpperBoundVirtualVertices(theGraph); ++R)
    {
        if (gp_VirtualVertexInUse(theGraph, R))
        {
            if (_OrientVerticesInBicomp(theGraph, R, 0) != OK)
                return NOTOK;
        }
    }
    return OK;
}

/********************************************************************
 _OrientVerticesInBicomp()
  As a result of the work done so far, the edges around each vertex have
 been put in order, but the orientation may be counterclockwise or
 clockwise for different vertices within the same bicomp.
 We need to reverse the orientations of those vertices that are not
 oriented the same way as the root of the bicomp.

 During embedding, a bicomp with root edge (v', c) may need to be flipped.
 We do this by inverting the root copy v' and implicitly inverting the
 orientation of the vertices in the subtree rooted by c by assigning -1
 to the sign of the DFSCHILD edge record leading to c.

 We now use these signs to help propagate a consistent vertex orientation
 throughout all vertices that have been merged into the given bicomp.
 The bicomp root contains the orientation to be imposed on all parent
 copy vertices.  We perform a standard depth first search to visit each
 vertex.  A vertex must be inverted if the product of the edge signs
 along the tree edges between the bicomp root and the vertex is -1.

 Finally, the PreserveSigns flag, if set, performs the inversions
 but does not change any of the edge signs.  This allows a second
 invocation of this function to restore the state of the bicomp
 as it was before the first call.

 This method uses the stack but preserves whatever may have been
 on it.  In debug mode, it will return NOTOK if the stack overflows.
 This method pushes at most two integers per vertext in the bicomp.

 Returns OK on success, NOTOK on implementation failure.
 ********************************************************************/

int _OrientVerticesInBicomp(graphP theGraph, int BicompRoot, int PreserveSigns)
{
    int W, e, invertedFlag;
    int stackBottom = sp_GetCurrentSize(theGraph->theStack);

    sp_Push2(theGraph->theStack, BicompRoot, 0);

    while (sp_GetCurrentSize(theGraph->theStack) > stackBottom)
    {
        /* Pop a vertex to orient */
        sp_Pop2(theGraph->theStack, W, invertedFlag);

        /* Invert the vertex if the inverted flag is set */
        if (invertedFlag)
            _InvertVertex(theGraph, W);

        /* Push the vertex's DFS children that are in the bicomp */
        e = gp_GetFirstEdge(theGraph, W);
        while (gp_IsEdge(theGraph, e))
        {
            if (gp_GetEdgeType(theGraph, e) == EDGE_TYPE_CHILD)
            {
                sp_Push2(theGraph->theStack, gp_GetNeighbor(theGraph, e),
                         invertedFlag ^ gp_GetEdgeFlagInverted(theGraph, e));

                if (!PreserveSigns)
                    gp_ClearEdgeFlagInverted(theGraph, e);
            }

            e = gp_GetNextEdge(theGraph, e);
        }
    }
    return OK;
}

/********************************************************************
 _JoinBicomps()
 The embedding algorithm works by only joining bicomps once the result
 forms a larger bicomp.  However, if the original graph was separable
 or disconnected, then the result of the embed function will be a
 graph that contains each bicomp as a distinct entity.  This function
 merges the bicomps into one connected graph.
 ********************************************************************/

int _JoinBicomps(graphP theGraph)
{
    for (int R = gp_LowerBoundVirtualVertices(theGraph); R < gp_UpperBoundVirtualVertices(theGraph); ++R)
    {
        // If the bicomp root is still active (i.e. an in-use virtual vertex)
        // then merge it with its parent copy vertex (non-virtual)
        if (gp_VirtualVertexInUse(theGraph, R))
            _MergeVertex(theGraph, _gp_GetVertexFromBicompRoot(theGraph, R), 0, R);
    }

    return OK;
}


////////////////////////////////////////////////////////////////////////////////
// Library Checker driver
////////////////////////////////////////////////////////////////////////////////

#include <string>

namespace {

char inbuf[1 << 16];
int inbuf_len = 0, inbuf_pos = 0;
inline int read_char() {
    if (inbuf_pos == inbuf_len) {
        inbuf_len = int(fread(inbuf, 1, sizeof(inbuf), stdin));
        inbuf_pos = 0;
        if (inbuf_len <= 0) return -1;
    }
    return inbuf[inbuf_pos++];
}
inline int read_int() {
    int c = read_char();
    while (c < '0' || c > '9') { if (c == -1) return 0; c = read_char(); }
    int x = 0;
    while (c >= '0' && c <= '9') { x = x * 10 + (c - '0'); c = read_char(); }
    return x;
}
void append_int(std::string& out, int x) {
    char tmp[12];
    int len = 0;
    if (x == 0) tmp[len++] = '0';
    while (x > 0) { tmp[len++] = char('0' + x % 10); x /= 10; }
    while (len > 0) out += tmp[--len];
}

} // namespace

int main() {
    int T = read_int();
    std::string out;
    while (T--) {
        int N = read_int();
        int M = read_int();
        if (N >= 3 && (long long)M > 3LL * N - 6) {
            for (int i = 0; i < 2 * M; i++) read_int();
            out += "No\n";
            continue;
        }
        // The library uses 1-based vertex indices (USE_1BASEDARRAYS).
        graphP g = gp_New();
        if (gp_EnsureVertexCapacity(g, N) != OK) return 1;
        for (int i = 0; i < M; i++) {
            int a = read_int();
            int b = read_int();
            if (gp_AddEdge(g, a + 1, 0, b + 1, 0) != OK) return 1;
        }
        int r = gp_Embed(g, EMBEDFLAGS_PLANAR);
        if (r == OK) {
            // gp_Embed leaves the vertices in DFS order; restore the input numbering.
            if (gp_SortVertices(g) != OK) return 1;
            out += "Yes\n";
            for (int v = gp_LowerBoundVertices(g); v < gp_UpperBoundVertices(g); v++) {
                bool first = true;
                for (int e = gp_GetFirstEdge(g, v); gp_IsEdge(g, e); e = gp_GetNextEdge(g, e)) {
                    if (!first) out += ' ';
                    first = false;
                    append_int(out, gp_GetNeighbor(g, e) - 1);
                }
                out += '\n';
            }
        } else if (r == NONEMBEDDABLE) {
            out += "No\n";
        } else {
            return 1;
        }
        gp_Free(&g);
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}

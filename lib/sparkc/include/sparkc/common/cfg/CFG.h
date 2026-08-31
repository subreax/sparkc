#pragma once
#include "Graph.h"
#include "CfgBlock.h"

template <typename I>
using CFG = Graph<CfgBlock<I>>;

/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "DexUtil.h"
#include "ProguardConfiguration.h"

namespace redex {

void show_configuration(std::ostream& output,
                        const Scope& classes,
                        const ProguardConfiguration& config);

std::string show_keep(const KeepSpec& keep_rule, bool show_source = true);
}

// namespace redex

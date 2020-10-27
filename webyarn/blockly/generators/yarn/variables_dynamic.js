/**
 * @license
 * Copyright 2018 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @fileoverview Generating Yarn for dynamic variable blocks.
 * @author fenichel@google.com (Rachel Fenichel)
 */
'use strict';

goog.provide('Blockly.Yarn.variablesDynamic');

goog.require('Blockly.Yarn');
goog.require('Blockly.Yarn.variables');


// Yarn is dynamically typed.
Blockly.Yarn['variables_get_dynamic'] =
    Blockly.Yarn['variables_get'];
Blockly.Yarn['variables_set_dynamic'] =
    Blockly.Yarn['variables_set'];
